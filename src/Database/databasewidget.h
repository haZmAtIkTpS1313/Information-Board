#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QSplitter>
#include <QLineEdit>
#include <QPushButton>
#include "databaseemanager.h"

class DatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseWidget(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~DatabaseWidget();

signals:
    void showTableRequested(const QString &tableName, const QString &tableData);

private slots:
    void onTableSelected(const QString &tableName);
    void onCreateTable();
    void onAddColumn();
    void onAddRow();
    void onDeleteSelected();
    void onShowOnBoard();
    void onRefreshTable();
    void onCellChanged(int row, int column);

private:
    void setupUI();
    void loadTablesList();
    void loadTableData(const QString &tableName);
    void setupTableWidget(const QStringList &columns);

    DatabaseManager *dbManager;
    QSplitter *mainSplitter;
    QListWidget *tablesList;
    QTableWidget *dataTable;
    QLineEdit *tableNameEdit;
    QLineEdit *columnNameEdit;
    QPushButton *createTableBtn;
    QPushButton *addColumnBtn;
    QPushButton *addRowBtn;
    QPushButton *deleteRowBtn;
    QPushButton *showOnBoardBtn;
    QPushButton *refreshBtn;

    QString currentTable;
    QStringList currentColumns;
    QList<int> currentRowIds;   // id строк, соответствующие строкам таблицы
    bool loadingData;           // защита от срабатывания onCellChanged при загрузке
};

#endif
