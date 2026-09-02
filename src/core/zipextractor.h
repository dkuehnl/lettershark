#ifndef ZIPEXTRACTOR_H
#define ZIPEXTRACTOR_H

#include <QString>
#include <vector>

/**
 * @brief Reads and decompresses .gz and .tar.gz log archives via libarchive.
 *
 * All functions operate on plain gzip/tar streams (used to ship compressed
 * log files), not general-purpose zip archives. QuaZip is a separate,
 * currently unused vendored dependency (third_party/quazip) kept available
 * for future zip-format support.
 */
namespace ZipExtractor {
    /**
     * @brief Lists the file entries contained in a .tar.gz archive.
     *
     * Returns an empty vector (and logs via qDebug) if the archive can't be
     * opened. Directory entries and the "." root entry are filtered out.
     *
     * @param folder Path to a .tar.gz archive; non-".tar.gz" paths always return an empty result.
     * @return Cleaned relative paths of all regular files inside the archive.
     */
    std::vector<QString> extract_filenames(const QString& folder);

    /**
     * @brief Decompresses a single plain .gz file (not a tar archive) into memory.
     * @param filepath Path to the .gz file.
     * @return The decompressed file content.
     * @throws std::runtime_error if the archive can't be opened, read, or the header is invalid.
     */
    QByteArray decomp_file(const QString& filepath);

    /**
     * @brief Decompresses a raw gzip byte buffer into memory.
     * @param input_data Gzip-compressed bytes.
     * @return The decompressed content.
     */
    QByteArray decompressGzip(const QByteArray& input_data);

    /**
     * @brief Splits a combined "archive.tar.gz/inner/path" string into archive path and inner entry path.
     *
     * Matches case-insensitively on the first ".tar.gz" occurrence. The returned
     * inner path is normalized to start with "./" and never with a leading slash.
     *
     * @param filepath Combined path in the form "<archive>.tar.gz<inner-path>".
     * @return Pair of (archive path up to and including ".tar.gz", normalized inner entry path).
     *         Both are empty if filepath does not contain ".tar.gz".
     */
    std::pair<QString, QString> split_filepath(const QString& filepath);

    /**
     * @brief Extracts a single named entry from a .tar.gz archive into memory.
     *
     * Locates the entry via split_filepath(), then decompresses just that entry
     * (itself gzip-compressed within the tar) using a nested libarchive read.
     *
     * @param filepath Combined path in the form "<archive>.tar.gz/<entry-name>".
     * @return The decompressed content of the matched entry.
     * @throws std::runtime_error if the tar can't be opened/read, the entry's
     *         inner gzip stream is invalid, or reading either stream fails.
     */
    QByteArray decomp_folder(const QString& filepath);
}

#endif // ZIPEXTRACTOR_H
