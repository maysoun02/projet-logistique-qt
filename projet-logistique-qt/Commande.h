#ifndef COMMANDE_H
#define COMMANDE_H

#include <QString>
#include <QDate>

class Commande {
public:
    int id;
    QDate dateCommande;
    QString typeLivraison;
    QString statutCommande;
    QString adresseLivraison;
    int clientId;

    Commande() = default;

    Commande(int id, QDate dateCommande, QString typeLivraison, QString statutCommande, QString adresseLivraison, int clientId)
        : id(id), dateCommande(dateCommande), typeLivraison(typeLivraison), statutCommande(statutCommande),
        adresseLivraison(adresseLivraison), clientId(clientId) {}
};

#endif // COMMANDE_H
