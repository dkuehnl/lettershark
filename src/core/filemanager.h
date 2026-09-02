#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <qobject.h>
#include <QMap>
#include <QList>

/**
 * @brief Tracks one opened log entry (a loaded file, or a file discovered
 *        while scanning a folder/archive) and the UI tab that displays it.
 *
 * page is a non-owning pointer into the QTabWidget's tab tree; it is null
 * until MainWindow actually builds a tab for this entry (see TabFactory),
 * and is not cleared automatically when that tab is closed/deleted.
 */
struct FileTabInfo {
    QString filename;    ///< Display name (file name only, no path).
    QString filepath;    ///< Full path used to (re-)open/parse the underlying content.
    QString root_folder; ///< Parent grouping key for folder/archive scans; empty for single opened files.
    QWidget* page;        ///< Non-owning pointer to the associated tab page; nullptr until a tab is created for this entry.
};

/**
 * @brief Owns the set of currently opened log entries and drives the
 *        file/folder/archive open dialogs.
 *
 * Pure bookkeeping and I/O-dialog layer: FileManager does not create or
 * touch any UI beyond the native Qt file dialogs it triggers. Discovered
 * entries are reported to callers via file_added()/folder_scanned(); it is
 * the caller's (MainWindow's) responsibility to build tree/tab UI from them.
 */
class FileManager : public QObject {
    Q_OBJECT

public:
    explicit FileManager(QObject* parent = nullptr);

    /**
     * @brief Looks up a tracked entry by its display filename.
     * @param filename Display name as stored in FileTabInfo::filename.
     * @return A mutable reference to the matching entry, or std::nullopt if not tracked.
     */
    std::optional<std::reference_wrapper<FileTabInfo>> get_entry(const QString& filename);

    /**
     * @brief Finds all tracked entries belonging to a given folder/archive grouping.
     * @param filename The root_folder key to match against (see FileTabInfo::root_folder).
     * @return Display filenames of all entries whose root_folder equals filename.
     */
    QList<QString> find_items_to_root(const QString& filename);

    /**
     * @brief Removes a tracked entry.
     * @param key Display filename to remove (see FileTabInfo::filename).
     * @return Number of entries removed (0 or 1, per QMap::remove()).
     */
    int delete_entry(const QString& key);

    /// @brief Clears all tracked entries. Does not touch any UI (tabs/tree items must be removed separately).
    void clear();

signals:
    /// @brief Emitted once per file after open_file() successfully adds a new, not-yet-tracked entry.
    void file_added(const FileTabInfo&);

    /// @brief Emitted once after open_folder()/open_archive() finish scanning, with all discovered entries and the grouping name (folder/archive base name).
    void folder_scanned(const QVector<FileTabInfo>&, QString);

public slots:
    /// @brief Opens a native multi-file selection dialog and tracks any newly selected, not-yet-tracked files.
    void open_file();

    /// @brief Opens a native folder selection dialog, recursively scans it, and tracks all discovered files.
    void open_folder();

    /// @brief Opens a native file selection dialog for a .tar.gz archive and tracks all entries found inside it.
    void open_archive();

private:
    QMap<QString, FileTabInfo> m_file_mapping;

    /// @brief Builds a FileTabInfo from a filesystem/archive path; folder_name overrides the derived grouping key when non-empty.
    FileTabInfo build_file_tab_info(const QString& filepath, const QString& filename, const QString& folder_name);
};

#endif // FILEMANAGER_H
