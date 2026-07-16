#include "databaseemanager.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QDebug>
#include <QDir>
#include <QMap>

QString DatabaseManager::escapeIdentifier(const QString &name)
{
    QString escaped = name;
    escaped.replace('"', "\"\"");
    return escaped;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    dbPath = QDir::homePath() + "/.InformationBoard/data.db";
    QDir dir(QDir::homePath() + "/.InformationBoard");
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbPath);

    if (!database.open()) {
        qDebug() << "Ошибка открытия БД:" << database.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS _metadata ("
               "table_name TEXT PRIMARY KEY,"
               "created_at TEXT,"
               "display_name TEXT)");

    qDebug() << "База данных инициализирована:" << dbPath;
    return true;
}

void DatabaseManager::closeDatabase()
{
    if (database.isOpen()) {
        database.close();
    }
}

bool DatabaseManager::createTable(const QString &tableName, const QStringList &columns)
{
    if (columns.isEmpty()) {
        qDebug() << "Нет колонок для создания таблицы";
        return false;
    }

    QStringList escapedColumns;
    for (const QString &col : columns) {
        escapedColumns << QString("\"%1\" TEXT").arg(escapeIdentifier(col));
    }

    QString sql = QString("CREATE TABLE IF NOT EXISTS \"%1\" (id INTEGER PRIMARY KEY AUTOINCREMENT, %2)")
                      .arg(escapeIdentifier(tableName))
                      .arg(escapedColumns.join(", "));

    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << "Ошибка создания таблицы:" << query.lastError().text();
        return false;
    }

    QSqlQuery metaQuery;
    metaQuery.prepare("INSERT OR REPLACE INTO _metadata (table_name, created_at, display_name) "
                      "VALUES (?, datetime('now'), ?)");
    metaQuery.addBindValue(tableName);
    metaQuery.addBindValue(tableName);
    metaQuery.exec();

    qDebug() << "Таблица создана:" << tableName;
    return true;
}

bool DatabaseManager::dropTable(const QString &tableName)
{
    QSqlQuery query;
    QString sql = QString("DROP TABLE IF EXISTS \"%1\"").arg(escapeIdentifier(tableName));

    if (!query.exec(sql)) {
        qDebug() << "Ошибка удаления таблицы:" << query.lastError().text();
        return false;
    }

    QSqlQuery metaQuery;
    metaQuery.prepare("DELETE FROM _metadata WHERE table_name = ?");
    metaQuery.addBindValue(tableName);
    metaQuery.exec();

    qDebug() << "Таблица удалена:" << tableName;
    return true;
}

QStringList DatabaseManager::getTableNames()
{
    QStringList tables;
    QSqlQuery query("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE '\\_%' ESCAPE '\\'");

    while (query.next()) {
        tables << query.value(0).toString();
    }

    return tables;
}

