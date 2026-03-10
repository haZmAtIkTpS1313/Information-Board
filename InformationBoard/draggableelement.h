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
    void setGridPosition(int row, int col, int rowSpan = 1, int colSpan = 1);
    void getGridPosition(int &row, int &col, int &rowSpan, int &colSpan) const;

    bool isPlaced() const { return misPlaced; }
    void setPlaced(bool placed) { misPlaced = placed; }

signals:
    void elementMoved(DraggableElement *element, int fromRow, int fromCol, int toRow, int toCol);
    void elementDeleted(DraggableElement *element);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString elementType;
    QString displayName;
    QString defaultText;
    bool misPlaced;
    QPoint dragStartPosition;

    int row;
    int col;
    int rowSpan;
    int colSpan;
};

#endif
