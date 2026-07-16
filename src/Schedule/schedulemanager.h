#ifndef SCHEDULEMANAGER_H
#define SCHEDULEMANAGER_H

#include <QObject>
#include <QTime>
#include <QSet>
#include <QList>
#include <QMap>
#include <QString>
#include <QDateTime>
#include <QTimer>

class QMediaPlayer;

struct ScheduleEntry {
    QString id;
    QString name;
    QTime time;
    QSet<int> days;      // Qt::DayOfWeek: 1 = понедельник ... 7 = воскресенье
    QString soundPath;
    QString message;
    QString designPath;  // необязательный .design-профиль, который доска применит при срабатывании
    int volume = 80;     // 0..100
    bool enabled = true;
};

class ScheduleManager : public QObject
{
    Q_OBJECT

public:
    explicit ScheduleManager(QObject *parent = nullptr);
    ~ScheduleManager();

    void start();

    const QList<ScheduleEntry>& entries() const { return m_entries; }
    void addEntry(const ScheduleEntry &entry);
    void updateEntry(int index, const ScheduleEntry &entry);
    void removeEntry(int index);

    bool globalEnabled() const { return m_globalEnabled; }
    void setGlobalEnabled(bool enabled);

    bool quietHoursEnabled() const { return m_quietHoursEnabled; }
    QTime quietHoursStart() const { return m_quietHoursStart; }
    QTime quietHoursEnd() const { return m_quietHoursEnd; }
    void setQuietHours(bool enabled, const QTime &start, const QTime &end);
    bool isQuietHoursNow() const;

    void testEntry(int index);

    // Ближайшее по времени включённое событие (просматривает неделю вперёд)
    bool nextEvent(ScheduleEntry &outEntry, QDateTime &outWhen) const;

    void save();
    void load();

signals:
    void triggered(const ScheduleEntry &entry);
    void entriesChanged();

private slots:
    void onMinuteTimer();

private:
    QString configPath() const;
    void playSound(const QString &path, int volume);
    void fireEntry(const ScheduleEntry &entry);
    void scheduleNextTick();   // пересчитывает интервал до начала следующей минуты заново каждый раз — таймер не накапливает дрейф

    QList<ScheduleEntry> m_entries;
    QTimer *m_timer;
    QMediaPlayer *m_player;

    bool m_globalEnabled;
    bool m_quietHoursEnabled;
    QTime m_quietHoursStart;
    QTime m_quietHoursEnd;

    QMap<QString, QString> m_lastFiredMinute; // id -> "yyyy-MM-dd hh:mm"
};

#endif // SCHEDULEMANAGER_H
