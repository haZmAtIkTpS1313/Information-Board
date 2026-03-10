#ifndef LIBRARYSTRUCTUR_H
#define LIBRARYSTRUCTUR_H

#include <QString>
#include <QDateTime>
#include <QList>

struct WeatherTemperature {
    double current;
    double feelsLike;
    double min;
    double max;
    double morning;
    double day;
    double evening;
    double night;
    QString unit;

    WeatherTemperature() : current(0), feelsLike(0), min(0), max(0),
        morning(0), day(0), evening(0), night(0), unit("C") {}

    QString getTemperatureColor() const {
        if (current < -10) return "#3498db";
        if (current < 0) return "#5DADE2";
        if (current < 10) return "#85C1E2";
        if (current < 20) return "#F7DC6F";
        if (current < 30) return "#F5B041";
        return "#E74C3C";
    }

    QString getFeelingText() const {
        if (current < -20) return "Арктический холод";
        if (current < -10) return "Очень холодно";
        if (current < 0) return "Холодно";
        if (current < 10) return "Прохладно";
        if (current < 20) return "Тепло";
        if (current < 30) return "Жарко";
        return "Очень жарко";
    }
};

struct WeatherCondition {
    int id;
    QString main;
    QString description;
    QString icon;

    bool isClear() const { return main == "Clear"; }
    bool isClouds() const { return main == "Clouds"; }
    bool isRain() const { return main == "Rain" || main == "Drizzle"; }
    bool isSnow() const { return main == "Snow"; }
    bool isThunderstorm() const { return main == "Thunderstorm"; }
    bool isFog() const { return main == "Mist" || main == "Fog" || main == "Haze"; }

    QString getEmoji() const {
        if (isClear()) return "☀️";
        if (isClouds()) {
            if (description.contains("few clouds")) return "🌤️";
            if (description.contains("scattered clouds")) return "⛅";
            if (description.contains("broken clouds")) return "🌥️";
            return "☁️";
        }
        if (isRain()) {
            if (description.contains("light rain")) return "🌦️";
            return "🌧️";
        }
        if (isSnow()) return "❄️";
        if (isThunderstorm()) return "⛈️";
        if (isFog()) return "🌫️";
        return "❓";
    }

    QString getWeatherColor() const {
        if (isClear()) return "#F39C12";
        if (isClouds()) return "#95A5A6";
        if (isRain()) return "#3498db";
        if (isSnow()) return "#ECF0F1";
        if (isThunderstorm()) return "#34495E";
        return "#BDC3C7";
    }
};

struct WeatherWind {
    double speed;
    double gust;
    int direction;
    QString directionText;
    double chill;
    QString unit;

    WeatherWind() : speed(0), gust(0), direction(0), chill(0), unit("m/s") {}

    QString getDirectionText() const {
        const QStringList directions = {"С", "ССВ", "СВ", "ВСВ", "В", "ВЮВ", "ЮВ", "ЮЮВ",
                                        "Ю", "ЮЮЗ", "ЮЗ", "ЗЮЗ", "З", "ЗСЗ", "СЗ", "ССЗ"};
        int index = static_cast<int>((direction + 11.25) / 22.5) % 16;
        return directions[index];
    }

    QString getBeaufortScale() const {
        if (speed < 0.3) return "Штиль";
        if (speed < 1.6) return "Тихий";
        if (speed < 3.4) return "Легкий";
        if (speed < 5.5) return "Слабый";
        if (speed < 8.0) return "Умеренный";
        if (speed < 10.8) return "Свежий";
        if (speed < 13.9) return "Сильный";
        if (speed < 17.2) return "Крепкий";
        if (speed < 20.8) return "Очень крепкий";
        if (speed < 24.5) return "Шторм";
        if (speed < 28.5) return "Сильный шторм";
        if (speed < 32.7) return "Жестокий шторм";
        return "Ураган";
    }
};

struct WeatherPrecipitation {
    QString type;
    double probability;
    double volume1h;
    double volume3h;
    double volume24h;
    double snowVolume1h;
    double snowVolume3h;
    QString unit;

    WeatherPrecipitation() : probability(0), volume1h(0), volume3h(0),
        volume24h(0), snowVolume1h(0), snowVolume3h(0),
        type("none"), unit("mm") {}

