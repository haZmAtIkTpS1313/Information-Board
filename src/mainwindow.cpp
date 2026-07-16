#include "mainwindow.h"
#include "Database/databaseemanager.h"
#include "Database/databasewidget.h"
#include "ui_mainwindow.h"
#include "Board/boardwindow.h"
#include "Board/boarddesignerwidget.h"
#include "Schedule/schedulewidget.h"
#include "Settings/settingsdialog.h"
#include "Announcements/announcementwidget.h"
#include "apikeys.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QVBoxLayout>
#include <QSettings>
#include <QDir>
#include <QUrl>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , boardWindow(nullptr)
    , designerWidget(nullptr)
    , dbManager(nullptr)
    , scheduleManager(nullptr)
    , announcementManager(nullptr)
    , slideshowManager(nullptr)
    , weatherManager(nullptr)
    , weatherUpdateTimer(nullptr)
    , newsUpdateTimer(nullptr)
    , countdownTimer(nullptr)
    , weatherWarningPlayer(nullptr)
    , weatherWarningActive(false)
{
    ui->setupUi(this);
    setWindowTitle("Консоль управления информационной доской");

    designerWidget = new BoardDesignerWidget(this);
    if (ui->tabWidget) {
        for (int i = 0; i < ui->tabWidget->count(); ++i) {
            if (ui->tabWidget->tabText(i) == "Дизайн") {
                QWidget *designTab = ui->tabWidget->widget(i);
                QLayout *existingLayout = designTab->layout();
                if (existingLayout) {
                    existingLayout->addWidget(designerWidget);
                } else {
                    QVBoxLayout *layout = new QVBoxLayout(designTab);
                    layout->addWidget(designerWidget);
                }
                break;
            }
        }
    }

    scheduleManager = new ScheduleManager(this);
    scheduleManager->load();
    if (ui->tabWidget) {
        for (int i = 0; i < ui->tabWidget->count(); ++i) {
            if (ui->tabWidget->tabText(i) == "Расписание") {
                QWidget *scheduleTab = ui->tabWidget->widget(i);
                ScheduleWidget *scheduleWidget = new ScheduleWidget(scheduleManager, scheduleTab);
                QVBoxLayout *layout = new QVBoxLayout(scheduleTab);
                layout->addWidget(scheduleWidget);
                break;
            }
        }
    }
    connect(scheduleManager, &ScheduleManager::triggered, this, &MainWindow::onScheduleTriggered);
    scheduleManager->start();

    announcementManager = new AnnouncementManager(this);
    announcementManager->load();
    if (ui->tabWidget) {
        for (int i = 0; i < ui->tabWidget->count(); ++i) {
            if (ui->tabWidget->tabText(i) == "Объявления") {
                QWidget *announcementTab = ui->tabWidget->widget(i);
                AnnouncementWidget *announcementWidget = new AnnouncementWidget(announcementManager, announcementTab);
                QVBoxLayout *layout = new QVBoxLayout(announcementTab);
                layout->addWidget(announcementWidget);
                break;
            }
        }
    }
    connect(announcementManager, &AnnouncementManager::showAnnouncement, this, &MainWindow::onAnnouncementShow);
    announcementManager->start();

    slideshowManager = new SlideshowManager(this);
    connect(slideshowManager, &SlideshowManager::imageChanged, this, &MainWindow::onSlideshowImageChanged);
    applySlideshowSettings();

    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::updateCountdown);
    countdownTimer->start(1000);
    updateCountdown();

    // База данных — храним менеджер как член класса (а не локально!)
    dbManager = new DatabaseManager(this);
    dbManager->initDatabase();

    databaseWidget = nullptr;
    if (ui->tabWidget) {
        for (int i = 0; i < ui->tabWidget->count(); ++i) {
            if (ui->tabWidget->tabText(i) == "База Данных") {
                QWidget *dbTab = ui->tabWidget->widget(i);
                databaseWidget = new DatabaseWidget(dbManager, dbTab);
                QVBoxLayout *layout = new QVBoxLayout(dbTab);
                layout->addWidget(databaseWidget);
                connect(databaseWidget, &DatabaseWidget::showTableRequested,
                        this, &MainWindow::onShowTable);
                break;
            }
        }
    }

    weatherManager = new WeatherManager(this);
    weatherWarningPlayer = new QMediaPlayer(this);

    setupConnections();
    statusBar()->showMessage("Готов к работе | Доска: не связана");

    connect(weatherManager, &WeatherManager::weatherUpdated,
            this, &MainWindow::onWeatherUpdated);
    connect(weatherManager, &WeatherManager::errorOccurred,
            this, [this](const QString &error) {
                statusBar()->showMessage("Ошибка погоды: " + error, 5000);
            });

    applyWeatherSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setBoardWindow(BoardWindow *window)
{
    boardWindow = window;
    if (boardWindow) {
        statusBar()->showMessage("Готов к работе | Доска: подключена");
        boardWindow->showPermanentText("Доска подключена к консоли управления");
        // Доска может выключить тревогу сама (смена макета/конфигурации во
        // время активного экстренного режима) — консоль должна узнать об
        // этом и обновить кнопку, а не полагаться только на свои клики.
        connect(boardWindow, &BoardWindow::emergencyModeChanged, this, &MainWindow::onEmergencyModeChanged);
    }
}

