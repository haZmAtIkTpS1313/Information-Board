#include "newsmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

NewsManager::NewsManager(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , updateTimer(new QTimer(this))
    , country("ru")
    , category("general")
    , isUpdating(false)
{
    connect(updateTimer, &QTimer::timeout, this, &NewsManager::onUpdateTimer);
}

NewsManager::~NewsManager()
{
}

void NewsManager::setApiKey(const QString &key)
{
    apiKey = key;
}

void NewsManager::setCountry(const QString &Country)
{
    country = Country;
}

void NewsManager::setCategory(const QString &Category)
{
    category = Category;
}

void NewsManager::startUpdates(int intervalMinutes)
{
    updateTimer->start(intervalMinutes * 60 * 1000);
    updateNow();
}

void NewsManager::stopUpdates()
{
    updateTimer->stop();
}

void NewsManager::updateNow()
{
    if (isUpdating) return;
    fetchNews();
}

void NewsManager::onUpdateTimer()
{
    updateNow();
}

void NewsManager::fetchNews()
{
    if (apiKey.isEmpty()) {
        emit errorOccurred("API ключ для новостей не установлен");
        return;
    }

    isUpdating = true;

    QUrl url("https://newsapi.org/v2/top-headlines");
    QUrlQuery query;
    query.addQueryItem("country", country);
    query.addQueryItem("category", category);
    query.addQueryItem("apiKey", apiKey);
    query.addQueryItem("pageSize", "5");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "InformationBoard/1.0");

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
        reply->deleteLater();
    });
}

void NewsManager::onReplyFinished(QNetworkReply *reply)
{
    isUpdating = false;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Ошибка сети: %1").arg(reply->errorString()));
        return;
    }

    QByteArray data = reply->readAll();

    qDebug() << "News API ответ:" << data.left(500);
    parseNewsData(data);
}

void NewsManager::parseNewsData(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        emit errorOccurred("Неверный формат данных новостей");
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("status") && obj["status"].toString() != "ok") {
        QString message = obj["message"].toString();
        emit errorOccurred(QString("Ошибка API новостей: %1").arg(message));
        return;
    }

    QList<NewsItem> newsList;

    if (obj.contains("articles")) {
        QJsonArray articles = obj["articles"].toArray();

        for (const auto &articleValue : articles) {
            QJsonObject article = articleValue.toObject();

            NewsItem item;
            item.title = article["title"].toString();
            item.description = article["description"].toString();
            item.link = article["url"].toString();
            item.pubDate = article["publishedAt"].toString();
            item.source = article["source"].toObject()["name"].toString();

            if (!item.title.isEmpty() && item.title != "[Removed]") {
                newsList.append(item);
            }
        }
    }

    latestNews = newsList;
    emit newsUpdated(newsList);

    qDebug() << "Новости обновлены, получено:" << newsList.size() << "статей";
}
