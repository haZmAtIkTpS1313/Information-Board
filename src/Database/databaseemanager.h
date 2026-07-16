#ifndef DATABASEEMANAGER_H
#define DATABASEEMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QVariant>
#include <QStringList>
#include <QList>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    bool initDatabase();
    void closeDatabase();

    bool createTable(const QString &tableName, const QStringList &columns);
    bool dropTable(const QString &tableName);
    QStringList getTableNames();

    bool insertRow(const QString &tableName, const QList<QVariant> &values);
    bool updateRow(const QString &tableName, int rowId, const QList<QVariant> &values);
    bool deleteRow(const QString &tableName, int rowId);
    bool addColumn(const QString &tableName, const QString &columnName);

    QList<QList<QVariant>> getTableData(const QString &tableName);
    QList<int> getRowIds(const QString &tableName);
    QStringList getColumnNames(const QString &tableName);

    QString formatTableAsText(const QString &tableName);

private:
    // Экранирует имя таблицы/колонки для подстановки в кавычках ("..."):
    // удваивает встроенные кавычки, как того требует синтаксис SQL для
    // идентификаторов. Имена приходят напрямую из полей ввода в UI и не
    // параметризуются через bind — без этого кавычка в имени "вырывается"
    // из идентификатора и остаток строки становится частью SQL-запроса.
    static QString escapeIdentifier(const QString &name);

    QSqlDatabase database;
    QString dbPath;
};

#endif // DATABASEEMANAGER_H

