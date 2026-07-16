#include "schedulemanager.h"
#include <QMediaPlayer>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QDate>
#include <QDebug>

ScheduleManager::ScheduleManager(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_player(new QMediaPlayer(this))
    , m_globalEnabled(true)
    , m_quietHoursEnabled(false)
    , m_quietHoursStart(22, 0)
    , m_quietHoursEnd(7, 0)
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        onMinuteTimer();
        scheduleNextTick();   // каждый раз заново — без накопления дрейфа от setInterval(60000)
    });
}

ScheduleManager::~ScheduleManager()
{
}

void ScheduleManager::start()
{
    scheduleNextTick();
}

void ScheduleManager::scheduleNextTick()
{
    // Выравниваем каждый следующий тик на начало реальной минуты заново —
    // если бы мы один раз выставили period=60000, любая заминка в событийном
    // цикле, выход системы из сна или коррекция часов накапливались бы, и
    // тик мог бы навсегда "уехать" мимо точной минуты события — событие
    // расписания перестало бы срабатывать (onMinuteTimer сравнивает время
    // строгим равенством часа/минуты).
    QTime now = QTime::currentTime();
    int msToNextMinute = (60 - now.second()) * 1000 - now.msec();
    if (msToNextMinute <= 0) msToNextMinute = 60000;
    m_timer->start(msToNextMinute);
}

void ScheduleManager::addEntry(const ScheduleEntry &entry)
{
    ScheduleEntry e = entry;
    if (e.id.isEmpty()) e.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_entries.append(e);
    save();
    emit entriesChanged();
}

void ScheduleManager::updateEntry(int index, const ScheduleEntry &entry)
{
    if (index < 0 || index >= m_entries.size()) return;
    ScheduleEntry e = entry;
    e.id = m_entries[index].id;
    m_entries[index] = e;
    save();
    emit entriesChanged();
}

void ScheduleManager::removeEntry(int index)
{
    if (index < 0 || index >= m_entries.size()) return;
    m_lastFiredMinute.remove(m_entries[index].id);
    m_entries.removeAt(index);
    save();
    emit entriesChanged();
}

void ScheduleManager::setGlobalEnabled(bool enabled)
{
    m_globalEnabled = enabled;
    save();
}

void ScheduleManager::setQuietHours(bool enabled, const QTime &start, const QTime &end)
{
    m_quietHoursEnabled = enabled;
    m_quietHoursStart = start;
    m_quietHoursEnd = end;
    save();
}

bool ScheduleManager::isQuietHoursNow() const
{
    if (!m_quietHoursEnabled) return false;

    QTime now = QTime::currentTime();
    if (m_quietHoursStart <= m_quietHoursEnd) {
        return now >= m_quietHoursStart && now < m_quietHoursEnd;
    }
    // интервал через полночь, например 22:00 - 07:00
    return now >= m_quietHoursStart || now < m_quietHoursEnd;
}

void ScheduleManager::testEntry(int index)
{
    if (index < 0 || index >= m_entries.size()) return;
    fireEntry(m_entries[index]);
}

bool ScheduleManager::nextEvent(ScheduleEntry &outEntry, QDateTime &outWhen) const
{
    QDateTime now = QDateTime::currentDateTime();
    bool found = false;
    QDateTime best;
    ScheduleEntry bestEntry;

    for (int offset = 0; offset < 8; ++offset) {
        QDate date = now.date().addDays(offset);
        int dow = date.dayOfWeek();
        for (const ScheduleEntry &entry : m_entries) {
            if (!entry.enabled) continue;
            if (!entry.days.contains(dow)) continue;
            QDateTime candidate(date, entry.time);
            if (candidate <= now) continue;
            if (!found || candidate < best) {
                found = true;
                best = candidate;
                bestEntry = entry;
            }
        }
    }

    if (!found) return false;
    outEntry = bestEntry;
    outWhen = best;
    return true;
}

