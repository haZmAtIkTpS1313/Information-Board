#include "boarddesignerwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>

BoardDesignerWidget::BoardDesignerWidget(QWidget *parent)
    : QWidget(parent)
    , designerLayout(nullptr)
    , paletteWidget(nullptr)
    , paletteLayout(nullptr)
    , gridRows(5)
    , gridCols(4)
    , highlightedRow(-1)
    , highlightedCol(-1)
{
    setupUI();
    createElementPalette();
    resetToDefaultLayout();

    setAcceptDrops(true);
    setMinimumSize(800, 600);
}

BoardDesignerWidget::~BoardDesignerWidget()
{
}

void BoardDesignerWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QLabel *titleLabel = new QLabel("🎨 Редактор макета доски", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   color: #2c3e50;"
        "   padding: 10px;"
        "   background-color: #ecf0f1;"
        "   border-radius: 5px;"
        "}"
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);

    paletteWidget = new QWidget(this);
    paletteWidget->setFixedWidth(150);
    paletteWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #f8f9fa;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 5px;"
        "}"
        );

    paletteLayout = new QGridLayout(paletteWidget);
    paletteLayout->setSpacing(10);
    paletteLayout->setContentsMargins(10, 10, 10, 10);
    paletteLayout->setAlignment(Qt::AlignTop);

    QLabel *paletteTitle = new QLabel("Доступные элементы", paletteWidget);
    paletteTitle->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   color: #2c3e50;"
        "   padding: 5px;"
        "   background-color: #bdc3c7;"
        "   border-radius: 3px;"
        "}"
        );
    paletteTitle->setAlignment(Qt::AlignCenter);
    paletteLayout->addWidget(paletteTitle, 0, 0, 1, 2);

    contentLayout->addWidget(paletteWidget);

    QWidget *designerArea = new QWidget(this);
    designerArea->setStyleSheet(
        "QWidget {"
        "   background-color: #2c3e50;"
        "   border: 3px solid #34495e;"
        "   border-radius: 5px;"
        "}"
        );

    QVBoxLayout *designerAreaLayout = new QVBoxLayout(designerArea);
    designerAreaLayout->setContentsMargins(20, 20, 20, 20);

    QWidget *gridContainer = new QWidget(designerArea);
    designerLayout = new QGridLayout(gridContainer);
    designerLayout->setSpacing(5);
    designerLayout->setContentsMargins(5, 5, 5, 5);

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            QLabel *cellLabel = new QLabel(gridContainer);
            cellLabel->setMinimumSize(CELL_WIDTH, CELL_HEIGHT);
            cellLabel->setStyleSheet(
                "QLabel {"
                "   background-color: #34495e;"
                "   border: 2px dashed #7f8c8d;"
                "   border-radius: 3px;"
                "   color: #95a5a6;"
                "   font-size: 10px;"
                "}"
                );
            cellLabel->setAlignment(Qt::AlignCenter);
            cellLabel->setText(QString("(%1,%2)").arg(row).arg(col));

            cellLabel->setProperty("row", row);
            cellLabel->setProperty("col", col);

            designerLayout->addWidget(cellLabel, row, col);
        }
    }

    for (int row = 0; row < gridRows; ++row) {
        designerLayout->setRowStretch(row, 1);
    }
    for (int col = 0; col < gridCols; ++col) {
        designerLayout->setColumnStretch(col, 1);
    }

    designerAreaLayout->addWidget(gridContainer);
    contentLayout->addWidget(designerArea, 1);

    mainLayout->addLayout(contentLayout, 1);
}

