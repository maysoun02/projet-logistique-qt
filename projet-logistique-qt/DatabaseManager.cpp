#include "DatabaseManager.h"
#include <QDebug>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QODBC");
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect(const QString& dsn, const QString& username, const QString& password) {
    db.setDatabaseName(dsn);        // Exemple : "OracleDSN"
    db.setUserName(username);       // Exemple : "SYSTEM"
    db.setPassword(password);       // Exemple : "oracle123"

    if (!db.open()) {
        qDebug() << "❌ Connexion échouée :" << db.lastError().text();
        return false;
    }

    qDebug() << "✅ Connexion réussie à la base Oracle";
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}

void DatabaseManager::disconnect() {
    if (db.isOpen())
        db.close();
}