void ScheduleManager::onMinuteTimer()
{
    if (!m_globalEnabled) return;

    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();
    int currentDay = today.dayOfWeek();
    QString minuteKey = today.toString("yyyy-MM-dd") + " " + QString("%1:%2").arg(now.hour(), 2, 10, QChar('0')).arg(now.minute(), 2, 10, QChar('0'));

    for (const ScheduleEntry &entry : m_entries) {
        if (!entry.enabled) continue;
        if (!entry.days.contains(currentDay)) continue;
        if (entry.time.hour() != now.hour() || entry.time.minute() != now.minute()) continue;
        if (m_lastFiredMinute.value(entry.id) == minuteKey) continue;

        m_lastFiredMinute[entry.id] = minuteKey;
        fireEntry(entry);
    }
}

void ScheduleManager::fireEntry(const ScheduleEntry &entry)
{
    emit triggered(entry);

    if (!entry.soundPath.isEmpty() && !isQuietHoursNow()) {
        playSound(entry.soundPath, entry.volume);
    }
}

void ScheduleManager::playSound(const QString &path, int volume)
{
    if (path.isEmpty() || !QFile::exists(path)) {
        qDebug() << "ScheduleManager: файл звука не найден:" << path;
        return;
    }

    m_player->setMedia(QUrl::fromLocalFile(path));
    m_player->setVolume(qBound(0, volume, 100));
    m_player->play();
}

QString ScheduleManager::configPath() const
{
    QString dirPath = QDir::homePath() + "/.InformationBoard";
    QDir().mkpath(dirPath);
    return dirPath + "/schedule.json";
}

void ScheduleManager::save()
{
    QJsonObject root;
    root["globalEnabled"] = m_globalEnabled;
    root["quietHoursEnabled"] = m_quietHoursEnabled;
    root["quietHoursStart"] = m_quietHoursStart.toString("HH:mm");
    root["quietHoursEnd"] = m_quietHoursEnd.toString("HH:mm");

    QJsonArray entriesArray;
    for (const ScheduleEntry &entry : m_entries) {
        QJsonObject obj;
        obj["id"] = entry.id;
        obj["name"] = entry.name;
        obj["time"] = entry.time.toString("HH:mm");
        obj["soundPath"] = entry.soundPath;
        obj["message"] = entry.message;
        obj["designPath"] = entry.designPath;
        obj["volume"] = entry.volume;
        obj["enabled"] = entry.enabled;

        QJsonArray daysArray;
        for (int day : entry.days) daysArray.append(day);
        obj["days"] = daysArray;

        entriesArray.append(obj);
    }
    root["entries"] = entriesArray;

    QFile file(configPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void ScheduleManager::load()
{
    QFile file(configPath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();

    m_globalEnabled = root.value("globalEnabled").toBool(true);
    m_quietHoursEnabled = root.value("quietHoursEnabled").toBool(false);
    m_quietHoursStart = QTime::fromString(root.value("quietHoursStart").toString("22:00"), "HH:mm");
    m_quietHoursEnd = QTime::fromString(root.value("quietHoursEnd").toString("07:00"), "HH:mm");

    m_entries.clear();
    QJsonArray entriesArray = root.value("entries").toArray();
    for (const auto &value : entriesArray) {
        QJsonObject obj = value.toObject();
        ScheduleEntry entry;
        entry.id = obj.value("id").toString();
        entry.name = obj.value("name").toString();
        entry.time = QTime::fromString(obj.value("time").toString(), "HH:mm");
        entry.soundPath = obj.value("soundPath").toString();
        entry.message = obj.value("message").toString();
        entry.designPath = obj.value("designPath").toString();
        entry.volume = obj.value("volume").toInt(80);
        entry.enabled = obj.value("enabled").toBool(true);

        for (const auto &dayValue : obj.value("days").toArray())
            entry.days.insert(dayValue.toInt());

        if (entry.id.isEmpty()) entry.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_entries.append(entry);
    }

    emit entriesChanged();
}