void MainWindow::setupConnections()
{
    // --- Управление окнами ---
    if (ui->showBoardButton)
        connect(ui->showBoardButton, &QPushButton::clicked, this, &MainWindow::handleShowBoardButtonClicked);
    if (ui->fullscreenButton)
        connect(ui->fullscreenButton, &QPushButton::clicked, this, &MainWindow::handleFullscreenButtonClicked);
    if (ui->alwaysOnTopCheck)
        connect(ui->alwaysOnTopCheck, &QCheckBox::toggled, this, &MainWindow::handleAlwaysOnTopCheckToggled);

    // --- Текст ---
    if (ui->showTextButton)
        connect(ui->showTextButton, &QPushButton::clicked, this, &MainWindow::handleShowTextButtonClicked);
    if (ui->clearBoardButton)
        connect(ui->clearBoardButton, &QPushButton::clicked, this, &MainWindow::handleClearBoardButtonClicked);

    // --- Размер / шрифт / стиль текста ---
    if (ui->applyTextSizeButton)
        connect(ui->applyTextSizeButton, &QPushButton::clicked, this, &MainWindow::updateBoardStyle);
    if (ui->applyFontButton)
        connect(ui->applyFontButton, &QPushButton::clicked, this, &MainWindow::updateBoardStyle);
    if (ui->applyFontStyleButton)
        connect(ui->applyFontStyleButton, &QPushButton::clicked, this, &MainWindow::updateBoardStyle);

    // --- Быстрые команды ---
    if (ui->quickWeatherButton)
        connect(ui->quickWeatherButton, &QPushButton::clicked, this, &MainWindow::handleQuickWeatherButtonClicked);
    if (ui->quickTimeButton)
        connect(ui->quickTimeButton, &QPushButton::clicked, this, &MainWindow::handleQuickTimeButtonClicked);
    if (ui->quickLogoButton)
        connect(ui->quickLogoButton, &QPushButton::clicked, this, &MainWindow::handleQuickLogoButtonClicked);

    // --- Цвет фона ---
    if (ui->setRedBackgroundButton)
        connect(ui->setRedBackgroundButton, &QPushButton::clicked, this, &MainWindow::handleSetRedBackgroundButtonClicked);
    if (ui->setBlueBackgroundButton)
        connect(ui->setBlueBackgroundButton, &QPushButton::clicked, this, &MainWindow::handleSetBlueBackgroundButtonClicked);
    if (ui->setGreenBackgroundButton)
        connect(ui->setGreenBackgroundButton, &QPushButton::clicked, this, &MainWindow::handleSetGreenBackgroundButtonClicked);
    if (ui->setBlackBackgroundButton)
        connect(ui->setBlackBackgroundButton, &QPushButton::clicked, this, &MainWindow::handleSetBlackBackgroundButtonClicked);
    if (ui->setCustomColorButton)
        connect(ui->setCustomColorButton, &QPushButton::clicked, this, &MainWindow::handleSetCustomColorButtonClicked);

    // --- Размер текста (радиокнопки) ---
    if (ui->textSmall)
        connect(ui->textSmall, &QRadioButton::toggled, this, &MainWindow::handleTextSmallToggled);
    if (ui->textMedium)
        connect(ui->textMedium, &QRadioButton::toggled, this, &MainWindow::handleTextMediumToggled);
    if (ui->textLarge)
        connect(ui->textLarge, &QRadioButton::toggled, this, &MainWindow::handleTextLargeToggled);
    if (ui->textHuge)
        connect(ui->textHuge, &QRadioButton::toggled, this, &MainWindow::handleTextHugeToggled);

    // --- Управление элементами ---
    if (ui->toggleLogoButton)
        connect(ui->toggleLogoButton, &QPushButton::clicked, this, &MainWindow::handleToggleLogoButtonClicked);
    // ИСПРАВЛЕНО: кнопка времени теперь переключает часы/дату, а не логотип
    if (ui->toggleTimeButton)
        connect(ui->toggleTimeButton, &QPushButton::clicked, this, &MainWindow::handleToggleClockButtonClicked);
    if (ui->toggleWeatherButton)
        connect(ui->toggleWeatherButton, &QPushButton::clicked, this, &MainWindow::handleToggleWeatherButtonClicked);
    if (ui->toggleTickerButton)
        connect(ui->toggleTickerButton, &QPushButton::clicked, this, &MainWindow::handleToggleTickerButtonClicked);
    if (ui->setLogoImageButton)
        connect(ui->setLogoImageButton, &QPushButton::clicked, this, &MainWindow::handleSetLogoImageButtonClicked);

    // --- Конфигурация ---
    if (ui->saveConfigButton)
        connect(ui->saveConfigButton, &QPushButton::clicked, this, &MainWindow::handleSaveConfigButtonClicked);
    if (ui->loadConfigButton)
        connect(ui->loadConfigButton, &QPushButton::clicked, this, &MainWindow::handleLoadConfigButtonClicked);
    if (ui->resetBoardButton)
        connect(ui->resetBoardButton, &QPushButton::clicked, this, &MainWindow::handleResetBoardButtonClicked);

    // --- Дизайн ---
    if (ui->applyDesignButton)
        connect(ui->applyDesignButton, &QPushButton::clicked, this, &MainWindow::handleApplyDesignButtonClicked);
    if (ui->resetDesignButton)
        connect(ui->resetDesignButton, &QPushButton::clicked, this, &MainWindow::handleResetDesignButtonClicked);
    if (ui->saveDesignButton)
        connect(ui->saveDesignButton, &QPushButton::clicked, this, &MainWindow::handleSaveDesignButtonClicked);
    if (ui->loadDesignButton)
        connect(ui->loadDesignButton, &QPushButton::clicked, this, &MainWindow::handleLoadDesignButtonClicked);
    // "Предпросмотр" убран из интерфейса — дублировал "Применить дизайн" один в один
    // (см. закомментированный MainWindow::on_previewDesignButton_clicked ниже).
    // if (ui->previewDesignButton_2)
    //     connect(ui->previewDesignButton_2, &QPushButton::clicked, this, &MainWindow::on_previewDesignButton_clicked);

    // --- База данных: DatabaseWidget встроен прямо во вкладку в конструкторе,
    // отдельной кнопки/окна больше нет ---

    // --- Настройки ---
    if (ui->settingsButton)
        connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::handleSettingsButtonClicked);

    // --- Экстренный режим ---
    if (ui->emergencyButton)
        connect(ui->emergencyButton, &QPushButton::clicked, this, &MainWindow::handleEmergencyButtonClicked);

    if (designerWidget) {
        connect(designerWidget, &BoardDesignerWidget::layoutChanged, this, [this]() {
            statusBar()->showMessage("Макет изменен. Нажмите 'Применить' для обновления доски", 3000);
        });
    }
}

