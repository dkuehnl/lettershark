#ifndef PARSER_H
#define PARSER_H

#include <QString>

struct TableRow {
    std::array<std::string, 9> cols;
};

class Parser {
public:
    Parser(QString filepath, bool vnmf_log = false);
    unsigned int get_parsed_lines() { return m_table.size(); };
    std::vector<TableRow> get_parsed_data() { return m_table; };

private:
    QString m_filepath;
    bool m_vnmf;
    std::vector<TableRow> m_table;

    void split_tag(std::string tag, TableRow& parsed_line);
    std::pair<std::vector<std::string>, size_t> get_tags_and_pos(const std::string& line);
    std::pair<size_t, size_t> parse_tags(std::string line, TableRow& parsed_line);
    void parse_message(std::string message_body, TableRow& parsed_line);
    void parse_body(std::string message, TableRow& parsed_line);
    TableRow parse_line(std::string line);

    void parse_vnmf(const std::string& line, TableRow& parsed_line);
};

#endif // PARSER_H