    bool hasPrecipitation() const { return type != "none" && volume1h > 0; }
    bool isRain() const { return type == "rain"; }
    bool isSnow() const { return type == "snow"; }

    QString getIntensity() const {
        if (!hasPrecipitation()) return "Нет";
        if (volume1h < 0.5) return "Слабые";
        if (volume1h < 4.0) return "Умеренные";
        if (volume1h < 50.0) return "Сильные";
        return "Очень сильные";
    }
};

struct WeatherAstronomy {
    QDateTime sunrise;
    QDateTime sunset;
    QDateTime moonrise;
    QDateTime moonset;
    QString moonPhase;
    double moonIllumination;
    int daylightDuration;

    WeatherAstronomy() : moonIllumination(0), daylightDuration(0) {}

    bool isDay(const QDateTime &currentTime) const {
        return currentTime >= sunrise && currentTime <= sunset;
    }

    int secondsToSunset(const QDateTime &currentTime) const {
        if (currentTime < sunset) {
            return currentTime.secsTo(sunset);
        }
        return 0;
    }

    int secondsToSunrise(const QDateTime &currentTime) const {
        if (currentTime > sunset) {
            QDateTime nextSunrise = sunrise.addDays(1);
            return currentTime.secsTo(nextSunrise);
        }
        return 0;
    }

    QString getMoonPhaseEmoji() const {
        if (moonPhase.contains("New")) return "🌑";
        if (moonPhase.contains("Waxing Crescent")) return "🌒";
        if (moonPhase.contains("First Quarter")) return "🌓";
        if (moonPhase.contains("Waxing Gibbous")) return "🌔";
        if (moonPhase.contains("Full")) return "🌕";
        if (moonPhase.contains("Waning Gibbous")) return "🌖";
        if (moonPhase.contains("Last Quarter")) return "🌗";
        if (moonPhase.contains("Waning Crescent")) return "🌘";
        return "🌑";
    }
};

struct WeatherCurrent {
    QDateTime observationTime;
    WeatherTemperature temperature;
    WeatherCondition condition;
    WeatherWind wind;
    WeatherPrecipitation precipitation;
    WeatherAstronomy astronomy;

    double pressure;
    double humidity;
    double visibility;
    double uvIndex;
    double dewPoint;

    QString pressureUnit;
    QString visibilityUnit;

    WeatherCurrent() : pressure(0), humidity(0), visibility(0),
        uvIndex(0), dewPoint(0), pressureUnit("hPa") {}

    QString getComfortLevel() const {
        if (temperature.current > 30 && humidity > 70) return "Душно";
        if (temperature.current > 30) return "Жарко";
        if (temperature.current < -20) return "Опасно холодно";
        if (temperature.current < -10) return "Очень холодно";
        if (wind.speed > 15) return "Ветрено";
        if (precipitation.hasPrecipitation()) return "Сыро";
        return "Комфортно";
    }
};

struct WeatherDailyForecast {
    QDateTime date;
    WeatherTemperature temperature;
    WeatherCondition condition;
    WeatherWind wind;
    WeatherPrecipitation precipitation;
    double pressure;
    double humidity;
    double uvIndex;
    WeatherAstronomy astronomy;
    int clouds;
    double popMax;

    WeatherDailyForecast() : pressure(0), humidity(0), uvIndex(0), clouds(0), popMax(0) {}

    QString getDaySummary() const {
        return QString("%1, %2°..%3°")
            .arg(condition.getEmoji())
            .arg(qRound(temperature.min))
            .arg(qRound(temperature.max));
    }
};

struct WeatherForecast {
    QList<WeatherDailyForecast> daily;
    int daysAvailable;
    QDateTime generatedTime;
    QString source;

    WeatherForecast() : daysAvailable(0) {}

    WeatherDailyForecast getDay(int index) const {
        if (index >= 0 && index < daily.size()) {
            return daily[index];
        }
        return WeatherDailyForecast();
    }

    double getMaxTemperature() const {
        double max = -100;
        for (const auto &day : daily) {
            if (day.temperature.max > max) max = day.temperature.max;
        }
        return max;
    }

    double getMinTemperature() const {
        double min = 100;
        for (const auto &day : daily) {
            if (day.temperature.min < min) min = day.temperature.min;
        }
        return min;
    }
};

