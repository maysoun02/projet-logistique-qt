#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ClientService.h"
#include "CommandeService.h"
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegularExpressionValidator>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    chargerClients();
    // Nom & Prénom : lettres et espaces uniquement
    QRegularExpressionValidator* nomValidator = new QRegularExpressionValidator(QRegularExpression("[a-zA-ZÀ-ÿ\\s'-]{2,}"), this);
    ui->lineNom->setValidator(nomValidator);
    ui->linePrenom->setValidator(nomValidator);

    // Email : format simple (exemple@domaine.com)
    QRegularExpressionValidator* emailValidator = new QRegularExpressionValidator(
        QRegularExpression("^[\\w\\.=-]+@[\\w\\.-]+\\.[a-zA-Z]{2,}$"), this);
    ui->lineEmail->setValidator(emailValidator);

    // Téléphone : déjà géré par inputMask dans .ui

    // Adresse : texte libre (pas de validator strict, juste un minimum de caractères)
    QRegularExpressionValidator* adresseValidator = new QRegularExpressionValidator(QRegularExpression(".{5,}"), this);
    ui->lineAdresse->setValidator(adresseValidator);

    // Clients
    connect(ui->btnAjouterClient, &QPushButton::clicked, this, &MainWindow::ajouterClient);
    connect(ui->btnModifierClient, &QPushButton::clicked, this, &MainWindow::modifierClient);
    connect(ui->btnSupprimerClient, &QPushButton::clicked, this, &MainWindow::supprimerClient);
    connect(ui->tableClients, &QTableWidget::cellClicked, this, &MainWindow::remplirChampsClient);


    // Commandes
    connect(ui->btnAjouterCommande, &QPushButton::clicked, this, &MainWindow::ajouterCommande);
    connect(ui->btnModifierCommande, &QPushButton::clicked, this, &MainWindow::modifierCommande);
    connect(ui->btnSupprimerCommande, &QPushButton::clicked, this, &MainWindow::supprimerCommande);
    connect(ui->btnActualiserCommande, &QPushButton::clicked, this, &MainWindow::chargerCommandes);
    connect(ui->tableCommandes, &QTableWidget::cellClicked, this, &MainWindow::remplirChampsCommande);

    chargerClients();
    chargerCommandes();
}

MainWindow::~MainWindow() {
    delete ui;
}

// Clients
void MainWindow::chargerClients() {
    ui->tableClients->setRowCount(0);
    ui->tableClients->setColumnCount(6);
    QStringList headers = {"ID", "Nom", "Prénom", "Email", "Téléphone", "Adresse"};
    ui->tableClients->setHorizontalHeaderLabels(headers);
    ui->tableClients->setColumnHidden(0, true);
    auto clients = ClientService::getAllClients();
    for (const auto& c : clients) {
        int row = ui->tableClients->rowCount();
        ui->tableClients->insertRow(row);
        ui->tableClients->setItem(row, 0, new QTableWidgetItem(QString::number(c.id)));
        ui->tableClients->setItem(row, 1, new QTableWidgetItem(c.nom));
        ui->tableClients->setItem(row, 2, new QTableWidgetItem(c.prenom));
        ui->tableClients->setItem(row, 3, new QTableWidgetItem(c.email));
        ui->tableClients->setItem(row, 4, new QTableWidgetItem(c.telephone));
        ui->tableClients->setItem(row, 5, new QTableWidgetItem(c.adresse));
    }
}

void MainWindow::ajouterClient() {
    QString nom = ui->lineNom->text().trimmed();
    QString prenom = ui->linePrenom->text().trimmed();
    QString email = ui->lineEmail->text().trimmed();
    QString tel = ui->lineTel->text().trimmed();
    QString adresse = ui->lineAdresse->text().trimmed();

    // Vérifier si un champ est vide
    if (nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || tel.isEmpty() || adresse.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Veuillez remplir tous les champs avant d’ajouter un client.");
        return;
    }

    Client c(0, nom, prenom, email, tel, adresse);
    if (ClientService::addClient(c)) {
        chargerClients();
        ui->lineNom->clear();
        ui->linePrenom->clear();
        ui->lineEmail->clear();
        ui->lineTel->clear();
        ui->lineAdresse->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "L'ajout du client a échoué.");
    }
}

