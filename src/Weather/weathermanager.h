#ifndef WEATHERMANAGER_H
#define WEATHERMANAGER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QTimer>
#include <QtNetwork/qnetworkaccessmanager.h>

struct WeatherData {
    double temperature;
    double feelsLike;
    int humidity;
    double pressure;
    double windSpeed;
    int windDirection;
    QString description;
    QString icon;
    QString cityName;
    bool isValid;

    WeatherData() : temperature(0), feelsLike(0), humidity(0), pressure(0),
        windSpeed(0), windDirection(0), isValid(false) {}
};

class WeatherManager : public QObject
{
    Q_OBJECT

public:
    explicit WeatherManager(QObject *parent = nullptr);
    ~WeatherManager();

    void setApiKey(const QString &key);
    void setCity(const QString &city);
    void startUpdates(int intervalMinutes = 30);
    void stopUpdates();
    void updateNow();

    const WeatherData& getCurrentWeather() const { return currentWeather; }

signals:
    void weatherUpdated(const WeatherData &data);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onUpdateTimer();

private:
    void fetchWeather();
    void parseWeatherData(const QByteArray &data);

    QNetworkAccessManager *networkManager;
    QTimer *updateTimer;
    QString apiKey;
    QString city;
    WeatherData currentWeather;
    bool isUpdating;
};

#endif
