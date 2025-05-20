#include "filemanager.h"
#include "zipextractor.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <archive.h>
#include <archive_entry.h>


FileManager::FileManager(QObject* parent)
    : QObject(parent) {}

FileTabInfo FileManager::build_file_tab_info(const QString& filepath, const QString& filename, const QString& folder_name = "") {
    QFileInfo fi(filepath);
    if (filename.isEmpty()) {
        return FileTabInfo{fi.fileName(), filepath, "", nullptr};
    }
    int index = filename.indexOf("/");
    QString inner_path = filename.left(index + 1);

    if (!folder_name.isEmpty()) {
        inner_path = folder_name;
    }

    return FileTabInfo{fi.fileName(), filepath, inner_path, nullptr};
}

void FileManager::open_file() {
    QStringList paths = QFileDialog::getOpenFileNames(nullptr, "Open new Logfile", QDir::homePath());
    if (paths.isEmpty()) return;

    for (const auto& path : paths) {
        FileTabInfo info = build_file_tab_info(path, "");
        if (!m_file_mapping.contains(info.filename)) {
            m_file_mapping.insert(info.filename, info);
            emit file_added(info);
        }
    }
}

void FileManager::open_folder() {
    QString folder_path = QFileDialog::getExistingDirectory(nullptr, tr("Choose Folder"), QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (folder_path.isEmpty()) return;

    QVector<FileTabInfo> all_infos;
    QDir dir(folder_path);

    QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    for (const QFileInfo& info : entries) {
        FileTabInfo file_info = build_file_tab_info(info.absoluteFilePath(), info.fileName(), QFileInfo(folder_path).fileName());
        qDebug() << file_info.filename;
        qDebug() << file_info.filepath;
        qDebug() << file_info.root_folder;

        m_file_mapping.insert(file_info.filename, file_info);
        all_infos.push_back(file_info);
    }
    emit folder_scanned(all_infos);
}

void FileManager::open_archive() {
    QString tar_gz = QFileDialog::getOpenFileName(nullptr, "Open new tar.gz/.zip");
    if (tar_gz.isEmpty()) return;

    auto archive_names = ZipExtractor::extract_filenames(tar_gz);
    QVector<FileTabInfo> all_infos;

    for (const auto& name : archive_names)  {
        QString full = tar_gz + "/" + name;
        FileTabInfo info = build_file_tab_info(full, name);
        m_file_mapping.insert(info.filename, info);
        all_infos.push_back(info);
    }
    emit folder_scanned(all_infos);
}

std::optional<std::reference_wrapper<FileTabInfo>> FileManager::get_entry(const QString& filename) {
    auto entry = m_file_mapping.find(filename);
    if (entry != m_file_mapping.end()) {
        return std::ref(entry.value());
    }
    return std::nullopt;
}

QList<QString> FileManager::find_items_to_root(const QString& filename) {
    QList<QString> filelist;
    for (auto entry = m_file_mapping.begin(); entry != m_file_mapping.end(); entry++) {
        const FileTabInfo& file = entry.value();
        if (file.root_folder == filename) {
            filelist.push_back(file.filename);
        }
    }
    return filelist;
}

int FileManager::delete_entry(const QString& key) {
    return m_file_mapping.remove(key);
}


void FileManager::clear() {
    m_file_mapping.clear();
}
