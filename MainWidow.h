//
// Created by qraim on 03/04/23.
//

#ifndef PACHA_MAINWIDOW_H
#define PACHA_MAINWIDOW_H

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
#include "bdd.h"

#include <map>
#include <cmath>
#include <iostream>

class MainWindow : public QWidget {
Q_OBJECT

public:
    MainWindow(std::shared_ptr<bdd> db,QWidget *parent = nullptr);
    void refresh(){
        Materiau.clear();
        std::vector<std::string> matiere_names = database->getAllMatiereNames();
        for (const auto& matiere_name : matiere_names) {
            Materiau.addItem(QString::fromStdString(matiere_name));
        }
    }
private:

    std::shared_ptr<bdd> database;

    QComboBox Materiau;
    QComboBox Pression;

    QLineEdit debit;
    QLineEdit vitesse;
    QLineEdit longueur;
    QLineEdit denivele;

    QLabel Champresultat;
    QLineEdit Champligne;
    QLineEdit ChampVitesse;

    QPushButton Changer;
    QPushButton Calcul;

    ///
    /// @param index : Parametre de la combobox1
    /// Selectionne les parametres en fonction de la premier combobox
    void updateSecondComboBox(int index);

    ///
    /// @return
    /// Permet de lancer le bon calcul en fonction des parametres disponibles
    float calculer();

    ///
    /// @return
    /// Renvoie un std::map (diametre extérieur, intérieur ) du tuyau de la matiere/pression choisis
    std::map<float, float> gettableau();

    ///
    /// @return
    /// Permet de lancer la fonction calculer() sur le boutton est pressé
    void onButtonClicked();

    ///
    /// @return
    /// Permet d'activer ou de désactiver le bouton Calculer si les champs actifs sont vides
    void updateButtonState();

    ///
    /// @return
    /// Permet de cacher/révéler les boutons suivant le besoin de l'utilisateur
    void onSwitchButtonClicked();

    ///
    /// @return
    /// Si Entrée est appuyé, soit lancer le calcul, soit changer le champ actif
    void onReturnPressed();

    ///
    /// @return
    /// Calcul du perimetre suivant le débit et la vitesse
    float calculdebitvitesse();

    ///
    /// @return
    ///  Calcul du perimetre suivant le débit, le denivelé et une longueurs
    float calcullongueurdeniv();

    ///
    /// @param diametre : diametre intérieur nécéssaire renvoyé par le calcul
    /// Renvoie parmis un std::map de tuyau celui qui à le diametre intérieur supérieur le plus proche
    void Gettuyau(float diametre);

    ///
    /// @param event : appuie sur une touche quelconque
    /// Permet des raccourcis clavier tel que UP,DOWN, SHIFT + UP,...
    void keyPressEvent(QKeyEvent *event);

    ///
    /// @return
    /// Change le champ actif pour le champ antérieur
    void focusPreviousInput();

    ///
    /// @return
    /// Change le champ actif pour le champ suivant
    void focusNextInput();

    ///
    /// @param vitesse : Vitesse de l'eau calculer
    /// Permet de rendre visible et de remplir le champ vitesse
    void setvitesse(int vitesse);
};


#endif //PACHA_MAINWIDOW_H
