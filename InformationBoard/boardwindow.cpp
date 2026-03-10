#include "boardwindow.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

BoardWindow::BoardWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainLayout(nullptr)
    , clockTimer(nullptr)
    , dateTimer(nullptr)
    , weatherTimer(nullptr)
{
    setupUI();
    createDefaultElements();
    setupTimers();
}

BoardWindow::~BoardWindow()
{
}

void BoardWindow::setupUI()
{
    setWindowTitle("Информационная доска");
    setMinimumSize(800, 600);

    centralWidget = new QWidget(this);
    mainLayout = new QGridLayout(centralWidget);

    for (int i = 0; i < GRID_ROWS; ++i) {
        mainLayout->setRowStretch(i, 1);
    }
    for (int j = 0; j < GRID_COLS; ++j) {
        mainLayout->setColumnStretch(j, 1);
    }

    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    setCentralWidget(centralWidget);

    backgroundColor = QColor("#2c3e50");
    currentStyle = QString(
                       "QLabel {"
                       "   background-color: %1;"
                       "   color: white;"
                       "   font-size: 16px;"
                       "   font-family: 'Arial';"
                       "   border: 1px solid #3498db;"
                       "   border-radius: 5px;"
                       "   padding: 10px;"
                       "}"
                       ).arg(backgroundColor.name());

    setStyleSheet(currentStyle);
}

void BoardWindow::createDefaultElements()
{
    QLabel *logo = createElement("logo", "🏢 Логотип", 0, 0);
    logo->setAlignment(Qt::AlignCenter);

    QLabel *clock = createElement("clock", QTime::currentTime().toString("hh:mm:ss"), 0, 2);
    clock->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *date = createElement("date", QDate::currentDate().toString("dd.MM.yyyy"), 1, 2);
    date->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *content = createElement("content", "Доска готова к работе", 1, 1);
    content->setAlignment(Qt::AlignCenter);

    QLabel *weather = createElement("weather", "☀️ +22°C", 2, 2);
    weather->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    QLabel *uvIndex = createElement("uvIndex", "УФ: 3", 0, 1);
    uvIndex->setAlignment(Qt::AlignCenter);

    QLabel *airQuality = createElement("airQuality", "AQI: 50", 2, 1);
    airQuality->setAlignment(Qt::AlignCenter);

    QLabel *wind = createElement("wind", "3 м/с С", 3, 2);
    wind->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QLabel *precipitation = createElement("precipitation", "0 мм", 3, 1);
    precipitation->setAlignment(Qt::AlignCenter);

    QLabel *humidity = createElement("humidity", "45%", 4, 1);
    humidity->setAlignment(Qt::AlignCenter);

    QLabel *pressure = createElement("pressure", "1013 гПа", 4, 2);
    pressure->setAlignment(Qt::AlignCenter);

    QLabel *sunrise = createElement("sunrise", "Восход 06:30", 2, 0);
    sunrise->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *sunset = createElement("sunset", "Закат 20:30", 3, 0);
    sunset->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *ticker = createElement("ticker", "Бегущая строка: новости и объявления", 4, 0, 1, 4);
    ticker->setAlignment(Qt::AlignCenter);
}

QLabel* BoardWindow::createElement(const QString &name, const QString &defaultText,
                                   int row, int col, int rowSpan, int colSpan)
{
    QLabel *label = new QLabel(defaultText, centralWidget);
    label->setObjectName(name);
    label->setStyleSheet(currentStyle);

    elements[name] = label;

    BoardElementConfig config;
    config.visible = true;
    config.text = defaultText;
    config.style = currentStyle;
    config.position = QPoint(row, col);
    config.size = QSize(rowSpan, colSpan);
    elementConfigs[name] = config;

    mainLayout->addWidget(label, row, col, rowSpan, colSpan);

    return label;
}

void BoardWindow::clearAllElements()
{
    QList<QString> keys = elements.keys();
    for (const QString &key : keys)
    {
        QLabel *element = elements[key];
        mainLayout->removeWidget(element);
        element->deleteLater();
    }
    elements.clear();
    elementConfigs.clear();
}

void BoardWindow::setupTimers()
{
    clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, &BoardWindow::updateClock);
    clockTimer->start(1000);

    dateTimer = new QTimer(this);
    connect(dateTimer, &QTimer::timeout, this, &BoardWindow::updateDate);
    dateTimer->start(60000);

    weatherTimer = new QTimer(this);
    connect(weatherTimer, &QTimer::timeout, this, &BoardWindow::updateWeather);
    weatherTimer->start(30000);

    updateClock();
    updateDate();
    updateWeather();
}

