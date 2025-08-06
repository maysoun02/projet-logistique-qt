#ifndef CLIENTSERVICE_H
#define CLIENTSERVICE_H

#include <QList>
#include "client.h"

class ClientService {
public:
    static QList<Client> getAllClients();
    static bool addClient(const Client& client);
    static bool updateClient(const Client& client);
    static bool deleteClient(int id);
};

#endif // CLIENTSERVICE_H
