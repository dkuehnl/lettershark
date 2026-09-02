#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

/**
 * @brief Small modal dialog used in two distinct modes: picking a file from
 *        a dropdown (export-target selection), or entering a search phrase
 *        together with a highlight color (shortcut search).
 *
 * @note Which constructor is used determines the active mode, but m_mode
 *       itself is only explicitly set to Mode::LineEdit by the LineEdit
 *       constructor; the ComboBox constructor leaves it default-initialized.
 *       selected_option()/search_pair() branch on m_mode, so only call the
 *       accessor matching the constructor actually used.
 */
class CustomDialog : public QDialog {
    Q_OBJECT

public:
    /// @brief Which input mode this dialog instance was built for.
    enum class Mode { ComboBox, LineEdit };

    /**
     * @brief Builds a dialog for picking one option from a dropdown (used for export-target file selection).
     * @param options Selectable entries shown in the combo box.
     * @param parent Parent widget, or nullptr.
     */
    CustomDialog(const QStringList& options, QWidget* parent = nullptr);

    /**
     * @brief Builds a dialog for entering free text plus a highlight color (used for shortcut search).
     * @param placeholder Placeholder text shown in the empty line edit.
     * @param parent Parent widget, or nullptr.
     */
    CustomDialog(const QString& placeholder, QWidget* parent = nullptr);

    /// @brief Returns the combo box's current selection. Only meaningful after the ComboBox-mode constructor was used and the dialog was accepted.
    QString selected_option() const;

    /// @brief Returns the entered search text paired with the chosen highlight color. Only meaningful after the LineEdit-mode constructor was used and the dialog was accepted.
    QPair<QString, QColor> search_pair() const;

private:
    Mode m_mode;
    QComboBox* m_combo_box = nullptr;
    QLineEdit* m_line_edit = nullptr;
    QComboBox* m_color_box = nullptr;
};

#endif // CUSTOMDIALOG_H