bool DatabaseManager::addColumn(const QString &tableName, const QString &columnName)
{
    QSqlQuery query;
    QString sql = QString("ALTER TABLE \"%1\" ADD COLUMN \"%2\" TEXT")
                      .arg(escapeIdentifier(tableName))
                      .arg(escapeIdentifier(columnName));
    if (!query.exec(sql)) {
        qDebug() << "Ошибка добавления колонки:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertRow(const QString &tableName, const QList<QVariant> &values)
{
    QStringList columns = getColumnNames(tableName);

    if (values.size() != columns.size()) {
        qDebug() << "Несоответствие количества значений колонкам";
        return false;
    }

    QStringList escapedColumns;
    QStringList placeholders;
    for (const QString &col : columns) {
        escapedColumns << QString("\"%1\"").arg(escapeIdentifier(col));
        placeholders << "?";
    }

    QString sql = QString("INSERT INTO \"%1\" (%2) VALUES (%3)")
                      .arg(escapeIdentifier(tableName))
                      .arg(escapedColumns.join(", "))
                      .arg(placeholders.join(", "));

    QSqlQuery query;
    query.prepare(sql);
    for (const QVariant &value : values) {
        query.addBindValue(value);
    }

    if (!query.exec()) {
        qDebug() << "Ошибка вставки строки:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::updateRow(const QString &tableName, int rowId, const QList<QVariant> &values)
{
    QStringList columns = getColumnNames(tableName);
    if (values.size() != columns.size()) {
        qDebug() << "Несоответствие количества значений колонкам при обновлении";
        return false;
    }

    QStringList assignments;
    for (const QString &col : columns) {
        assignments << QString("\"%1\" = ?").arg(escapeIdentifier(col));
    }

    QString sql = QString("UPDATE \"%1\" SET %2 WHERE id = ?")
                      .arg(escapeIdentifier(tableName))
                      .arg(assignments.join(", "));

    QSqlQuery query;
    query.prepare(sql);
    for (const QVariant &value : values) {
        query.addBindValue(value);
    }
    query.addBindValue(rowId);

    if (!query.exec()) {
        qDebug() << "Ошибка обновления строки:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteRow(const QString &tableName, int rowId)
{
    QSqlQuery query;
    query.prepare(QString("DELETE FROM \"%1\" WHERE id = ?").arg(escapeIdentifier(tableName)));
    query.addBindValue(rowId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления строки:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<QList<QVariant>> DatabaseManager::getTableData(const QString &tableName)
{
    QList<QList<QVariant>> data;

    QSqlQuery query(QString("SELECT * FROM \"%1\" ORDER BY id").arg(escapeIdentifier(tableName)));

    // Число столбцов берём из записи, а не из query.size() (для SQLite он == -1)
    int columnCount = query.record().count();

    while (query.next()) {
        QList<QVariant> row;
        for (int i = 1; i < columnCount; ++i) {   // i=1 — пропускаем столбец id
            row << query.value(i);
        }
        data.append(row);
    }

    return data;
}

QList<int> DatabaseManager::getRowIds(const QString &tableName)
{
    QList<int> ids;
    QSqlQuery query(QString("SELECT id FROM \"%1\" ORDER BY id").arg(escapeIdentifier(tableName)));
    while (query.next()) {
        ids << query.value(0).toInt();
    }
    return ids;
}

QStringList DatabaseManager::getColumnNames(const QString &tableName)
{
    QStringList columns;

    QSqlQuery query(QString("PRAGMA table_info(\"%1\")").arg(escapeIdentifier(tableName)));

    while (query.next()) {
        QString columnName = query.value(1).toString();
        if (columnName != "id") {
            columns << columnName;
        }
    }

    return columns;
}

QString DatabaseManager::formatTableAsText(const QString &tableName)
{
    QStringList columns = getColumnNames(tableName);
    QList<QList<QVariant>> data = getTableData(tableName);

    if (columns.isEmpty()) {
        return "Таблица пуста";
    }

    QMap<int, int> columnWidths;
    for (int i = 0; i < columns.size(); ++i) {
        columnWidths[i] = columns[i].length();
    }

    for (const auto &row : data) {
        for (int i = 0; i < row.size(); ++i) {
            int width = row[i].toString().length();
            if (width > columnWidths[i]) {
                columnWidths[i] = width;
            }
        }
    }

    int totalWidth = 0;
    for (int i = 0; i < columns.size(); ++i) {
        totalWidth += columnWidths[i];
    }
    totalWidth += (columns.size() - 1) * 3;

    QString result = "📊 ТАБЛИЦА: " + tableName + "\n\n";

    for (int i = 0; i < columns.size(); ++i) {
        result += QString("%1").arg(columns[i], -columnWidths[i]);
        result += (i < columns.size() - 1) ? " │ " : "";
    }

    result += "\n" + QString("-").repeated(totalWidth) + "\n";

    for (const auto &row : data) {
        for (int i = 0; i < row.size(); ++i) {
            result += QString("%1").arg(row[i].toString(), -columnWidths[i]);
            result += (i < row.size() - 1) ? " │ " : "";
        }
        result += "\n";
    }

    return result;
}
