
#include "databasewidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QDebug>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QRegularExpression>

#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qregularexpression.h>


DatabaseWidget::DatabaseWidget(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent)
    , dbManager(dbManager)
    , loadingData(false)
{
    setupUI();
    loadTablesList();
}

DatabaseWidget::~DatabaseWidget()
{
}

void DatabaseWidget::setupUI()
{
    QPushButton *importExcelBth = new QPushButton("Импорт из Excel (CSV)");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *createGroup = new QGroupBox("Создать новую таблицу", this);
    QHBoxLayout *createLayout = new QHBoxLayout(createGroup);

    createLayout->addWidget(new QLabel("Имя таблицы:", createGroup));
    tableNameEdit = new QLineEdit(createGroup);
    tableNameEdit->setPlaceholderText("например: Сотрудники");
    createLayout->addWidget(tableNameEdit);

    createLayout->addWidget(new QLabel("Колонки (через запятую):", createGroup));
    columnNameEdit = new QLineEdit(createGroup);
    columnNameEdit->setPlaceholderText("например: Имя, Фамилия, Должность");
    createLayout->addWidget(columnNameEdit);

    createTableBtn = new QPushButton("Создать", createGroup);
    createLayout->addWidget(createTableBtn);
    connect(createTableBtn, &QPushButton::clicked, this, &DatabaseWidget::onCreateTable);

    mainLayout->addWidget(createGroup);

    mainSplitter = new QSplitter(Qt::Horizontal, this);

    QWidget *leftPanel = new QWidget(mainSplitter);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(new QLabel("Таблицы:", leftPanel));
    tablesList = new QListWidget(leftPanel);
    connect(tablesList, &QListWidget::currentTextChanged, this, &DatabaseWidget::onTableSelected);
    leftLayout->addWidget(tablesList);
    leftPanel->setLayout(leftLayout);

    QWidget *rightPanel = new QWidget(mainSplitter);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addColumnBtn = new QPushButton("+ Добавить колонку", rightPanel);
    addRowBtn = new QPushButton("+ Добавить строку", rightPanel);
    deleteRowBtn = new QPushButton("✖ Удалить строку", rightPanel);
    showOnBoardBtn = new QPushButton("📋 Показать на доске", rightPanel);
    refreshBtn = new QPushButton("🔄 Обновить", rightPanel);

    buttonLayout->addWidget(addColumnBtn);
    buttonLayout->addWidget(addRowBtn);
    buttonLayout->addWidget(deleteRowBtn);
    buttonLayout->addWidget(showOnBoardBtn);
    buttonLayout->addWidget(refreshBtn);
    buttonLayout->addWidget(importExcelBth);

    rightLayout->addLayout(buttonLayout);

    dataTable = new QTableWidget(rightPanel);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    rightLayout->addWidget(dataTable);

    rightLayout->addWidget(new QLabel("💡 Двойной клик по ячейке — редактирование, изменения сохраняются автоматически", rightPanel));

    rightPanel->setLayout(rightLayout);

    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({200, 600});

    mainLayout->addWidget(mainSplitter);

    connect(addColumnBtn, &QPushButton::clicked, this, &DatabaseWidget::onAddColumn);
    connect(addRowBtn, &QPushButton::clicked, this, &DatabaseWidget::onAddRow);
    connect(deleteRowBtn, &QPushButton::clicked, this, &DatabaseWidget::onDeleteSelected);
    connect(showOnBoardBtn, &QPushButton::clicked, this, &DatabaseWidget::onShowOnBoard);
    connect(refreshBtn, &QPushButton::clicked, this, &DatabaseWidget::onRefreshTable);
    connect(dataTable, &QTableWidget::cellChanged, this, &DatabaseWidget::onCellChanged);
    connect(importExcelBth, &QPushButton::clicked, this, &DatabaseWidget::onImportFromExcel);
}

void DatabaseWidget::loadTablesList()
{
    tablesList->clear();
    QStringList tables = dbManager->getTableNames();
    tablesList->addItems(tables);
}

void DatabaseWidget::onTableSelected(const QString &tableName)
{
    if (tableName.isEmpty()) return;
    currentTable = tableName;
    loadTableData(tableName);
}

void DatabaseWidget::loadTableData(const QString &tableName)
{
    loadingData = true;   // отключаем реакцию на изменение ячеек, пока заполняем

    currentColumns = dbManager->getColumnNames(tableName);
    QList<QList<QVariant>> data = dbManager->getTableData(tableName);
    currentRowIds = dbManager->getRowIds(tableName);

    setupTableWidget(currentColumns);

    dataTable->setRowCount(data.size());
    for (int row = 0; row < data.size(); ++row) {
        for (int col = 0; col < data[row].size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(data[row][col].toString());
            dataTable->setItem(row, col, item);
        }
    }

    loadingData = false;
}

void DatabaseWidget::setupTableWidget(const QStringList &columns)
{
    dataTable->clear();
    dataTable->setColumnCount(columns.size());
    dataTable->setHorizontalHeaderLabels(columns);
    dataTable->setRowCount(0);
}

