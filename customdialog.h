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
    CustomDialog(const QStringList& options, QWidget* parent);
    QString selected_option() const;

private:
    QComboBox* m_combo_box;
};

#endif // CUSTOMDIALOG_H