void BoardWindow::updateClock()
{
    if (elements.contains("clock")) {
        QString timeStr = QTime::currentTime().toString("hh:mm:ss");
        elements["clock"]->setText(timeStr);
    }
}

void BoardWindow::updateDate()
{
    if (elements.contains("date")) {
        QString dateStr = QDate::currentDate().toString("dd.MM.yyyy (dddd)");
        elements["date"]->setText(dateStr);
    }
}

void BoardWindow::updateWeather()
{
    if (elements.contains("weather")) {
        static int weatherState = 0;
        QStringList weathers = {
            "☀️ +22°C Ясно",
            "⛅ +19°C Облачно",
            "☔ +15°C Дождь",
            "❄️ -5°C Снег"
        };

        weatherState = (weatherState + 1) % weathers.size();
        elements["weather"]->setText(weathers[weatherState]);
    }
}

void BoardWindow::showText(const QString &text)
{
    if (elements.contains("content")) {
        elements["content"]->setText(text);
    }
}

void BoardWindow::showPermanentText(const QString &text)
{
    if (elements.contains("content")) {
        elements["content"]->setText(text);
        elementConfigs["content"].text = text;
    }
}

void BoardWindow::clearScreen()
{
    if (elements.contains("content")) {
        elements["content"]->setText("");
    }
}

void BoardWindow::setLogo(const QString &imagePath, int row, int col)
{
    if (!elements.contains("logo")) {
        createElement("logo", "", row, col);
    }

    QLabel *logo = elements["logo"];

    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        logo->setPixmap(pixmap.scaled(200, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logo->setFixedSize(200, 100);
    } else {
        logo->setText("❌ Логотип не найден");
    }

    if (row != -1 && col != -1) {
        mainLayout->addWidget(logo, row, col, 1, 1);
        elementConfigs["logo"].position = QPoint(row, col);
    }
}

void BoardWindow::setClock(int row, int col, const QString &format)
{
    Q_UNUSED(format);

    if (!elements.contains("clock")) {
        createElement("clock", QTime::currentTime().toString("hh:mm:ss"), row, col);
    } else if (row != -1 && col != -1) {
        QLabel *clock = elements["clock"];
        mainLayout->addWidget(clock, row, col, 1, 1);
        elementConfigs["clock"].position = QPoint(row, col);
    }
}

void BoardWindow::setDate(int row, int col, const QString &format)
{
    Q_UNUSED(format);

    if (!elements.contains("date")) {
        createElement("date", QDate::currentDate().toString("dd.MM.yyyy"), row, col);
    } else if (row != -1 && col != -1) {
        QLabel *date = elements["date"];
        mainLayout->addWidget(date, row, col, 1, 1);
        elementConfigs["date"].position = QPoint(row, col);
    }
}

void BoardWindow::setWeather(const QString &weatherText, int row, int col)
{
    updateElementText("weather", weatherText);

    if (row != -1 && col != -1 && elements.contains("weather")) {
        QLabel *weather = elements["weather"];
        mainLayout->addWidget(weather, row, col, 1, 1);
        elementConfigs["weather"].position = QPoint(row, col);
    }
}

void BoardWindow::setTicker(const QString &text, int row, int col, int colSpan)
{
    updateElementText("ticker", text);

    if (row != -1 && col != -1 && elements.contains("ticker")) {
        QLabel *ticker = elements["ticker"];
        mainLayout->addWidget(ticker, row, col, 1, colSpan);
        elementConfigs["ticker"].position = QPoint(row, col);
        elementConfigs["ticker"].size = QSize(1, colSpan);
    }
}

void BoardWindow::setBackgroundColor(const QColor &color)
{
    backgroundColor = color;

    QString style = QString(
                        "QLabel {"
                        "   background-color: %1;"
                        "   color: white;"
                        "   font-size: 16px;"
                        "   font-family: 'Arial';"
                        "   border: 1px solid #3498db;"
                        "   border-radius: 5px;"
                        "   padding: 10px;"
                        "}"
                        ).arg(color.name());

    setStyleSheet(style);
    currentStyle = style;

    for (auto label : elements.values()) {
        label->setStyleSheet(style);
    }
}

void BoardWindow::setCustomStyle(const QString &style)
{
    currentStyle = style;

    for (auto label : elements.values()) {
        label->setStyleSheet(style);
    }
}

void BoardWindow::setElementStyle(const QString &elementName, const QString &style)
{
    if (elements.contains(elementName)) {
        elements[elementName]->setStyleSheet(style);
        elementConfigs[elementName].style = style;
    }
}

void BoardWindow::setElementPosition(const QString &elementName, int row, int col, int rowSpan, int colSpan)
{
    if (elements.contains(elementName)) {
        QLabel *element = elements[elementName];
        mainLayout->addWidget(element, row, col, rowSpan, colSpan);

        elementConfigs[elementName].position = QPoint(row, col);
        elementConfigs[elementName].size = QSize(rowSpan, colSpan);
    }
}

void BoardWindow::showElement(const QString &elementName, bool show)
{
    if (elements.contains(elementName)) {
        elements[elementName]->setVisible(show);
        elementConfigs[elementName].visible = show;
    }
}

void BoardWindow::hideElement(const QString &elementName)
{
    showElement(elementName, false);
}

void BoardWindow::resetToDefaults()
{
    clearAllElements();
    createDefaultElements();
    setupTimers();
}

void BoardWindow::saveConfiguration(const QString &filename)
{
    QJsonObject root;
    QJsonArray elementsArray;

    for (auto it = elementConfigs.begin(); it != elementConfigs.end(); ++it) {
        QJsonObject elemObj;
        elemObj["name"] = it.key();
        elemObj["visible"] = it.value().visible;
        elemObj["text"] = it.value().text;
        elemObj["style"] = it.value().style;
        elemObj["row"] = it.value().position.x();
        elemObj["col"] = it.value().position.y();
        elemObj["rowSpan"] = it.value().size.width();
        elemObj["colSpan"] = it.value().size.height();

        elementsArray.append(elemObj);
    }

    root["elements"] = elementsArray;
    root["backgroundColor"] = backgroundColor.name();

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
    }
}

