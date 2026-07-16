#include "boardwindow.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QMediaPlayer>
#include <QFontMetrics>

BoardWindow::BoardWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , clockTimer(nullptr)
    , dateTimer(nullptr)
    , tickerTimer(nullptr)
    , takeoverLabel(nullptr)
    , takeoverActive(false)
    , takeoverFillBoard(false)
    , emergencyBlinkTimer(new QTimer(this))
    , emergencyPlayer(new QMediaPlayer(this))
    , emergencyActive(false)
    , emergencyBlinkState(false)
{
    setupUI();
    createDefaultElements();
    setupTimers();

    // Плашка захвата экрана — не входит в elements/elementConfigs, поэтому её
    // не может перезаписать расписание, объявления, кнопки быстрого текста
    // и т.д. Используется и для показа таблицы из БД, и для экстренного режима.
    takeoverLabel = new QLabel(centralWidget);
    takeoverLabel->setAlignment(Qt::AlignCenter);
    takeoverLabel->setWordWrap(true);
    takeoverLabel->setStyleSheet(defaultTakeoverStyle());
    takeoverLabel->hide();

    connect(emergencyBlinkTimer, &QTimer::timeout, this, &BoardWindow::onEmergencyBlink);
    connect(emergencyPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && emergencyActive) {
            emergencyPlayer->play();   // зацикливаем сирену, пока режим активен
        }
    });
}

BoardWindow::~BoardWindow()
{
}

void BoardWindow::setupUI()
{
    setWindowTitle("Информационная доска");
    setMinimumSize(800, 600);

    centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #2c3e50;");
    setCentralWidget(centralWidget);

    backgroundColor = QColor("#2c3e50");
    // Плитки прозрачные и без рамок — доска выглядит единым полотном,
    // а не набором отдельных ячеек, хотя каждый элемент по-прежнему
    // самостоятельно позиционируется и перетаскивается в редакторе макета.
    currentStyle = QString(
                       "QLabel {"
                       "   background-color: transparent;"
                       "   color: white;"
                       "   font-size: 16px;"
                       "   font-family: 'Arial';"
                       "   border: none;"
                       "   padding: 8px;"
                       "}"
                       );

    setStyleSheet(currentStyle);
}

void BoardWindow::createDefaultElements()
{
    createElement("logo", "🏢 Логотип", 20, 20, 120, 80);
    createElement("clock", QTime::currentTime().toString("hh:mm:ss"), 660, 20, 120, 80);
    createElement("date", QDate::currentDate().toString("dd.MM.yyyy"), 660, 110, 120, 80);
    createElement("slideshow", "🖼️ Слайдшоу", 180, 20, 440, 120);
    createElement("content", "Доска готова к работе", 180, 150, 440, 120);
    createElement("weather", "☀️ +22°C", 660, 200, 120, 80);
    // Заготовки под данные, которые пока не получают реальных значений ни от одного API:
    // createElement("uvIndex", "УФ: 3", 20, 110, 100, 70);
    // createElement("airQuality", "AQI: 50", 20, 200, 120, 80);
    createElement("wind", "3 м/с С", 660, 290, 120, 80);
    // createElement("precipitation", "0 мм", 660, 380, 100, 70);
    createElement("humidity", "45%", 20, 380, 100, 70);
    createElement("pressure", "1013 гПа", 140, 380, 100, 70);
    // createElement("sunrise", "Восход 06:30", 20, 470, 100, 70);
    // createElement("sunset", "Закат 20:30", 140, 470, 100, 70);
    createElement("countdown", "Нет предстоящих событий", 20, 470, 240, 70);
    createElement("announcement", "Объявлений пока нет", 280, 470, 480, 70);
    createElement("ticker", "Бегущая строка: новости и объявления", 20, 560, 760, 60);

    tickerText = "Бегущая строка: новости и объявления";
}

QLabel* BoardWindow::createElement(const QString &name, const QString &defaultText,
                                   int x, int y, int width, int height)
{
    QLabel *label = new QLabel(defaultText, centralWidget);
    label->setObjectName(name);
    label->setGeometry(x, y, width, height);
    label->setStyleSheet(currentStyle);
    label->setAlignment(Qt::AlignCenter);
    label->show();

    elements[name] = label;

    ElementConfig config;
    config.type = name;
    config.displayName = name;
    config.defaultText = defaultText;
    config.x = x;
    config.y = y;
    config.width = width;
    config.height = height;
    config.visible = true;
    elementConfigs[name] = config;

    return label;
}

