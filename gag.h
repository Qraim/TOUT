//
// Created by qraim on 07/04/23.
//

#ifndef GAG_GAG_H
#define GAG_GAG_H


#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QKeyEvent>
#include <cmath>
#include <iostream>
#include <QFormLayout>
#include <QDialog>
#include <QShortcut>
#include <QPushButton>
#include <QDoubleValidator>
#include <QLocale>
#include <QSpacerItem>


#include <vector>
#include "bdd.h"

class gag : public QWidget {
Q_OBJECT

public:
    explicit gag(std::shared_ptr<bdd> db,QWidget *parent = nullptr);

    void refresh(){
        Materiau->clear();
        std::vector<std::string> matiere_names = database->getAllMatiereNames();
        for (const auto& matiere_name : matiere_names) {
            Materiau->addItem(QString::fromStdString(matiere_name));
        }
    }
private:

    std::shared_ptr<bdd> database;

    QComboBox *Materiau;

    QLineEdit *Debit;
    QLineEdit *Espacement;
    QLineEdit *Diametre;
    QLineEdit *Longueur;
    QLineEdit *Hauteur;

    QGridLayout *scrollAreaLayout;

    QLineEdit *CumulLongueur;
    QLineEdit *Cumulhauteur;
    QLineEdit *CumulPerte;
    QLineEdit *CumulPiezo;

    std::vector<std::vector<float>> _Donnees;

    /// @brief Gère les événements de pression des touches pour les éléments de l'interface utilisateur.
    /// @param event : Événement de pression de touche
    void keyPressEvent(QKeyEvent *event) override;

    /// @brief Ajoute des données à la base de données.
    void AjoutDonnee();

    /// @brief Effectue le calcul pour les données saisies.
    void calcul();

    /// @brief Change le focus pour sélectionner l'entrée précédente dans l'interface utilisateur.
    void focusPreviousInput();

    /// @brief Change le focus pour sélectionner la prochaine entrée dans l'interface utilisateur.
    void focusNextInput();

    /// @brief Met à jour le tableau affiché avec les dernières données.
    void RafraichirTableau();

    /// @brief Copie les données de la dernière ligne du tableau.
    void recopiederniereligne();

    /// @brief Ajoute une nouvelle ligne au tableau.
    void AjoutLigne();

    /// @brief Met à jour les données dans la base de données en fonction des paramètres fournis.
    /// @param numeroLigne : Numéro de la ligne à mettre à jour
    /// @param debit : Valeur du débit à mettre à jour
    /// @param espacement : Valeur de l'espacement à mettre à jour
    /// @param diametre : Valeur du diamètre à mettre à jour
    /// @param longueur : Valeur de la longueur à mettre à jour
    /// @param hauteur : Valeur de la hauteur à mettre à jour
    void updateData(int numeroLigne, float debit,float espacement, float diametre, float longueur, float hauteur);

    /// @brief Affiche une boîte de dialogue permettant de mettre à jour les données.
    void showUpdateDialog();

    /// @brief Supprime une ligne du tableau.
    void enleverLigne();

    /// @brief Efface toutes les données saisies.
    void clear();

};



#endif //GAG_GAG_H
