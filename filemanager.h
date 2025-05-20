#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <qobject.h>
#include <QMap>
#include <QList>

struct FileTabInfo {
    QString filename;
    QString filepath;
    QString root_folder;
    QWidget* page;
};


class FileManager : public QObject {
    Q_OBJECT

public:
    explicit FileManager(QObject* parent = nullptr);
    std::optional<std::reference_wrapper<FileTabInfo>> get_entry(const QString& filename);
    QList<QString> find_items_to_root(const QString& filename);
    int delete_entry(const QString& key);

    void clear();

signals:
    void file_added(const FileTabInfo&);
    void folder_scanned(const QVector<FileTabInfo>&);
public slots:
    void open_file();
    void open_folder();
    void open_archive();
private:
    QMap<QString, FileTabInfo> m_file_mapping;
    FileTabInfo build_file_tab_info(const QString& filepath, const QString& filebname, const QString& folder_name);

};

#endif // FILEMANAGER_H