void BoardWindow::clearAllElements()
{
    QList<QString> keys = elements.keys();
    for (const QString &key : keys) {
        QLabel *element = elements[key];
        element->deleteLater();
    }
    elements.clear();
    elementConfigs.clear();
}

void BoardWindow::setupTimers()
{
    // Таймеры создаются один раз за время жизни окна, чтобы не плодить дубликаты
    // при повторных вызовах (resetToDefaults / applyLayout).
    if (!clockTimer) {
        clockTimer = new QTimer(this);
        connect(clockTimer, &QTimer::timeout, this, &BoardWindow::updateClock);
        clockTimer->start(1000);
    }
    if (!dateTimer) {
        dateTimer = new QTimer(this);
        connect(dateTimer, &QTimer::timeout, this, &BoardWindow::updateDate);
        dateTimer->start(60000);
    }
    if (!tickerTimer) {
        tickerTimer = new QTimer(this);
        connect(tickerTimer, &QTimer::timeout, this, &BoardWindow::updateTicker);
        tickerTimer->start(250);   // скорость прокрутки бегущей строки
    }

    updateClock();
    updateDate();
}

void BoardWindow::updateClock()
{
    if (elements.contains("clock")) {
        elements["clock"]->setText(QTime::currentTime().toString("hh:mm:ss"));
    }
}

void BoardWindow::updateDate()
{
    if (elements.contains("date")) {
        elements["date"]->setText(QDate::currentDate().toString("dd.MM.yyyy (dddd)"));
    }
}

void BoardWindow::updateTicker()
{
    if (!elements.contains("ticker")) return;
    if (tickerText.trimmed().isEmpty()) return;

    // Прокрутка: первый символ уходит в конец
    tickerText = tickerText.mid(1) + tickerText.left(1);

    // Показываем «окно» текста фиксированной ширины, чтобы строка выглядела бегущей
    const int windowSize = 60;
    QString padded = tickerText;
    if (padded.length() < windowSize)
        padded = padded.leftJustified(windowSize, ' ');
    elements["ticker"]->setText(padded.left(windowSize));
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
        elementConfigs["content"].defaultText = text;
    }
}

void BoardWindow::clearScreen()
{
    // Экстренный режим отключается только своей кнопкой — случайный клик по
    // "Очистить доску" не должен тихо снимать активную тревогу.
    if (takeoverActive && !emergencyActive) {
        exitTakeoverMode();
    }
    if (elements.contains("content")) {
        elements["content"]->setText("");
    }
}

void BoardWindow::setElementText(const QString &elementName, const QString &text)
{
    updateElementText(elementName, text);
    if (elementName == "ticker") {
        tickerText = text;   // обновляем источник для анимации
    }
}

void BoardWindow::setLogo(const QString &imagePath, int x, int y, int width, int height)
{
    if (!elements.contains("logo")) {
        createElement("logo", "", x, y, width, height);
    }
    QLabel *logo = elements["logo"];

    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        logo->setPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logo->setFixedSize(width, height);
        logo->setText("");
    } else {
        logo->setText("❌ Логотип не найден");
    }

    if (x != -1 && y != -1) {
        logo->setGeometry(x, y, width, height);
        elementConfigs["logo"].x = x;
        elementConfigs["logo"].y = y;
        elementConfigs["logo"].width = width;
        elementConfigs["logo"].height = height;
    }
}

void BoardWindow::setSlideshowImage(const QString &imagePath)
{
    if (!elements.contains("slideshow")) return;   // элемент не добавлен в текущий макет

    QLabel *label = elements["slideshow"];
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setText("");
    } else {
        label->setText("❌ Изображение не найдено");
    }
}

void BoardWindow::setClock(int x, int y, int width, int height)
{
    if (!elements.contains("clock")) {
        createElement("clock", QTime::currentTime().toString("hh:mm:ss"), x, y, width, height);
    } else {
        QLabel *clock = elements["clock"];
        clock->setGeometry(x, y, width, height);
        elementConfigs["clock"].x = x;
        elementConfigs["clock"].y = y;
        elementConfigs["clock"].width = width;
        elementConfigs["clock"].height = height;
    }
}

void BoardWindow::setDate(int x, int y, int width, int height)
{
    if (!elements.contains("date")) {
        createElement("date", QDate::currentDate().toString("dd.MM.yyyy"), x, y, width, height);
    } else {
        QLabel *date = elements["date"];
        date->setGeometry(x, y, width, height);
        elementConfigs["date"].x = x;
        elementConfigs["date"].y = y;
        elementConfigs["date"].width = width;
        elementConfigs["date"].height = height;
    }
}

