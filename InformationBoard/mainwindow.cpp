#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QTime>
#include <QDate>
#include <QDebug>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , boardWindow(nullptr)
    , designerWidget(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Консоль управления информационной доской");

    designerWidget = new BoardDesignerWidget(this);

    if (ui->tabWidget) {
        QWidget *designTab = new QWidget();
        QVBoxLayout *designLayout = new QVBoxLayout(designTab);
        designLayout->addWidget(designerWidget);
        ui->tabWidget->addTab(designTab, "🎨 Дизайн");
    }

    setupConnections();
    statusBar()->showMessage("Готов к работе | Доска: не связана");
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
    }
}

void MainWindow::setupConnections()
{
    if (ui->showBoardButton)
        connect(ui->showBoardButton, &QPushButton::clicked,
                this, &MainWindow::on_showBoardButton_clicked);

    if (ui->fullscreenButton)
        connect(ui->fullscreenButton, &QPushButton::clicked,
                this, &MainWindow::on_fullscreenButton_clicked);

    if (ui->alwaysOnTopCheck)
        connect(ui->alwaysOnTopCheck, &QCheckBox::toggled,
                this, &MainWindow::on_alwaysOnTopCheck_toggled);

    if (ui->showTextButton)
        connect(ui->showTextButton, &QPushButton::clicked,
                this, &MainWindow::on_showTextButton_clicked);

    if (ui->clearBoardButton)
        connect(ui->clearBoardButton, &QPushButton::clicked,
                this, &MainWindow::on_clearBoardButton_clicked);

    if (ui->quickNewsButton)
        connect(ui->quickNewsButton, &QPushButton::clicked,
                this, &MainWindow::on_quickNewsButton_clicked);

    if (ui->quickWeatherButton)
        connect(ui->quickWeatherButton, &QPushButton::clicked,
                this, &MainWindow::on_quickWeatherButton_clicked);

    if (ui->quickTimeButton)
        connect(ui->quickTimeButton, &QPushButton::clicked,
                this, &MainWindow::on_quickTimeButton_clicked);

    if (ui->quickLogoButton)
        connect(ui->quickLogoButton, &QPushButton::clicked,
                this, &MainWindow::on_quickLogoButton_clicked);

    if (ui->setRedBackgroundButton)
        connect(ui->setRedBackgroundButton, &QPushButton::clicked,
                this, &MainWindow::on_setRedBackgroundButton_clicked);

    if (ui->setBlueBackgroundButton)
        connect(ui->setBlueBackgroundButton, &QPushButton::clicked,
                this, &MainWindow::on_setBlueBackgroundButton_clicked);

    if (ui->setGreenBackgroundButton)
        connect(ui->setGreenBackgroundButton, &QPushButton::clicked,
                this, &MainWindow::on_setGreenBackgroundButton_clicked);

    if (ui->setBlackBackgroundButton)
        connect(ui->setBlackBackgroundButton, &QPushButton::clicked,
                this, &MainWindow::on_setBlackBackgroundButton_clicked);

    if (ui->setCustomColorButton)
        connect(ui->setCustomColorButton, &QPushButton::clicked,
                this, &MainWindow::on_setCustomColorButton_clicked);

    if (ui->fontFamilyCombo)
        connect(ui->fontFamilyCombo, &QComboBox::currentTextChanged,
                this, &MainWindow::on_fontFamilyCombo_currentTextChanged);

    if (ui->boldTextCheck)
        connect(ui->boldTextCheck, &QCheckBox::toggled,
                this, &MainWindow::on_boldTextCheck_toggled);

    if (ui->italicTextCheck)
        connect(ui->italicTextCheck, &QCheckBox::toggled,
                this, &MainWindow::on_italicTextCheck_toggled);

    if (ui->textSmall)
        connect(ui->textSmall, &QRadioButton::toggled,
                this, &MainWindow::on_textSmall_toggled);

    if (ui->textMedium)
        connect(ui->textMedium, &QRadioButton::toggled,
                this, &MainWindow::on_textMedium_toggled);

    if (ui->textLarge)
        connect(ui->textLarge, &QRadioButton::toggled,
                this, &MainWindow::on_textLarge_toggled);

    if (ui->textHuge)
        connect(ui->textHuge, &QRadioButton::toggled,
                this, &MainWindow::on_textHuge_toggled);

    if (ui->toggleLogoButton)
        connect(ui->toggleLogoButton, &QPushButton::clicked,
                this, &MainWindow::on_toggleLogoButton_clicked);

    if (ui->toggleClockButton)
        connect(ui->toggleClockButton, &QPushButton::clicked,
                this, &MainWindow::on_toggleClockButton_clicked);

    if (ui->toggleWeatherButton)
        connect(ui->toggleWeatherButton, &QPushButton::clicked,
                this, &MainWindow::on_toggleWeatherButton_clicked);

    if (ui->toggleTickerButton)
        connect(ui->toggleTickerButton, &QPushButton::clicked,
                this, &MainWindow::on_toggleTickerButton_clicked);

    if (ui->setLogoImageButton)
        connect(ui->setLogoImageButton, &QPushButton::clicked,
                this, &MainWindow::on_setLogoImageButton_clicked);

    if (ui->saveConfigButton)
        connect(ui->saveConfigButton, &QPushButton::clicked,
                this, &MainWindow::on_saveConfigButton_clicked);

    if (ui->loadConfigButton)
        connect(ui->loadConfigButton, &QPushButton::clicked,
                this, &MainWindow::on_loadConfigButton_clicked);

    if (ui->resetBoardButton)
        connect(ui->resetBoardButton, &QPushButton::clicked,
                this, &MainWindow::on_resetBoardButton_clicked);

    if (ui->applyDesignButton)
        connect(ui->applyDesignButton, &QPushButton::clicked,
                this, &MainWindow::on_applyDesignButton_clicked);

    if (ui->resetDesignButton)
        connect(ui->resetDesignButton, &QPushButton::clicked,
                this, &MainWindow::on_resetDesignButton_clicked);

    if (ui->saveDesignButton)
        connect(ui->saveDesignButton, &QPushButton::clicked,
                this, &MainWindow::on_saveDesignButton_clicked);

    if (ui->loadDesignButton)
        connect(ui->loadDesignButton, &QPushButton::clicked,
                this, &MainWindow::on_loadDesignButton_clicked);

    if (ui->previewDesignButton)
        connect(ui->previewDesignButton, &QPushButton::clicked,
                this, &MainWindow::on_previewDesignButton_clicked);

    if (designerWidget) {
        connect(designerWidget, &BoardDesignerWidget::layoutChanged,
                this, [this]() {
                    statusBar()->showMessage("Макет изменен. Нажмите 'Применить' для обновления доски", 3000);
                });
    }
}