void BoardDesignerWidget::createElementPalette()
{
    struct ElementInfo {
        QString type;
        QString name;
        QString defaultText;
        int defaultRow;
        int defaultCol;
        int defaultRowSpan;
        int defaultColSpan;

        ElementInfo(QString t, QString n, QString txt, int r, int c, int rs = 1, int cs = 1)
            : type(t), name(n), defaultText(txt), defaultRow(r), defaultCol(c),
            defaultRowSpan(rs), defaultColSpan(cs) {}
    };

    QList<ElementInfo> elementsList = {
        {"logo", "Логотип", " Логотип", 0, 0},
        {"clock", "Часы", "12:00:00", 0, 2},
        {"date", "Дата", "01.01.2024", 1, 2},
        {"content", "Контент", "Текст", 1, 1},
        {"weather", "Погода", "☀️ +22°C", 2, 2},
        {"uvIndex", "УФ-индекс", "УФ: 3", 0, 1},
        {"airQuality", "Качество воздуха", "AQI: 50", 2, 1},
        {"wind", "Ветер", "3 м/с С", 3, 2},
        {"precipitation", "Осадки", "0 мм", 3, 1},
        {"humidity", "Влажность", "45%", 4, 1},
        {"pressure", "Давление", "1013 гПа", 4, 2},
        {"sunrise", "Восход", "06:30", 2, 0},
        {"sunset", "Закат", "20:30", 3, 0},
        {"ticker", "Бегущая строка", "Новости...", 4, 0, 1, 4}
    };

    int row = 1;
    int col = 0;

    for (const auto &elem : elementsList) {
        DraggableElement *element = new DraggableElement(
            elem.type,
            elem.name,
            elem.defaultText,
            paletteWidget
            );

        element->setFixedSize(130, 50);
        element->setText(elem.name);
        element->setStyleSheet(
            "DraggableElement {"
            "   background-color: #27ae60;"
            "   color: white;"
            "   border: 2px solid #229954;"
            "   border-radius: 5px;"
            "   font-size: 12px;"
            "   font-weight: bold;"
            "}"
            "DraggableElement:hover {"
            "   background-color: #229954;"
            "   border: 2px solid #1e8449;"
            "}"
            );


        elements[elem.type] = element;
        element->setPlaced(false);

        paletteLayout->addWidget(element, row, col);

        col++;
        if (col > 1) {
            col = 0;
            row++;
        }
    }
}

void BoardDesignerWidget::addElement(const QString &elementType, const QString &displayName,
                                     const QString &defaultText, int row, int col,
                                     int rowSpan, int colSpan)
{
    if (!isCellFree(row, col, rowSpan, colSpan)) {
        return;
    }

    DraggableElement *element = new DraggableElement(elementType, displayName, defaultText, this);
    element->setGridPosition(row, col, rowSpan, colSpan);
    element->setPlaced(true);

    if (elements.contains(elementType)) {
        DraggableElement *oldElement = elements[elementType];
        if (oldElement->isPlaced()) {
            designerLayout->removeWidget(oldElement);
            oldElement->deleteLater();
        }
    }

    elements[elementType] = element;

    designerLayout->addWidget(element, row, col, rowSpan, colSpan);

    for (int r = row; r < row + rowSpan; ++r) {
        for (int c = col; c < col + colSpan; ++c) {
            QString cellKey = QString("%1,%2").arg(r).arg(c);
            cellOccupancy[cellKey] = element;
        }
    }

    connect(element, &DraggableElement::elementMoved,
            this, &BoardDesignerWidget::onElementMoved);
    connect(element, &DraggableElement::elementDeleted,
            this, &BoardDesignerWidget::onElementDeleted);

    emit layoutChanged();
}

void BoardDesignerWidget::removeElement(const QString &elementType)
{
    if (!elements.contains(elementType)) return;

    DraggableElement *element = elements[elementType];

    if (element->isPlaced()) {
        int row, col, rowSpan, colSpan;
        element->getGridPosition(row, col, rowSpan, colSpan);

        for (int r = row; r < row + rowSpan; ++r) {
            for (int c = col; c < col + colSpan; ++c) {
                QString cellKey = QString("%1,%2").arg(r).arg(c);
                cellOccupancy.remove(cellKey);
            }
        }

        designerLayout->removeWidget(element);
    }

    element->deleteLater();
    elements.remove(elementType);

    emit layoutChanged();
}

