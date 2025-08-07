#include "mainwindow.h"
#include "DatabaseManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString dsn = "Source_Projet2A";         // Le nom que tu as mis dans ODBC
    QString username = "SYSTEM";       // Ton utilisateur Oracle
    QString password = "123456789";
    if (!DatabaseManager::instance().connect(dsn, username, password)) {
        return -1; // arrête si la connexion échoue
    }
    MainWindow w;
    w.show();
    return a.exec();
    QSqlQuery testQuery;
    if (!testQuery.exec("SELECT * FROM CLIENT")) {
        qDebug() << "Erreur SELECT CLIENT:" << testQuery.lastError().text();
    } else {
        qDebug() << "Requête test CLIENT réussie.";
    }
}
