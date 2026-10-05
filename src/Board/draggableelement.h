
#ifndef DRAGGABLEELEMENT_H
#define DRAGGABLEELEMENT_H

#include <QLabel>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QPaintEvent>
#include <qcoreevent.h>
#include <qevent.h>
#include <QRect>

class DraggableElement : public QLabel
{
    Q_OBJECT

public:
    enum class ResizeHandle {
        None, 
        TopLeft, Top, TopRight,
        Left, Right,
        BottomLeft, Bottom, BottomRight
    };

    explicit DraggableElement(const QString &elementType, const QString &displayName,
                              const QString &defaultText, QWidget *parent = nullptr);

    QString getElementType() const { return elementType; }
    QString getDisplayName() const { return displayName; }
    QString getDefaultText() const { return defaultText; }
    void setDefaultText(const QString &text) { defaultText = text; }

    // Стиль (цвет фона/текста, шрифт), который будет применён к элементу на
    // ЖИВОЙ доске. Внешний вид самого элемента в редакторе не меняется —
    // там всегда используется единая палитра для наглядности редактирования.
    QString getCustomStyle() const { return customStyle; }
    void setCustomStyle(const QString &style) { customStyle = style; }

    bool isPlaced() const { return misPlaced; }
    void setPlaced(bool placed) { misPlaced = placed; }

signals:
    void elementMoved(DraggableElement *element, int deltaX, int deltaY);
    void elementResized(DraggableElement *element, int deltaWidth, int deltaHeight);
    void elementGeometryChanged(DraggableElement *element, const QRect &newGeometry);
    void elementDeleted(DraggableElement *element);
    void elementPropertiesRequested(DraggableElement *element);
    void elementReleased(DraggableElement *element);
    

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override; 
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;  
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    

private:
    bool isInResizeCorner(const QPoint &pos) const;
    void updateCursorForHandle(ResizeHandle handle);

    QString elementType;
    QString displayName;
    QString defaultText;
    QString customStyle;
    bool misPlaced;
    QPoint dragStartPosition;
    bool isDragging;
    bool isResizing;
    ResizeHandle handleAt(const QPoint &pos) const;
    ResizeHandle activeHandle;
    QRect originalGeometry;
    bool showHandles;

};

#endif

