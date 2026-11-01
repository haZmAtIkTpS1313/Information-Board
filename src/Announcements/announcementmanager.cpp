#include "announcementmanager.h"
#include "Utils/logger.h"
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
#include <qdir.h>
#include <qobject.h>

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
    LOG_INFO("Запуск менеджера объявлений...");
    rebuildRotation();
    m_timer->start(m_intervalSeconds * 1000);
    LOG_INFO(QString("Таймер запущен с интервалом %1 сек").arg(m_intervalSeconds));
    onTick();
}

void AnnouncementManager::addAnnouncement(const Announcement &announcement)
{
    Announcement a = announcement;
    if (a.id.isEmpty()) a.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    LOG_INFO(QString("Добавление нового объявления: \"%1\" (ID: %2)")
             .arg(a.text.left(40))
             .arg(a.id));
    
    m_announcements.append(a);
    rebuildRotation();
    save();
    emit announcementsChanged();
}

void AnnouncementManager::updateAnnouncement(int index, const Announcement &announcement)
{
    if (index < 0 || index >= m_announcements.size()) {
        LOG_WARNING(QString("Попытка обновить объявление с невалидным индексом: %1").arg(index));
        return;
    }
    
    Announcement a = announcement;
    a.id = m_announcements[index].id;
    
    LOG_INFO(QString("Обновление объявления #%1: \"%2\"")
             .arg(index)
             .arg(a.text.left(40)));
    
    m_announcements[index] = a;
    rebuildRotation();
    save();
    emit announcementsChanged();
}

void AnnouncementManager::removeAnnouncement(int index)
{
    if (index < 0 || index >= m_announcements.size()) {
        LOG_WARNING(QString("Попытка удалить объявление с невалидным индексом: %1").arg(index));
        return;
    }
    
    LOG_INFO(QString("Удаление объявления #%1: \"%2\"")
             .arg(index)
             .arg(m_announcements[index].text.left(40)));
    
    m_announcements.removeAt(index);
    rebuildRotation();
    save();
    emit announcementsChanged();
}

void AnnouncementManager::setGlobalEnabled(bool enabled)
{
    LOG_INFO(QString("Глобальное состояние ротации: %1")
             .arg(enabled ? "ВКЛЮЧЕНО" : "ВЫКЛЮЧЕНО"));
    
    m_globalEnabled = enabled;
    save();
}

void AnnouncementManager::setIntervalSeconds(int seconds)
{
    int newInterval = qMax(1, seconds);
    
    LOG_INFO(QString("Изменение интервала ротации: %1 сек -> %2 сек")
             .arg(m_intervalSeconds)
             .arg(newInterval));
    
    m_intervalSeconds = newInterval;
    if (m_timer->isActive()) {
        m_timer->start(m_intervalSeconds * 1000);
        LOG_DEBUG("Таймер перезапущен с новым интервалом");
    }
    save();
}


bool AnnouncementManager::isAnnouncementActiveNow(const Announcement &a) const
{

    if (!a.enabled) 
    {
        LOG_DEBUG(QString("Объявление \"%1\" сейчас не активно").arg(a.text.left(40)));
        return false;
    }

    if (!a.hasSchedule)
    {
        LOG_DEBUG(QString("Объявление \"%1\"не имеет расписания").arg(a.text.left(40)));
        return true;
    }   

    int today = QDate::currentDate().dayOfWeek();   
    if (!a.days.isEmpty() && !a.days.contains(today)) {
        LOG_DEBUG(QString("Объявление \"%1\" не активно (сегодня не тот день недели)")
                  .arg(a.text.left(40)));
        return false;
    }

    QTime now = QTime::currentTime();
    
    bool inTimeRange = false;
    if (a.timeStart <= a.timeEnd) {
        inTimeRange = now >= a.timeStart && now < a.timeEnd;
    } else {
        inTimeRange = now >= a.timeStart || now < a.timeEnd;
    }
    
    if (!inTimeRange) {
        LOG_DEBUG(QString("Объявление \"%1\" не активно (вне времени: сейчас %2, диапазон %3-%4)")
                  .arg(a.text.left(40))
                  .arg(now.toString("HH:mm"))
                  .arg(a.timeStart.toString("HH:mm"))
                  .arg(a.timeEnd.toString("HH:mm")));
        return false;
    }
    
    LOG_DEBUG(QString("Объявление \"%1\" АКТИВНО (день: %2, время: %3)")
              .arg(a.text.left(40))
              .arg(today)
              .arg(now.toString("HH:mm")));
    return true;
}

void AnnouncementManager::rebuildRotation()
{
    LOG_INFO("Перестроение ротации объявлений");

    m_rotation.clear();

    int skippedDisabled = 0;
    int skippedInactive = 0;

    for (int i = 0; i < m_announcements.size(); ++i) {
        if (!m_announcements[i].enabled) continue;
        if (!isAnnouncementActiveNow(m_announcements[i])) continue;
        
        int weight = qBound(1, m_announcements[i].priority, 5);
        for (int w = 0; w < weight; ++w) m_rotation.append(i);

        LOG_DEBUG(QString("Объявлегие \"%1\" добавлено в ротации с весом %2")
            .arg(m_announcements[i].text.left(40))
            .arg(weight));
    }
    std::shuffle(m_rotation.begin(), m_rotation.end(), *QRandomGenerator::global());
    m_rotationPos = 0;

    LOG_INFO(QString("Ротация перестроена. Итоговый размер: %1 (пропущено: %2 неактивных, %3 выключенных)")
             .arg(m_rotation.size())
             .arg(skippedInactive)
             .arg(skippedDisabled));
}

