
#ifndef NEWSMANAGER_H
#define NEWSMANAGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

struct NewsItem {
    QString title;
    QString description;
    QString link;
    QString pubDate;
    QString source;
};

class NewsManager : public QObject
{
    Q_OBJECT

public:
    explicit NewsManager(QObject *parent = nullptr);
    ~NewsManager();

    void setApiKey(const QString &key);
    void setCountry(const QString &country);
    void setCategory(const QString &category);
    void startUpdates(int intervalMinutes = 60);
    void stopUpdates();
    void updateNow();

    QList<NewsItem> getLatestNews() const { return latestNews; }

signals:
    void newsUpdated(const QList<NewsItem> &news);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onUpdateTimer();

private:
    void fetchNews();
    void parseNewsData(const QByteArray &data);

    QNetworkAccessManager *networkManager;
    QTimer *updateTimer;
    QString apiKey;
    QString country;
    QString category;
    QList<NewsItem> latestNews;
    bool isUpdating;
};

#endif