void BoardWindow::loadConfiguration(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) return;

    QJsonObject root = doc.object();

    if (root.contains("backgroundColor")) {
        setBackgroundColor(QColor(root["backgroundColor"].toString()));
    }

    if (root.contains("elements") && root["elements"].isArray()) {
        QJsonArray elementsArray = root["elements"].toArray();

        for (const auto &elemValue : elementsArray) {
            QJsonObject elemObj = elemValue.toObject();
            QString name = elemObj["name"].toString();

            if (!elements.contains(name)) continue;

            if (elemObj.contains("visible")) {
                showElement(name, elemObj["visible"].toBool());
            }

            if (elemObj.contains("text")) {
                updateElementText(name, elemObj["text"].toString());
            }

            if (elemObj.contains("style")) {
                setElementStyle(name, elemObj["style"].toString());
            }

            if (elemObj.contains("row") && elemObj.contains("col")) {
                int row = elemObj["row"].toInt();
                int col = elemObj["col"].toInt();
                int rowSpan = elemObj.contains("rowSpan") ? elemObj["rowSpan"].toInt() : 1;
                int colSpan = elemObj.contains("colSpan") ? elemObj["colSpan"].toInt() : 1;

                setElementPosition(name, row, col, rowSpan, colSpan);
            }
        }
    }
}

void BoardWindow::applyLayout(const QList<ElementLayoutConfig> &layout)
{
    clearAllElements();

    for (const auto &config : layout) {
        if (!config.visible) continue;

        QLabel *element = createElement(
            config.elementType,
            config.defaultText,
            config.row,
            config.col,
            config.rowSpan,
            config.colSpan
            );

        if (element) {
            element->setAlignment(Qt::AlignCenter);

            if (config.elementType == "clock") {
                element->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else if (config.elementType == "date") {
                element->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else if (config.elementType == "weather") {
                element->setAlignment(Qt::AlignRight | Qt::AlignBottom);
            } else if (config.elementType == "wind") {
                element->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else if (config.elementType == "sunrise") {
                element->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            } else if (config.elementType == "sunset") {
                element->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            }
        }
    }

    setupTimers();
}

void BoardWindow::applyElementStyle(const QString &name, const QString &style)
{
    if (elements.contains(name)) {
        elements[name]->setStyleSheet(style);
    }
}

void BoardWindow::updateElementText(const QString &name, const QString &text)
{
    if (elements.contains(name)) {
        elements[name]->setText(text);
        elementConfigs[name].text = text;
    }
}

void BoardWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        if (isFullScreen()) {
            showNormal();
        }
        break;

    case Qt::Key_F11:
        if (isFullScreen()) {
            showNormal();
        } else {
            showFullScreen();
        }
        break;

    case Qt::Key_F5:
        updateClock();
        updateDate();
        updateWeather();
        break;

    case Qt::Key_F1:
        showPermanentText(
            "Управление доской\n\n"
            "F1 - эта справка\n"
            "F5 - обновить все\n"
            "F11 - полный экран\n"
            "ESC - выход из полноэкранного режима"
            );
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void BoardWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void BoardWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}