void MainWindow::handleShowBoardButtonClicked()
{
    if (!boardWindow) {
        QMessageBox::warning(this, "Ошибка", "Окно доски не подключено!");
        return;
    }
    boardWindow->show();
    boardWindow->raise();
    boardWindow->activateWindow();
    statusBar()->showMessage("Окно доски поднято на передний план");
}

void MainWindow::handleFullscreenButtonClicked()
{
    if (!boardWindow) return;
    if (boardWindow->isFullScreen()) {
        boardWindow->showNormal();
        ui->fullscreenButton->setText("Полный экран");
        statusBar()->showMessage("Доска: оконный режим");
    } else {
        boardWindow->showFullScreen();
        ui->fullscreenButton->setText("Оконный режим");
        statusBar()->showMessage("Доска: полноэкранный режим");
    }
}

void MainWindow::handleAlwaysOnTopCheckToggled(bool checked)
{
    if (!boardWindow) return;
    Qt::WindowFlags flags = boardWindow->windowFlags();
    if (checked) {
        boardWindow->setWindowFlags(flags | Qt::WindowStaysOnTopHint);
        statusBar()->showMessage("Доска поверх всех окон");
    } else {
        boardWindow->setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
        statusBar()->showMessage("Доска в обычном режиме");
    }
    boardWindow->show();
}

