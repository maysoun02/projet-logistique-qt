#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ClientService.h"
#include "CommandeService.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Clients
    connect(ui->btnAjouterClient, &QPushButton::clicked, this, &MainWindow::ajouterClient);
    connect(ui->btnModifierClient, &QPushButton::clicked, this, &MainWindow::modifierClient);
    connect(ui->btnSupprimerClient, &QPushButton::clicked, this, &MainWindow::supprimerClient);
    connect(ui->btnActualiserClient, &QPushButton::clicked, this, &MainWindow::chargerClients);
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
    Client c(0, ui->lineNom->text(), ui->linePrenom->text(), ui->lineEmail->text(), ui->lineTel->text(), ui->lineAdresse->text());
    if (ClientService::addClient(c)) chargerClients();
}

void MainWindow::modifierClient() {
    int row = ui->tableClients->currentRow();
    if (row < 0) return;
    int id = ui->tableClients->item(row, 0)->text().toInt();
    Client c(id, ui->lineNom->text(), ui->linePrenom->text(), ui->lineEmail->text(), ui->lineTel->text(), ui->lineAdresse->text());
    if (ClientService::updateClient(c)) chargerClients();
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
