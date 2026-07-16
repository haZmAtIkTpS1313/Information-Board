#ifndef SCHEDULEENTRYDIALOG_H
#define SCHEDULEENTRYDIALOG_H

#include <QDialog>
#include "schedulemanager.h"

class QLineEdit;
class QTimeEdit;
class QCheckBox;
class QSlider;
class QPlainTextEdit;

class ScheduleEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScheduleEntryDialog(QWidget *parent = nullptr);

    void setEntry(const ScheduleEntry &entry);
    ScheduleEntry entry() const;

private slots:
    void onBrowseSound();
    void onPlayPreview();
    void onBrowseDesign();

private:
    void setupUI();

    QLineEdit *nameEdit;
    QTimeEdit *timeEdit;
    QCheckBox *dayChecks[7];
    QLineEdit *soundPathEdit;
    QLineEdit *designPathEdit;
    QPlainTextEdit *messageEdit;
    QSlider *volumeSlider;
    QCheckBox *enabledCheck;

    QString m_id;
};

#endif // SCHEDULEENTRYDIALOG_H
