#include "announcemententrydialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QTimeEdit>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>

static const char *kDayShort[7] = {"Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"};

AnnouncementEntryDialog::AnnouncementEntryDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void AnnouncementEntryDialog::setupUI()
{
    setWindowTitle("Объявление");
    setMinimumWidth(450);

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
    
    durationSpin = new QSpinBox(this);
    durationSpin->setRange(0, 300);
    durationSpin->setValue(10);
    durationSpin->setSuffix(" сек");
    durationSpin->setToolTip("0 = висит до следующего объявления");
    form->addRow("Показывать:", durationSpin);
    
    mainLayout->addLayout(form);

    enabledCheck = new QCheckBox("Активно", this);
    enabledCheck->setChecked(true);
    mainLayout->addWidget(enabledCheck);

    scheduleGroup = new QGroupBox("Показывать по расписанию", this);
    scheduleGroup->setCheckable(true);
    scheduleGroup->setChecked(false);
    QVBoxLayout *scheduleLayout = new QVBoxLayout(scheduleGroup);
    
    connect(scheduleGroup, &QGroupBox::toggled, this, &AnnouncementEntryDialog::updateScheduleVisibility);
    
    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(new QLabel("С", this));
    timeStartEdit = new QTimeEdit(QTime(0, 0), this);
    timeStartEdit->setDisplayFormat("HH:mm");
    timeLayout->addWidget(timeStartEdit);
    timeLayout->addWidget(new QLabel("до", this));
    timeEndEdit = new QTimeEdit(QTime(23, 59), this);
    timeEndEdit->setDisplayFormat("HH:mm");
    timeLayout->addWidget(timeEndEdit);
    timeLayout->addStretch();
    scheduleLayout->addLayout(timeLayout);
    
    QLabel *daysLabel = new QLabel("Дни недели:", this);
    scheduleLayout->addWidget(daysLabel);
    
    QHBoxLayout *daysLayout = new QHBoxLayout();
    for (int i = 0; i < 7; ++i) {
        dayChecks[i] = new QCheckBox(kDayShort[i], this);
        dayChecks[i]->setChecked(true);
        daysLayout->addWidget(dayChecks[i]);
    }
    daysLayout->addStretch();
    scheduleLayout->addLayout(daysLayout);
    
    mainLayout->addWidget(scheduleGroup);
    updateScheduleVisibility();

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);
}

void AnnouncementEntryDialog::updateScheduleVisibility()
{
    bool enabled = scheduleGroup->isChecked();
    timeStartEdit->setEnabled(enabled);
    timeEndEdit->setEnabled(enabled);
    for (int i = 0; i < 7; ++i) {
        dayChecks[i]->setEnabled(enabled);
    }
}

void AnnouncementEntryDialog::setAnnouncement(const Announcement &announcement)
{
    m_id = announcement.id;
    textEdit->setPlainText(announcement.text);
    prioritySpin->setValue(announcement.priority);
    durationSpin->setValue(announcement.displayDuration);
    enabledCheck->setChecked(announcement.enabled);
    
    scheduleGroup->setChecked(announcement.hasSchedule);
    if (announcement.timeStart.isValid()) timeStartEdit->setTime(announcement.timeStart);
    if (announcement.timeEnd.isValid()) timeEndEdit->setTime(announcement.timeEnd);
    
    for (int i = 0; i < 7; ++i) {
        dayChecks[i]->setChecked(announcement.days.isEmpty() || announcement.days.contains(i + 1));
    }
    
    updateScheduleVisibility();
}

Announcement AnnouncementEntryDialog::announcement() const
{
    Announcement a;
    a.id = m_id;
    a.text = textEdit->toPlainText();
    a.priority = prioritySpin->value();
    a.displayDuration = durationSpin->value();
    a.enabled = enabledCheck->isChecked();
    
    a.hasSchedule = scheduleGroup->isChecked();
    if (a.hasSchedule) {
        a.timeStart = timeStartEdit->time();
        a.timeEnd = timeEndEdit->time();
        for (int i = 0; i < 7; ++i) {
            if (dayChecks[i]->isChecked()) {
                a.days.insert(i + 1);
            }
        }
    }
    
    return a;
}