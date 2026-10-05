#include "boarddesignerwidget.h"
#include "elementpropertiesdialog.h"
#include "src/Board/draggableelement.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <qglobal.h>
#include <qpaintdevice.h>

BoardDesignerWidget::BoardDesignerWidget(QWidget *parent)
    : QWidget(parent)
    , paletteWidget(nullptr)
    , paletteLayout(nullptr)
    , designerArea(nullptr)
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

    QLabel *titleLabel = new QLabel(
        "🎨 Редактор макета доски — перетащить: переместить · уголок: изменить размер · "
        "ПКМ: свойства (цвет, шрифт, текст) · двойной клик: удалить", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 16px;"
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

    // === ПАНЕЛЬ ЭЛЕМЕНТОВ (слева, с прокруткой) ===
    paletteWidget = new QWidget();
    paletteWidget->setStyleSheet(
        "QWidget {"
        "   background-color: #f8f9fa;"
        "}"
    );
    paletteLayout = new QVBoxLayout(paletteWidget);
    paletteLayout->setSpacing(8);
    paletteLayout->setContentsMargins(10, 10, 10, 10);
    paletteLayout->setAlignment(Qt::AlignTop);

    // Заголовок палитры
    QLabel *paletteTitle = new QLabel("Доступные элементы", paletteWidget);
    paletteTitle->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   color: #2c3e50;"
        "   padding: 8px;"
        "   background-color: #bdc3c7;"
        "   border-radius: 4px;"
        "}"
    );
    paletteTitle->setAlignment(Qt::AlignCenter);
    paletteLayout->addWidget(paletteTitle);

    // Скролл-область для палитры
    paletteScrollArea = new QScrollArea(this);
    paletteScrollArea->setWidget(paletteWidget);
    paletteScrollArea->setWidgetResizable(true);
    paletteScrollArea->setFixedWidth(180);
    paletteScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    paletteScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    paletteScrollArea->setStyleSheet(
        "QScrollArea {"
        "   background-color: #f8f9fa;"
        "   border: 2px solid #bdc3c7;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar:vertical {"
        "   background: #ecf0f1;"
        "   width: 12px;"
        "   border-radius: 6px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #95a5a6;"
        "   border-radius: 6px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #7f8c8d;"
        "}"
    );

    contentLayout->addWidget(paletteScrollArea);

    // === ОБЛАСТЬ ДИЗАЙНЕРА (справа) ===
    designerArea = new DesignerArea(this);
    designerArea->setStyleSheet(
        "QWidget {"
        "   background-color: #2c3e50;"
        "   border: 3px solid #34495e;"
        "   border-radius: 5px;"
        "}"
    );
    designerArea->setAcceptDrops(true);
    designerArea->setMouseTracking(true);

    contentLayout->addWidget(designerArea, 1);

    mainLayout->addLayout(contentLayout, 1);
}

void BoardDesignerWidget::createElementPalette()
{
    struct ElementInfo {
        QString type;
        QString name;
        QString defaultText;
        int defaultWidth;
        int defaultHeight;
        ElementInfo(QString t, QString n, QString txt, int w, int h)
            : type(t), name(n), defaultText(txt), defaultWidth(w), defaultHeight(h) {}
    };

    QList<ElementInfo> elementsList = {
        {"logo", "Логотип", "🏢 Логотип", 120, 80},
        {"clock", "Часы", "12:00:00", 120, 80},
        {"date", "Дата", "01.01.2024", 120, 80},
        {"slideshow", "Слайдшоу", "🖼️ Слайдшоу", 440, 120},
        {"content", "Контент", "Текст", 200, 100},
        {"weather", "Погода", "☀️ +22°C", 120, 80},
        {"wind", "Ветер", "3 м/с С", 120, 80},
        {"humidity", "Влажность", "45%", 100, 70},
        {"pressure", "Давление", "1013 гПа", 100, 70},
        {"countdown", "Обратный отсчёт", "До события: --:--:--", 240, 70},
        {"announcement", "Объявление", "Объявлений пока нет", 480, 70},
        {"ticker", "Бегущая строка", "Новости...", 400, 60}
    };

    for (const auto &elem : elementsList) {
        DraggableElement *element = new DraggableElement(
            elem.type, elem.name, elem.defaultText, paletteWidget
        );
        element->setFixedHeight(45);       // высота одной строки
        element->setMinimumWidth(150);     // ширина под панель
        element->setText(elem.name);
        element->setStyleSheet(
            "DraggableElement {"
            "   background-color: #27ae60;"
            "   color: white;"
            "   border: 2px solid #229954;"
            "   border-radius: 5px;"
            "   font-size: 13px;"
            "   font-weight: bold;"
            "   padding: 5px;"
            "}"
            "DraggableElement:hover {"
            "   background-color: #229954;"
            "   border: 2px solid #1e8449;"
            "}"
        );
        element->setPlaced(false);
        elements[elem.type] = element;

        paletteLayout->addWidget(element);   // по одному в строку
    }

    // Растягивающий spacer внизу, чтобы элементы были прижаты к верху
    paletteLayout->addStretch();
}