void MainWindow::handleShowTextButtonClicked()
{
    if (!boardWindow) {
        QMessageBox::warning(this, "Ошибка", "Окно доски не подключено!");
        return;
    }
    QString text = ui->textInput ? ui->textInput->text() : "Тестовое сообщение";
    if (text.isEmpty()) text = "Введите текст для отображения";
    boardWindow->showPermanentText(text);
    statusBar()->showMessage("Текст отправлен на доску: " + text.left(20) + "...");
}

void MainWindow::handleClearBoardButtonClicked()
{
    if (boardWindow) {
        boardWindow->clearScreen();
        statusBar()->showMessage("Доска очищена");
    }
}

void MainWindow::handleQuickWeatherButtonClicked()
{
    if (!boardWindow) return;

    if (lastWeatherData.isValid) {
        QString detailedWeather = QString(
                                      "🌡️ ПОГОДА В %1\n\n"
                                      "🌡️ Температура: %2°C\n"
                                      "🤔 Ощущается как: %3°C\n"
                                      "💧 Влажность: %4%%\n"
                                      "🌬️ Ветер: %5 м/с\n"
                                      "📝 Описание: %6\n"
                                      "🔄 Обновлено: %7"
                                      ).arg(lastWeatherData.cityName)
                                      .arg(qRound(lastWeatherData.temperature))
                                      .arg(qRound(lastWeatherData.feelsLike))
                                      .arg(lastWeatherData.humidity)
                                      .arg(lastWeatherData.windSpeed)
                                      .arg(lastWeatherData.description)
                                      .arg(QTime::currentTime().toString("hh:mm:ss"));

        boardWindow->showPermanentText(detailedWeather);
        statusBar()->showMessage("Показана подробная погода", 2000);
    } else {
        boardWindow->showPermanentText("🌡️ ПОГОДА\n\nДанные загружаются...\nПожалуйста, подождите.");
        statusBar()->showMessage("Погода ещё не загружена", 2000);
    }
}

void MainWindow::handleQuickTimeButtonClicked()
{
    if (!boardWindow) return;
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString timeString = QString("🕐 ТЕКУЩЕЕ ВРЕМЯ\n\nДата: %1\nВремя: %2\nДень недели: %3")
                             .arg(date.toString("dd.MM.yyyy"))
                             .arg(time.toString("hh:mm:ss"))
                             .arg(date.toString("dddd"));
    boardWindow->showPermanentText(timeString);
    statusBar()->showMessage("Показано текущее время");
}

void MainWindow::handleQuickLogoButtonClicked()
{
    if (!boardWindow) return;
    boardWindow->showPermanentText(
        "🏢 НАША КОМПАНИЯ\n\n"
        "Информационные технологии\n"
        "Основано в 2024 году\n"
        "Мы создаем будущее"
        );
    statusBar()->showMessage("Показан логотип");
}

void MainWindow::handleSetRedBackgroundButtonClicked()
{
    if (boardWindow) boardWindow->setBackgroundColor(Qt::red);
    statusBar()->showMessage("Установлен красный фон");
}

void MainWindow::handleSetBlueBackgroundButtonClicked()
{
    if (boardWindow) boardWindow->setBackgroundColor(Qt::blue);
    statusBar()->showMessage("Установлен синий фон");
}

void MainWindow::handleSetGreenBackgroundButtonClicked()
{
    if (boardWindow) boardWindow->setBackgroundColor(Qt::green);
    statusBar()->showMessage("Установлен зеленый фон");
}

void MainWindow::handleSetBlackBackgroundButtonClicked()
{
    if (boardWindow) boardWindow->setBackgroundColor(Qt::black);
    statusBar()->showMessage("Установлен черный фон");
}

