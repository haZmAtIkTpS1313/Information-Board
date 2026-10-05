#include "draggableelement.h"
#include <QApplication>
#include <QToolTip>
#include <QMenu>
#include <QContextMenuEvent>
#include <QDebug>

static const int HANDLE_SIZE = 8;       // визуальный размер ручки
static const int HANDLE_HIT_AREA = 10;  // зона попадания мыши (чуть больше визуальной)

DraggableElement::DraggableElement(const QString &elementType, const QString &displayName,
                                   const QString &defaultText, QWidget *parent)
    : QLabel(parent)
    , elementType(elementType)
    , displayName(displayName)
    , defaultText(defaultText)
    , misPlaced(false)
    , isDragging(false)
    , isResizing(false)
    , activeHandle(ResizeHandle::None)
    , showHandles(false)
{
    setText(defaultText);
    setAlignment(Qt::AlignCenter);
    setMinimumSize(60, 40);
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

// Определяем, в какую зону попал курсор
DraggableElement::ResizeHandle DraggableElement::handleAt(const QPoint &pos) const
{
    int w = width();
    int h = height();
    int hs = HANDLE_HIT_AREA;

    // Углы
    if (pos.x() < hs && pos.y() < hs) return ResizeHandle::TopLeft;
    if (pos.x() > w - hs && pos.y() < hs) return ResizeHandle::TopRight;
    if (pos.x() < hs && pos.y() > h - hs) return ResizeHandle::BottomLeft;
    if (pos.x() > w - hs && pos.y() > h - hs) return ResizeHandle::BottomRight;

    // Стороны
    if (pos.y() < hs) return ResizeHandle::Top;
    if (pos.y() > h - hs) return ResizeHandle::Bottom;
    if (pos.x() < hs) return ResizeHandle::Left;
    if (pos.x() > w - hs) return ResizeHandle::Right;

    return ResizeHandle::None;
}

void DraggableElement::updateCursorForHandle(ResizeHandle handle)
{
    switch (handle) {
        case ResizeHandle::TopLeft:
        case ResizeHandle::BottomRight:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case ResizeHandle::TopRight:
        case ResizeHandle::BottomLeft:
            setCursor(Qt::SizeBDiagCursor);
            break;
        case ResizeHandle::Top:
        case ResizeHandle::Bottom:
            setCursor(Qt::SizeVerCursor);
            break;
        case ResizeHandle::Left:
        case ResizeHandle::Right:
            setCursor(Qt::SizeHorCursor);
            break;
        default:
            unsetCursor();
            break;
    }
}

void DraggableElement::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();

        if (misPlaced) {
            ResizeHandle handle = handleAt(event->pos());
            if (handle != ResizeHandle::None) {
                isResizing = true;
                isDragging = false;
                activeHandle = handle;
                originalGeometry = geometry();  // запоминаем исходную геометрию
            } else {
                isResizing = false;
                isDragging = true;
                activeHandle = ResizeHandle::None;
            }
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
        // Наведение курсора — показываем соответствующий курсор
        if (misPlaced) {
            ResizeHandle handle = handleAt(event->pos());
            updateCursorForHandle(handle);
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
        // Вычисляем новую геометрию на основе исходной и дельты мыши
        QPoint delta = event->pos() - dragStartPosition;
        QRect newGeom = originalGeometry;

        switch (activeHandle) {
            case ResizeHandle::TopLeft:
                newGeom.setTopLeft(originalGeometry.topLeft() + delta);
                break;
            case ResizeHandle::Top:
                newGeom.setTop(originalGeometry.top() + delta.y());
                break;
            case ResizeHandle::TopRight:
                newGeom.setTopRight(originalGeometry.topRight() + delta);
                break;
            case ResizeHandle::Left:
                newGeom.setLeft(originalGeometry.left() + delta.x());
                break;
            case ResizeHandle::Right:
                newGeom.setRight(originalGeometry.right() + delta.x());
                break;
            case ResizeHandle::BottomLeft:
                newGeom.setBottomLeft(originalGeometry.bottomLeft() + delta);
                break;
            case ResizeHandle::Bottom:
                newGeom.setBottom(originalGeometry.bottom() + delta.y());
                break;
            case ResizeHandle::BottomRight:
                newGeom.setBottomRight(originalGeometry.bottomRight() + delta);
                break;
            default:
                break;
        }

        // Не даём элементу сжаться меньше минимального размера
        if (newGeom.width() < minimumWidth()) {
            if (activeHandle == ResizeHandle::Left || activeHandle == ResizeHandle::TopLeft || activeHandle == ResizeHandle::BottomLeft) {
                newGeom.setLeft(originalGeometry.right() - minimumWidth());
            } else {
                newGeom.setWidth(minimumWidth());
            }
        }
        if (newGeom.height() < minimumHeight()) {
            if (activeHandle == ResizeHandle::Top || activeHandle == ResizeHandle::TopLeft || activeHandle == ResizeHandle::TopRight) {
                newGeom.setTop(originalGeometry.bottom() - minimumHeight());
            } else {
                newGeom.setHeight(minimumHeight());
            }
        }

        emit elementGeometryChanged(this, newGeom);

    } else if (isDragging) {
        QPoint delta = event->pos() - dragStartPosition;
        if (delta.manhattanLength() > 0) {
            emit elementMoved(this, delta.x(), delta.y());
        }
    }
}

void DraggableElement::mouseReleaseEvent(QMouseEvent *event)
{
    if (misPlaced && (isDragging || isResizing)) {
        emit elementReleased(this);
    }
    isDragging = false;
    isResizing = false;
    activeHandle = ResizeHandle::None;
    QLabel::mouseReleaseEvent(event);
}

void DraggableElement::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (misPlaced && event->button() == Qt::LeftButton && handleAt(event->pos()) == ResizeHandle::None) {
        emit elementDeleted(this);
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
    event->ignore();
}

// Отрисовка ручек изменения размера
void DraggableElement::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    if (!misPlaced || !showHandles) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor handleColor(255, 255, 255);
    QColor borderColor(41, 128, 185);

    painter.setBrush(handleColor);
    painter.setPen(QPen(borderColor, 1));

    int hs = HANDLE_SIZE;
    int half = hs / 2;
    int w = width();
    int h = height();

    // 8 ручек: углы и середины сторон
    QList<QPoint> handlePositions = {
        QPoint(0, 0),             // TopLeft
        QPoint(w/2, 0),           // Top
        QPoint(w, 0),             // TopRight
        QPoint(0, h/2),           // Left
        QPoint(w, h/2),           // Right
        QPoint(0, h),             // BottomLeft
        QPoint(w/2, h),           // Bottom
        QPoint(w, h)              // BottomRight
    };

    for (const QPoint &pos : handlePositions) {
        painter.drawRect(pos.x() - half, pos.y() - half, hs, hs);
    }
}

void DraggableElement::enterEvent(QEvent *event)
{
    if (misPlaced) {
        showHandles = true;
        update();
    }
    QLabel::enterEvent(event);
}

void DraggableElement::leaveEvent(QEvent *event)
{
    if (!isResizing && !isDragging) {
        showHandles = false;
        update();
    }
    QLabel::leaveEvent(event);
}