void MainWindow::on_showBoardButton_clicked()
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

void MainWindow::on_fullscreenButton_clicked()
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

void MainWindow::on_alwaysOnTopCheck_toggled(bool checked)
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

void MainWindow::on_showTextButton_clicked()
{
    if (!boardWindow) {
        QMessageBox::warning(this, "Ошибка", "Окно доски не подключено!");
        return;
    }

    QString text = ui->textInput ? ui->textInput->text() : "Тестовое сообщение";
    if (text.isEmpty()) {
        text = "Введите текст для отображения";
    }

    boardWindow->showPermanentText(text);
    statusBar()->showMessage("Текст отправлен на доску: " + text.left(20) + "...");
}

void MainWindow::on_clearBoardButton_clicked()
{
    if (boardWindow) {
        boardWindow->clearScreen();
        statusBar()->showMessage("Доска очищена");
    }
}

void MainWindow::on_quickNewsButton_clicked()
{
    if (!boardWindow) return;

    QString news = "📰 НОВОСТИ\n\n"
                   "• Qt 6.5 вышла с новыми возможностями\n"
                   "• C++20 получает новые фичи\n"
                   "• Информационные доски становятся популярнее";

    boardWindow->showPermanentText(news);
    statusBar()->showMessage("Показаны новости");
}

void MainWindow::on_quickWeatherButton_clicked()
{
    if (!boardWindow) return;

    QString weather = "☀️ ПОГОДА\n\n"
                      "Москва: +22°C, ясно\n"
                      "Санкт-Петербург: +19°C, облачно\n"
                      "Владивосток: +24°C, солнечно";

    boardWindow->showPermanentText(weather);
    statusBar()->showMessage("Показана погода");
}

void MainWindow::on_quickTimeButton_clicked()
{
    if (!boardWindow) return;

    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();

    QString timeString = QString("🕐 ТЕКУЩЕЕ ВРЕМЯ\n\n"
                                 "Дата: %1\n"
                                 "Время: %2\n"
                                 "День недели: %3")
                             .arg(date.toString("dd.MM.yyyy"))
                             .arg(time.toString("hh:mm:ss"))
                             .arg(date.toString("dddd"));

    boardWindow->showPermanentText(timeString);
    statusBar()->showMessage("Показано текущее время");
}

