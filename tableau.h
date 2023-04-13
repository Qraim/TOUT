//
// Created by qraim on 05/04/23.
//
#ifndef PERTEDECHARGEHERSE_TABLEAU_H
#define PERTEDECHARGEHERSE_TABLEAU_H

#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QShortcut>
#include <QIcon>
#include <QPixmap>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QDoubleValidator>
#include <iostream>
#include <QScrollArea>
#include <QScrollBar>
#include <QDialog>


#include <vector>
#include <math.h>
#include "bdd.h"

class pertechargeherse : public QWidget {
Q_OBJECT

public:
    pertechargeherse(std::shared_ptr<bdd> db,QWidget *parent = nullptr);
    ~pertechargeherse();

private:

    std::shared_ptr<bdd> database;

    int ligne;

    QLineEdit *inputD;
    QLineEdit *inputQ;
    QLineEdit *inputH;
    QLineEdit *inputL;
    QComboBox *Materiau;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget* scrollWidget;

    QLineEdit *sigmadebitcase;
    QLineEdit *sigmalongueurcase;
    QLineEdit *sigmapertecase;
    QLineEdit *sigmapiezocase;

    std::vector<std::vector<float>> _Donnees;

    ///
    /// Ajoute les données saisies dans les champs de saisie à la dernière ligne du tableau
    void AjoutDonne();

    ///
    /// Ajoute une ligne vide à la fin du tableau
    void AjoutLigne();

    ///
    /// Change le champ actif pour le champ suivant
    void focusNextInput();

    ///
    /// Change le champ actif pour le champ précédent
    void focusPreviousInput();

    ///
    /// Traite l'appui sur une touche du clavier
    /// @param event : L'événement clavier qui a été détecté
    void keyPressEvent(QKeyEvent *event);

    ///
    /// Vérifie si tous les champs de saisie sont remplis
    /// @return : Vrai si tous les champs sont remplis, faux sinon
    bool Allinputfill();

    ///
    /// Effectue les calculs nécessaires et affiche les résultats
    void calcul();

    ///
    /// Actualise l'affichage du tableau
    void RafraichirTableau();

    ///
    /// Supprime tous les widgets enfants du layout principal
    void clearchild();

    ///
    /// Copie les valeurs de la dernière ligne dans la ligne courante
    void recopiederniereligne();

    ///
    /// Affiche une fenêtre de dialogue permettant de modifier une ligne du tableau
    void showUpdateDialog();

    ///
    /// Met à jour les données de la ligne spécifiée dans le tableau
    /// @param rowNumber : Numéro de la ligne à mettre à jour
    /// @param debit : Valeur du champ Débit de la ligne
    /// @param diameter : Valeur du champ Diamètre de la ligne
    /// @param length : Valeur du champ Longueur de la ligne
    /// @param height : Valeur du champ Hauteur de la ligne
    void updateData(int rowNumber, float debit, float diameter, float length, float height);

    ///
    /// Supprime une ligne du tableau
    void enleverLigne();


};

#endif //PERTEDECHARGEHERSE_TABLEAU_H
