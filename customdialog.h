#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

class CustomDialog : public QDialog {
    Q_OBJECT

public:
    enum class Mode { ComboBox, LineEdit };

    CustomDialog(const QStringList& options, QWidget* parent = nullptr);
    CustomDialog(const QString& placeholder, QWidget* parent = nullptr);
    QString selected_option() const;
    QPair<QString, QColor> search_pair() const;

private:
    Mode m_mode;
    QComboBox* m_combo_box = nullptr;
    QLineEdit* m_line_edit = nullptr;
    QComboBox* m_color_box = nullptr;
};

#endif // CUSTOMDIALOG_H
