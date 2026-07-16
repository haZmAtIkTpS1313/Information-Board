#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QLineEdit;
class QSpinBox;
class QCheckBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onSave();
    void onBrowseEmergencySound();
    void onBrowseWeatherWarningSound();
    void onBrowseSlideshowFolder();

private:
    void setupUI();
    void loadSettings();

    QLineEdit *cityEdit;
    QLineEdit *apiKeyEdit;
    QSpinBox *intervalSpin;
    QLineEdit *emergencyTextEdit;
    QLineEdit *emergencySoundEdit;
    QLineEdit *weatherWarningSoundEdit;
    QCheckBox *slideshowEnabledCheck;
    QLineEdit *slideshowFolderEdit;
    QSpinBox *slideshowIntervalSpin;
};

#endif // SETTINGSDIALOG_H