void BoardDesignerWidget::clearDesigner()
{
    QList<QString> keys = elements.keys();
    for (const QString &key : keys) {
        DraggableElement *element = elements[key];
        if (element->isPlaced()) {
            designerLayout->removeWidget(element);
            element->deleteLater();
            elements.remove(key);
        }
    }

    cellOccupancy.clear();

    emit layoutChanged();
}

bool BoardDesignerWidget::isCellFree(int row, int col, int rowSpan, int colSpan) const
{
    for (int r = row; r < row + rowSpan; ++r) {
        for (int c = col; c < col + colSpan; ++c) {
            QString cellKey = QString("%1,%2").arg(r).arg(c);
            if (cellOccupancy.contains(cellKey)) {
                return false;
            }
        }
    }
    return true;
}

void BoardDesignerWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void BoardDesignerWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (!event->mimeData()->hasFormat("application/x-element-type")) {
        event->ignore();
        return;
    }

    QPoint pos = event->pos();

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            QLayoutItem *item = designerLayout->itemAtPosition(row, col);
            if (item && item->widget()) {
                QRect cellRect = item->widget()->geometry();
                if (cellRect.contains(pos)) {
                    highlightDropZone(row, col);
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }

    clearHighlight();
    event->ignore();
}

void BoardDesignerWidget::dropEvent(QDropEvent *event)
{
    clearHighlight();

    if (!event->mimeData()->hasFormat("application/x-element-type")) {
        event->ignore();
        return;
    }

    QString elementType = event->mimeData()->data("application/x-element-type");
    QString displayName = event->mimeData()->data("application/x-display-name");
    QString defaultText = event->mimeData()->data("application/x-default-text");

    QPoint pos = event->pos();
    int targetRow = -1;
    int targetCol = -1;

    for (int row = 0; row < gridRows; ++row) {
        for (int col = 0; col < gridCols; ++col) {
            QLayoutItem *item = designerLayout->itemAtPosition(row, col);
            if (item && item->widget()) {
                QRect cellRect = item->widget()->geometry();
                if (cellRect.contains(pos)) {
                    targetRow = row;
                    targetCol = col;
                    break;
                }
            }
        }
        if (targetRow != -1) break;
    }

    if (targetRow == -1 || targetCol == -1) {
        event->ignore();
        return;
    }

    if (!isCellFree(targetRow, targetCol)) {
        event->ignore();
        return;
    }

    addElement(elementType, displayName, defaultText, targetRow, targetCol);

    event->acceptProposedAction();
    emit layoutChanged();
}

void BoardDesignerWidget::highlightDropZone(int row, int col)
{
    if (highlightedRow == row && highlightedCol == col) return;

    clearHighlight();

    QLayoutItem *item = designerLayout->itemAtPosition(row, col);
    if (item && item->widget()) {
        QLabel *cell = qobject_cast<QLabel*>(item->widget());
        if (cell) {
            cell->setStyleSheet(
                "QLabel {"
                "   background-color: #27ae60;"
                "   border: 3px solid #2ecc71;"
                "   border-radius: 3px;"
                "   color: white;"
                "   font-size: 10px;"
                "}"
                );
            highlightedRow = row;
            highlightedCol = col;
        }
    }
}

void BoardDesignerWidget::clearHighlight()
{
    if (highlightedRow != -1 && highlightedCol != -1) {
        QLayoutItem *item = designerLayout->itemAtPosition(highlightedRow, highlightedCol);
        if (item && item->widget()) {
            QLabel *cell = qobject_cast<QLabel*>(item->widget());
            if (cell) {
                cell->setStyleSheet(
                    "QLabel {"
                    "   background-color: #34495e;"
                    "   border: 2px dashed #7f8c8d;"
                    "   border-radius: 3px;"
                    "   color: #95a5a6;"
                    "   font-size: 10px;"
                    "}"
                    );
            }
        }
        highlightedRow = -1;
        highlightedCol = -1;
    }
}

