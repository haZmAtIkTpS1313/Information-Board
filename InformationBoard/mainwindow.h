#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "boardwindow.h"
#include "boarddesignerwidget.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setBoardWindow(BoardWindow *window);

private slots:
    void on_showBoardButton_clicked();
    void on_fullscreenButton_clicked();
    void on_alwaysOnTopCheck_toggled(bool checked);

    void on_showTextButton_clicked();
    void on_clearBoardButton_clicked();

    void on_quickNewsButton_clicked();
    void on_quickWeatherButton_clicked();
    void on_quickTimeButton_clicked();
    void on_quickLogoButton_clicked();

    void on_setRedBackgroundButton_clicked();
    void on_setBlueBackgroundButton_clicked();
    void on_setGreenBackgroundButton_clicked();
    void on_setBlackBackgroundButton_clicked();
    void on_setCustomColorButton_clicked();

    void on_fontFamilyCombo_currentTextChanged(const QString &font);
    void on_boldTextCheck_toggled(bool checked);
    void on_italicTextCheck_toggled(bool checked);

    void on_textSmall_toggled(bool checked);
    void on_textMedium_toggled(bool checked);
    void on_textLarge_toggled(bool checked);
    void on_textHuge_toggled(bool checked);

    void on_toggleLogoButton_clicked();
    void on_toggleClockButton_clicked();
    void on_toggleWeatherButton_clicked();
    void on_toggleTickerButton_clicked();
    void on_setLogoImageButton_clicked();

    void on_saveConfigButton_clicked();
    void on_loadConfigButton_clicked();
    void on_resetBoardButton_clicked();

    void on_applyDesignButton_clicked();
    void on_resetDesignButton_clicked();
    void on_saveDesignButton_clicked();
    void on_loadDesignButton_clicked();
    void on_previewDesignButton_clicked();

private:
    Ui::MainWindow *ui;
    BoardWindow *boardWindow;
    BoardDesignerWidget *designerWidget;

    void setupConnections();
    void updateBoardStyle();
    void applyDesignToBoard();
};

#endif
