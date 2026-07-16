#include "weathermanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

WeatherManager::WeatherManager(QObject *parent)
    : QObject(parent)
    , networkManager(new QNetworkAccessManager(this))
    , updateTimer(new QTimer(this))
    , isUpdating(false)
{
    connect(updateTimer, &QTimer::timeout, this, &WeatherManager::onUpdateTimer);
}

WeatherManager::~WeatherManager()
{
}

void WeatherManager::setApiKey(const QString &key)
{
    apiKey = key;
}

void WeatherManager::setCity(const QString &City)
{
    city = City;
}

void WeatherManager::startUpdates(int intervalMinutes)
{
    updateTimer->start(intervalMinutes * 60 * 1000);
    updateNow();
}

void WeatherManager::stopUpdates()
{
    updateTimer->stop();
}

void WeatherManager::updateNow()
{
    if (isUpdating) return;
    fetchWeather();
}

void WeatherManager::onUpdateTimer()
{
    updateNow();
}

void WeatherManager::fetchWeather()
{
    if (apiKey.isEmpty()) {
        emit errorOccurred("API ключ не установлен");
        return;
    }

    if (city.isEmpty()) {
        emit errorOccurred("Город не указан");
        return;
    }

    isUpdating = true;

    QUrl url("https://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;
    query.addQueryItem("q", city);
    query.addQueryItem("appid", apiKey);
    query.addQueryItem("units", "metric");
    query.addQueryItem("lang", "ru");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "InformationBoard/1.0");
    // Без таймаута зависшее соединение (недоступный сервер, файрвол, обрыв
    // сети без RST) никогда не пришлёт finished() — isUpdating останется
    // true навсегда, и обновления погоды молча прекратятся до перезапуска.
    request.setTransferTimeout(15000);

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
        reply->deleteLater();
    });
}

void WeatherManager::onReplyFinished(QNetworkReply *reply)
{
    isUpdating = false;

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Ошибка сети: %1").arg(reply->errorString()));
        return;
    }

    QByteArray data = reply->readAll();
    parseWeatherData(data);
}

void WeatherManager::parseWeatherData(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        emit errorOccurred("Неверный формат данных");
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("cod") && obj["cod"].toInt() != 200) {
        QString message = obj["message"].toString();
        emit errorOccurred(QString("Ошибка API: %1").arg(message));
        return;
    }

    WeatherData weather;
    weather.isValid = true;

    if (obj.contains("name")) {
        weather.cityName = obj["name"].toString();
    }

    if (obj.contains("main")) {
        QJsonObject main = obj["main"].toObject();
        weather.temperature = main["temp"].toDouble();
        weather.feelsLike = main["feels_like"].toDouble();
        weather.humidity = main["humidity"].toInt();
        weather.pressure = main["pressure"].toDouble();
    }

    if (obj.contains("wind")) {
        QJsonObject wind = obj["wind"].toObject();
        weather.windSpeed = wind["speed"].toDouble();
        weather.windDirection = wind["deg"].toInt();
    }

    if (obj.contains("weather")) {
        QJsonArray weatherArray = obj["weather"].toArray();
        if (!weatherArray.isEmpty()) {
            QJsonObject weatherObj = weatherArray[0].toObject();
            weather.description = weatherObj["description"].toString();
            weather.icon = weatherObj["icon"].toString();
        }
    }

    currentWeather = weather;
    emit weatherUpdated(weather);

    qDebug() << "Погода обновлена:" << weather.cityName << weather.temperature << "°C";
}
