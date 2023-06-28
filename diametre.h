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
        std::vector<std::string> matiere_names = database->getAllMatiereNames();
        Materiau.clear();
        for (const auto& matiere_name : matiere_names) {
            Materiau.addItem(QString::fromStdString(matiere_name));
        }
        Materiau.setCurrentText("PVC");
        Pression.setCurrentText("10");

        debit.clear();
        vitesse.clear();
        longueur.clear();
        denivele.clear();
        Champligne.clear();
        ChampVitesse.clear();
        Champligne2.clear();
        Champligne3.clear();
        Champresultat.clear();
    }
private:

    std::shared_ptr<bdd> database;

    QComboBox Materiau;
    QComboBox Pression;
    QComboBox *Unite;

    QLineEdit debit;
    QLineEdit vitesse;
    QLineEdit longueur;
    QLineEdit denivele;

    QLabel *debitLabel;
    QLabel *vitesseLabel;
    QLabel *longueurLabel;
    QLabel *deniveleLabel;
    QLabel *vitesseUnite;
    QLabel *longueurUnite;
    QLabel *deniveleUnite;

    QLabel Champresultat;
    QLineEdit Champligne;
    QLineEdit ChampVitesse;
    QLineEdit Champligne2;
    QLineEdit Champligne3;


    QPushButton Changer;
    QPushButton Calcul;

    ///
    /// @param index : Parametre de la combobox1
    /// @brief Selectionne les parametres en fonction de la premier combobox
    void updateSecondComboBox(int index);

    ///
    /// @return
    /// @brief Permet de lancer le bon calcul en fonction des parametres disponibles
    float calculer();

    ///
    /// @return
    /// @brief Permet de lancer la fonction calculer() sur le boutton est pressé
    void onButtonClicked();

    ///
    /// @return
    /// @brief Permet d'activer ou de désactiver le bouton Calculer si les champs actifs sont vides
    void updateButtonState();

    ///
    /// @return
    /// @brief Permet de cacher/révéler les boutons suivant le besoin de l'utilisateur
    void onSwitchButtonClicked();

    ///
    /// @return
    /// @brief Si Entrée est appuyé, soit lancer le calcul, soit changer le champ actif
    void onReturnPressed();

    ///
    /// @return
    /// @brief Calcul du perimetre suivant le débit et la vitesse
    float calculdebitvitesse();

    ///
    /// @return
    /// @brief Calcul du perimetre suivant le débit, le denivelé et une longueurs
    float calcullongueurdeniv();

    ///
    /// @param diametre : diametre intérieur nécéssaire renvoyé par le calcul
    /// @brief Renvoie parmis un std::map de tuyau celui qui à le diametre intérieur supérieur le plus proche
    void Gettuyau(float diametre);

    ///
    /// @param event : appuie sur une touche quelconque
    /// @brief Permet des raccourcis clavier tel que UP,DOWN, SHIFT + UP,...
    void keyPressEvent(QKeyEvent *event);

    ///
    /// @return
    /// @brief Change le champ actif pour le champ antérieur
    void focusPreviousInput();

    ///
    /// @return
    /// @brief Change le champ actif pour le champ suivant
    void focusNextInput();

    ///
    /// @param vitesse : Vitesse de l'eau calculer
    /// @brief Permet de rendre visible et de remplir le champ vitesse
    void setvitesse(int vitesse);
};


#endif //PACHA_MAINWIDOW_H
