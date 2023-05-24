
#ifndef ETUDE_H
#define ETUDE_H


#include <QMainWindow>
#include <QWidget>
#include <vector>
#include <memory>
#include "bdd.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QClipboard>
#include "parcelle.h"

class etude : public QWidget
{
    Q_OBJECT
public:
    etude(std::shared_ptr<bdd> db,QWidget *parent = nullptr);

signals:
private :
    std::shared_ptr<bdd> database;
    std::vector<std::vector<float>> _Donnees;
    std::vector<parcelle> _parcelles;

    bool milieu;
    bool limitations;
    bool poste;

    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;

    ///
    /// @brief Initialise les données ou les paramètres nécessaires.
    ///
    void init();

    ///
    /// @param data : Texte que l'on copie/colle depuis géogag pour extraire les données
    /// @brief Traite les données en entrée.
    ///
    void traitements(QString data);

    ///
    /// @brief Rafraîchit les données affichées dans le tableau.
    ///
    void rafraichirTableau();

    ///
    /// @brief Efface les données ou les éléments enfants.
    ///
    void clearchild();

    ///
    /// @brief Initialise les calculs à effectuer.
    ///
    void initCalcul();

    ///
    /// @brief Divise les données pour le traitement.
    ///
    void divideData();

    ///
    /// @brief Effectue un calcul en fonction des valeurs entrées.
    ///
    void calcul();

    ///
    /// @brief Met à jour les données en fonction des modifications apportées.
    ///
    void updateDonnees();

    ///
    /// @brief Affiche la boîte de dialogue des options.
    ///
    void showOptionsDialog();

    ///
    /// @brief Permet de choisir le poste de commande.
    ///
    void chooseCommandPost();

    ///
    /// @brief Appelle la boîte de dialogue de réglage du diamètre.
    ///
    void appelSetDiametreDialog();

    ///
    /// @param row : index de la colonne ou l'on veut modifier le diametre
    /// @param newDiameter : Diametre modifié de la ligne
    /// @brief Met à jour le diamètre à la ligne spécifiée avec la nouvelle valeur de diamètre.
    ///
    void updateDiameter(int row, const QString& newDiameter);


};

#endif // ETUDE_H
