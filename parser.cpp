#include "parser.h"
#include "zipextractor.h"
#include "parser_exceptions.h"

#include <iostream>
#include <sstream>

#include <QDebug>
#include <QTextStream>
#include <QFileInfo>
#include <QBuffer>

Parser::Parser(QString filepath)
    : m_filepath(filepath) {
    QFileInfo tmp_file(filepath);

    if (tmp_file.suffix().toLower() != "log" && tmp_file.suffix().toLower() != "txt" && tmp_file.suffix().toLower() != "gz") {
        throw unspupported_format_exception("Unsupported file-type");
    }

    if (tmp_file.suffix() == "gz") {
        QByteArray decompressed_data;
        QBuffer buffer;

        if (filepath.contains(".tar.gz")) {
            decompressed_data = ZipExtractor::decomp_folder(filepath);
        } else {
            decompressed_data = ZipExtractor::decomp_file(filepath);
        }

        buffer.setData(decompressed_data);
        buffer.open(QIODevice::ReadOnly);
        QTextStream stream(&buffer);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            TableRow row = parse_line(line.toStdString());
            m_table.push_back(row);
        }
    } else {
        QFile file(m_filepath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            throw unable_to_read_file("File could not be readed.");
        }

        QTextStream in(&file);
        QString line;
        while (!in.atEnd()) {
            line = in.readLine();
            TableRow row = parse_line(line.toStdString());
            m_table.push_back(row);
        }
    }
};

void Parser::split_tag(std::string tag, TableRow& parsed_line) {
    std::istringstream iss(tag);
    std::string word;
    size_t i = 0;
    while (iss >> word) {
        if (i == 4) {
            word = " " + word;
            parsed_line.cols[i-1] += word;
        } else {
            parsed_line.cols[i] = word;
        }
        i++;
    }
}

std::pair<std::vector<std::string>, size_t> Parser::get_tags_and_pos(const std::string& line) {
    size_t pos = 0;
    std::vector<std::string> tags;

    while(pos < line.size() && std::isspace(line[pos])) ++pos;

    while(pos < line.size() && line[pos] == '<') {
        size_t end = line.find('>', pos);
        if (end == std::string::npos) break;
        tags.push_back(line.substr(pos+1, end-pos-1));
        pos = end+1;
        if (pos >= line.size() || line[pos] != '<') break;
    }

    return {std::move(tags), pos};
}

std::pair<size_t, size_t> Parser::parse_tags(std::string line, TableRow& parsed_line) {
    auto [tags, end_pos] = get_tags_and_pos(line);
    if (tags.size() == 0) {
        return {0,0};
    } else if (tags.size() == 1) {
        split_tag(tags[0], parsed_line);
        parsed_line.cols[4] = "";
        parsed_line.cols[5] = "";
    } else if (tags.size() == 2) {
        split_tag(tags[0], parsed_line);
        parsed_line.cols[4] = "";
        parsed_line.cols[5] = tags[1];
    } else if (tags.size() == 3) {
        split_tag(tags[0], parsed_line);
        parsed_line.cols[4] = tags[1];
        parsed_line.cols[5] = tags[2];
    } else {
        qDebug() << "Mehr Tags als verarbeitbar: " + line;
    }
    return {tags.size(), end_pos};
}

void Parser::parse_message(std::string message_body, TableRow& parsed_line) {
    size_t function_start = 0;
    if ((function_start = message_body.find('[', function_start)) != std::string::npos) {
        size_t function_end = message_body.find(']', function_start);
        parsed_line.cols[6] = message_body.substr(function_start+1, function_end-1);
        parsed_line.cols[7] = message_body.substr(function_end+1);
    } else {
        parsed_line.cols[6] = "";
        parsed_line.cols[7] = message_body;
    }
}

void Parser::parse_body(std::string message, TableRow& parsed_line) {
    if (message.empty()) {
        parsed_line.cols[7] = "";
    }
    parse_message(message, parsed_line);
}

TableRow Parser::parse_line(std::string line) {
    TableRow parsed_line;
    auto [tag_size, end_pos] = Parser::parse_tags(line, parsed_line);
    if (tag_size == 0) {
        parsed_line.cols[7] = line;
        return parsed_line;
    }
    std::string message = line.substr(end_pos);
    Parser::parse_body(message, parsed_line);
    return parsed_line;
}

unsigned int Parser::get_parsed_lines() {
    return m_table.size();
}

std::vector<TableRow> Parser::get_parsed_data() {
    return m_table;
}