void MainWindow::handleSetCustomColorButtonClicked()
{
    if (!boardWindow) return;
    QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет фона");
    if (color.isValid()) {
        boardWindow->setBackgroundColor(color);
        statusBar()->showMessage("Установлен пользовательский цвет: " + color.name());
    }
}

// --- Быстрые пресеты размера текста ---
void MainWindow::applyTextSize(int size)
{
    if (ui->textSizeSpin) ui->textSizeSpin->setValue(size);
    updateBoardStyle();
}

void MainWindow::handleTextSmallToggled(bool checked)  { if (checked) applyTextSize(16); }
void MainWindow::handleTextMediumToggled(bool checked) { if (checked) applyTextSize(24); }
void MainWindow::handleTextLargeToggled(bool checked)  { if (checked) applyTextSize(36); }
void MainWindow::handleTextHugeToggled(bool checked)   { if (checked) applyTextSize(48); }

void MainWindow::updateBoardStyle()
{
    if (!boardWindow) return;

    int fontSize = ui->textSizeSpin ? ui->textSizeSpin->value() : 24;
    QString fontFamily = ui->fontCombo ? ui->fontCombo->currentFont().family() : "Arial";

    QString fontWeight = (ui->boldCheck && ui->boldCheck->isChecked()) ? "bold" : "normal";
    QString fontStyle = (ui->italicCheck && ui->italicCheck->isChecked()) ? "italic" : "normal";
    QString textDecoration = "";
    if (ui->underlineCheck && ui->underlineCheck->isChecked()) textDecoration += "underline ";
    if (ui->strikeoutCheck && ui->strikeoutCheck->isChecked()) textDecoration += "line-through";
    if (textDecoration.isEmpty()) textDecoration = "none";

    QString style = QString(
                        "QLabel {"
                        "   background-color: transparent;"
                        "   color: white;"
                        "   font-family: '%1';"
                        "   font-size: %2px;"
                        "   font-weight: %3;"
                        "   font-style: %4;"
                        "   text-decoration: %5;"
                        "   border: none;"
                        "   padding: 8px;"
                        "}"
                        ).arg(fontFamily)
                        .arg(fontSize)
                        .arg(fontWeight)
                        .arg(fontStyle)
                        .arg(textDecoration);

    boardWindow->setCustomStyle(style);
    statusBar()->showMessage("Стиль текста обновлён", 2000);
}

void MainWindow::handleToggleLogoButtonClicked()
{
    if (!boardWindow) return;
    static bool visible = true;
    visible = !visible;
    boardWindow->showElement("logo", visible);
    ui->toggleLogoButton->setText(visible ? "Скрыть логотип" : "Показать логотип");
}

void MainWindow::handleToggleClockButtonClicked()
{
    if (!boardWindow) return;
    static bool visible = true;
    visible = !visible;
    boardWindow->showElement("clock", visible);
    boardWindow->showElement("date", visible);
    if (ui->toggleTimeButton)
        ui->toggleTimeButton->setText(visible ? "Скрыть время" : "Показать время");
}

void MainWindow::handleToggleWeatherButtonClicked()
{
    if (!boardWindow) return;
    static bool visible = true;
    visible = !visible;
    boardWindow->showElement("weather", visible);
    ui->toggleWeatherButton->setText(visible ? "Скрыть погоду" : "Показать погоду");
}

void MainWindow::handleToggleTickerButtonClicked()
{
    if (!boardWindow) return;
    static bool visible = true;
    visible = !visible;
    boardWindow->showElement("ticker", visible);
    ui->toggleTickerButton->setText(visible ? "Скрыть бегущую строку" : "Показать бегущую строку");
}

void MainWindow::handleSetLogoImageButtonClicked()
{
    if (!boardWindow) return;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите изображение для логотипа", "",
                                                    "Изображения (*.png *.jpg *.bmp *.gif)");
    if (!fileName.isEmpty()) {
        boardWindow->setLogo(fileName);
        statusBar()->showMessage("Логотип обновлен: " + fileName);
    }
}

void MainWindow::handleSaveConfigButtonClicked()
{
    if (!boardWindow) return;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить конфигурацию доски", "board_config.json",
                                                    "JSON файлы (*.json)");
    if (!fileName.isEmpty()) {
        boardWindow->saveConfiguration(fileName);
        statusBar()->showMessage("Конфигурация сохранена");
    }
}

