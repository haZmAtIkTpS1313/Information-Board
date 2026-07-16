#include "slideshowmanager.h"
#include <QDir>

SlideshowManager::SlideshowManager(QObject *parent)
    : QObject(parent)
    , m_currentIndex(-1)
    , m_intervalSeconds(10)
    , m_enabled(false)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &SlideshowManager::onTick);
}

void SlideshowManager::setFolderPath(const QString &path)
{
    if (m_folderPath == path) return;
    m_folderPath = path;
    m_currentIndex = -1;
    rescan();

    if (m_enabled) {
        if (m_images.isEmpty()) {
            // Старая картинка из прежней папки больше не актуальна — явно
            // сообщаем "нечего показывать", а не оставляем её висеть вечно.
            emit imageChanged(QString());
        } else {
            onTick();   // сразу показать новую папку, не дожидаясь текущего интервала
        }
    }
}

void SlideshowManager::setIntervalSeconds(int seconds)
{
    m_intervalSeconds = qMax(2, seconds);
    if (m_timer->isActive()) m_timer->start(m_intervalSeconds * 1000);
}

void SlideshowManager::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (m_enabled) {
        start();
    } else {
        m_timer->stop();
    }
}

void SlideshowManager::rescan()
{
    m_images.clear();
    if (m_folderPath.isEmpty()) return;

    QDir dir(m_folderPath);
    if (!dir.exists()) return;

    QStringList filters = {"*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif"};
    for (const QString &fileName : dir.entryList(filters, QDir::Files, QDir::Name)) {
        m_images << dir.filePath(fileName);
    }
}

void SlideshowManager::start()
{
    if (!m_enabled) return;
    rescan();
    if (m_images.isEmpty()) return;

    m_timer->start(m_intervalSeconds * 1000);
    onTick();   // показать первую картинку сразу, не дожидаясь таймера
}

void SlideshowManager::onTick()
{
    if (m_images.isEmpty()) return;
    m_currentIndex = (m_currentIndex + 1) % m_images.size();
    emit imageChanged(m_images[m_currentIndex]);
}
