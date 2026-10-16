#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <qboxlayout.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qpushbutton.h>
#include "../apikeys.h"

static const char *kDefaultEmergencyText = "⚠️ ЭКСТРЕННАЯ СИТУАЦИЯ — СЛЕДУЙТЕ ИНСТРУКЦИЯМ ⚠️";

static QString settingsFilePath()
{
    return QDir::homePath() + "/.InformationBoard/settings.ini";
}

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI()
{
    setWindowTitle("Настройки");
    setMinimumWidth(420);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *form = new QFormLayout();

    cityEdit = new QLineEdit(this);
    cityEdit->setPlaceholderText("Например: Moscow");
    form->addRow("Город (погода):", cityEdit);

    QHBoxLayout *keyLayout = new QHBoxLayout();
    apiKeyEdit = new QLineEdit(this);
    apiKeyEdit->setEchoMode(QLineEdit::Password);
    apiKeyEdit->setPlaceholderText("Ключ OpenWeatherMap");
    QPushButton *toggleKeyButton = new QPushButton("👁", this);
    toggleKeyButton->setCheckable(true);
    toggleKeyButton->setMaximumWidth(36);
    keyLayout->addWidget(apiKeyEdit);
    keyLayout->addWidget(toggleKeyButton);
    form->addRow("API-ключ погоды:", keyLayout);

    intervalSpin = new QSpinBox(this);
    intervalSpin->setRange(1, 180);
    intervalSpin->setSuffix(" мин");
    form->addRow("Интервал обновления погоды:", intervalSpin);

    mainLayout->addLayout(form);

    QLabel *hint = new QLabel(
        "Бесплатный ключ можно получить на openweathermap.org в разделе API keys.", this);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: #6c757d; font-size: 11px;");
    mainLayout->addWidget(hint);

    QFormLayout *emergencyForm = new QFormLayout();

    emergencyTextEdit = new QLineEdit(this);
    emergencyTextEdit->setPlaceholderText(kDefaultEmergencyText);
    emergencyForm->addRow("Текст экстренного режима:", emergencyTextEdit);

    QHBoxLayout *soundLayout = new QHBoxLayout();
    emergencySoundEdit = new QLineEdit(this);
    emergencySoundEdit->setPlaceholderText("Файл сирены (.wav, .mp3), необязательно");
    QPushButton *browseSoundButton = new QPushButton("Обзор...", this);
    soundLayout->addWidget(emergencySoundEdit);
    soundLayout->addWidget(browseSoundButton);
    emergencyForm->addRow("Звук сирены:", soundLayout);

    QFormLayout *weatherWarningForm = new QFormLayout();
    QHBoxLayout *warningSoundLayout = new QHBoxLayout();
    weatherWarningSoundEdit = new QLineEdit(this);
    weatherWarningSoundEdit->setPlaceholderText("Звук погодного предупреждения, необязательно");
    QPushButton *browseWarningSoundButton = new QPushButton("Обзор...", this);
    warningSoundLayout->addWidget(weatherWarningSoundEdit);
    warningSoundLayout->addWidget(browseWarningSoundButton);
    weatherWarningForm->addRow("Звук погодного предупреждения:", warningSoundLayout);
    mainLayout->addLayout(weatherWarningForm);

    QLabel *warningHint = new QLabel(
        "Предупреждение показывается при грозе, ветре от 15 м/с или морозе от -15°C.", this);
    warningHint->setWordWrap(true);
    warningHint->setStyleSheet("color: #6c757d; font-size: 11px;");
    mainLayout->addWidget(warningHint);

    mainLayout->addLayout(emergencyForm);

    slideshowEnabledCheck = new QCheckBox("Слайдшоу изображений включено", this);
    mainLayout->addWidget(slideshowEnabledCheck);

    QFormLayout *slideshowForm = new QFormLayout();
    QHBoxLayout *slideshowFolderLayout = new QHBoxLayout();
    slideshowFolderEdit = new QLineEdit(this);
    slideshowFolderEdit->setPlaceholderText("Папка с изображениями (.png, .jpg, .bmp, .gif)");
    QPushButton *browseFolderButton = new QPushButton("Обзор...", this);
    slideshowFolderLayout->addWidget(slideshowFolderEdit);
    slideshowFolderLayout->addWidget(browseFolderButton);
    slideshowForm->addRow("Папка слайдшоу:", slideshowFolderLayout);

    slideshowIntervalSpin = new QSpinBox(this);
    slideshowIntervalSpin->setRange(2, 600);
    slideshowIntervalSpin->setSuffix(" сек");
    slideshowForm->addRow("Интервал смены картинок:", slideshowIntervalSpin);
    mainLayout->addLayout(slideshowForm);

    QGroupBox *filesGroup = new QGroupBox("Или выберите конкретные файлы", this);
    QVBoxLayout *filesLayout = new QVBoxLayout(filesGroup);

    selectedFilesList = new QListWidget(this);
    selectedFilesList->setMaximumHeight(120);
    filesLayout->addWidget(selectedFilesList);

    QHBoxLayout *filesButtonsLayot = new QHBoxLayout();
    addFilesButton = new QPushButton("Добавить файл",this);
    removeFilesButton = new QPushButton("Удалить выбранное", this);
    removeFilesButton->setEnabled(selectedFilesList);
    filesButtonsLayot->addWidget(addFilesButton);
    filesButtonsLayot->addWidget(removeFilesButton);
    filesButtonsLayot->addStretch();
    filesLayout->addLayout(filesButtonsLayot);

    QLabel *filesHint = new QLabel(
        "Если выбраны конкретные файлы, папка выше игнорируется. Оставьте список пустым, чтобы использовать папку.",
        this);
    filesHint->setWordWrap(true);
    filesHint->setStyleSheet("color: #6c757d; font-size: 11px;");
    filesLayout->addWidget(filesHint);

    mainLayout->addWidget(filesGroup);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::onSave);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    connect(toggleKeyButton, &QPushButton::toggled, this, [this](bool checked) {
        apiKeyEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
    connect(browseSoundButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseEmergencySound);
    connect(browseWarningSoundButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseWeatherWarningSound);
    connect(browseFolderButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseSlideshowFolder);

    connect(addFilesButton, &QPushButton::clicked, this, &SettingsDialog::onAddSlideshowFiles);
    connect(removeFilesButton, &QPushButton::clicked, this, &SettingsDialog::onRemoveSlideshowFiles);
    connect(selectedFilesList, &QListWidget::currentRowChanged, this, [this](int row) {
        removeFilesButton->setEnabled(row >= 0);
    });
}

void SettingsDialog::onBrowseSlideshowFolder()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Выберите папку с изображениями");
    if (!dirName.isEmpty()) {
        slideshowFolderEdit->setText(dirName);
    }
}

