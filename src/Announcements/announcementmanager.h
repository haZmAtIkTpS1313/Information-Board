#ifndef ANNOUNCEMENTMANAGER_H
#define ANNOUNCEMENTMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QTimer>

struct Announcement {
    QString id;
    QString text;
    int priority = 3;    // 1..5, чем выше — тем чаще показывается
    bool enabled = true;
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
    void showAnnouncement(const QString &text);
    void announcementsChanged();

private slots:
    void onTick();

private:
    QString configPath() const;
    void rebuildRotation();

    QList<Announcement> m_announcements;
    QList<int> m_rotation;   // индексы в m_announcements, взвешенные по приоритету и перемешанные
    int m_rotationPos;

    QTimer *m_timer;
    bool m_globalEnabled;
    int m_intervalSeconds;
};

#endif // ANNOUNCEMENTMANAGER_H
