
#ifndef DRAGGABLEELEMENT_H
#define DRAGGABLEELEMENT_H

#include <QLabel>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QPainter>

class DraggableElement : public QLabel
{
    Q_OBJECT

public:
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
    void elementDeleted(DraggableElement *element);
    void elementPropertiesRequested(DraggableElement *element);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;   // двойной клик — удалить с макета
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;  // правый клик — свойства элемента

private:
    bool isInResizeCorner(const QPoint &pos) const;

    QString elementType;
    QString displayName;
    QString defaultText;
    QString customStyle;
    bool misPlaced;
    QPoint dragStartPosition;
    bool isDragging;
    bool isResizing;
};

#endif

