#ifndef BOARDWINDOW_H
#define BOARDWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include <QFont>
#include "boarddesignerwidget.h"

class QMediaPlayer;

    class BoardWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BoardWindow(QWidget *parent = nullptr);
    ~BoardWindow();

    void showText(const QString &text);
    void showPermanentText(const QString &text);

    void setLogo(const QString &imagePath, int x = 20, int y = 20, int width = 120, int height = 80);
    void setSlideshowImage(const QString &imagePath);
    void setClock(int x = 660, int y = 20, int width = 120, int height = 80);
    void setDate(int x = 660, int y = 110, int width = 120, int height = 80);
    void setWeather(const QString &weatherText, int x = 660, int y = 200, int width = 120, int height = 80);
    void setTicker(const QString &text, int x = 20, int y = 560, int width = 760, int height = 60);

    // Публичный доступ к тексту любого элемента (используется консолью для живых метео-плиток)
    void setElementText(const QString &elementName, const QString &text);

    void setBackgroundColor(const QColor &color);
    void setCustomStyle(const QString &style);
    void setElementStyle(const QString &elementName, const QString &style);
    void setElementPosition(const QString &elementName, int x, int y, int width, int height);

    void showElement(const QString &elementName, bool show = true);
    void hideElement(const QString &elementName);

    void saveConfiguration(const QString &filename);
    void loadConfiguration(const QString &filename);

    void applyLayout(const QList<ElementConfig> &layout);

    // Режим "захвата экрана": все обычные плитки прячутся, доска показывает
    // ОДНО сообщение по центру доски (используется и для показа таблицы из БД,
    // и как основа экстренного режима). Размер плашки считается по содержимому
    // (шрифту и тексту), а не растягивается на весь экран — центрируется и
    // подгоняется под размер текста, в пределах разумных минимума/максимума.
    void showTakeoverContent(const QString &text, const QString &style,
                              Qt::Alignment alignment, const QFont &font,
                              bool fillBoard = false);
    void exitTakeoverMode();
    bool isTakeoverActive() const { return takeoverActive; }

    // Экстренный режим: мигающий красно-чёрный фон, крупное сообщение и (опционально) зацикленная сирена.
    // Реализован поверх showTakeoverContent/exitTakeoverMode — прячет все плитки так же, как показ таблицы.
    void setEmergencyMode(bool active, const QString &message = QString(), const QString &soundPath = QString());
    bool isEmergencyActive() const { return emergencyActive; }

signals:
    // Испускается всегда, когда состояние реально меняется — в том числе если
    // тревогу выключил не пользователь кнопкой, а сама доска (applyLayout/
    // resetToDefaults/loadConfiguration). Консоль слушает этот сигнал, чтобы
    // кнопка "Экстренный режим" никогда не показывала неверное состояние.
    void emergencyModeChanged(bool active);

public slots:
    void clearScreen();
    void resetToDefaults();

    void updateClock();
    void updateDate();
    void updateTicker();   // анимация бегущей строки

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;   // держит плашку захвата экрана во весь экран
    // closeEvent убран: только звал базовую реализацию без своей логики

private slots:
    void onEmergencyBlink();

private:
    void setupUI();
    void createDefaultElements();
    void setupTimers();

    QLabel* createElement(const QString &name, const QString &defaultText,
                          int x, int y, int width, int height);
    void updateElementText(const QString &name, const QString &text);
    void clearAllElements();
    void applySharedStyle(const QString &style);
    void updateTakeoverLabelGeometry();
    QString defaultTakeoverStyle() const;
    QFont defaultTakeoverFont() const;

    QWidget *centralWidget;
    QMap<QString, QLabel*> elements;
    QMap<QString, ElementConfig> elementConfigs;

    QTimer *clockTimer;
    QTimer *dateTimer;
    QTimer *tickerTimer;

    QString tickerText;   // полный текст для прокрутки

    QString currentStyle;
    QColor backgroundColor;

    // Захват экрана — общий механизм и для показа таблицы, и для экстренного режима
    QLabel *takeoverLabel;
    bool takeoverActive;
    bool takeoverFillBoard;   // true — плашка растягивается почти на всю доску (таблица), false — по размеру содержимого (тревога)
    QMap<QString, bool> preTakeoverVisibility;   // видимость плиток до захвата — чтобы вернуть как было

    QTimer *emergencyBlinkTimer;
    QMediaPlayer *emergencyPlayer;
    bool emergencyActive;
    bool emergencyBlinkState;
    QColor preEmergencyColor;
};

#endif