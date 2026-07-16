#include "announcementmanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QRandomGenerator>
#include <algorithm>

AnnouncementManager::AnnouncementManager(QObject *parent)
    : QObject(parent)
    , m_rotationPos(0)
    , m_timer(new QTimer(this))
    , m_globalEnabled(true)
    , m_intervalSeconds(8)
{
    connect(m_timer, &QTimer::timeout, this, &AnnouncementManager::onTick);
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

void AnnouncementManager::rebuildRotation()
{
    m_rotation.clear();
    for (int i = 0; i < m_announcements.size(); ++i) {
        if (!m_announcements[i].enabled) continue;
        int weight = qBound(1, m_announcements[i].priority, 5);
        for (int w = 0; w < weight; ++w) m_rotation.append(i);
    }
    std::shuffle(m_rotation.begin(), m_rotation.end(), *QRandomGenerator::global());
    m_rotationPos = 0;
}

void AnnouncementManager::onTick()
{
    if (!m_globalEnabled || m_rotation.isEmpty()) return;

    if (m_rotationPos >= m_rotation.size()) {
        rebuildRotation();
        if (m_rotation.isEmpty()) return;
    }

    int idx = m_rotation[m_rotationPos++];
    emit showAnnouncement(m_announcements[idx].text);
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
        if (a.id.isEmpty()) a.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_announcements.append(a);
    }

    rebuildRotation();
    emit announcementsChanged();
}
