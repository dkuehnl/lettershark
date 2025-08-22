#include "zipextractor.h"
#include "quazipfile.h"

#include <archive.h>
#include <archive_entry.h>
#include <qdebug.h>
#include <qdir.h>
#include <zlib.h>
#include <qstringliteral.h>
#include <quazip/quazip.h>


std::vector<QString> ZipExtractor::extract_filenames(const QString& folder) {
    std::vector<QString> filenames;

    if (folder.endsWith(".tar.gz")) {
        struct archive* a = archive_read_new();
        archive_read_support_filter_gzip(a);
        archive_read_support_format_tar(a);

        std::wstring wpath = folder.toStdWString();
        if (archive_read_open_filename_w(a, wpath.c_str(), 10240) != ARCHIVE_OK) {
            qDebug() << "Archive konnte nicht gelesen werden";
            archive_read_free(a);
            return {};
        }

        struct archive_entry* entry;
        while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
            const char* name = archive_entry_pathname(entry);
            bool is_dir = archive_entry_filetype(entry) == AE_IFDIR;
            QString path = QDir::cleanPath(QString::fromUtf8(name));
            if (path == ".") continue;
            if (is_dir) {
                continue;
            } else {
                filenames.push_back(path);
            }
        }
        archive_read_close(a);
        archive_read_free(a);
        return filenames;
    }
    return {};
}

QByteArray ZipExtractor::decomp_folder(const QString& filepath) {
    QByteArray final_log;

    struct archive* tar_archive = archive_read_new();
    archive_read_support_filter_gzip(tar_archive);
    archive_read_support_format_tar(tar_archive);

    auto [tar_gz, log_gz] = ZipExtractor::split_filepath(filepath);
    std::wstring wpath = tar_gz.toStdWString();
    if (archive_read_open_filename_w(tar_archive, wpath.c_str(), 10240) != ARCHIVE_OK) {
        QString err = archive_error_string(tar_archive);
        qDebug() << err;
        archive_read_free(tar_archive);
        throw std::runtime_error(err.toStdString());
        return {};
    }

    struct archive_entry* entry;
    while (archive_read_next_header(tar_archive, &entry) == ARCHIVE_OK) {
        QString entry_name = QString::fromUtf8(archive_entry_pathname(entry));
        qDebug() << entry_name;
        if (entry_name == log_gz) {
            qDebug() << "Gefunden";
            QByteArray tar_gz;
            const int CHUNK_SIZE = 8192;
            char buffer[CHUNK_SIZE];
            la_ssize_t len;

            while ((len = archive_read_data(tar_archive, buffer, CHUNK_SIZE)) > 0) {
                tar_gz.append(buffer, static_cast<int>(len));
            }
            if (len < 0) {
                QString err = archive_error_string(tar_archive);
                qDebug() << err << "Line 93";
                archive_read_free(tar_archive);
                throw std::runtime_error(err.toStdString());
                return {};
            }

            struct archive* log_archive = archive_read_new();
            archive_read_support_filter_gzip(log_archive);
            archive_read_support_format_raw(log_archive);

            struct archive_entry* log_entry = nullptr;
            if (archive_read_open_memory(log_archive, tar_gz.data(), tar_gz.size()) !=ARCHIVE_OK) {
                QString err = archive_error_string(log_archive);
                qDebug() << err << "Line 106";
                archive_read_free(log_archive);
                throw std::runtime_error(err.toStdString());
                break;
            }

            if (archive_read_next_header(log_archive, &log_entry) != ARCHIVE_OK) {
                QString err = archive_error_string(log_archive);
                qDebug() << err << "Line 114";
                throw std::runtime_error(err.toStdString());
            }

            char log_buf[CHUNK_SIZE];
            la_ssize_t log_len;
            while ((log_len = archive_read_data(log_archive, log_buf, CHUNK_SIZE)) >0) {
                final_log.append(log_buf, static_cast<int>(log_len));
            }
            if (log_len < 0) {
                QString err = archive_error_string(log_archive);
                qDebug() << err << "Line 119";
                throw std::runtime_error(err.toStdString());
            }

            archive_read_free(log_archive);
            break;
        } else {
            archive_read_data_skip(tar_archive);
        }
    }
    archive_read_close(tar_archive);
    archive_read_free(tar_archive);
    return final_log;
}

QByteArray ZipExtractor::decomp_file(const QString& filepath) {
    struct archive* a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_raw(a);
    if (archive_read_open_filename(a, filepath.toUtf8().constData(), 10240) != ARCHIVE_OK) {
        throw std::runtime_error(".gz-File couldn't be opened");
        return {};
    }

    struct archive_entry* entry = nullptr;
    if (archive_read_next_header(a, &entry) != ARCHIVE_OK) {
        QString err = archive_error_string(a);
        qDebug() << err;
        archive_read_free(a);
        throw std::runtime_error(err.toStdString());
    }


    QByteArray output;
    const int CHUNK_SIZE = 8192;
    char buffer[CHUNK_SIZE];
    la_ssize_t len;

    while ((len = archive_read_data(a, buffer, CHUNK_SIZE)) > 0) {
        output.append(buffer, static_cast<int>(len));
    }
    if (len < 0) {
        QString err = archive_error_string(a);
        qDebug() << err;
        archive_read_free(a);
        throw std::runtime_error(err.toStdString());
    }

    archive_read_close(a);
    archive_read_free(a);
    return output;
}

std::pair<QString, QString> ZipExtractor::split_filepath(const QString& filepath) {
    QString pattern = ".tar.gz";
    int index = filepath.indexOf(pattern, Qt::CaseInsensitive);

    if (index == -1) {
        return { "", ""};
    }

    int split_index = index + pattern.length();

    QString tar_gz = filepath.left(split_index);
    QString internal_path = filepath.mid(split_index);
    internal_path = "." + internal_path;

    if (internal_path.startsWith("/") || internal_path.startsWith("\\")) {
        internal_path.remove(0,1);
    }

    return {tar_gz, internal_path};
}
