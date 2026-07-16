#ifndef ANNOUNCEMENTENTRYDIALOG_H
#define ANNOUNCEMENTENTRYDIALOG_H

#include <QDialog>
#include "announcementmanager.h"

class QPlainTextEdit;
class QSpinBox;
class QCheckBox;

class AnnouncementEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnouncementEntryDialog(QWidget *parent = nullptr);

    void setAnnouncement(const Announcement &announcement);
    Announcement announcement() const;

private:
    void setupUI();

    QPlainTextEdit *textEdit;
    QSpinBox *prioritySpin;
    QCheckBox *enabledCheck;

    QString m_id;
};

#endif // ANNOUNCEMENTENTRYDIALOG_H
