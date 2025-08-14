#include "mainwindow.h"
#include "ui_mainwindow.h"  // Important : pour avoir la définition complète de Ui::MainWindow

#include "ClientService.h"
#include "CommandeService.h"

// Inclusions Qt nécessaires
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDate>

// =========================
// Constructeur
// =========================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupValidators();

    // Connexions - Clients
    connect(ui->btnAjouterClient, &QPushButton::clicked, this, &MainWindow::ajouterClient);
    connect(ui->btnModifierClient, &QPushButton::clicked, this, &MainWindow::modifierClient);
    connect(ui->btnSupprimerClient, &QPushButton::clicked, this, &MainWindow::supprimerClient);
    connect(ui->tableClients, &QTableWidget::cellClicked, this, &MainWindow::remplirChampsClient);

    // Connexions - Commandes
    connect(ui->btnAjouterCommande, &QPushButton::clicked, this, &MainWindow::ajouterCommande);
    connect(ui->btnModifierCommande, &QPushButton::clicked, this, &MainWindow::modifierCommande);
    connect(ui->btnSupprimerCommande, &QPushButton::clicked, this, &MainWindow::supprimerCommande);
    connect(ui->tableCommandes, &QTableWidget::cellClicked, this, &MainWindow::remplirChampsCommande);

    // Charger les données initiales
    chargerClients();
    chargerCommandes();
}

MainWindow::~MainWindow() {
    delete ui;
}

// =========================
// Validators pour Clients
// =========================
void MainWindow::setupValidators() {
    QRegularExpressionValidator* nomValidator = new QRegularExpressionValidator(QRegularExpression("[a-zA-ZÀ-ÿ\\s'-]{2,}"), this);
    ui->lineNom->setValidator(nomValidator);
    ui->linePrenom->setValidator(nomValidator);

    QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    QRegularExpressionValidator* emailValidator = new QRegularExpressionValidator(emailRegex, this);
    ui->lineEmail->setValidator(emailValidator);
    connect(ui->lineEmail, &QLineEdit::textChanged, this, [this]() {
        ui->labelEmailError->setVisible(!ui->lineEmail->hasAcceptableInput());
    });

    connect(ui->lineTel, &QLineEdit::textChanged, this, [this]() {
        ui->labelTelError->setVisible(ui->lineTel->text().trimmed().length() < 8);
    });

    QRegularExpressionValidator* adresseValidator = new QRegularExpressionValidator(QRegularExpression(".{5,}"), this);
    ui->lineAdresse->setValidator(adresseValidator);
    connect(ui->lineAdresse, &QLineEdit::textChanged, this, [this]() {
        ui->labelAdresseError->setVisible(ui->lineAdresse->text().trimmed().length() < 5);
    });
}

// =========================
// Gestion Clients
// =========================
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

    if (nom.isEmpty() || prenom.isEmpty() || email.isEmpty() || tel.isEmpty() || adresse.isEmpty()) {
        QMessageBox::warning(this, "Champs manquants", "Veuillez remplir tous les champs.");
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
    }
}

void MainWindow::modifierClient() {
    int row = ui->tableClients->currentRow();
    if (row < 0) return;

    int id = ui->tableClients->item(row, 0)->text().toInt();
    Client c(id,
             ui->lineNom->text(),
             ui->linePrenom->text(),
             ui->lineEmail->text(),
             ui->lineTel->text(),
             ui->lineAdresse->text());

    if (ClientService::updateClient(c)) {
        chargerClients();
    }
}

void MainWindow::supprimerClient() {
    int row = ui->tableClients->currentRow();
    if (row < 0) return;

    int id = ui->tableClients->item(row, 0)->text().toInt();
    if (ClientService::deleteClient(id)) {
        chargerClients();
    }
}

void MainWindow::remplirChampsClient(int row, int) {
    ui->lineNom->setText(ui->tableClients->item(row, 1)->text());
    ui->linePrenom->setText(ui->tableClients->item(row, 2)->text());
    ui->lineEmail->setText(ui->tableClients->item(row, 3)->text());
    ui->lineTel->setText(ui->tableClients->item(row, 4)->text());
    ui->lineAdresse->setText(ui->tableClients->item(row, 5)->text());
}

// =========================
// Gestion Commandes
// =========================
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
    Commande c(0,
               ui->dateCommande->date(),
               ui->comboTypeLivraison->currentText(),
               ui->comboStatutCommande->currentText(),
               ui->adresseLivraison->text(),
               ui->idClientCommande->text().toInt());
    if (CommandeService::addCommande(c)) chargerCommandes();
}

void MainWindow::modifierCommande() {
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;

    int id = ui->tableCommandes->item(row, 0)->text().toInt();
    Commande c(id,
               ui->dateCommande->date(),
               ui->comboTypeLivraison->currentText(),
               ui->comboStatutCommande->currentText(),
               ui->adresseLivraison->text(),
               ui->idClientCommande->text().toInt());
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
    ui->comboTypeLivraison->setCurrentText(ui->tableCommandes->item(row, 2)->text());
    ui->comboStatutCommande->setCurrentText(ui->tableCommandes->item(row, 3)->text());
    ui->adresseLivraison->setText(ui->tableCommandes->item(row, 4)->text());
    ui->idClientCommande->setText(ui->tableCommandes->item(row, 5)->text());
}
