#ifndef BOARDDESIGNERWIDGET_H
#define BOARDDESIGNERWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QMap>
#include <QPushButton>
#include <QScrollArea>
#include "draggableelement.h"

struct ElementLayoutConfig {
    QString elementType;
    QString displayName;
    QString defaultText;
    int row;
    int col;
    int rowSpan;
    int colSpan;
    bool visible;

    ElementLayoutConfig() : row(0), col(0), rowSpan(1), colSpan(1), visible(true) {}
};

class BoardDesignerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BoardDesignerWidget(QWidget *parent = nullptr);
    ~BoardDesignerWidget();

    void setGridSize(int rows, int cols);
    void getGridSize(int &rows, int &cols) const { rows = gridRows; cols = gridCols; }

    void addElement(const QString &elementType, const QString &displayName,
                    const QString &defaultText, int row, int col,
                    int rowSpan = 1, int colSpan = 1);
    void removeElement(const QString &elementType);
    void clearDesigner();

    QList<ElementLayoutConfig> getCurrentLayout() const;
    void applyLayout(const QList<ElementLayoutConfig> &layout);

    void clearCell(int row, int col);

signals:
    void layoutChanged();
    void elementSelected(const QString &elementType);

public slots:
    void resetToDefaultLayout();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void onElementMoved(DraggableElement *element, int fromRow, int fromCol, int toRow, int toCol);
    void onElementDeleted(DraggableElement *element);

private:
    void setupUI();
    void createElementPalette();
    void highlightDropZone(int row, int col);
    void clearHighlight();

    bool isCellFree(int row, int col, int rowSpan = 1, int colSpan = 1) const;

    QGridLayout *designerLayout;
    QWidget *paletteWidget;
    QGridLayout *paletteLayout;

    int gridRows;
    int gridCols;

    QMap<QString, DraggableElement*> elements;
    QMap<QString, DraggableElement*> cellOccupancy;

    int highlightedRow;
    int highlightedCol;

    static constexpr int CELL_WIDTH = 120;
    static constexpr int CELL_HEIGHT = 80;
};

#endif
