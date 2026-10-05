#ifndef BOARDDESIGNERWIDGET_H
#define BOARDDESIGNERWIDGET_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QGridLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QLine>
#include <QScrollArea>
#include <qboxlayout.h>
#include "draggableelement.h"

struct ShiftControll{
    QString id_element;
    int x;
    int y;
    int height;
    int weight;
    bool shift_flag = false;
};

struct ElementConfig {
    QString type;
    QString displayName;
    QString defaultText;
    QString style;
    int x;
    int y;
    int width;
    int height;
    bool visible;

    ElementConfig() : x(0), y(0), width(100), height(50), visible(true) {}
};

class BoardDesignerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardDesignerWidget(QWidget *parent = nullptr);
    ~BoardDesignerWidget();

    void addElement(const QString &elementType, const QString &displayName,
                    const QString &defaultText, int x, int y, int width, int height,
                    const QString &style = QString());
    void removeElement(const QString &elementType);
    void clearDesigner();

    QList<ElementConfig> getCurrentLayout() const;
    void applyLayout(const QList<ElementConfig> &layout);

signals:
    void layoutChanged();

public slots:
    void resetToDefaultLayout();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onElementMoved(DraggableElement *element, int deltaX, int deltaY);
    //void onElementResized(DraggableElement *element, int deltaWidth, int deltaHeight);
    void onElementGeometryChanged(DraggableElement *element, const QRect &newGeometry);
    void onElementDeleted(DraggableElement *element);
    void onElementPropertiesRequested(DraggableElement *element);
private:
    // === НОВЫЕ КОНСТАНТЫ ===
    static const int GRID_SIZE = 10;       // шаг сетки в пикселях
    static const int SNAP_THRESHOLD = 8;   // порог срабатывания snap

    // === ВЛОЖЕННЫЙ КЛАСС для области дизайнера ===
    // Рисуем сетку и линии выравнивания поверх элементов
    class DesignerArea : public QWidget {
    public:
        using QWidget::QWidget;
        void setActiveGuides(const QList<QLine> &guides) {
            activeGuides = guides;
            update();  // перерисовываем
        }
    protected:
        void paintEvent(QPaintEvent *event) override {
            QWidget::paintEvent(event);
            QPainter painter(this);
            painter.setRenderHint(QPainter::Antialiasing);

            // Рисуем сетку
            painter.setPen(QPen(QColor(255, 255, 255, 25), 1, Qt::DotLine));
            for (int x = 0; x <= width(); x += GRID_SIZE) {
                painter.drawLine(x, 0, x, height());
            }
            for (int y = 0; y <= height(); y += GRID_SIZE) {
                painter.drawLine(0, y, width(), y);
            }

            // Рисуем линии выравнивания (розовые пунктирные)
            if (!activeGuides.isEmpty()) {
                painter.setPen(QPen(QColor(255, 20, 147, 200), 2, Qt::DashLine));
                for (const QLine &line : activeGuides) {
                    painter.drawLine(line);
                }
            }
        }
    private:
        QList<QLine> activeGuides;
    };

    void setupUI();
    void createElementPalette();
    QList<QLine> findAlignmentGuides(DraggableElement *element, int newX, int newY,
                                     int &snappedX, int &snappedY);
    QList<QLine> findSizeGuides(DraggableElement *element, const QRect &newGeom, int &snappedWidth, int &snappedHeight);

    QScrollArea *paletteScrollArea;
    QWidget *paletteWidget;
    QVBoxLayout *paletteLayout;
    DesignerArea *designerArea;                    // <-- ТЕПЕРЬ DesignerArea, а не QWidget
    QMap<QString, DraggableElement*> elements;
    QMap<QString, DraggableElement*> placedElements;
};

#endif