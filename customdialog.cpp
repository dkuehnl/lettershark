#include "customdialog.h"

namespace {
const static QString dialogstyle = R"(
        /* 1) Dialog-Hintergrund */
        CustomDialog {
            background-color: #2b2b2b;
        }

        /* 2) Label stylen und zentrieren */
        CustomDialog QLabel#dialog_label {
            color: #e0e0e0;
            font-size: 12pt;
            qproperty-alignment: 'AlignCenter';         /* zentriert den Text im Label */
            margin-bottom: 12px;
            background-color: #2b2b2b;
        }

        /* 3) ComboBox dunkel + Text hell */
        CustomDialog QComboBox#dialog_combo {
            background-color: #3c3f41;
            color: #e0e0e0;
            border: 1px solid #555;
            padding: 4px 8px;
            min-width: 200px;
        }
        /* Aufgeklappt-Liste ebenfalls dunkel */
        CustomDialog QComboBox#dialog_combo QAbstractItemView {
            background-color: #3c3f41;
            selection-background-color: #5b5f61;
            color: #e0e0e0;
        }

        /* 4) ButtonBox */
        CustomDialog QDialogButtonBox QPushButton {
            background-color: #3c3f41;
            color: #e0e0e0;
            border: 1px solid #555;
            border-radius: 4px;
            padding: 6px 14px;
            min-width: 80px;
            margin: 0 6px;
        }
        CustomDialog QDialogButtonBox QPushButton:hover {
            background-color: #484a4c;
        }
        CustomDialog QDialogButtonBox QPushButton:pressed {
            background-color: #242526;
        }

        /* Optional: Fokus-Rahmen */
        CustomDialog QWidget:focus {
            outline: 1px solid #6a8caf;
        }
    )";
}

CustomDialog::CustomDialog(const QStringList& options, QWidget* parent = nullptr)
    : QDialog(parent) {
    setWindowTitle("Select Exportfile");

    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Please choose which file to export the results:" , this);
    label->setObjectName("dialog_label");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    m_combo_box = new QComboBox(this);
    m_combo_box->setObjectName("dialog_combo");
    m_combo_box->addItems(options);
    layout->addWidget(m_combo_box);
    layout->setAlignment(m_combo_box, Qt::AlignCenter);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(button_box, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(button_box);
    this->setStyleSheet(dialogstyle);
}

QString CustomDialog::selected_option() const {
    return m_combo_box->currentText();
}
