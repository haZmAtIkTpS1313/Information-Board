#include "draggableelement.h"
#include <QApplication>

DraggableElement::DraggableElement(const QString &elementType, const QString &displayName,
                                   const QString &defaultText, QWidget *parent)
    : QLabel(parent)
    , elementType(elementType)
    , displayName(displayName)
    , defaultText(defaultText)
    , misPlaced(false)
    , row(0)
    , col(0)
    , rowSpan(1)
    , colSpan(1)
{
    setText(defaultText);
    setAlignment(Qt::AlignCenter);
    setMinimumSize(80, 60);
    setMaximumSize(150, 100);

    setStyleSheet(
        "DraggableElement {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: 2px solid #2980b9;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "DraggableElement:hover {"
        "   background-color: #2980b9;"
        "   border: 2px solid #1c5a8a;"
        "}"
        );

    setAcceptDrops(true);
}

void DraggableElement::setGridPosition(int row, int col, int rowSpan, int colSpan)
{
    row = row;
    col = col;
    rowSpan = rowSpan;
    colSpan = colSpan;
}

void DraggableElement::getGridPosition(int &row, int &col, int &rowSpan, int &colSpan) const
{
    row = row;
    col = col;
    rowSpan = rowSpan;
    colSpan = colSpan;
}

void DraggableElement::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
    }
    QLabel::mousePressEvent(event);
}

void DraggableElement::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        QLabel::mouseMoveEvent(event);
        return;
    }

    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        QLabel::mouseMoveEvent(event);
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText(elementType);
    mimeData->setData("application/x-element-type", elementType.toUtf8());
    mimeData->setData("application/x-display-name", displayName.toUtf8());
    mimeData->setData("application/x-default-text", defaultText.toUtf8());

    mimeData->setData("application/x-source-row", QString::number(row).toUtf8());
    mimeData->setData("application/x-source-col", QString::number(col).toUtf8());

    drag->setMimeData(mimeData);

    QPixmap pixmap(size());
    render(&pixmap);
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.fillRect(pixmap.rect(), QColor(0, 0, 0, 128));
    painter.end();

    drag->setPixmap(pixmap);
    drag->setHotSpot(dragStartPosition);

    drag->exec(Qt::MoveAction);
}

void DraggableElement::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        event->acceptProposedAction();

        setStyleSheet(
            "DraggableElement {"
            "   background-color: #27ae60;"
            "   color: white;"
            "   border: 3px solid #2ecc71;"
            "   border-radius: 5px;"
            "   padding: 5px;"
            "   font-size: 12px;"
            "   font-weight: bold;"
            "}"
            );
    } else {
        event->ignore();
    }
}

void DraggableElement::dropEvent(QDropEvent *event)
{
    setStyleSheet(
        "DraggableElement {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: 2px solid #2980b9;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        );

    if (event->mimeData()->hasFormat("application/x-element-type")) {
        QString sourceType = event->mimeData()->data("application/x-element-type");
        int sourceRow = event->mimeData()->data("application/x-source-row").toInt();
        int sourceCol = event->mimeData()->data("application/x-source-col").toInt();

        if (sourceType == elementType) {
            emit elementMoved(this, sourceRow, sourceCol, row, col);
        } else {
            event->ignore();
            return;
        }

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
