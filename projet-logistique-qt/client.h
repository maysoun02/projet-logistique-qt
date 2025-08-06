#ifndef CLIENT_H
#define CLIENT_H

#include <QString>

class Client {
public:
    int id;
    QString nom;
    QString prenom;
    QString email;
    QString telephone;
    QString adresse;

    Client() = default;
    Client(int id, QString nom, QString prenom, QString email, QString tel, QString adr)
        : id(id), nom(nom), prenom(prenom), email(email), telephone(tel), adresse(adr) {}
};

#endif // CLIENT_H