void MainWindow::handleLoadConfigButtonClicked()
{
    if (!boardWindow) return;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Загрузить конфигурацию доски", "",
                                                    "JSON файлы (*.json)");
    if (!fileName.isEmpty()) {
        boardWindow->loadConfiguration(fileName);
        statusBar()->showMessage("Конфигурация загружена");
    }
}

void MainWindow::handleResetBoardButtonClicked()
{
    if (!boardWindow) return;
    int reply = QMessageBox::question(this, "Сброс доски",
                                      "Вернуть все настройки доски к значениям по умолчанию?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        boardWindow->resetToDefaults();
        statusBar()->showMessage("Доска сброшена к настройкам по умолчанию");
    }
}

void MainWindow::handleApplyDesignButtonClicked()
{
    if (!boardWindow || !designerWidget) {
        QMessageBox::warning(this, "Ошибка", "Доска или дизайнер не инициализированы!");
        return;
    }
    applyDesignToBoard();
    statusBar()->showMessage("Дизайн применен к главной доске", 3000);
}

void MainWindow::handleResetDesignButtonClicked()
{
    if (!designerWidget) return;
    int reply = QMessageBox::question(this, "Сброс дизайна",
                                      "Сбросить макет к настройкам по умолчанию?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        designerWidget->resetToDefaultLayout();
        statusBar()->showMessage("Макет сброшен к настройкам по умолчанию", 3000);
    }
}

void MainWindow::handleSaveDesignButtonClicked()
{
    if (!designerWidget) return;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить дизайн доски", "design_",
                                                    "Design files (*.design)");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".design")) fileName += ".design";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream stream(&file);
        QList<ElementConfig> layout = designerWidget->getCurrentLayout();
        stream << layout.size();
        for (const auto &cfg : layout) {
            stream << cfg.type << cfg.displayName << cfg.defaultText
                   << cfg.x << cfg.y << cfg.width << cfg.height
                   << cfg.visible << cfg.style;
        }
        file.close();
        statusBar()->showMessage("Дизайн сохранен: " + fileName, 3000);
    }
}

void MainWindow::handleLoadDesignButtonClicked()
{
    if (!designerWidget) return;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Загрузить дизайн доски", "",
                                                    "Design files (*.design)");
    if (fileName.isEmpty()) return;

    QList<ElementConfig> layout;
    if (loadDesignFromFile(fileName, layout)) {
        designerWidget->applyLayout(layout);
        statusBar()->showMessage("Дизайн загружен: " + fileName, 3000);
    }
}

bool MainWindow::loadDesignFromFile(const QString &fileName, QList<ElementConfig> &layout)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QDataStream stream(&file);
    int count;
    stream >> count;
    for (int i = 0; i < count; ++i) {
        ElementConfig cfg;
        stream >> cfg.type >> cfg.displayName >> cfg.defaultText
            >> cfg.x >> cfg.y >> cfg.width >> cfg.height
            >> cfg.visible >> cfg.style;
        layout.append(cfg);
    }
    file.close();
    return true;
}

// Кнопка "Предпросмотр" убрана из интерфейса: делала ровно то же самое, что
// "Применить дизайн" (applyDesignToBoard), только с другой подписью в статус-баре.
// void MainWindow::on_previewDesignButton_clicked()
// {
//     if (!boardWindow || !designerWidget) return;
//     applyDesignToBoard();
//     statusBar()->showMessage("Предпросмотр дизайна на главной доске", 2000);
// }

void MainWindow::applyDesignToBoard()
{
    if (!boardWindow || !designerWidget) return;
    QList<ElementConfig> layout = designerWidget->getCurrentLayout();
    boardWindow->applyLayout(layout);
    statusBar()->showMessage(QString("Применён дизайн с %1 элементами").arg(layout.size()), 2000);
}

void MainWindow::onWeatherUpdated(const WeatherData &weather)
{
    if (!boardWindow) return;

    QString weatherCompact = QString("%1 %2°C\n%3")
                                 .arg(getWeatherEmoji(weather.description))
                                 .arg(qRound(weather.temperature))
                                 .arg(weather.description);

    boardWindow->setWeather(weatherCompact);

    // Живые метео-плитки на доске из реальных данных
    boardWindow->setElementText("humidity", QString("%1%").arg(weather.humidity));
    boardWindow->setElementText("pressure", QString("%1 гПа").arg(qRound(weather.pressure)));
    boardWindow->setElementText("wind", QString("%1 м/с").arg(weather.windSpeed, 0, 'f', 1));

    lastWeatherData = weather;
    checkWeatherWarnings(weather);

    statusBar()->showMessage(QString("Погода обновлена: %1°C, %2")
                                 .arg(qRound(weather.temperature))
                                 .arg(weather.description), 3000);
}

