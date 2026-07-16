#include "scheduleentrydialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QTimeEdit>
#include <QCheckBox>
#include <QSlider>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QUrl>
#include <QMediaPlayer>

static const char *kDayLabels[7] = {"Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"};

ScheduleEntryDialog::ScheduleEntryDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void ScheduleEntryDialog::setupUI()
{
    setWindowTitle("Событие расписания");
    setMinimumWidth(420);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *form = new QFormLayout();
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Например: Обед");
    form->addRow("Название:", nameEdit);

    timeEdit = new QTimeEdit(this);
    timeEdit->setDisplayFormat("HH:mm");
    timeEdit->setTime(QTime(12, 0));
    form->addRow("Время:", timeEdit);

    mainLayout->addLayout(form);

    QGroupBox *daysGroup = new QGroupBox("Дни недели", this);
    QHBoxLayout *daysLayout = new QHBoxLayout(daysGroup);
    for (int i = 0; i < 7; ++i) {
        dayChecks[i] = new QCheckBox(kDayLabels[i], daysGroup);
        dayChecks[i]->setChecked(true);
        daysLayout->addWidget(dayChecks[i]);
    }
    mainLayout->addWidget(daysGroup);

    QHBoxLayout *soundLayout = new QHBoxLayout();
    soundPathEdit = new QLineEdit(this);
    soundPathEdit->setPlaceholderText("Файл звука (.wav, .mp3)");
    QPushButton *browseButton = new QPushButton("Обзор...", this);
    QPushButton *playButton = new QPushButton("▶", this);
    playButton->setMaximumWidth(36);
    soundLayout->addWidget(soundPathEdit);
    soundLayout->addWidget(browseButton);
    soundLayout->addWidget(playButton);
    mainLayout->addLayout(soundLayout);

    QFormLayout *volumeForm = new QFormLayout();
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(80);
    volumeForm->addRow("Громкость:", volumeSlider);
    mainLayout->addLayout(volumeForm);

    mainLayout->addWidget(new QLabel("Сообщение на доске:", this));
    messageEdit = new QPlainTextEdit(this);
    messageEdit->setPlaceholderText("Текст, который появится на доске (необязательно)");
    messageEdit->setMaximumHeight(80);
    mainLayout->addWidget(messageEdit);

    mainLayout->addWidget(new QLabel("Профиль оформления доски (необязательно):", this));
    QHBoxLayout *designLayout = new QHBoxLayout();
    designPathEdit = new QLineEdit(this);
    designPathEdit->setPlaceholderText("Переключить макет доски на .design-профиль");
    QPushButton *browseDesignButton = new QPushButton("Обзор...", this);
    designLayout->addWidget(designPathEdit);
    designLayout->addWidget(browseDesignButton);
    mainLayout->addLayout(designLayout);

    enabledCheck = new QCheckBox("Активно", this);
    enabledCheck->setChecked(true);
    mainLayout->addWidget(enabledCheck);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttons);

    connect(browseButton, &QPushButton::clicked, this, &ScheduleEntryDialog::onBrowseSound);
    connect(playButton, &QPushButton::clicked, this, &ScheduleEntryDialog::onPlayPreview);
    connect(browseDesignButton, &QPushButton::clicked, this, &ScheduleEntryDialog::onBrowseDesign);
}

void ScheduleEntryDialog::onBrowseDesign()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите профиль оформления", "", "Design files (*.design)");
    if (!fileName.isEmpty()) {
        designPathEdit->setText(fileName);
    }
}

void ScheduleEntryDialog::onBrowseSound()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Выберите звуковой файл", "", "Аудио (*.wav *.mp3 *.ogg)");
    if (!fileName.isEmpty()) {
        soundPathEdit->setText(fileName);
    }
}

void ScheduleEntryDialog::onPlayPreview()
{
    if (soundPathEdit->text().isEmpty()) return;

    QMediaPlayer *player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(soundPathEdit->text()));
    player->setVolume(volumeSlider->value());
    connect(player, &QMediaPlayer::mediaStatusChanged, player, [player](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia)
            player->deleteLater();
    });
    player->play();
}

void ScheduleEntryDialog::setEntry(const ScheduleEntry &entry)
{
    m_id = entry.id;
    nameEdit->setText(entry.name);
    timeEdit->setTime(entry.time.isValid() ? entry.time : QTime(12, 0));
    for (int i = 0; i < 7; ++i)
        dayChecks[i]->setChecked(entry.days.contains(i + 1));
    soundPathEdit->setText(entry.soundPath);
    designPathEdit->setText(entry.designPath);
    messageEdit->setPlainText(entry.message);
    volumeSlider->setValue(entry.volume);
    enabledCheck->setChecked(entry.enabled);
}

ScheduleEntry ScheduleEntryDialog::entry() const
{
    ScheduleEntry entry;
    entry.id = m_id;
    entry.name = nameEdit->text();
    entry.time = timeEdit->time();
    for (int i = 0; i < 7; ++i)
        if (dayChecks[i]->isChecked()) entry.days.insert(i + 1);
    entry.soundPath = soundPathEdit->text();
    entry.designPath = designPathEdit->text();
    entry.message = messageEdit->toPlainText();
    entry.volume = volumeSlider->value();
    entry.enabled = enabledCheck->isChecked();
    return entry;
}
