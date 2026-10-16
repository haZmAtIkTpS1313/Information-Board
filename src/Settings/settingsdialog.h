#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <qlist.h>
#include <QListWidget>
#include <qpushbutton.h>

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
    void onAddSlideshowFiles();
    void onRemoveSlideshowFiles();

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
    QListWidget *selectedFilesList;
    QPushButton *addFilesButton;
    QPushButton *removeFilesButton;
};

#endif // SETTINGSDIALOG_H
