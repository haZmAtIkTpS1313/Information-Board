
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Board/boardwindow.h"
#include "Weather/weathermanager.h"
//#include "News/newsmanager.h"
#include "Board/boarddesignerwidget.h"
#include "Schedule/schedulemanager.h"
#include "Announcements/announcementmanager.h"
#include "Media/slideshowmanager.h"
#include "Database/databasewidget.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DatabaseManager;   // forward declaration
class QMediaPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setBoardWindow(BoardWindow *window);

private slots:
    void handleShowBoardButtonClicked();
    void handleFullscreenButtonClicked();
    void handleAlwaysOnTopCheckToggled(bool checked);

    void handleShowTextButtonClicked();
    void handleClearBoardButtonClicked();

    //void on_quickNewsButton_clicked();
    void handleQuickWeatherButtonClicked();
    void handleQuickTimeButtonClicked();
    void handleQuickLogoButtonClicked();

    void handleSetRedBackgroundButtonClicked();
    void handleSetBlueBackgroundButtonClicked();
    void handleSetGreenBackgroundButtonClicked();
    void handleSetBlackBackgroundButtonClicked();
    void handleSetCustomColorButtonClicked();

    void handleTextSmallToggled(bool checked);
    void handleTextMediumToggled(bool checked);
    void handleTextLargeToggled(bool checked);
    void handleTextHugeToggled(bool checked);

    void handleToggleLogoButtonClicked();
    void handleToggleClockButtonClicked();
    void handleToggleWeatherButtonClicked();
    void handleToggleTickerButtonClicked();
    void handleSetLogoImageButtonClicked();

    void handleSaveConfigButtonClicked();
    void handleLoadConfigButtonClicked();
    void handleResetBoardButtonClicked();

    void handleApplyDesignButtonClicked();
    void handleResetDesignButtonClicked();
    void handleSaveDesignButtonClicked();
    void handleLoadDesignButtonClicked();
    // on_previewDesignButton_clicked() убран — дублировал handleApplyDesignButtonClicked()

    void onWeatherUpdated(const WeatherData &weather);
    //void onNewsUpdated(const QList<NewsItem> &news);

    void onShowTable(const QString &tableName, const QString &tableData);

    void onScheduleTriggered(const ScheduleEntry &entry);
    void updateCountdown();

    void handleSettingsButtonClicked();

    void onAnnouncementShow(const QString &text);

    void handleEmergencyButtonClicked();
    void onEmergencyModeChanged(bool active);

    void onSlideshowImageChanged(const QString &imagePath);

private:
    Ui::MainWindow *ui;
    BoardWindow *boardWindow;
    BoardDesignerWidget *designerWidget;
    DatabaseManager *dbManager;
    DatabaseWidget *databaseWidget;
    ScheduleManager *scheduleManager;
    AnnouncementManager *announcementManager;
    SlideshowManager *slideshowManager;

    void setupConnections();
    void updateBoardStyle();
    void applyDesignToBoard();
    void applyTextSize(int size);
    bool loadDesignFromFile(const QString &fileName, QList<ElementConfig> &layout);
    void applyWeatherSettings();
    void checkWeatherWarnings(const WeatherData &weather);
    void applySlideshowSettings();

    WeatherManager *weatherManager;
    QTimer *weatherUpdateTimer;
    QTimer *newsUpdateTimer;
    QTimer *countdownTimer;
    QMediaPlayer *weatherWarningPlayer;
    bool weatherWarningActive;

    WeatherData lastWeatherData;

    QString getWeatherEmoji(const QString &description);
};

#endif

