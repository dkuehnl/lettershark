#ifndef PARSER_EXCEPTIONS_H
#define PARSER_EXCEPTIONS_H

#include <QString>
#include <stdexcept>

/**
 * @brief Thrown by Parser when the input file's extension is not one of the
 *        supported log formats (.log, .txt, .gz).
 */
struct unspupported_format_exception : std::runtime_error {
    unspupported_format_exception(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

/**
 * @brief Thrown by Parser when the input file exists but could not be opened
 *        for reading (permissions, in-use, or I/O failure).
 */
struct unable_to_read_file : std::runtime_error {
    unable_to_read_file(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

#endif // PARSER_EXCEPTIONS_H
