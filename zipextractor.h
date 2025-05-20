#ifndef ZIPEXTRACTOR_H
#define ZIPEXTRACTOR_H

#include <QString>
#include <vector>

namespace ZipExtractor {
    std::vector<QString> extract_filenames(const QString& folder);
    QByteArray decomp_file(const QString& filepath);
    QByteArray decompressGzip(const QByteArray& input_data);
    std::pair<QString, QString> split_filepath(const QString& filepath);
    QByteArray decomp_folder(const QString& filepath);
}

#endif // ZIPEXTRACTOR_H