void BoardDesignerWidget::addElement(const QString &elementType, const QString &displayName,
                                     const QString &defaultText, int x, int y, int width, int height,
                                     const QString &style)
{
    if (placedElements.contains(elementType)) {
        removeElement(elementType);
    }

    DraggableElement *element = new DraggableElement(elementType, displayName, defaultText, designerArea);
    element->setGeometry(x, y, width, height);
    element->setPlaced(true);
    element->setVisible(true);
    element->setCustomStyle(style);
    element->setToolTip("Перетащить — переместить · за уголок — изменить размер\nПКМ — свойства (цвет/шрифт) · двойной клик — удалить");
    // Собственный вид элемента в редакторе намеренно не зависит от custom style —
    // он всегда в единой палитре, чтобы плитки было удобно различать и хватать
    // мышью. custom style уходит на живую доску при "Применить дизайн".
    element->setStyleSheet(
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

    placedElements[elementType] = element;
    elements[elementType] = element;

    connect(element, &DraggableElement::elementMoved,
            this, &BoardDesignerWidget::onElementMoved);
    /*connect(element, &DraggableElement::elementResized,
            this, &BoardDesignerWidget::onElementResized);*/
    connect(element, &DraggableElement::elementGeometryChanged,
        this, &BoardDesignerWidget::onElementGeometryChanged);
    connect(element, &DraggableElement::elementDeleted,
            this, &BoardDesignerWidget::onElementDeleted);
    connect(element, &DraggableElement::elementPropertiesRequested,
            this, &BoardDesignerWidget::onElementPropertiesRequested);
            
    connect(element, &DraggableElement::elementReleased, this, [this](DraggableElement *) {
        designerArea->setActiveGuides(QList<QLine>());
    });
    emit layoutChanged();
}

void BoardDesignerWidget::removeElement(const QString &elementType)
{
    if (!placedElements.contains(elementType)) return;

    DraggableElement *element = placedElements[elementType];
    element->deleteLater();
    placedElements.remove(elementType);
    if (elements.value(elementType) == element) elements.remove(elementType);

    emit layoutChanged();
}

void BoardDesignerWidget::clearDesigner()
{
    for (auto it = placedElements.begin(); it != placedElements.end(); ++it) {
        it.value()->deleteLater();
    }
    placedElements.clear();
    emit layoutChanged();
}

QList<ElementConfig> BoardDesignerWidget::getCurrentLayout() const
{
    QList<ElementConfig> layout;
    for (auto it = placedElements.begin(); it != placedElements.end(); ++it) {
        DraggableElement *element = it.value();
        ElementConfig cfg;
        cfg.type = element->getElementType();
        cfg.displayName = element->getDisplayName();
        cfg.defaultText = element->getDefaultText();
        QRect geom = element->geometry();
        cfg.x = geom.x();
        cfg.y = geom.y();
        cfg.width = geom.width();
        cfg.height = geom.height();
        cfg.visible = element->isVisible();
        cfg.style = element->getCustomStyle();
        layout.append(cfg);
    }
    return layout;
}

void BoardDesignerWidget::applyLayout(const QList<ElementConfig> &layout)
{
    clearDesigner();
    for (const auto &cfg : layout) {
        addElement(cfg.type, cfg.displayName, cfg.defaultText,
                   cfg.x, cfg.y, cfg.width, cfg.height, cfg.style);
    }
}

void BoardDesignerWidget::resetToDefaultLayout()
{
    clearDesigner();

    addElement("logo", "Логотип", "🏢 Логотип", 20, 20, 120, 80);
    addElement("clock", "Часы", "12:00:00", 660, 20, 120, 80);
    addElement("date", "Дата", "01.01.2024", 660, 110, 120, 80);
    addElement("slideshow", "Слайдшоу", "🖼️ Слайдшоу", 180, 20, 440, 120);
    addElement("content", "Контент", "Доска готова к работе", 180, 150, 440, 120);
    addElement("weather", "Погода", "☀️ +22°C", 660, 200, 120, 80);
    // Заготовки под данные, которые пока не получают реальных значений ни от одного API:
    // addElement("uvIndex", "УФ-индекс", "УФ: 3", 20, 110, 100, 70);
    // addElement("airQuality", "Качество воздуха", "AQI: 50", 20, 200, 120, 80);
    addElement("wind", "Ветер", "3 м/с С", 660, 290, 120, 80);
    // addElement("precipitation", "Осадки", "0 мм", 660, 380, 100, 70);
    addElement("humidity", "Влажность", "45%", 20, 380, 100, 70);
    addElement("pressure", "Давление", "1013 гПа", 140, 380, 100, 70);
    // addElement("sunrise", "Восход", "06:30", 20, 470, 100, 70);
    // addElement("sunset", "Закат", "20:30", 140, 470, 100, 70);
    addElement("countdown", "Обратный отсчёт", "До события: --:--:--", 20, 470, 240, 70);
    addElement("announcement", "Объявление", "Объявлений пока нет", 280, 470, 480, 70);
    addElement("ticker", "Бегущая строка", "Новости...", 20, 560, 760, 60);

    emit layoutChanged();
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
    if (event->mimeData()->hasFormat("application/x-element-type")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void BoardDesignerWidget::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat("application/x-element-type")) {
        event->ignore();
        return;
    }

    QString elementType = QString::fromUtf8(event->mimeData()->data("application/x-element-type"));
    QString displayName = QString::fromUtf8(event->mimeData()->data("application/x-display-name"));
    QString defaultText = QString::fromUtf8(event->mimeData()->data("application/x-default-text"));

    QPoint pos = event->pos();
    QPoint designerPos = designerArea->mapFromParent(pos);
    if (!designerArea->rect().contains(designerPos)) {
        event->ignore();
        return;
    }

    int defaultWidth = 120;
    int defaultHeight = 80;
    if (elementType == "content") { defaultWidth = 200; defaultHeight = 100; }
    if (elementType == "ticker") { defaultWidth = 400; defaultHeight = 60; }
    if (elementType == "countdown") { defaultWidth = 240; defaultHeight = 70; }
    if (elementType == "announcement") { defaultWidth = 480; defaultHeight = 70; }
    if (elementType == "slideshow") { defaultWidth = 440; defaultHeight = 120; }

    addElement(elementType, displayName, defaultText,
               designerPos.x(), designerPos.y(), defaultWidth, defaultHeight);

    event->acceptProposedAction();
    emit layoutChanged();
}

void BoardDesignerWidget::onElementMoved(DraggableElement *element, int deltaX, int deltaY)
{
    if (!element) return;
    QRect geom = element->geometry();

    int newX = geom.x() + deltaX;
    int newY = geom.y() + deltaY;

    int maxX = designerArea->width() - geom.width();
    int maxY = designerArea->height() - geom.height();
    newX = qBound(0, newX, qMax(0, maxX));
    newY = qBound(0, newY, qMax(0, maxY));

    int gridX = qRound(double(newX) / GRID_SIZE) * GRID_SIZE;
    int gridY = qRound(double(newY) / GRID_SIZE) * GRID_SIZE;

    int finalX = newX;
    int finalY = newY;
    QList<QLine> guides = findAlignmentGuides(element, newX, newY, finalX, finalY);

    if (guides.isEmpty()) {
        finalX = gridX;
        finalY = gridY;
    }

    element->setGeometry(finalX, finalY, geom.width(), geom.height());

    designerArea->setActiveGuides(guides);

    emit layoutChanged();
}

QList<QLine> BoardDesignerWidget::findAlignmentGuides(DraggableElement *element, int newX, int newY, int &snappedX, int &snappedY) {
    QList<QLine> guides; 
    QRect geom = element->geometry();
    snappedX = newX;
    snappedY = newY;

    bool snappedXFlag = false;
    bool snappedYFlag = false;

    for (auto it = placedElements.begin(); it != placedElements.end(); ++it) {
        DraggableElement *other = it.value();
        if (other == element) continue;

        QRect og = other->geometry();

        // ВЕРТИКАЛЬНАЯ ЛИНИЯ

        if(!snappedXFlag && qAbs(newX - og.left()) < SNAP_THRESHOLD) {
            snappedX = og.left();
            snappedXFlag = true;
        }

        if(!snappedXFlag && qAbs(newX-og.right()) < SNAP_THRESHOLD) {
            snappedX = og.right();
            snappedXFlag = true;
        }

        if(!snappedXFlag && qAbs(newX + geom.width() - og.left()) < SNAP_THRESHOLD) {
            snappedX = og.left() - geom.width();
            snappedXFlag = true;
        }

        if(!snappedXFlag && qAbs(newX +geom.width() - og.right()) < SNAP_THRESHOLD) {
            snappedX = og.right() - geom.width();
            snappedXFlag = true;
        }

        if(!snappedXFlag && qAbs((newX + geom.width()/2) - (og.left() + og.width()/2)) < SNAP_THRESHOLD) {
            snappedX = (og.left() - og.width()/2) - geom.width()/2;
            snappedXFlag = true;
        }

        // ГОРИЗОНТАЛНАЯ ЛИНИЯ

        if(!snappedYFlag && qAbs(newY - og.top()) < SNAP_THRESHOLD) {
            snappedY = og.top();
            snappedYFlag = true;
        }

        if(!snappedYFlag && qAbs(newY - og.bottom()) < SNAP_THRESHOLD) {
            snappedY = og.bottom();
            snappedYFlag = true;
        }

        if(!snappedYFlag && qAbs((newY + geom.height()) - og.top()) < SNAP_THRESHOLD) {
            snappedY = og.top() - geom.height();
            snappedYFlag = true;
        }

        if(!snappedYFlag && qAbs((newY + geom.height()) - og.bottom()) < SNAP_THRESHOLD) {
            snappedY = og.bottom() - geom.height();
            snappedYFlag = true;
        }
        if(!snappedYFlag && qAbs((newY + geom.height()/2) - (og.top() + og.height()/2)) < SNAP_THRESHOLD) {
            snappedY = (og.top() + og.height()/2) - geom.height()/2;
            snappedYFlag = true;
        }
    }

    if (snappedXFlag) {
        int lineX = snappedX;
        if (qAbs((snappedX + geom.width()) - snappedX) == 0) {
        } else {
            lineX = snappedX;
        }
        if (qAbs(snappedX - newX) < SNAP_THRESHOLD) lineX = snappedX;
        else if (qAbs((snappedX + geom.width()) - (newX + geom.width())) < SNAP_THRESHOLD) lineX = snappedX + geom.width();
        else lineX = snappedX + geom.width()/2;

        int topY = qMin(snappedY, newY);
        int bottomY = qMax(snappedY + geom.height(), newY + geom.height());
        for (auto it = placedElements.begin(); it != placedElements.end(); ++it) {
            if (it.value() != element) {
                QRect og = it.value()->geometry();
                topY = qMin(topY, og.top());
                bottomY = qMax(bottomY, og.bottom());
            }
        }
        guides.append(QLine(lineX, topY, lineX, bottomY));
    }

    if (snappedYFlag) {
        int lineY = snappedY;
        if (qAbs(snappedY - newY) < SNAP_THRESHOLD) lineY = snappedY;
        else if (qAbs((snappedY + geom.height()) - (newY + geom.height())) < SNAP_THRESHOLD) lineY = snappedY + geom.height();
        else lineY = snappedY + geom.height()/2;

        int leftX = qMin(snappedX, newX);
        int rightX = qMax(snappedX + geom.width(), newX + geom.width());
        for (auto it = placedElements.begin(); it != placedElements.end(); ++it) {
            if (it.value() != element) {
                QRect og = it.value()->geometry();
                leftX = qMin(leftX, og.left());
                rightX = qMax(rightX, og.right());
            }
        }
        guides.append(QLine(leftX, lineY, rightX, lineY));
    }

    return guides;
}

QList<QLine> BoardDesignerWidget::findSizeGuides(DraggableElement *element, const QRect &newGeom,
                                                  int &snappedWidth, int &snappedHeight)
{
    QList<QLine> guides;
    snappedWidth = newGeom.width();
    snappedHeight = newGeom.height();

    bool widthSnapped = false;
    bool heightSnapped = false;

    for (auto it = placedElements.begin(); it != placedElements.end(); ++it) {
        DraggableElement *other = it.value();
        if (other == element) continue;

        QRect og = other->geometry();

        // Совпадение ширины
        if (!widthSnapped && qAbs(newGeom.width() - og.width()) < SNAP_THRESHOLD) {
            snappedWidth = og.width();
            widthSnapped = true;
            
            // Рисуем вертикальную линию по правому краю текущего элемента
            int lineX = newGeom.x() + snappedWidth;
            int topY = qMin(newGeom.y(), og.y());
            int bottomY = qMax(newGeom.y() + newGeom.height(), og.y() + og.height());
            guides.append(QLine(lineX, topY, lineX, bottomY));
        }

        // Совпадение высоты
        if (!heightSnapped && qAbs(newGeom.height() - og.height()) < SNAP_THRESHOLD) {
            snappedHeight = og.height();
            heightSnapped = true;
            
            // Рисуем горизонтальную линию по нижнему краю текущего элемента
            int lineY = newGeom.y() + snappedHeight;
            int leftX = qMin(newGeom.x(), og.x());
            int rightX = qMax(newGeom.x() + newGeom.width(), og.x() + og.width());
            guides.append(QLine(leftX, lineY, rightX, lineY));
        }
    }

    return guides;
}

// void BoardDesignerWidget::onElementResized(DraggableElement *element, int deltaWidth, int deltaHeight)
// {
//     if (!element) return;
//     QRect geom = element->geometry();

//     int newWidth = geom.width() + deltaWidth;
//     int newHeight = geom.height() + deltaHeight;

//     // Не даём элементу вылезти за пределы области дизайнера и не даём сжаться
//     // меньше собственного минимального размера самого элемента
//     int maxWidth = qMax(element->minimumWidth(), designerArea->width() - geom.x());
//     int maxHeight = qMax(element->minimumHeight(), designerArea->height() - geom.y());
//     newWidth = qBound(element->minimumWidth(), newWidth, maxWidth);
//     newHeight = qBound(element->minimumHeight(), newHeight, maxHeight);

//     element->setGeometry(geom.x(), geom.y(), newWidth, newHeight);
//     emit layoutChanged();
// }

void BoardDesignerWidget::onElementGeometryChanged(DraggableElement *element, const QRect &newGeometry)
{
    if (!element) return;

    QRect geom = newGeometry;

    // 1. Ограничения границами области
    int minX = 0, minY = 0;
    int maxX = designerArea->width();
    int maxY = designerArea->height();

    if (geom.left() < minX) geom.setLeft(minX);
    if (geom.top() < minY) geom.setTop(minY);
    if (geom.right() > maxX) geom.setRight(maxX);
    if (geom.bottom() > maxY) geom.setBottom(maxY);

    // 2. Минимальный размер
    if (geom.width() < element->minimumWidth()) geom.setWidth(element->minimumWidth());
    if (geom.height() < element->minimumHeight()) geom.setHeight(element->minimumHeight());

    // 3. Ищем совпадение размеров с другими элементами
    int snappedWidth = geom.width();
    int snappedHeight = geom.height();
    QList<QLine> sizeGuides = findSizeGuides(element, geom, snappedWidth, snappedHeight);

    if (!sizeGuides.isEmpty()) {
        // Применяем snap по размеру
        geom.setWidth(snappedWidth);
        geom.setHeight(snappedHeight);
    }

    // 4. Ищем совпадение позиций (для линий выравнивания по краям)
    int snappedX = geom.x();
    int snappedY = geom.y();
    QList<QLine> alignGuides = findAlignmentGuides(element, geom.x(), geom.y(), snappedX, snappedY);
    
    if (!alignGuides.isEmpty()) {
        geom.moveLeft(snappedX);
        geom.moveTop(snappedY);
    } else if (sizeGuides.isEmpty()) {
        // Если никаких совпадений нет — применяем snap-to-grid
        int gridX = qRound(double(geom.x()) / GRID_SIZE) * GRID_SIZE;
        int gridY = qRound(double(geom.y()) / GRID_SIZE) * GRID_SIZE;
        geom.moveLeft(gridX);
        geom.moveTop(gridY);
    }

    element->setGeometry(geom);

    // 5. Показываем все линии
    QList<QLine> allGuides;
    allGuides.append(sizeGuides);
    allGuides.append(alignGuides);
    designerArea->setActiveGuides(allGuides);

    emit layoutChanged();
}

void BoardDesignerWidget::onElementDeleted(DraggableElement *element)
{
    if (!element) return;
    QString elementType = element->getElementType();
    removeElement(elementType);
}

void BoardDesignerWidget::onElementPropertiesRequested(DraggableElement *element)
{
    if (!element) return;

    ElementPropertiesDialog dialog(this);
    dialog.setText(element->getDefaultText());
    dialog.setSize(element->width(), element->height());
    dialog.setStyle(element->getCustomStyle());

    if (dialog.exec() == QDialog::Accepted) {
        element->setDefaultText(dialog.text());
        element->setText(dialog.text());
        element->resize(dialog.widthValue(), dialog.heightValue());
        element->setCustomStyle(dialog.buildStyle());
        emit layoutChanged();
    }
}
