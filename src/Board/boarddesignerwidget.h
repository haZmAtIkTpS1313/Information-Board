#ifndef BOARDDESIGNERWIDGET_H
#define BOARDDESIGNERWIDGET_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QGridLayout>
#include "draggableelement.h"

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
    void onElementResized(DraggableElement *element, int deltaWidth, int deltaHeight);
    void onElementDeleted(DraggableElement *element);
    void onElementPropertiesRequested(DraggableElement *element);

private:
    void setupUI();
    void createElementPalette();

    QWidget *paletteWidget;
    QGridLayout *paletteLayout;
    QWidget *designerArea;                    // область для размещения элементов
    QMap<QString, DraggableElement*> elements;       // все элементы (включая неразмещённые)
    QMap<QString, DraggableElement*> placedElements; // размещённые элементы
};

#endif