struct WeatherUVIndex {
    double value;
    QString riskLevel;
    QString recommendation;
    QDateTime time;
    double maxUV;
    QDateTime maxUVTime;

    QString getRiskColor() const {
        if (value < 3) return "#3DBB5A";
        if (value < 6) return "#FDB813";
        if (value < 8) return "#F5821F";
        if (value < 11) return "#ED1C24";
        return "#A348A6";
    }

    QString getRiskText() const {
        if (value < 3) return "Низкий";
        if (value < 6) return "Средний";
        if (value < 8) return "Высокий";
        if (value < 11) return "Очень высокий";
        return "Экстремальный";
    }
};

struct WeatherAirQualityComponent {
    QString name;
    double value;
    QString unit;
    double standard;
    QString category;
};

struct WeatherAirQuality {
    int aqi;
    QString aqiCategory;
    QString aqiColor;
    QString aqiDescription;

    WeatherAirQualityComponent pm25;
    WeatherAirQualityComponent pm10;
    WeatherAirQualityComponent ozone;
    WeatherAirQualityComponent nitrogenDioxide;
    WeatherAirQualityComponent sulfurDioxide;
    WeatherAirQualityComponent carbonMonoxide;

    QList<WeatherAirQualityComponent> otherPollutants;
    QDateTime measurementTime;
    QString stationName;
    QString dominantPollutant;
    QString healthRecommendation;
    QString sensitiveGroupsRecommendation;

    QString getAqiColor() const {
        if (aqi <= 50) return "#00E400";
        if (aqi <= 100) return "#FFFF00";
        if (aqi <= 150) return "#FF7E00";
        if (aqi <= 200) return "#FF0000";
        if (aqi <= 300) return "#8F3F97";
        return "#7E0023";
    }

    QString getAqiCategoryText() const {
        if (aqi <= 50) return "Хорошее";
        if (aqi <= 100) return "Умеренное";
        if (aqi <= 150) return "Вредно для чувствительных групп";
        if (aqi <= 200) return "Вредное";
        if (aqi <= 300) return "Очень вредное";
        return "Опасное";
    }
};

struct WeatherAlert {
    QString eventName;
    QString eventType;
    QString severity;
    QString urgency;
    QString certainty;

    QDateTime startTime;
    QDateTime endTime;
    QDateTime issuedTime;

    QString description;
    QString instruction;
    QString headline;

    QStringList affectedAreas;
    QString areaDescription;

    QString source;
    QStringList parameters;

    QString getSeverityColor() const {
        if (severity.contains("Extreme", Qt::CaseInsensitive)) return "#FF0000";
        if (severity.contains("Severe", Qt::CaseInsensitive)) return "#FFA500";
        if (severity.contains("Moderate", Qt::CaseInsensitive)) return "#FFFF00";
        if (severity.contains("Minor", Qt::CaseInsensitive)) return "#00FF00";
        return "#808080";
    }

    QString getIconName() const {
        if (eventType.contains("Storm", Qt::CaseInsensitive)) return "storm";
        if (eventType.contains("Hurricane", Qt::CaseInsensitive)) return "hurricane";
        if (eventType.contains("Flood", Qt::CaseInsensitive)) return "flood";
        if (eventType.contains("Tornado", Qt::CaseInsensitive)) return "tornado";
        if (eventType.contains("Fog", Qt::CaseInsensitive)) return "fog";
        if (eventType.contains("Snow", Qt::CaseInsensitive)) return "snow";
        if (eventType.contains("Heat", Qt::CaseInsensitive)) return "heat";
        if (eventType.contains("Cold", Qt::CaseInsensitive)) return "cold";
        return "alert";
    }
};

struct WeatherCompleteData {
    WeatherCurrent current;
    WeatherForecast forecast;
    QString cityName;
    QString countryCode;
    double latitude;
    double longitude;
    QDateTime lastUpdate;
    bool isValid;
    QString errorMessage;

    WeatherCompleteData() : latitude(0), longitude(0), isValid(false) {}

    bool isFresh(int maxAgeMinutes = 30) const {
        if (!isValid) return false;
        return lastUpdate.secsTo(QDateTime::currentDateTime()) < maxAgeMinutes * 60;
    }
};

#endif