void MainWindow::checkWeatherWarnings(const WeatherData &weather)
{
    if (!boardWindow) return;

    QStringList warnings;
    if (weather.description.contains("гроза", Qt::CaseInsensitive))
        warnings << "гроза";
    if (weather.windSpeed >= 15.0)
        warnings << "сильный ветер";
    if (weather.temperature <= -15.0)
        warnings << "сильный мороз";

    bool wasActive = weatherWarningActive;
    weatherWarningActive = !warnings.isEmpty();

    if (weatherWarningActive) {
        boardWindow->setElementStyle("weather",
            "QLabel {"
            "   background-color: #c0392b;"
            "   color: white;"
            "   font-weight: bold;"
            "   border: 2px solid #922b21;"
            "   border-radius: 5px;"
            "   padding: 10px;"
            "}");

        if (!wasActive) {
            statusBar()->showMessage("⚠️ Погодное предупреждение: " + warnings.join(", "), 8000);

            QSettings settings(QDir::homePath() + "/.InformationBoard/settings.ini", QSettings::IniFormat);
            QString soundPath = settings.value("weather/warningSoundPath").toString();
            if (!soundPath.isEmpty() && QFile::exists(soundPath)) {
                weatherWarningPlayer->setMedia(QUrl::fromLocalFile(soundPath));
                weatherWarningPlayer->play();
            }
        }
    } else if (wasActive) {
        boardWindow->setElementStyle("weather", "");
    }
}

QString MainWindow::getWeatherEmoji(const QString &description)
{
    if (description.contains("ясно", Qt::CaseInsensitive)) return "☀️";
    if (description.contains("облачно", Qt::CaseInsensitive)) return "☁️";
    if (description.contains("дождь", Qt::CaseInsensitive)) return "🌧️";
    if (description.contains("снег", Qt::CaseInsensitive)) return "❄️";
    if (description.contains("гроза", Qt::CaseInsensitive)) return "⛈️";
    if (description.contains("туман", Qt::CaseInsensitive)) return "🌫️";
    return "🌡️";
}

void MainWindow::onShowTable(const QString &tableName, const QString &tableData)
{
    Q_UNUSED(tableName);
    if (!boardWindow) return;

    // Остальные плитки прячутся, а плашка с таблицей растягивается почти на
    // всю доску (fillBoard=true), шрифт увеличен в 5 раз для читаемости издалека.
    // Моноширинный шрифт и выравнивание по левому краю сохраняют столбцы
    // ровными, т.к. DatabaseManager::formatTableAsText выравнивает текст
    // пробелами в расчёте на моноширинный шрифт. Вернуться к обычному виду —
    // кнопка "Очистить доску".
    QString style =
        "QLabel {"
        "   background-color: rgba(0, 0, 0, 170);"
        "   color: white;"
        "   font-family: 'monospace';"
        "   font-size: 38px;"
        "   font-weight: normal;"
        "   border: 4px solid white;"
        "   border-radius: 16px;"
        "   padding: 24px;"
        "}";
    QFont tableFont("monospace");
    tableFont.setStyleHint(QFont::Monospace);
    tableFont.setPixelSize(38);
    boardWindow->showTakeoverContent(tableData, style, Qt::AlignLeft | Qt::AlignVCenter, tableFont, true);
    statusBar()->showMessage("Таблица показана на доске | «Очистить доску» — вернуться к обычному виду", 5000);
}

void MainWindow::onScheduleTriggered(const ScheduleEntry &entry)
{
    if (boardWindow && !entry.designPath.isEmpty()) {
        QList<ElementConfig> layout;
        if (loadDesignFromFile(entry.designPath, layout)) {
            if (designerWidget) designerWidget->applyLayout(layout);
            boardWindow->applyLayout(layout);
        }
    }

    if (boardWindow && !entry.message.isEmpty()) {
        boardWindow->showPermanentText(entry.message);
    }
    statusBar()->showMessage("Событие расписания: " + entry.name, 5000);
}