void BoardDesignerWidget::onElementMoved(DraggableElement *element, int fromRow, int fromCol, int toRow, int toCol)
{
    int row, col, rowSpan, colSpan;
    element->getGridPosition(row, col, rowSpan, colSpan);

    if (!isCellFree(toRow, toCol, rowSpan, colSpan)) {
        return;
    }

    for (int r = fromRow; r < fromRow + rowSpan; ++r) {
        for (int c = fromCol; c < fromCol + colSpan; ++c) {
            QString cellKey = QString("%1,%2").arg(r).arg(c);
            cellOccupancy.remove(cellKey);
        }
    }

    element->setGridPosition(toRow, toCol, rowSpan, colSpan);

    for (int r = toRow; r < toRow + rowSpan; ++r) {
        for (int c = toCol; c < toCol + colSpan; ++c) {
            QString cellKey = QString("%1,%2").arg(r).arg(c);
            cellOccupancy[cellKey] = element;
        }
    }

    designerLayout->addWidget(element, toRow, toCol, rowSpan, colSpan);

    emit layoutChanged();
}

void BoardDesignerWidget::onElementDeleted(DraggableElement *element)
{
    QString elementType = element->getElementType();
    removeElement(elementType);
}

QList<ElementLayoutConfig> BoardDesignerWidget::getCurrentLayout() const
{
    QList<ElementLayoutConfig> layout;

    for (auto it = elements.begin(); it != elements.end(); ++it) {
        DraggableElement *element = it.value();
        if (element->isPlaced()) {
            ElementLayoutConfig config;
            config.elementType = element->getElementType();
            config.displayName = element->getDisplayName();
            config.defaultText = element->getDefaultText();
            element->getGridPosition(config.row, config.col, config.rowSpan, config.colSpan);
            config.visible = element->isVisible();

            layout.append(config);
        }
    }

    return layout;
}

void BoardDesignerWidget::applyLayout(const QList<ElementLayoutConfig> &layout)
{
    clearDesigner();

    for (const auto &config : layout) {
        addElement(config.elementType, config.displayName, config.defaultText,
                   config.row, config.col, config.rowSpan, config.colSpan);
    }
}

void BoardDesignerWidget::resetToDefaultLayout()
{
    clearDesigner();

    addElement("logo", "Логотип", "🏢 Логотип", 0, 0);
    addElement("clock", "Часы", "12:00:00", 0, 2);
    addElement("date", "Дата", "01.01.2024", 1, 2);
    addElement("content", "Контент", "Текст", 1, 1);
    addElement("weather", "Погода", "☀️ +22°C", 2, 2);
    addElement("uvIndex", "УФ-индекс", "УФ: 3", 0, 1);
    addElement("airQuality", "Качество воздуха", "AQI: 50", 2, 1);
    addElement("wind", "Ветер", "3 м/с С", 3, 2);
    addElement("precipitation", "Осадки", "0 мм", 3, 1);
    addElement("humidity", "Влажность", "45%", 4, 1);
    addElement("pressure", "Давление", "1013 гПа", 4, 2);
    addElement("sunrise", "Восход", "06:30", 2, 0);
    addElement("sunset", "Закат", "20:30", 3, 0);
    addElement("ticker", "Бегущая строка", "Новости...", 4, 0, 1, 4);

    emit layoutChanged();
}

void BoardDesignerWidget::setGridSize(int rows, int cols)
{
    gridRows = rows;
    gridCols = cols;
}

void BoardDesignerWidget::clearCell(int row, int col)
{
    QString cellKey = QString("%1,%2").arg(row).arg(col);
    if (cellOccupancy.contains(cellKey)) {
        DraggableElement *element = cellOccupancy[cellKey];
        removeElement(element->getElementType());
    }
}
