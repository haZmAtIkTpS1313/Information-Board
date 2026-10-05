#ifndef ANNOUNCEMENTMANAGER_H
#define ANNOUNCEMENTMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>
#include <QTime>
#include <QSet>

struct Announcement {
    QString id;
    QString text;
    int priority = 3;        
    bool enabled = true;
    int displayDuration = 10;    
    bool hasSchedule = false;    
    QTime timeStart;             
    QTime timeEnd;               
    QSet<int> days;              
};

class AnnouncementManager : public QObject
{
    Q_OBJECT

public:
    explicit AnnouncementManager(QObject *parent = nullptr);

    void start();

    const QList<Announcement>& announcements() const { return m_announcements; }
    void addAnnouncement(const Announcement &announcement);
    void updateAnnouncement(int index, const Announcement &announcement);
    void removeAnnouncement(int index);

    bool globalEnabled() const { return m_globalEnabled; }
    void setGlobalEnabled(bool enabled);

    int intervalSeconds() const { return m_intervalSeconds; }
    void setIntervalSeconds(int seconds);

    void save();
    void load();

signals:
    void showAnnouncement(const QString &text, int durationSeconds);
    void hideAnnouncement();      
    void announcementsChanged();

private slots:
    void onTick();
    void onHideTimer();            

private:
    QString configPath() const;
    void rebuildRotation();
    bool isAnnouncementActiveNow(const Announcement &a) const;  

    QList<Announcement> m_announcements;
    QList<int> m_rotation;
    int m_rotationPos;

    QTimer *m_timer;
    QTimer *m_hideTimer;           
    bool m_globalEnabled;
    int m_intervalSeconds;
};

#endif // ANNOUNCEMENTMANAGER_H