void SettingsDialog::onBrowseEmergencySound()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите звук сирены", "", "Аудио (*.wav *.mp3 *.ogg)");
    if (!fileName.isEmpty()) {
        emergencySoundEdit->setText(fileName);
    }
}

void SettingsDialog::onBrowseWeatherWarningSound()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите звук погодного предупреждения", "", "Аудио (*.wav *.mp3 *.ogg)");
    if (!fileName.isEmpty()) {
        weatherWarningSoundEdit->setText(fileName);
    }
}

void SettingsDialog::onAddSlideshowFiles()
{
    QStringList filters;
    filters << "Изображения (*.png *.jpg *.jpeg *.bmp *.gif *.webp *.tif *.tiff *.heic *.heif *.avif)"
            << "Все файлы (*)";
    
            QStringList files = QFileDialog::getOpenFileNames(
                this, "Выберите изображения для слайдшоу", "", filters.join(";;")
            );

            for (const QString &file :files) {
                bool alreadyAdded = false;
                for (int i = 0; i < selectedFilesList->count(); ++i) {
                    if (selectedFilesList->item(i)->data(Qt::UserRole).toString() == false) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    QListWidgetItem *item = new QListWidgetItem(QFileInfo(file).fileName());
                    item->setData(Qt::UserRole, file);
                    item->setToolTip(file);
                    selectedFilesList->addItem(item);
                }
            }
}

void SettingsDialog::onRemoveSlideshowFiles()
{
    QList<QListWidgetItem*> selected = selectedFilesList->selectedItems();
    for (QListWidgetItem *item : selected) {
        delete selectedFilesList->takeItem(selectedFilesList->row(item));
    } 
    removeFilesButton->setEnabled(selectedFilesList->currentRow() >= 0);
} 

void SettingsDialog::loadSettings()
{
    QSettings settings(settingsFilePath(), QSettings::IniFormat);
    cityEdit->setText(settings.value("weather/city", "Moscow").toString());
    QString apiKey = settings.value("weather/apiKey").toString();
    apiKeyEdit->setText(apiKey.isEmpty() ? kDefaultWeatherApiKey : apiKey);
    intervalSpin->setValue(settings.value("weather/intervalMinutes", 30).toInt());

    emergencyTextEdit->setText(settings.value("emergency/text", kDefaultEmergencyText).toString());
    emergencySoundEdit->setText(settings.value("emergency/soundPath").toString());
    weatherWarningSoundEdit->setText(settings.value("weather/warningSoundPath").toString());

    slideshowEnabledCheck->setChecked(settings.value("slideshow/enabled", false).toBool());
    slideshowFolderEdit->setText(settings.value("slideshow/folderPath").toString());
    slideshowIntervalSpin->setValue(settings.value("slideshow/intervalSeconds", 10).toInt());

    selectedFilesList->clear();
    QStringList savedFiles = settings.value("slideshow/selectedFiles").toStringList();
    for (const QString &file :savedFiles) {
        if (QFile::exists(file)) {
            QListWidgetItem *item = new QListWidgetItem(QFileInfo(file).fileName());
            item->setData(Qt::UserRole, file);
            item->setToolTip(file);
            selectedFilesList->addItem(item);
        }
    }
}

void SettingsDialog::onSave()
{
    QSettings settings(settingsFilePath(), QSettings::IniFormat);
    settings.setValue("weather/city", cityEdit->text().trimmed());
    settings.setValue("weather/apiKey", apiKeyEdit->text().trimmed());
    settings.setValue("weather/intervalMinutes", intervalSpin->value());
    settings.setValue("weather/warningSoundPath", weatherWarningSoundEdit->text().trimmed());
    settings.setValue("emergency/text", emergencyTextEdit->text().trimmed());
    settings.setValue("emergency/soundPath", emergencySoundEdit->text().trimmed());
    settings.setValue("slideshow/enabled", slideshowEnabledCheck->isChecked());
    settings.setValue("slideshow/folderPath", slideshowFolderEdit->text().trimmed());
    settings.setValue("slideshow/intervalSeconds", slideshowIntervalSpin->value());

    QStringList file;
    for (int i = 0; i < selectedFilesList->count(); ++i) {
        file << selectedFilesList->item(i)->data(Qt::UserRole).toString();
    }
    settings.setValue("slideshow/selectedFiles", file);
    settings.sync();
    accept();
}