void MainWindow::updateCountdown()
{
    if (!boardWindow || !scheduleManager) return;

    ScheduleEntry entry;
    QDateTime when;
    if (scheduleManager->nextEvent(entry, when)) {
        qint64 secs = QDateTime::currentDateTime().secsTo(when);
        if (secs < 0) secs = 0;
        int h = static_cast<int>(secs / 3600);
        int m = static_cast<int>((secs % 3600) / 60);
        int s = static_cast<int>(secs % 60);
        QString text = QString("До «%1»: %2:%3:%4")
                           .arg(entry.name)
                           .arg(h, 2, 10, QChar('0'))
                           .arg(m, 2, 10, QChar('0'))
                           .arg(s, 2, 10, QChar('0'));
        boardWindow->setElementText("countdown", text);
    } else {
        boardWindow->setElementText("countdown", "Нет предстоящих событий");
    }
}

void MainWindow::onAnnouncementShow(const QString &text)
{
    if (!boardWindow) return;
    boardWindow->setElementText("announcement", text);
}

void MainWindow::applyWeatherSettings()
{
    if (!weatherManager) return;

    QSettings settings(QDir::homePath() + "/.InformationBoard/settings.ini", QSettings::IniFormat);
    QString city = settings.value("weather/city", "Moscow").toString();
    QString apiKey = settings.value("weather/apiKey").toString();
    if (apiKey.isEmpty()) apiKey = kDefaultWeatherApiKey;
    int intervalMinutes = settings.value("weather/intervalMinutes", 30).toInt();

    weatherManager->setApiKey(apiKey);
    weatherManager->setCity(city);
    weatherManager->stopUpdates();
    weatherManager->startUpdates(intervalMinutes);
}

void MainWindow::handleSettingsButtonClicked()
{
    SettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        applyWeatherSettings();
        applySlideshowSettings();
        statusBar()->showMessage("Настройки сохранены", 3000);
    }
}

void MainWindow::onSlideshowImageChanged(const QString &imagePath)
{
    if (!boardWindow) return;
    boardWindow->setSlideshowImage(imagePath);
}

void MainWindow::applySlideshowSettings()
{
    if (!slideshowManager) return;

    QSettings settings(QDir::homePath() + "/.InformationBoard/settings.ini", QSettings::IniFormat);
    bool enabled = settings.value("slideshow/enabled", false).toBool();
    QString folderPath = settings.value("slideshow/folderPath").toString();
    int intervalSeconds = settings.value("slideshow/intervalSeconds", 10).toInt();

    slideshowManager->setFolderPath(folderPath);
    slideshowManager->setIntervalSeconds(intervalSeconds);
    slideshowManager->setEnabled(enabled);
}

void MainWindow::handleEmergencyButtonClicked()
{
    if (!boardWindow) {
        QMessageBox::warning(this, "Ошибка", "Окно доски не подключено!");
        return;
    }

    // Единственный источник истины о состоянии тревоги — сама доска
    // (boardWindow->isEmergencyActive()), а не отдельная переменная в консоли:
    // доска может выключить тревогу сама (смена макета), и кнопка должна
    // это увидеть через emergencyModeChanged, а не разъехаться с реальностью.
    bool targetActive = !boardWindow->isEmergencyActive();

    if (targetActive) {
        QSettings settings(QDir::homePath() + "/.InformationBoard/settings.ini", QSettings::IniFormat);
        QString text = settings.value("emergency/text").toString();
        if (text.isEmpty()) text = "⚠️ ЭКСТРЕННАЯ СИТУАЦИЯ — СЛЕДУЙТЕ ИНСТРУКЦИЯМ ⚠️";
        QString soundPath = settings.value("emergency/soundPath").toString();
        boardWindow->setEmergencyMode(true, text, soundPath);
    } else {
        boardWindow->setEmergencyMode(false);
    }
}

void MainWindow::onEmergencyModeChanged(bool active)
{
    if (ui->emergencyButton) {
        ui->emergencyButton->setText(active ? "✅ Выключить экстренный режим" : "🚨 Включить экстренный режим");
    }
    statusBar()->showMessage(active ? "Экстренный режим включён" : "Экстренный режим выключен",
                              active ? 5000 : 3000);
}

