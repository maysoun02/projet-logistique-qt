#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ClientService.h"
#include "CommandeService.h"
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QDate>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLegend>

// =========================
// Constructeur
// =========================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dateCommande->setCalendarPopup(true);

    ui->labelAdresseLivraisonError->setVisible(false);
    ui->labelIdClientCommandeError->setVisible(false);

    setupValidators();

    // Connexions - Clients
    connect(ui->btnAjouterClient, &QPushButton::clicked, this, [this]{
        ajouterClient();
        afficherGraphClients();
    });
    connect(ui->btnModifierClient, &QPushButton::clicked, this, &MainWindow::modifierClient);
    connect(ui->btnSupprimerClient, &QPushButton::clicked, this, [this]{
        supprimerClient();
        afficherGraphClients();
    });
    connect(ui->tableClients, &QTableWidget::cellClicked, this, &MainWindow::remplirChampsClient);

    // Connexions - Commandes
    connect(ui->btnAjouterCommande, &QPushButton::clicked, this, [this]{
        ajouterCommande();
        afficherGraphCommandes();
    });
    connect(ui->btnModifierCommande, &QPushButton::clicked, this, &MainWindow::modifierCommande);
    connect(ui->btnSupprimerCommande, &QPushButton::clicked, this, [this]{
        supprimerCommande();
        afficherGraphCommandes();
    });
    connect(ui->tableCommandes, &QTableWidget::cellClicked, this, &MainWindow::remplirChampsCommande);

    // Charger données initiales
    chargerClients();
    chargerCommandes();
    afficherGraphClients();
    afficherGraphCommandes();
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
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client.");
        return;
    }

    auto getTextSafe = [&](int col) -> QString {
        auto item = ui->tableClients->item(row, col);
        return item ? item->text() : "";
    };

    QString nom     = getTextSafe(1);
    QString prenom  = getTextSafe(2);
    QString email   = getTextSafe(3);
    QString tel     = getTextSafe(4);
    QString adresse = getTextSafe(5);

    if (nom.isEmpty() || prenom.isEmpty() || email.isEmpty() ||
        tel.isEmpty() || adresse.isEmpty()) {
        QMessageBox::critical(this, "Erreur", "Les données du client sélectionné sont incomplètes.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Modifier Client");
    QFormLayout form(&dialog);

    QLineEdit nomEdit(nom);
    form.addRow("Nom :", &nomEdit);

    QLineEdit prenomEdit(prenom);
    form.addRow("Prénom :", &prenomEdit);

    QLineEdit emailEdit(email);
    form.addRow("Email :", &emailEdit);

    QLineEdit telEdit(tel);
    form.addRow("Téléphone :", &telEdit);

    QLineEdit adresseEdit(adresse);
    form.addRow("Adresse :", &adresseEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (nomEdit.text().trimmed().isEmpty() || prenomEdit.text().trimmed().isEmpty() ||
            emailEdit.text().trimmed().isEmpty() || telEdit.text().trimmed().isEmpty() ||
            adresseEdit.text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis.");
            return;
        }

        int id = ui->tableClients->item(row, 0)->text().toInt();
        Client c(id,
                 nomEdit.text(),
                 prenomEdit.text(),
                 emailEdit.text(),
                 telEdit.text(),
                 adresseEdit.text());

        if (ClientService::updateClient(c)) {
            chargerClients();
        }
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
    ui->tableCommandes->setColumnCount(6);

    QStringList headers = {"ID", "Date Commande", "Type Livraison", "Statut Commande", "Adresse Livraison", "ID Client"};
    ui->tableCommandes->setHorizontalHeaderLabels(headers);

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

    ui->tableCommandes->setColumnHidden(0, true);
    ui->tableCommandes->resizeColumnsToContents();

}

// =========================
// Ajout commande
// =========================
void MainWindow::ajouterCommande() {
    if (ui->adresseLivraison->text().trimmed().length() < 5) {
        QMessageBox::warning(this, "Erreur", "L'adresse de livraison doit contenir au moins 5 caractères.");
        return;
    }
    if (ui->idClientCommande->text().toInt() <= 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID client valide.");
        return;
    }

    Commande c(0,
               ui->dateCommande->date(),
               ui->comboTypeLivraison->currentText(),
               ui->comboStatutCommande->currentText(),
               ui->adresseLivraison->text(),
               ui->idClientCommande->text().toInt());

    if (CommandeService::addCommande(c)) {
        chargerCommandes();
    }
}

// =========================
// Modifier commande (popup)
// =========================
void MainWindow::modifierCommande() {
    int row = ui->tableCommandes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une commande.");
        return;
    }

    auto getTextSafe = [&](int col) -> QString {
        auto item = ui->tableCommandes->item(row, col);
        return item ? item->text() : "";
    };

    QString dateText     = getTextSafe(1);
    QString typeText     = getTextSafe(2);
    QString statutText   = getTextSafe(3);
    QString adresseText  = getTextSafe(4);
    QString clientIdText = getTextSafe(5);

    if (dateText.isEmpty() || typeText.isEmpty() || statutText.isEmpty() ||
        adresseText.isEmpty() || clientIdText.isEmpty()) {
        QMessageBox::critical(this, "Erreur", "Les données de la commande sélectionnée sont incomplètes.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Modifier Commande");
    QFormLayout form(&dialog);

    QDateEdit dateEdit;
    dateEdit.setDate(QDate::fromString(dateText, "yyyy-MM-dd"));
    dateEdit.setCalendarPopup(true);
    form.addRow("Date commande :", &dateEdit);

    QComboBox typeCombo;
    typeCombo.addItems({"Express", "Standard"});
    typeCombo.setCurrentText(typeText);
    form.addRow("Type livraison :", &typeCombo);

    QComboBox statutCombo;
    statutCombo.addItems({"En cours", "Livrée", "Annulée"});
    statutCombo.setCurrentText(statutText);
    form.addRow("Statut commande :", &statutCombo);

    QLineEdit adresseEdit(adresseText);
    form.addRow("Adresse livraison :", &adresseEdit);

    QLineEdit clientIdEdit(clientIdText);
    clientIdEdit.setValidator(new QIntValidator(1, 999999, this));
    form.addRow("ID client :", &clientIdEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (adresseEdit.text().trimmed().length() < 5) {
            QMessageBox::warning(this, "Erreur", "L'adresse de livraison doit contenir au moins 5 caractères.");
            return;
        }
        if (clientIdEdit.text().toInt() <= 0) {
            QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID client valide.");
            return;
        }

        int id = ui->tableCommandes->item(row, 0)->text().toInt();
        Commande c(id,
                   dateEdit.date(),
                   typeCombo.currentText(),
                   statutCombo.currentText(),
                   adresseEdit.text(),
                   clientIdEdit.text().toInt());

        if (CommandeService::updateCommande(c)) {
            chargerCommandes();
        }
    }
}

// =========================
// Supprimer commande
// =========================
void MainWindow::supprimerCommande() {
    int row = ui->tableCommandes->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une commande.");
        return;
    }

    int id = ui->tableCommandes->item(row, 0)->text().toInt();
    if (CommandeService::deleteCommande(id)) {
        chargerCommandes();
    }
}