void BoardWindow::setWeather(const QString &weatherText, int x, int y, int width, int height)
{
    updateElementText("weather", weatherText);
    if (elements.contains("weather")) {
        QLabel *weather = elements["weather"];
        weather->setGeometry(x, y, width, height);
        elementConfigs["weather"].x = x;
        elementConfigs["weather"].y = y;
        elementConfigs["weather"].width = width;
        elementConfigs["weather"].height = height;
    }
}

void BoardWindow::setTicker(const QString &text, int x, int y, int width, int height)
{
    tickerText = text;
    updateElementText("ticker", text);
    if (elements.contains("ticker")) {
        QLabel *ticker = elements["ticker"];
        ticker->setGeometry(x, y, width, height);
        elementConfigs["ticker"].x = x;
        elementConfigs["ticker"].y = y;
        elementConfigs["ticker"].width = width;
        elementConfigs["ticker"].height = height;
    }
}

void BoardWindow::applySharedStyle(const QString &style)
{
    // Плитки с собственным индивидуальным стилем (elementConfigs[name].style —
    // выставляется через ПКМ "Свойства элемента" в редакторе или погодным
    // предупреждением) не должны затираться общим стилем доски. Раньше это
    // происходило при каждой смене цвета фона/шрифта и при каждой вспышке
    // экстренного режима (дважды в секунду), из-за чего индивидуальное
    // оформление незаметно слетало почти сразу после того, как его задали.
    for (auto it = elements.begin(); it != elements.end(); ++it) {
        const QString &name = it.key();
        if (elementConfigs.contains(name) && !elementConfigs[name].style.isEmpty()) {
            it.value()->setStyleSheet(elementConfigs[name].style);
        } else {
            it.value()->setStyleSheet(style);
        }
    }
}

void BoardWindow::setBackgroundColor(const QColor &color)
{
    backgroundColor = color;
    centralWidget->setStyleSheet(QString("QWidget { background-color: %1; }").arg(color.name()));
    QString style =
                        "QLabel {"
                        "   background-color: transparent;"
                        "   color: white;"
                        "   font-size: 16px;"
                        "   font-family: 'Arial';"
                        "   border: none;"
                        "   padding: 8px;"
                        "}";
    setStyleSheet(style);
    currentStyle = style;
    applySharedStyle(style);
}

void BoardWindow::setCustomStyle(const QString &style)
{
    currentStyle = style;
    applySharedStyle(style);
}

void BoardWindow::setElementStyle(const QString &elementName, const QString &style)
{
    if (elements.contains(elementName)) {
        elements[elementName]->setStyleSheet(style);
        elementConfigs[elementName].style = style;
    }
}

