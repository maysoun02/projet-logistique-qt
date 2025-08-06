#ifndef COMMANDESERVICE_H
#define COMMANDESERVICE_H

#include <QList>
#include "Commande.h"

class CommandeService {
public:
    static QList<Commande> getAllCommandes();
    static bool addCommande(const Commande& commande);
    static bool updateCommande(const Commande& commande);
    static bool deleteCommande(int id);
};

#endif // COMMANDESERVICE_H
