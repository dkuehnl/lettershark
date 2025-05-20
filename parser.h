#ifndef PARSER_H
#define PARSER_H

#include <QString>

struct TableRow {
    std::array<std::string, 8> cols;
};

class Parser {
public:
    Parser(QString filepath);
    unsigned int get_parsed_lines();
    std::vector<TableRow> get_parsed_data();

private:
    QString m_filepath;
    std::vector<TableRow> m_table;

    void split_tag(std::string tag, TableRow& parsed_line);
    std::pair<std::vector<std::string>, size_t> get_tags_and_pos(const std::string& line);
    std::pair<size_t, size_t> parse_tags(std::string line, TableRow& parsed_line);
    void parse_message(std::string message_body, TableRow& parsed_line);
    void parse_body(std::string message, TableRow& parsed_line);
    TableRow parse_line(std::string line);
};

#endif // PARSER_H
