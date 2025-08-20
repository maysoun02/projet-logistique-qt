#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVector>
#include <QDate>

// Qt Charts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLegend>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Clients
    void chargerClients();
    void ajouterClient();
    void modifierClient();
    void supprimerClient();

    void remplirChampsClient(int row, int column);
    void rechercherClientsParNom(const QString &critere);
    void rechercherClientsParPrenom(const QString &critere);
    void rechercherClientsParEmail(const QString &critere);

    void trierClientsParNom();
    void trierClientsParPrenom();
    void trierClientsParEmail();

    // Commandes
    void chargerCommandes();
    void ajouterCommande();
    void modifierCommande();
    void supprimerCommande();
    void remplirChampsCommande(int row, int column);

    void rechercherCommandesParDate(const QString &critere);
    void rechercherCommandesParType(const QString &critere);
    void rechercherCommandesParStatut(const QString &critere);

    void trierCommandesParDate();
    void trierCommandesParType();
    void trierCommandesParStatut();

private:
    Ui::MainWindow *ui;
    void setupValidators();
    bool validerChampsCommande();

    // --- Graphiques ---
    void afficherGraphClients();
    void afficherGraphCommandes();
};

#endif // MAINWINDOW_H
