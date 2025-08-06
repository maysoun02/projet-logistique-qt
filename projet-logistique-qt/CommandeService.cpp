#include "CommandeService.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QList<Commande> CommandeService::getAllCommandes() {
    QList<Commande> commandes;
    QSqlQuery query(DatabaseManager::instance().getDatabase());

    if (query.exec("SELECT id_commande, date_commande, type_livraison, statut_commande, adresse_livraison, id_client FROM COMMANDE")) {
        while (query.next()) {
            Commande c(
                query.value(0).toInt(),
                query.value(1).toDate(),
                query.value(2).toString(),
                query.value(3).toString(),
                query.value(4).toString(),
                query.value(5).toInt()
                );
            commandes.append(c);
        }
    } else {
        qDebug() << "❌ Erreur SELECT commandes:" << query.lastError().text();
    }

    return commandes;
}

bool CommandeService::addCommande(const Commande& c) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO COMMANDE (date_commande, type_livraison, statut_commande, adresse_livraison, id_client) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(c.dateCommande);
    query.addBindValue(c.typeLivraison);
    query.addBindValue(c.statutCommande);
    query.addBindValue(c.adresseLivraison);
    query.addBindValue(c.clientId);

    if (!query.exec()) {
        qDebug() << "❌ Erreur INSERT commande:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CommandeService::updateCommande(const Commande& c) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("UPDATE COMMANDE SET date_commande=?, type_livraison=?, statut_commande=?, adresse_livraison=?, id_client=? WHERE id_commande=?");
    query.addBindValue(c.dateCommande);
    query.addBindValue(c.typeLivraison);
    query.addBindValue(c.statutCommande);
    query.addBindValue(c.adresseLivraison);
    query.addBindValue(c.clientId);
    query.addBindValue(c.id);

    if (!query.exec()) {
        qDebug() << "❌ Erreur UPDATE commande:" << query.lastError().text();
        return false;
    }
    return true;
}

bool CommandeService::deleteCommande(int id) {
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("DELETE FROM COMMANDE WHERE id_commande=?");
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "❌ Erreur DELETE commande:" << query.lastError().text();
        return false;
    }
    return true;
}
