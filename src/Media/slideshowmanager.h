#ifndef SLIDESHOWMANAGER_H
#define SLIDESHOWMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

class SlideshowManager : public QObject
{
    Q_OBJECT

public:
    explicit SlideshowManager(QObject *parent = nullptr);

    void setFolderPath(const QString &path);
    void setIntervalSeconds(int seconds);
    void setEnabled(bool enabled);

    void start();
    bool hasImages() const { return !m_images.isEmpty(); }

signals:
    void imageChanged(const QString &imagePath);

private slots:
    void onTick();

private:
    void rescan();

    QString m_folderPath;
    QStringList m_images;
    int m_currentIndex;
    int m_intervalSeconds;
    bool m_enabled;

    QTimer *m_timer;
};

#endif // SLIDESHOWMANAGER_H
