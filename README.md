# LetterShark

A dark-themed Windows desktop tool for browsing and searching large, structured log files — single files, whole folders, or `.tar.gz` archives — side by side, without opening a text editor per file.

---

## Why LetterShark?

Structured log dumps (tag-based or pipe-delimited formats) are painful to work through file by file in a plain text editor: no tabular view of the fields, no cross-file search, no quick way to isolate and export just the matching lines. LetterShark parses each file into a proper table, keeps every opened file in its own tab, and lets you search — globally across everything that's open, or scoped to one file — with results highlighted in place and exportable to a text file.

---

## Features

- **Dual log-format parsing** — bracketed-tag lines (`<tag><tag>message`) and pipe-delimited "VNMF" lines (`[field] | [field] | ...`), auto-mapped to the matching column layout
- **Open files, folders, or `.tar.gz` archives** — folders are scanned recursively, archives are read and decompressed in memory via libarchive; every discovered file becomes its own tab
- **Global or scoped search** — search every open tab at once or a single selected file, with live match highlighting and a running match-count summary
- **Shortcut search** — `Ctrl+F` / Find Next / Find Previous / `Ctrl+R` for quick in-table highlighting in a user-chosen color, independent of the main search bar
- **Export** — write global or per-file search matches to a text file in the Downloads folder
- **RAM guard** — warns before parsing large files/folders or running a global search if available physical memory looks low, so you don't lock up on a multi-GB log dump
- **Per-table column visibility toggle** via right-click on any table header
- **Custom frameless dark UI** — draggable title bar, Aero-Snap-style maximize-on-drag-to-top

---

## Architecture

| Component | Responsibility |
|---|---|
| `MainWindow` | Top-level window; wires everything together, owns tab/tree state |
| `FileManager` | Tracks opened files/folders/archives; drives the native open dialogs |
| `Parser` | Parses a single log file (tagged or VNMF format) into tabular rows |
| `ZipExtractor` | Decompresses `.gz` / `.tar.gz` archives via libarchive |
| `TabFactory` | Builds/removes per-file tab pages and applies table styling/headers |
| `SearchController` | Runs global/specific search, match highlighting, and export |
| `ContextMenuHandler` | Right-click "Parse File / Parse Folder / Unload" menu on the opened-files tree |
| `CustomDialog` | Small modal used for export-file selection and shortcut-search input |
| `TitleBar` / `WindowDragger` | Custom frameless window chrome (menu bar, min/max/close, drag-to-move) |
| `RamWatcher` | Checks available physical RAM before memory-heavy operations (Windows-only) |

---

## Requirements

| Dependency | Notes |
|---|---|
| C++ | 17 |
| CMake | 3.16+ |
| Qt | 6, Widgets module, via MSYS2 MinGW64 |
| libarchive | via MSYS2 — used for `.gz` / `.tar.gz` decompression |
| QuaZip | vendored in `third_party/quazip` — zip-format support, currently unused (reserved for future archive formats) |

Target platform: **Windows only** (uses the Win32 `GlobalMemoryStatusEx` API for RAM checks, and `windeployqt` for release packaging).

---

## Usage

Use the **File** menu (in the title bar) to open a single log file, a folder (scanned recursively), or a `.tar.gz` archive — each discovered file appears as its own entry in the tree on the left and gets parsed into a tab on first open.

Search either globally (all open tabs) or in one selected file via the search bar at the bottom, or use `Ctrl+F` for a quick, color-coded highlight search in the currently active tab. Export matched results via the **Export** menu.

Supported input formats: `.log`, `.txt`, and gzip-compressed variants (`.gz`, `.tar.gz`). Enable the VNMF checkbox for pipe-delimited logs; leave it off for the default bracketed-tag format.

---

## Project Structure

```
src/
  core/        # Parser, FileManager, ZipExtractor, RamWatcher — business logic, minimal QWidget dependency
  gui/         # MainWindow and all QWidget-based UI (tabs, dialogs, title bar, ...)
resources/     # Icons, dark theme stylesheet, resources.qrc
third_party/
  quazip/      # Vendored zip-format library (currently unused, reserved for future archive-format support)
```
