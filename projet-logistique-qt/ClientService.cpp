#include "ClientService.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<Client> ClientService::getAllClients() {
    QList<Client> clients;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (query.exec("SELECT id_client, nom, prenom, email, telephone, adresse FROM CLIENT")) {
        while (query.next()) {
            Client c(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toString(),
                query.value(3).toString(),
                query.value(4).toString(),
                query.value(5).toString()
                );
            clients.append(c);
        }
    } else {
        qDebug() << "❌ Erreur SELECT clients:" << query.lastError().text();
    }

    return clients;
}

bool ClientService::addClient(const Client& c) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO CLIENT (nom, prenom, email, telephone, adresse) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(c.nom);
    query.addBindValue(c.prenom);
    query.addBindValue(c.email);
    query.addBindValue(c.telephone);
    query.addBindValue(c.adresse);

    if (!query.exec()) {
        qDebug() << "❌ Erreur INSERT client:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ClientService::updateClient(const Client& c) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE CLIENT SET nom=?, prenom=?, email=?, telephone=?, adresse=? WHERE id_client=?");
    query.addBindValue(c.nom);
    query.addBindValue(c.prenom);
    query.addBindValue(c.email);
    query.addBindValue(c.telephone);
    query.addBindValue(c.adresse);
    query.addBindValue(c.id);

    if (!query.exec()) {
        qDebug() << "❌ Erreur UPDATE client:" << query.lastError().text();
        return false;
    }
    return true;
}

bool ClientService::deleteClient(int id) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM CLIENT WHERE id_client=?");
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "❌ Erreur DELETE client:" << query.lastError().text();
        return false;
    }
    return true;
}