void MainWindow::on_quickLogoButton_clicked()
{
    if (!boardWindow) return;

    boardWindow->showPermanentText("🏢 НАША КОМПАНИЯ\n\n"
                                   "Информационные технологии\n"
                                   "Основано в 2024 году\n"
                                   "Мы создаем будущее");
    statusBar()->showMessage("Показан логотип");
}

void MainWindow::on_setRedBackgroundButton_clicked()
{
    if (boardWindow) {
        boardWindow->setBackgroundColor(Qt::red);
        statusBar()->showMessage("Установлен красный фон");
    }
}

void MainWindow::on_setBlueBackgroundButton_clicked()
{
    if (boardWindow) {
        boardWindow->setBackgroundColor(Qt::blue);
        statusBar()->showMessage("Установлен синий фон");
    }
}

void MainWindow::on_setGreenBackgroundButton_clicked()
{
    if (boardWindow) {
        boardWindow->setBackgroundColor(Qt::green);
        statusBar()->showMessage("Установлен зеленый фон");
    }
}

void MainWindow::on_setBlackBackgroundButton_clicked()
{
    if (boardWindow) {
        boardWindow->setBackgroundColor(Qt::black);
        statusBar()->showMessage("Установлен черный фон");
    }
}

void MainWindow::on_setCustomColorButton_clicked()
{
    if (!boardWindow) return;

    QColor color = QColorDialog::getColor(Qt::white, this, "Выберите цвет фона");

    if (color.isValid()) {
        boardWindow->setBackgroundColor(color);
        statusBar()->showMessage("Установлен пользовательский цвет: " + color.name());
    }
}

void MainWindow::on_fontFamilyCombo_currentTextChanged(const QString &font)
{
    Q_UNUSED(font);
    updateBoardStyle();
}

void MainWindow::on_boldTextCheck_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateBoardStyle();
}

void MainWindow::on_italicTextCheck_toggled(bool checked)
{
    Q_UNUSED(checked);
    updateBoardStyle();
}

void MainWindow::on_textSmall_toggled(bool checked)
{
    if (checked) updateBoardStyle();
}

void MainWindow::on_textMedium_toggled(bool checked)
{
    if (checked) updateBoardStyle();
}

void MainWindow::on_textLarge_toggled(bool checked)
{
    if (checked) updateBoardStyle();
}

void MainWindow::on_textHuge_toggled(bool checked)
{
    if (checked) updateBoardStyle();
}

void MainWindow::on_toggleLogoButton_clicked()
{
    if (!boardWindow) return;

    static bool logoVisible = true;
    logoVisible = !logoVisible;

    if (logoVisible) {
        boardWindow->showElement("logo");
        ui->toggleLogoButton->setText("Скрыть логотип");
    } else {
        boardWindow->hideElement("logo");
        ui->toggleLogoButton->setText("Показать логотип");
    }
}

void MainWindow::on_toggleClockButton_clicked()
{
    if (!boardWindow) return;

    static bool clockVisible = true;
    clockVisible = !clockVisible;

    if (clockVisible) {
        boardWindow->showElement("clock");
        boardWindow->showElement("date");
        ui->toggleClockButton->setText("Скрыть часы");
    } else {
        boardWindow->hideElement("clock");
        boardWindow->hideElement("date");
        ui->toggleClockButton->setText("Показать часы");
    }
}

void MainWindow::on_toggleWeatherButton_clicked()
{
    if (!boardWindow) return;

    static bool weatherVisible = true;
    weatherVisible = !weatherVisible;

    if (weatherVisible) {
        boardWindow->showElement("weather");
        ui->toggleWeatherButton->setText("Скрыть погоду");
    } else {
        boardWindow->hideElement("weather");
        ui->toggleWeatherButton->setText("Показать погоду");
    }
}

void MainWindow::on_toggleTickerButton_clicked()
{
    if (!boardWindow) return;

    static bool tickerVisible = true;
    tickerVisible = !tickerVisible;

    if (tickerVisible) {
        boardWindow->showElement("ticker");
        ui->toggleTickerButton->setText("Скрыть бегущую строку");
    } else {
        boardWindow->hideElement("ticker");
        ui->toggleTickerButton->setText("Показать бегущую строку");
    }
}

void MainWindow::on_setLogoImageButton_clicked()
{
    if (!boardWindow) return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите изображение для логотипа",
                                                    "",
                                                    "Изображения (*.png *.jpg *.bmp *.gif)");

    if (!fileName.isEmpty()) {
        boardWindow->setLogo(fileName);
        statusBar()->showMessage("Логотип обновлен: " + fileName);
    }
}

