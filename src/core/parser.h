#ifndef PARSER_H
#define PARSER_H

#include <QString>

/**
 * @brief One parsed log line, split into up to 9 display columns.
 *
 * Column usage depends on which format Parser was constructed for:
 * - Default (tagged) format uses cols[0..7] (Timestamp, Log-Level, Tag, PID,
 *   ID, Module, Function, Message); cols[8] is left empty. See TabFactory::setup_default_headers().
 * - VNMF (pipe-delimited) format uses all cols[0..8]. See TabFactory::setup_vnmf_headers().
 */
struct TableRow {
    std::array<std::string, 9> cols;
};

/**
 * @brief Reads a single log file (plain text or .gz/.tar.gz-compressed) and
 *        parses it line-by-line into TableRow entries for display.
 *
 * Supports two independent line formats, selected once at construction via
 * vnmf_log: a bracketed-tag format ("<tag><tag><tag>message") parsed by
 * parse_tags()/split_tag()/parse_message(), and a pipe-delimited "VNMF"
 * format ("[field] | [field] | ...") parsed by parse_vnmf(). The entire file
 * is parsed eagerly in the constructor; results are retrieved afterwards via
 * get_parsed_data().
 */
class Parser {
public:
    /**
     * @brief Opens filepath, decompressing it first if it ends in .gz/.tar.gz, and parses it fully.
     * @param filepath Path to a .log, .txt, or .gz/.tar.gz log file.
     * @param vnmf_log If true, parse every line with the pipe-delimited VNMF format instead of the default tagged format.
     * @throws unspupported_format_exception if the file extension is not .log/.txt/.gz.
     * @throws unable_to_read_file if a non-archive file can't be opened for reading.
     */
    Parser(QString filepath, bool vnmf_log = false);

    /// @brief Returns the number of successfully parsed lines.
    unsigned int get_parsed_lines() { return m_table.size(); };

    /// @brief Returns a copy of all parsed rows, in file order.
    std::vector<TableRow> get_parsed_data() { return m_table; };

private:
    QString m_filepath;
    bool m_vnmf;
    std::vector<TableRow> m_table;

    /// @brief Splits a single tag's whitespace-separated content into cols[0..3], re-joining a 5th+ word onto cols[3].
    void split_tag(std::string tag, TableRow& parsed_line);

    /// @brief Extracts all leading "<...>" tag groups from the start of a line (after skipping leading whitespace).
    /// @return The extracted tag contents (without angle brackets) and the position right after the last tag.
    std::pair<std::vector<std::string>, size_t> get_tags_and_pos(const std::string& line);

    /// @brief Parses up to 3 leading tags via get_tags_and_pos() and distributes them into parsed_line.cols[0..5]; logs via qDebug if more than 3 tags are found.
    /// @return Pair of (number of tags found, position right after the last tag).
    std::pair<size_t, size_t> parse_tags(std::string line, TableRow& parsed_line);

    /// @brief Splits a message body into an optional leading "[function]" (cols[6]) and the remaining text (cols[7]).
    void parse_message(std::string message_body, TableRow& parsed_line);

    /// @brief Forwards a non-empty message body to parse_message(); clears cols[7] if message is empty.
    void parse_body(std::string message, TableRow& parsed_line);

    /// @brief Parses a single line using the format selected at construction (parse_vnmf() or the tagged format).
    /// @return The parsed row. If the tagged format finds no leading tags, the raw line is stored in cols[7] unmodified.
    TableRow parse_line(std::string line);

    /// @brief Parses a single line in the pipe-delimited VNMF format ("[field] | [field] | ...") into cols[0..8].
    ///
    /// Falls back to storing the raw line in cols[8] if it contains neither
    /// '[' nor '|'. Stops distributing fields after 8 columns are filled;
    /// any remaining text becomes cols[8].
    void parse_vnmf(const std::string& line, TableRow& parsed_line);
};

#endif // PARSER_H