void AnnouncementManager::onTick()
{
    if (!m_globalEnabled) {
        LOG_DEBUG("Ротация объявлений глобально отключена, пропускаем тик");
        return;
    }

    if (m_rotationPos >= m_rotation.size()) {
        LOG_INFO("Достигнут конец ротации, перестраиваем список");
        rebuildRotation();
    }

    if (m_rotation.isEmpty()) {
        LOG_DEBUG("ция пуста (нет активных объявлений), скрываем доску");
        emit hideAnnouncement();
        return;
    }

    int attempts = 0;
    while (attempts < m_rotation.size()) {
        int idx = m_rotation[m_rotationPos];
        m_rotationPos = (m_rotationPos + 1) % m_rotation.size();
        
        if (isAnnouncementActiveNow(m_announcements[idx])) {
            int duration = m_announcements[idx].displayDuration;
            LOG_INFO(QString("Поках объявлений: \"%1\" (приоритет: %2, длительность: 3% сек)")
                .arg(m_announcements[idx].text.left(50))
                .arg(m_announcements[idx].priority)
                .arg(duration));
            emit showAnnouncement(m_announcements[idx]);
            if (duration > 0) {
                m_hideTimer->start(duration * 1000);
            } else {
                m_hideTimer->stop();  
            }
            return;
        }
        attempts++;
    }
    
    LOG_DEBUG("Ни одно объявление не активно в текущее время, скрываем доску");
    emit hideAnnouncement();
}

void AnnouncementManager::onHideTimer()
{
    LOG_DEBUG("Таймер скрытия объявления сработал, отправляем сигнал hideAnnouncement");
    emit hideAnnouncement();
}

QString AnnouncementManager::configPath() const
{
    LOG_DEBUG("Получение пути к конфигурации объявлений");
    QString dirPath = QDir::homePath() + "/.InformationBoard";
    if(!QDir(dirPath).exists()){
        LOG_INFO(QString("СОздание директории: %1").arg(dirPath));

        if(!QDir().mkpath(dirPath)) {
            LOG_ERROR(QString("Не удалось создать директорию: %1").arg(dirPath));
        } else {
            LOG_INFO("Директория успешно создага");
        }
    }

    QString fullPath = dirPath + "/announcements.json";
    LOG_DEBUG(QString("Путь к конфигурации: %1").arg(fullPath));

    return fullPath;
}

void AnnouncementManager::save()
{
    LOG_INFO("Сохранение списка объявлений"); 
    
    QJsonArray arr;
    for (const auto &a : m_announcements) {
        QJsonObject obj;
        obj["id"] = a.id;
        obj["text"] = a.text;
        obj["priority"] = a.priority;
        obj["enabled"] = a.enabled;
        obj["displayDuration"] = a.displayDuration;
        obj["hasSchedule"] = a.hasSchedule;
        if (a.hasSchedule) {
            obj["timeStart"] = a.timeStart.toString("HH:mm");
            obj["timeEnd"] = a.timeEnd.toString("HH:mm");
            QJsonArray daysArr;
            for (int d : a.days) daysArr.append(d);
            obj["days"] = daysArr;
        }
        obj["fontName"] = a.fontName;
        obj["fontSize"] = a.fontSize;
        obj["textColor"] = a.textColor;
        obj["bgColor"] = a.bgColor;
        
        arr.append(obj);
    }

    QJsonDocument doc(arr);
    QFile file(configPath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        LOG_INFO("Объявления успешно сохранены"); 
    } else {
        LOG_ERROR(QString("Не удалось сохранить объявления в %1").arg(configPath())); 
    }

}

void AnnouncementManager::load()
{
    LOG_INFO("Загрузка списка объявлений");

    QFile file(configPath());
    if (!file.exists()) {
        LOG_INFO("Файл объявления не найден, используем пустой список");
        return;
    }

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

        a.displayDuration = obj.value("displayDuration").toInt(10);
        a.hasSchedule = obj.value("hasSchedule").toBool(false);
        a.timeStart = QTime::fromString(obj.value("timeStart").toString("00:00"), "HH:mm");
        a.timeEnd = QTime::fromString(obj.value("timeEnd").toString("23:59"), "HH:mm");

        a.fontName = obj.value("fontName").toString("Arial");
        a.fontSize = obj.value("fontSize").toInt(24);
        a.textColor = obj.value("textColor").toString("#ffffff");
        a.bgColor = obj.value("bgColor").toString("rgba(0,0,0,180)");
        for (const auto &dayValue : obj.value("days").toArray()) {
            a.days.insert(dayValue.toInt());
        }
        
        if (a.id.isEmpty()) a.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_announcements.append(a);
    }

    rebuildRotation();
    emit announcementsChanged();
}