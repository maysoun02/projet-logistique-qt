#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool connect(const QString& dsn, const QString& username, const QString& password);
    QSqlDatabase& getDatabase();
    void disconnect();

private:
    DatabaseManager(); // constructeur privé
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