bool MainWindow::validerChampsCommande() {
    bool valide = true;

    if (ui->adresseLivraison->text().trimmed().length() < 5) {
        ui->labelAdresseLivraisonError->setVisible(true);
        valide = false;
    } else {
        ui->labelAdresseLivraisonError->setVisible(false);
    }

    QRegularExpression re("\\d+");
    if (!re.match(ui->idClientCommande->text()).hasMatch()) {
        ui->labelIdClientCommandeError->setVisible(true);
        valide = false;
    } else {
        ui->labelIdClientCommandeError->setVisible(false);
    }

    return valide;
}

void MainWindow::remplirChampsCommande(int row, int) {
    if (row < 0) return;

    auto getTextSafe = [&](int col) -> QString {
        auto item = ui->tableCommandes->item(row, col);
        return item ? item->text() : "";
    };

    QString dateText     = getTextSafe(1);
    QString typeText     = getTextSafe(2);
    QString statutText   = getTextSafe(3);
    QString adresseText  = getTextSafe(4);
    QString clientIdText = getTextSafe(5);

    if (!dateText.isEmpty())
        ui->dateCommande->setDate(QDate::fromString(dateText, "yyyy-MM-dd"));
    ui->comboTypeLivraison->setCurrentText(typeText);
    ui->comboStatutCommande->setCurrentText(statutText);
    ui->adresseLivraison->setText(adresseText);
    ui->idClientCommande->setText(clientIdText);
}

