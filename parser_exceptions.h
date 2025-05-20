#ifndef PARSER_EXCEPTIONS_H
#define PARSER_EXCEPTIONS_H

#include <QString>
#include <stdexcept>

struct unspupported_format_exception : std::runtime_error {
    unspupported_format_exception(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

struct unable_to_read_file : std::runtime_error {
    unable_to_read_file(const QString& msg)
        : std::runtime_error(msg.toStdString()) {}
};

#endif // PARSER_EXCEPTIONS_H
