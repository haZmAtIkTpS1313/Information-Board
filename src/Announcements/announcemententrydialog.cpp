#include "announcemententrydialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>

AnnouncementEntryDialog::AnnouncementEntryDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void AnnouncementEntryDialog::setupUI()
{
    setWindowTitle("Объявление");
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(new QLabel("Текст объявления:", this));
    textEdit = new QPlainTextEdit(this);
    textEdit->setPlaceholderText("Например: Столовая работает до 15:00");
    textEdit->setMaximumHeight(100);
    mainLayout->addWidget(textEdit);

    QFormLayout *form = new QFormLayout();
    prioritySpin = new QSpinBox(this);
    prioritySpin->setRange(1, 5);
    prioritySpin->setValue(3);
    prioritySpin->setToolTip("1 — редко, 5 — часто");
    form->addRow("Приоритет (1-5):", prioritySpin);
    mainLayout->addLayout(form);

    enabledCheck = new QCheckBox("Активно", this);
    enabledCheck->setChecked(true);
    mainLayout->addWidget(enabledCheck);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void AnnouncementEntryDialog::setAnnouncement(const Announcement &announcement)
{
    m_id = announcement.id;
    textEdit->setPlainText(announcement.text);
    prioritySpin->setValue(announcement.priority);
    enabledCheck->setChecked(announcement.enabled);
}

Announcement AnnouncementEntryDialog::announcement() const
{
    Announcement a;
    a.id = m_id;
    a.text = textEdit->toPlainText();
    a.priority = prioritySpin->value();
    a.enabled = enabledCheck->isChecked();
    return a;
}
