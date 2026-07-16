
#include "draggableelement.h"
#include <QApplication>
#include <QToolTip>
#include <QMenu>
#include <QContextMenuEvent>

DraggableElement::DraggableElement(const QString &elementType, const QString &displayName,
                                   const QString &defaultText, QWidget *parent)
    : QLabel(parent)
    , elementType(elementType)
    , displayName(displayName)
    , defaultText(defaultText)
    , misPlaced(false)
    , isDragging(false)
    , isResizing(false)
{
    setText(defaultText);
    setAlignment(Qt::AlignCenter);
    setMinimumSize(60, 40);
    // Максимальный размер намеренно не ограничиваем: размещённые элементы
    // должны уметь занимать любую часть доски (см. изменение размера за
    // нижний правый угол). Палитра слева задаёт себе фиксированный размер
    // отдельно через setFixedSize(), так что сюда это не относится.
    setMouseTracking(true);

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

bool DraggableElement::isInResizeCorner(const QPoint &pos) const
{
    const int gripSize = 16;
    return pos.x() >= width() - gripSize && pos.y() >= height() - gripSize;
}

void DraggableElement::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        if (misPlaced && isInResizeCorner(event->pos())) {
            isResizing = true;
            isDragging = false;
        } else {
            isResizing = false;
            isDragging = true;
        }
    }
    QLabel::mousePressEvent(event);
}

void DraggableElement::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        // Наводим указатель на уголок — подсказываем курсором, что элемент можно растянуть
        if (misPlaced && isInResizeCorner(event->pos())) {
            setCursor(Qt::SizeFDiagCursor);
        } else {
            unsetCursor();
        }
        QLabel::mouseMoveEvent(event);
        return;
    }

    if (!misPlaced) {
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
    } else if (isResizing) {
        // В отличие от перемещения, при ресайзе левый верхний угол виджета
        // НЕ двигается — значит event->pos() каждый раз даёт накопленное
        // смещение от точки нажатия, а не приращение с прошлого события.
        // Поэтому dragStartPosition обязательно сбрасываем после каждого
        // шага: иначе onElementResized (который трактует delta как
        // приращение и прибавляет его к текущему размеру) получал бы
        // растущую величину на каждое событие мыши и раздувал бы элемент
        // квадратично вместо линейного отклика на движение курсора.
        QPoint delta = event->pos() - dragStartPosition;
        if (delta.manhattanLength() > 0) {
            emit elementResized(this, delta.x(), delta.y());
            dragStartPosition = event->pos();
        }
    } else if (isDragging) {
        // Элемент уже размещён — перемещаем его в области дизайнера
        QPoint delta = event->pos() - dragStartPosition;
        if (delta.manhattanLength() > 0) {
            emit elementMoved(this, delta.x(), delta.y());
            // dragStartPosition не сбрасываем: позиция мыши внутри виджета
            // не меняется, пока сам виджет двигается вслед за курсором
        }
    }
}

void DraggableElement::mouseReleaseEvent(QMouseEvent *event)
{
    isDragging = false;
    isResizing = false;
    QLabel::mouseReleaseEvent(event);
}

void DraggableElement::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (misPlaced && event->button() == Qt::LeftButton && !isInResizeCorner(event->pos())) {
        emit elementDeleted(this);   // удаляем элемент с макета
        return;
    }
    QLabel::mouseDoubleClickEvent(event);
}

void DraggableElement::contextMenuEvent(QContextMenuEvent *event)
{
    if (!misPlaced) {
        QLabel::contextMenuEvent(event);
        return;
    }

    QMenu menu(this);
    QAction *propertiesAction = menu.addAction("Свойства элемента...");
    QAction *chosen = menu.exec(event->globalPos());
    if (chosen == propertiesAction) {
        emit elementPropertiesRequested(this);
    }
}

void DraggableElement::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void DraggableElement::dropEvent(QDropEvent *event)
{
    event->ignore(); // В новой архитектуре drop обрабатывается в BoardDesignerWidget
}