// =========================
// Statistiques (Graphiques)
// =========================
void MainWindow::afficherGraphClients()
{
    int nbClients = ui->tableClients->rowCount();

    QPieSeries *series = new QPieSeries();
    series->append("Clients", nbClients);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statistiques Clients");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLayoutItem *child;
    while ((child = ui->layoutGraphClients->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    ui->layoutGraphClients->addWidget(chartView);
}

void MainWindow::afficherGraphCommandes()
{
    int enCours = 0, livree = 0, annulee = 0;

    for (int i = 0; i < ui->tableCommandes->rowCount(); ++i) {
        QString statut = ui->tableCommandes->item(i, 3)->text();
        if (statut == "En cours") enCours++;
        else if (statut == "Livrée") livree++;
        else if (statut == "Annulée") annulee++;
    }

    QPieSeries *series = new QPieSeries();
    series->append("En cours", enCours);
    series->append("Livrée", livree);
    series->append("Annulée", annulee);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition des Commandes");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QLayoutItem *child;
    while ((child = ui->layoutGraphCommandes->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    ui->layoutGraphCommandes->addWidget(chartView);
}

// =========================
// Recherche & Tri multicritères - Clients
// =========================
void MainWindow::rechercherClientsParNom(const QString &critere)
{
    for (int i = 0; i < ui->tableClients->rowCount(); ++i) {
        QString valeur = ui->tableClients->item(i, 1)->text();
        ui->tableClients->setRowHidden(i, !valeur.contains(critere, Qt::CaseInsensitive));
    }
}

void MainWindow::rechercherClientsParPrenom(const QString &critere)
{
    for (int i = 0; i < ui->tableClients->rowCount(); ++i) {
        QString valeur = ui->tableClients->item(i, 2)->text();
        ui->tableClients->setRowHidden(i, !valeur.contains(critere, Qt::CaseInsensitive));
    }
}

void MainWindow::rechercherClientsParEmail(const QString &critere)
{
    for (int i = 0; i < ui->tableClients->rowCount(); ++i) {
        QString valeur = ui->tableClients->item(i, 3)->text();
        ui->tableClients->setRowHidden(i, !valeur.contains(critere, Qt::CaseInsensitive));
    }
}

void MainWindow::trierClientsParNom()
{
    ui->tableClients->sortItems(1, Qt::AscendingOrder);
}

void MainWindow::trierClientsParPrenom()
{
    ui->tableClients->sortItems(2, Qt::AscendingOrder);
}

void MainWindow::trierClientsParEmail()
{
    ui->tableClients->sortItems(3, Qt::AscendingOrder);
}

// =========================
// Recherche & Tri multicritères - Commandes
// =========================
void MainWindow::rechercherCommandesParDate(const QString &critere)
{
    for (int i = 0; i < ui->tableCommandes->rowCount(); ++i) {
        QString valeur = ui->tableCommandes->item(i, 1)->text();
        ui->tableCommandes->setRowHidden(i, !valeur.contains(critere, Qt::CaseInsensitive));
    }
}

void MainWindow::rechercherCommandesParType(const QString &critere)
{
    for (int i = 0; i < ui->tableCommandes->rowCount(); ++i) {
        QString valeur = ui->tableCommandes->item(i, 2)->text();
        ui->tableCommandes->setRowHidden(i, !valeur.contains(critere, Qt::CaseInsensitive));
    }
}

void MainWindow::rechercherCommandesParStatut(const QString &critere)
{
    for (int i = 0; i < ui->tableCommandes->rowCount(); ++i) {
        QString valeur = ui->tableCommandes->item(i, 3)->text();
        ui->tableCommandes->setRowHidden(i, !valeur.contains(critere, Qt::CaseInsensitive));
    }
}

void MainWindow::trierCommandesParDate()
{
    ui->tableCommandes->sortItems(1, Qt::AscendingOrder);
}

void MainWindow::trierCommandesParType()
{
    ui->tableCommandes->sortItems(2, Qt::AscendingOrder);
}

void MainWindow::trierCommandesParStatut()
{
    ui->tableCommandes->sortItems(3, Qt::AscendingOrder);
}