void MainWindow::modifierClient() {
    int row = ui->tableClients->currentRow();
    if (row < 0) return;

    // 1. Récupère le client sélectionné
    int id = ui->tableClients->item(row, 0)->text().toInt();
    QString nom = ui->tableClients->item(row, 1)->text();
    QString prenom = ui->tableClients->item(row, 2)->text();
    QString email = ui->tableClients->item(row, 3)->text();
    QString tel = ui->tableClients->item(row, 4)->text();
    QString adresse = ui->tableClients->item(row, 5)->text();

    // 2. Créer une boîte de dialogue
    QDialog dialog(this);
    dialog.setWindowTitle("Modifier le client");

    QFormLayout form(&dialog);

    QLineEdit lineNom(nom);
    QLineEdit linePrenom(prenom);
    QLineEdit lineEmail(email);
    QLineEdit lineTel(tel);
    QLineEdit lineAdresse(adresse);

    form.addRow("Nom:", &lineNom);
    form.addRow("Prénom:", &linePrenom);
    form.addRow("Email:", &lineEmail);
    form.addRow("Téléphone:", &lineTel);
    form.addRow("Adresse:", &lineAdresse);

    // Boutons enregistrer/annuler
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 3. Si l’utilisateur clique sur "OK"
    if (dialog.exec() == QDialog::Accepted) {
        Client c(id,
                 lineNom.text(),
                 linePrenom.text(),
                 lineEmail.text(),
                 lineTel.text(),
                 lineAdresse.text());

        if (ClientService::updateClient(c)) {
            QMessageBox::information(this, "Succès", "Client modifié !");
            chargerClients();
        } else {
            QMessageBox::warning(this, "Erreur", "La modification a échoué.");
        }
    }
}

void MainWindow::supprimerClient() {
    int row = ui->tableClients->currentRow();
    if (row < 0) return;
    int id = ui->tableClients->item(row, 0)->text().toInt();
    if (ClientService::deleteClient(id)) chargerClients();
}

void MainWindow::remplirChampsClient(int row, int) {
    ui->lineNom->setText(ui->tableClients->item(row, 1)->text());
    ui->linePrenom->setText(ui->tableClients->item(row, 2)->text());
    ui->lineEmail->setText(ui->tableClients->item(row, 3)->text());
    ui->lineTel->setText(ui->tableClients->item(row, 4)->text());
    ui->lineAdresse->setText(ui->tableClients->item(row, 5)->text());
}

// Commandes
void MainWindow::chargerCommandes() {
    ui->tableCommandes->setRowCount(0);
    auto commandes = CommandeService::getAllCommandes();
    for (const auto& c : commandes) {
        int row = ui->tableCommandes->rowCount();
        ui->tableCommandes->insertRow(row);
        ui->tableCommandes->setItem(row, 0, new QTableWidgetItem(QString::number(c.id)));
        ui->tableCommandes->setItem(row, 1, new QTableWidgetItem(c.dateCommande.toString("yyyy-MM-dd")));
        ui->tableCommandes->setItem(row, 2, new QTableWidgetItem(c.typeLivraison));
        ui->tableCommandes->setItem(row, 3, new QTableWidgetItem(c.statutCommande));
        ui->tableCommandes->setItem(row, 4, new QTableWidgetItem(c.adresseLivraison));
        ui->tableCommandes->setItem(row, 5, new QTableWidgetItem(QString::number(c.clientId)));
    }
}

void MainWindow::ajouterCommande() {
    Commande c(0, ui->dateCommande->date(), ui->typeLivraison->currentText(), ui->statutCommande->currentText(), ui->adresseLivraison->text(), ui->idClient->text().toInt());
    if (CommandeService::addCommande(c)) chargerCommandes();
}

void MainWindow::modifierCommande() {
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;
    int id = ui->tableCommandes->item(row, 0)->text().toInt();
    Commande c(id, ui->dateCommande->date(), ui->typeLivraison->currentText(), ui->statutCommande->currentText(), ui->adresseLivraison->text(), ui->idClient->text().toInt());
    if (CommandeService::updateCommande(c)) chargerCommandes();
}

void MainWindow::supprimerCommande() {
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;
    int id = ui->tableCommandes->item(row, 0)->text().toInt();
    if (CommandeService::deleteCommande(id)) chargerCommandes();
}

void MainWindow::remplirChampsCommande(int row, int) {
    ui->dateCommande->setDate(QDate::fromString(ui->tableCommandes->item(row, 1)->text(), "yyyy-MM-dd"));
    ui->typeLivraison->setCurrentText(ui->tableCommandes->item(row, 2)->text());
    ui->statutCommande->setCurrentText(ui->tableCommandes->item(row, 3)->text());
    ui->adresseLivraison->setText(ui->tableCommandes->item(row, 4)->text());
    ui->idClient->setText(ui->tableCommandes->item(row, 5)->text());
}
