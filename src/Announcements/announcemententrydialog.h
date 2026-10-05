#ifndef ANNOUNCEMENTENTRYDIALOG_H
#define ANNOUNCEMENTENTRYDIALOG_H

#include <QDialog>
#include "announcementmanager.h"

class QPlainTextEdit;
class QSpinBox;
class QCheckBox;
class QTimeEdit;
class QGroupBox;

class AnnouncementEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnouncementEntryDialog(QWidget *parent = nullptr);

    void setAnnouncement(const Announcement &announcement);
    Announcement announcement() const;

private:
    void setupUI();
    void updateScheduleVisibility();  

    QPlainTextEdit *textEdit;
    QSpinBox *prioritySpin;
    QSpinBox *durationSpin;            
    QCheckBox *enabledCheck;
    
    
    QTimeEdit *timeStartEdit;
    QTimeEdit *timeEndEdit;
    QCheckBox *dayChecks[7];           
    QGroupBox *scheduleGroup;

    QString m_id;
};

#endif // ANNOUNCEMENTENTRYDIALOG_H