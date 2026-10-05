#include "announcementmanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QRandomGenerator>
#include <QDateTime>
#include <QJsonArray>
#include <algorithm>

AnnouncementManager::AnnouncementManager(QObject *parent)
    : QObject(parent)
    , m_rotationPos(0)
    , m_timer(new QTimer(this))
    , m_hideTimer(new QTimer(this))
    , m_globalEnabled(true)
    , m_intervalSeconds(8)
{
    connect(m_timer, &QTimer::timeout, this, &AnnouncementManager::onTick);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, &AnnouncementManager::onHideTimer);
}

void AnnouncementManager::start()
{
    rebuildRotation();
    m_timer->start(m_intervalSeconds * 1000);
    onTick();
}

void AnnouncementManager::addAnnouncement(const Announcement &announcement)
{
    Announcement a = announcement;
    if (a.id.isEmpty()) a.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    m_announcements.append(a);
    rebuildRotation();
    save();
    emit announcementsChanged();
}

void AnnouncementManager::updateAnnouncement(int index, const Announcement &announcement)
{
    if (index < 0 || index >= m_announcements.size()) return;
    Announcement a = announcement;
    a.id = m_announcements[index].id;
    m_announcements[index] = a;
    rebuildRotation();
    save();
    emit announcementsChanged();
}

void AnnouncementManager::removeAnnouncement(int index)
{
    if (index < 0 || index >= m_announcements.size()) return;
    m_announcements.removeAt(index);
    rebuildRotation();
    save();
    emit announcementsChanged();
}

void AnnouncementManager::setGlobalEnabled(bool enabled)
{
    m_globalEnabled = enabled;
    save();
}

void AnnouncementManager::setIntervalSeconds(int seconds)
{
    m_intervalSeconds = qMax(1, seconds);
    if (m_timer->isActive()) m_timer->start(m_intervalSeconds * 1000);
    save();
}


bool AnnouncementManager::isAnnouncementActiveNow(const Announcement &a) const
{
    if (!a.enabled) return false;
    if (!a.hasSchedule) return true;   

    int today = QDate::currentDate().dayOfWeek();   
    if (!a.days.isEmpty() && !a.days.contains(today)) {
        return false;
    }

    QTime now = QTime::currentTime();
    
    if (a.timeStart <= a.timeEnd) {
        return now >= a.timeStart && now < a.timeEnd;
    } else {
        return now >= a.timeStart || now < a.timeEnd;
    }
}

void AnnouncementManager::rebuildRotation()
{
    m_rotation.clear();
    for (int i = 0; i < m_announcements.size(); ++i) {
        if (!m_announcements[i].enabled) continue;
        if (!isAnnouncementActiveNow(m_announcements[i])) continue;
        
        int weight = qBound(1, m_announcements[i].priority, 5);
        for (int w = 0; w < weight; ++w) m_rotation.append(i);
    }
    std::shuffle(m_rotation.begin(), m_rotation.end(), *QRandomGenerator::global());
    m_rotationPos = 0;
}

void AnnouncementManager::onTick()
{
    if (!m_globalEnabled) return;

    if (m_rotationPos >= m_rotation.size()) {
        rebuildRotation();
    }

    if (m_rotation.isEmpty()) {
        emit hideAnnouncement();
        return;
    }

    int attempts = 0;
    while (attempts < m_rotation.size()) {
        int idx = m_rotation[m_rotationPos];
        m_rotationPos = (m_rotationPos + 1) % m_rotation.size();
        
        if (isAnnouncementActiveNow(m_announcements[idx])) {
            int duration = m_announcements[idx].displayDuration;
            emit showAnnouncement(m_announcements[idx].text, duration);
            
            if (duration > 0) {
                m_hideTimer->start(duration * 1000);
            } else {
                m_hideTimer->stop();  
            }
            return;
        }
        attempts++;
    }
    
    emit hideAnnouncement();
}

void AnnouncementManager::onHideTimer()
{
    emit hideAnnouncement();
}

QString AnnouncementManager::configPath() const
{
    QString dirPath = QDir::homePath() + "/.InformationBoard";
    QDir().mkpath(dirPath);
    return dirPath + "/announcements.json";
}

void AnnouncementManager::save()
{
    QJsonObject root;
    root["globalEnabled"] = m_globalEnabled;
    root["intervalSeconds"] = m_intervalSeconds;

    QJsonArray array;
    for (const Announcement &a : m_announcements) {
        QJsonObject obj;
        obj["id"] = a.id;
        obj["text"] = a.text;
        obj["priority"] = a.priority;
        obj["enabled"] = a.enabled;
        
        obj["displayDuration"] = a.displayDuration;
        obj["hasSchedule"] = a.hasSchedule;
        obj["timeStart"] = a.timeStart.toString("HH:mm");
        obj["timeEnd"] = a.timeEnd.toString("HH:mm");
        
        QJsonArray daysArray;
        for (int day : a.days) daysArray.append(day);
        obj["days"] = daysArray;
        
        array.append(obj);
    }
    root["announcements"] = array;

    QFile file(configPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void AnnouncementManager::load()
{
    QFile file(configPath());
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();
    m_globalEnabled = root.value("globalEnabled").toBool(true);
    m_intervalSeconds = root.value("intervalSeconds").toInt(8);

    m_announcements.clear();
    for (const auto &value : root.value("announcements").toArray()) {
        QJsonObject obj = value.toObject();
        Announcement a;
        a.id = obj.value("id").toString();
        a.text = obj.value("text").toString();
        a.priority = obj.value("priority").toInt(3);
        a.enabled = obj.value("enabled").toBool(true);
        
        // === НОВЫЕ ПОЛЯ ===
        a.displayDuration = obj.value("displayDuration").toInt(10);
        a.hasSchedule = obj.value("hasSchedule").toBool(false);
        a.timeStart = QTime::fromString(obj.value("timeStart").toString("00:00"), "HH:mm");
        a.timeEnd = QTime::fromString(obj.value("timeEnd").toString("23:59"), "HH:mm");
        
        for (const auto &dayValue : obj.value("days").toArray()) {
            a.days.insert(dayValue.toInt());
        }
        
        if (a.id.isEmpty()) a.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_announcements.append(a);
    }

    rebuildRotation();
    emit announcementsChanged();
}