void BoardWindow::setElementPosition(const QString &elementName, int x, int y, int width, int height)
{
    if (elements.contains(elementName)) {
        QLabel *element = elements[elementName];
        element->setGeometry(x, y, width, height);
        elementConfigs[elementName].x = x;
        elementConfigs[elementName].y = y;
        elementConfigs[elementName].width = width;
        elementConfigs[elementName].height = height;
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
    // Смена макета под активным захватом экрана (тревога или показ таблицы)
    // недопустима — elements будет полностью пересоздан, а preTakeoverVisibility
    // ссылался бы на уже несуществующие плитки.
    if (emergencyActive) setEmergencyMode(false);
    else if (takeoverActive) exitTakeoverMode();
    clearAllElements();
    createDefaultElements();
    setupTimers();   // теперь идемпотентно — новые таймеры не создаются
}

void BoardWindow::saveConfiguration(const QString &filename)
{
    QJsonObject root;
    QJsonArray elementsArray;

    for (auto it = elementConfigs.begin(); it != elementConfigs.end(); ++it) {
        QJsonObject elemObj;
        elemObj["type"] = it.value().type;
        elemObj["displayName"] = it.value().displayName;
        elemObj["defaultText"] = it.value().defaultText;
        elemObj["x"] = it.value().x;
        elemObj["y"] = it.value().y;
        elemObj["width"] = it.value().width;
        elemObj["height"] = it.value().height;
        elemObj["visible"] = it.value().visible;
        if (!it.value().style.isEmpty()) elemObj["style"] = it.value().style;
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
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) return;

    QJsonObject root = doc.object();

    if (root.contains("backgroundColor")) {
        setBackgroundColor(QColor(root["backgroundColor"].toString()));
    }

    if (root.contains("elements") && root["elements"].isArray()) {
        QJsonArray elementsArray = root["elements"].toArray();
        // Та же защита, что и в applyLayout/resetToDefaults — нельзя пересоздавать
        // elements, пока активен захват экрана (тревога или показ таблицы).
        if (emergencyActive) setEmergencyMode(false);
        else if (takeoverActive) exitTakeoverMode();
        clearAllElements();

        for (const auto &elemValue : elementsArray) {
            QJsonObject elemObj = elemValue.toObject();
            QString type = elemObj["type"].toString();
            QString defaultText = elemObj["defaultText"].toString();
            int x = elemObj["x"].toInt();
            int y = elemObj["y"].toInt();
            int w = elemObj["width"].toInt();
            int h = elemObj["height"].toInt();
            bool visible = elemObj["visible"].toBool();
            QString style = elemObj["style"].toString();

            createElement(type, defaultText, x, y, w, h);
            if (!style.isEmpty()) setElementStyle(type, style);
            showElement(type, visible);
        }

        if (elementConfigs.contains("ticker"))
            tickerText = elementConfigs["ticker"].defaultText;

        setupTimers();   // восстанавливаем работу таймеров после перезагрузки элементов
    }
}

void BoardWindow::applyLayout(const QList<ElementConfig> &layout)
{
    if (emergencyActive) setEmergencyMode(false);   // смена макета под активным захватом экрана недопустима
    else if (takeoverActive) exitTakeoverMode();
    clearAllElements();

    for (const auto &cfg : layout) {
        QLabel *element = createElement(cfg.type, cfg.defaultText, cfg.x, cfg.y, cfg.width, cfg.height);
        if (element) {
            if (cfg.type == "clock" || cfg.type == "date" || cfg.type == "wind") {
                element->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            } else if (cfg.type == "weather") {
                element->setAlignment(Qt::AlignRight | Qt::AlignBottom);
            } else if (cfg.type == "sunrise" || cfg.type == "sunset") {
                element->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            } else {
                element->setAlignment(Qt::AlignCenter);
            }
            if (!cfg.style.isEmpty()) setElementStyle(cfg.type, cfg.style);
            showElement(cfg.type, cfg.visible);
        }
    }

    if (elementConfigs.contains("ticker"))
        tickerText = elementConfigs["ticker"].defaultText;

    setupTimers();   // идемпотентно
}

QString BoardWindow::defaultTakeoverStyle() const
{
    return
        "QLabel {"
        "   background-color: rgba(0, 0, 0, 170);"
        "   color: white;"
        "   font-size: 28px;"
        "   font-weight: bold;"
        "   border: 4px solid white;"
        "   border-radius: 16px;"
        "   padding: 24px;"
        "}";
}

QFont BoardWindow::defaultTakeoverFont() const
{
    // Должен соответствовать font-size/font-weight из defaultTakeoverStyle() —
    // именно по этому шрифту считается размер плашки в updateTakeoverLabelGeometry().
    QFont font("Arial");
    font.setPixelSize(28);
    font.setBold(true);
    return font;
}

void BoardWindow::showTakeoverContent(const QString &text, const QString &style, Qt::Alignment alignment, const QFont &font, bool fillBoard)
{
    if (!takeoverActive) {
        // Запоминаем видимость каждой плитки, чтобы вернуть её как было —
        // сама плитка не удаляется, просто прячется на время захвата.
        preTakeoverVisibility.clear();
        for (auto it = elements.begin(); it != elements.end(); ++it) {
            preTakeoverVisibility[it.key()] = it.value()->isVisible();
            it.value()->setVisible(false);
        }
        takeoverActive = true;
    }

    takeoverFillBoard = fillBoard;
    takeoverLabel->setStyleSheet(style.isEmpty() ? defaultTakeoverStyle() : style);
    takeoverLabel->setFont(font.family().isEmpty() ? defaultTakeoverFont() : font);
    takeoverLabel->setAlignment(alignment);
    takeoverLabel->setText(text);
    updateTakeoverLabelGeometry();   // считает размер плашки по шрифту+тексту и центрирует (или на всю доску, если fillBoard)
    takeoverLabel->show();
    takeoverLabel->raise();   // остальные плитки скрыты, но на всякий случай — точно поверх всего
}

void BoardWindow::exitTakeoverMode()
{
    if (!takeoverActive) return;

    takeoverLabel->hide();
    for (auto it = preTakeoverVisibility.begin(); it != preTakeoverVisibility.end(); ++it) {
        if (elements.contains(it.key())) elements[it.key()]->setVisible(it.value());
    }
    preTakeoverVisibility.clear();
    takeoverActive = false;
}

void BoardWindow::setEmergencyMode(bool active, const QString &message, const QString &soundPath)
{
    if (active == emergencyActive) return;
    emergencyActive = active;

    if (active) {
        preEmergencyColor = backgroundColor;

        QString text = message.isEmpty() ? "⚠️ ЭКСТРЕННАЯ СИТУАЦИЯ ⚠️" : message;
        showTakeoverContent(text, defaultTakeoverStyle(), Qt::AlignCenter, defaultTakeoverFont());

        emergencyBlinkState = false;
        onEmergencyBlink();              // первая вспышка — сразу, не ждём таймер
        emergencyBlinkTimer->start(500);

        if (!soundPath.isEmpty() && QFile::exists(soundPath)) {
            emergencyPlayer->setMedia(QUrl::fromLocalFile(soundPath));
            emergencyPlayer->play();
        }
    } else {
        emergencyBlinkTimer->stop();
        emergencyPlayer->stop();
        exitTakeoverMode();
        setBackgroundColor(preEmergencyColor);
    }

    emit emergencyModeChanged(active);
}

void BoardWindow::onEmergencyBlink()
{
    emergencyBlinkState = !emergencyBlinkState;
    setBackgroundColor(emergencyBlinkState ? QColor(Qt::red) : QColor(Qt::black));
}

void BoardWindow::updateTakeoverLabelGeometry()
{
    if (takeoverFillBoard) {
        // Плашка растянута на всю доску (используется для таблицы из БД) —
        // без расчёта по содержимому, с небольшим отступом от краёв доски.
        const int margin = qMax(20, int(centralWidget->width() * 0.02));
        takeoverLabel->setGeometry(margin, margin,
                                    centralWidget->width() - margin * 2,
                                    centralWidget->height() - margin * 2);
        return;
    }

    // Размер плашки считаем по содержимому (самая широкая строка × число строк
    // в текущем шрифте), а не растягиваем на весь экран — центрируем на доске,
    // в пределах разумных минимума/максимума.
    QFontMetrics fm(takeoverLabel->font());
    QStringList lines = takeoverLabel->text().split('\n');
    int textWidth = 0;
    for (const QString &line : lines) {
        textWidth = qMax(textWidth, fm.horizontalAdvance(line));
    }
    int textHeight = fm.lineSpacing() * qMax(1, lines.size());

    const int padding = 80;   // отступы внутри плашки (см. padding в стиле) + запас
    int desiredWidth = textWidth + padding;
    int desiredHeight = textHeight + padding;

    int maxWidth = qMax(200, int(centralWidget->width() * 0.92));
    int maxHeight = qMax(120, int(centralWidget->height() * 0.92));
    int minWidth = qMin(maxWidth, qMax(300, int(centralWidget->width() * 0.3)));
    int minHeight = qMin(maxHeight, 120);

    int width = qBound(minWidth, desiredWidth, maxWidth);
    int height = qBound(minHeight, desiredHeight, maxHeight);

    int x = (centralWidget->width() - width) / 2;
    int y = (centralWidget->height() - height) / 2;

    takeoverLabel->setGeometry(x, y, width, height);
}

void BoardWindow::updateElementText(const QString &name, const QString &text)
{
    if (elements.contains(name)) {
        elements[name]->setText(text);
        elementConfigs[name].defaultText = text;
    }
}

void BoardWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        if (isFullScreen()) showNormal();
        break;
    case Qt::Key_F11:
        isFullScreen() ? showNormal() : showFullScreen();
        break;
    case Qt::Key_F5:
        updateClock(); updateDate();
        break;
    case Qt::Key_F1:
        showPermanentText(
            "Управление доской\n\n"
            "F1 - эта справка\nF5 - обновить время\nF11 - полный экран\nESC - выход из полноэкранного режима"
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
    // Плашка захвата экрана считает размер от centralWidget один раз при
    // включении — без этого при переключении в полноэкранный режим (F11) во
    // время активного захвата (тревога/показ таблицы) она осталась бы
    // прежнего маленького размера.
    if (takeoverActive) updateTakeoverLabelGeometry();
}

// closeEvent(QCloseEvent*) был объявлен выше, но лишь звал реализацию
// QMainWindow по умолчанию (accept() без изменений) — убран.