void DatabaseWidget::onCreateTable()
{
    QString tableName = tableNameEdit->text().trimmed();
    QString columnsStr = columnNameEdit->text().trimmed();

    if (tableName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя таблицы");
        return;
    }

    if (columnsStr.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите названия колонок");
        return;
    }

    QStringList columns = columnsStr.split(',');
    for (QString &col : columns) {
        col = col.trimmed();
    }
    columns.removeAll("");

    if (dbManager->createTable(tableName, columns)) {
        tableNameEdit->clear();
        columnNameEdit->clear();
        loadTablesList();
        QMessageBox::information(this, "Успех", QString("Таблица '%1' создана").arg(tableName));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать таблицу");
    }
}

void DatabaseWidget::onAddColumn()
{
    if (currentTable.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите таблицу");
        return;
    }

    bool ok;
    QString columnName = QInputDialog::getText(this, "Добавить колонку",
                                               "Название колонки:",
                                               QLineEdit::Normal, "", &ok).trimmed();
    if (ok && !columnName.isEmpty()) {
        if (dbManager->addColumn(currentTable, columnName)) {
            loadTableData(currentTable);
            QMessageBox::information(this, "Успех", "Колонка добавлена");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить колонку");
        }
    }
}

void DatabaseWidget::onAddRow()
{
    if (currentTable.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите таблицу");
        return;
    }

    if (currentColumns.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "В таблице нет колонок");
        return;
    }

    QList<QVariant> values;
    for (const QString &col : currentColumns) {
        bool ok;
        QString value = QInputDialog::getText(this, "Добавить строку",
                                              QString("Значение для '%1':").arg(col),
                                              QLineEdit::Normal, "", &ok);
        if (!ok) return;
        values << value;
    }

    if (dbManager->insertRow(currentTable, values)) {
        loadTableData(currentTable);
        QMessageBox::information(this, "Успех", "Строка добавлена");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить строку");
    }
}

void DatabaseWidget::onDeleteSelected()
{
    int row = dataTable->currentRow();
    if (row < 0 || row >= currentRowIds.size()) {
        QMessageBox::warning(this, "Ошибка", "Выберите строку для удаления");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение", "Удалить выбранную строку?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int id = currentRowIds[row];
        if (dbManager->deleteRow(currentTable, id)) {
            loadTableData(currentTable);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить строку");
        }
    }
}

void DatabaseWidget::onCellChanged(int row, int column)
{
    Q_UNUSED(column);
    if (loadingData) return;                       // игнорируем программное заполнение
    if (row < 0 || row >= currentRowIds.size()) return;
    if (currentTable.isEmpty()) return;

    // Собираем все значения строки и обновляем запись целиком
    QList<QVariant> values;
    for (int col = 0; col < currentColumns.size(); ++col) {
        QTableWidgetItem *item = dataTable->item(row, col);
        values << (item ? item->text() : QString());
    }

    int id = currentRowIds[row];
    if (!dbManager->updateRow(currentTable, id, values)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изменения");
        loadTableData(currentTable);   // откатываем к данным из БД
    }
}

void DatabaseWidget::onImportFromExcel()
{
    // Выбор файла
    QString filename = QFileDialog::getOpenFileName(
        this, 
        "Выберите файл экспорта из Excel",
        QString(),
        "CSV файлы (*.csv);;Текст с табудяцией (*.txt, *.tsv);;Все файлы (*)"
    );
    if (filename.isEmpty()) return;

    // Имя новой таблицы
    QString tableName = QInputDialog::getText(
        this, "Имя таблицы",
        "Введите имя таблицы для импорта: "
    );
    if (tableName.isEmpty()) return;

    // Открываем файл
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл: " + filename);
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QStringList columns;
    QList<QList<QVariant>> rows;
    int rowIdx = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList values = line.split(";");

        for (int i = 0; i < values.size(); ++i) {
            values[i] = values[i].remove(QRegularExpression("^\"|\"$")).trimmed();
        }
        if (rowIdx == 0) {
            columns = values;
        } else {
            QList<QVariant> row;
            for (const QString &val : values) {
                row.append(QVariant(val));
            }
            rows.append(row);
        }
        rowIdx++;
    }
    file.close();

    if (columns.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Файл пуст или не удалось прочитать заголовок");
        return;
    }

    // Создаем таблицу в БД
    if (!dbManager->createTable(tableName, columns)) {
        QMessageBox::critical(this, "Ошибка", 
            "Не удалось создать таблицу '" + tableName + "'. Возможно, имя уже занято.");
        return;
    }

    //Вставляем данные 
    int successCount = 0;
    for (const auto &row : rows) {
        if (dbManager->insertRow(tableName, row)) {
            successCount++;
        }
    }

    // Результат
    QMessageBox::information(this, "Импорт завершён",
    QString("Импортировано %1 строк в таблицу '%2'")
        .arg(successCount)
        .arg(tableName));

    loadTablesList();
    onTableSelected(tableName);
}

void DatabaseWidget::onShowOnBoard()
{
    if (currentTable.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите таблицу");
        return;
    }

    QString tableData = dbManager->formatTableAsText(currentTable);
    emit showTableRequested(currentTable, tableData);
}

void DatabaseWidget::onRefreshTable()
{
    if (!currentTable.isEmpty()) {
        loadTableData(currentTable);
    }
}