void MainWindow::on_saveConfigButton_clicked()
{
    if (!boardWindow) return;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить конфигурацию доски",
                                                    "board_config.json",
                                                    "JSON файлы (*.json)");

    if (!fileName.isEmpty()) {
        boardWindow->saveConfiguration(fileName);
        statusBar()->showMessage("Конфигурация сохранена");
    }
}

void MainWindow::on_loadConfigButton_clicked()
{
    if (!boardWindow) return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Загрузить конфигурацию доски",
                                                    "",
                                                    "JSON файлы (*.json)");

    if (!fileName.isEmpty()) {
        boardWindow->loadConfiguration(fileName);
        statusBar()->showMessage("Конфигурация загружена");
    }
}

void MainWindow::on_resetBoardButton_clicked()
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

void MainWindow::updateBoardStyle()
{
    if (!boardWindow) return;

    int fontSize = 24;
    if (ui->textMedium && ui->textMedium->isChecked()) fontSize = 48;
    if (ui->textLarge && ui->textLarge->isChecked()) fontSize = 72;
    if (ui->textHuge && ui->textHuge->isChecked()) fontSize = 96;

    QString fontWeight = ui->boldTextCheck && ui->boldTextCheck->isChecked() ? "bold" : "normal";
    QString fontStyle = ui->italicTextCheck && ui->italicTextCheck->isChecked() ? "italic" : "normal";
    QString fontFamily = ui->fontFamilyCombo ? ui->fontFamilyCombo->currentText() : "Arial";

    QString style = QString(
                        "QLabel {"
                        "   background-color: #2c3e50;"
                        "   color: white;"
                        "   font-family: '%1';"
                        "   font-size: %2px;"
                        "   font-weight: %3;"
                        "   font-style: %4;"
                        "   border: 1px solid #3498db;"
                        "   border-radius: 5px;"
                        "   padding: 10px;"
                        "}"
                        ).arg(fontFamily).arg(fontSize).arg(fontWeight).arg(fontStyle);

    boardWindow->setCustomStyle(style);
    statusBar()->showMessage("Стиль доски обновлен");
}

void MainWindow::on_applyDesignButton_clicked()
{
    if (!boardWindow || !designerWidget) {
        QMessageBox::warning(this, "Ошибка", "Доска или дизайнер не инициализированы!");
        return;
    }

    applyDesignToBoard();
    statusBar()->showMessage("Дизайн применен к главной доске", 3000);
}

void MainWindow::on_resetDesignButton_clicked()
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

void MainWindow::on_saveDesignButton_clicked()
{
    if (!designerWidget) return;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить дизайн доски",
                                                    "design_",
                                                    "Design files (*.design)");

    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".design")) {
        fileName += ".design";
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream stream(&file);

        QList<ElementLayoutConfig> layout = designerWidget->getCurrentLayout();

        int rows, cols;
        designerWidget->getGridSize(rows, cols);
        stream << rows << cols;

        stream << layout.size();

        for (const auto &config : layout) {
            stream << config.elementType
                   << config.displayName
                   << config.defaultText
                   << config.row
                   << config.col
                   << config.rowSpan
                   << config.colSpan
                   << config.visible;
        }

        file.close();
        statusBar()->showMessage("Дизайн сохранен: " + fileName, 3000);
    }
}

void MainWindow::on_loadDesignButton_clicked()
{
    if (!designerWidget) return;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Загрузить дизайн доски",
                                                    "",
                                                    "Design files (*.design)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);

        int rows, cols;
        stream >> rows >> cols;
        designerWidget->setGridSize(rows, cols);

        int count;
        stream >> count;

        QList<ElementLayoutConfig> layout;
        for (int i = 0; i < count; ++i) {
            ElementLayoutConfig config;
            stream >> config.elementType
                >> config.displayName
                >> config.defaultText
                >> config.row
                >> config.col
                >> config.rowSpan
                >> config.colSpan
                >> config.visible;
            layout.append(config);
        }

        designerWidget->applyLayout(layout);

        file.close();
        statusBar()->showMessage("Дизайн загружен: " + fileName, 3000);
    }
}

void MainWindow::on_previewDesignButton_clicked()
{
    if (!boardWindow || !designerWidget) return;

    applyDesignToBoard();
    statusBar()->showMessage("Предпросмотр дизайна на главной доске", 2000);
}

void MainWindow::applyDesignToBoard()
{
    if (!boardWindow || !designerWidget) return;

    QList<ElementLayoutConfig> layout = designerWidget->getCurrentLayout();

    statusBar()->showMessage(QString("Применяется дизайн с %1 элементами...").arg(layout.size()), 2000);

    qDebug() << "Применяется дизайн к главной доске, элементов:" << layout.size();
}
