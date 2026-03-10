#ifndef BOARDWINDOW_H
#define BOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include "boarddesignerwidget.h"

struct BoardElementConfig {
    bool visible= false;
    QString text;
    QString style;
    QPoint position;
    QSize size;
};

class BoardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BoardWindow(QWidget *parent = nullptr);
    ~BoardWindow();

    void showText(const QString &text);
    void showPermanentText(const QString &text);

    void setLogo(const QString &imagePath, int row = 0, int col = 0);
    void setClock(int row = 0, int col = 2, const QString &format = "hh:mm:ss");
    void setDate(int row = 1, int col = 2, const QString &format = "dd.MM.yyyy");
    void setWeather(const QString &weatherText, int row = 2, int col = 2);
    void setTicker(const QString &text, int row = 3, int col = 0, int colSpan = 3);

    void setBackgroundColor(const QColor &color);
    void setCustomStyle(const QString &style);
    void setElementStyle(const QString &elementName, const QString &style);
    void setElementPosition(const QString &elementName, int row, int col, int rowSpan = 1, int colSpan = 1);

    void showElement(const QString &elementName, bool show = true);
    void hideElement(const QString &elementName);

    void saveConfiguration(const QString &filename);
    void loadConfiguration(const QString &filename);

    void applyLayout(const QList<ElementLayoutConfig> &layout);

public slots:
    void clearScreen();
    void resetToDefaults();

    void updateClock();
    void updateDate();
    void updateWeather();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void setupUI();
    void createDefaultElements();
    void setupTimers();

    QLabel* createElement(const QString &name, const QString &defaultText,
                          int row, int col, int rowSpan = 1, int colSpan = 1);
    void applyElementStyle(const QString &name, const QString &style);
    void updateElementText(const QString &name, const QString &text);
    void clearAllElements();

    QWidget *centralWidget;
    QGridLayout *mainLayout;

    QMap<QString, QLabel*> elements;
    QMap<QString, BoardElementConfig> elementConfigs;

    QTimer *clockTimer;
    QTimer *dateTimer;
    QTimer *weatherTimer;

    QString currentStyle;
    QColor backgroundColor;

    static constexpr int GRID_ROWS = 5;
    static constexpr int GRID_COLS = 4;
};

#endif
