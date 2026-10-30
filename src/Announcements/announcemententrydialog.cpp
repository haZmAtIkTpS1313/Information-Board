#include "announcemententrydialog.h"
#include "Utils/logger.h"
#include <QColorDialog>
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
#include <cerrno>
#include <qformlayout.h>
#include <qgroupbox.h>
#include <qobject.h>

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

    QGroupBox *styleGroup = new QGroupBox("Оформление текста", this);
    QFormLayout *styleLayout = new QFormLayout(styleGroup);

    fontCombo = new QFontComboBox(this);
    fontCombo->setCurrentFont(QFont("Arial"));
    styleLayout->addRow("Шрифт:", fontCombo);

    fontSizeSpin = new QSpinBox(this);
    fontSizeSpin->setRange(12, 120);
    fontSizeSpin->setValue(24);
    styleLayout->addRow("Размер:", fontSizeSpin);

    QHBoxLayout *colorLayout = new QHBoxLayout();
    
    m_textColor = QColor("#FFFFFF");
    textColorBtn = new QPushButton("Цвет текста", this);
    textColorBtn->setStyleSheet(QString("background-color: %1; color: black; border: 1px solid #ccc; border-radius: 4px;").arg(m_textColor.name()));
    connect(textColorBtn, &QPushButton::clicked, this, &AnnouncementEntryDialog::onPickTextColor);
    colorLayout->addWidget(textColorBtn);

    m_bgColor = QColor(0, 0, 0, 180); 
    bgColorBtn = new QPushButton("Цвет фона", this);
    bgColorBtn->setStyleSheet(QString("background-color: %1; color: white; border: 1px solid #ccc; border-radius: 4px;").arg(m_bgColor.name()));
    connect(bgColorBtn, &QPushButton::clicked, this, &AnnouncementEntryDialog::onPickBgColor);
    colorLayout->addWidget(bgColorBtn);

    styleLayout->addRow("Цвета:", colorLayout);
    mainLayout->addWidget(styleGroup);
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

    fontCombo->setCurrentFont(QFont(announcement.fontName));
    fontSizeSpin->setValue(announcement.fontSize);
    m_textColor = QColor(announcement.textColor);
    m_bgColor = QColor(announcement.bgColor);
    
    textColorBtn->setStyleSheet(QString("background-color: %1; color: black; border: 1px solid #ccc; border-radius: 4px;").arg(m_textColor.name()));
    bgColorBtn->setStyleSheet(QString("background-color: %1; color: white; border: 1px solid #ccc; border-radius: 4px;").arg(m_bgColor.name()));
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
    
    a.fontName = fontCombo->currentFont().family();
    a.fontSize = fontSizeSpin->value();
    a.textColor = m_textColor.name();
    a.bgColor = m_bgColor.name(); 

    return a;
}
void AnnouncementEntryDialog::onPickTextColor()
{
    QColor color = QColorDialog::getColor(m_textColor, this, "Выберите цвет текста");
    if (color.isValid()) {
        m_textColor = color;
        textColorBtn->setStyleSheet(QString("background-color: %1; color: black; border: 1px solid #ccc; border-radius: 4px;").arg(m_textColor.name()));
    }
}

void AnnouncementEntryDialog::onPickBgColor()
{
    QColor color = QColorDialog::getColor(m_bgColor, this, "Выберите цвет фона");
    if (color.isValid()) {
        m_bgColor = color;
        bgColorBtn->setStyleSheet(QString("background-color: %1; color: white; border: 1px solid #ccc; border-radius: 4px;").arg(m_bgColor.name()));
    }
}