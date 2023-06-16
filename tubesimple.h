//
// Created by qraim on 04/04/23.
//

#ifndef TUBESIMPLE_TUBESIMPLE_H
#define TUBESIMPLE_TUBESIMPLE_H

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
#include <memory>

#include <iostream>
#include "bdd.h"


class tubesimple : public QWidget {
Q_OBJECT

public:

    tubesimple(std::shared_ptr<bdd> db, QWidget *parent = nullptr);
    ~tubesimple();
    void refresh(){
        debit.clear();
        diametre.clear();
        longueur.clear();
        denivele.clear();
        Perte.clear();
        Piezo.clear();
        Vitesse.clear();
        std::vector<std::string> matiere_names = database->getAllMatiereNames();
        materiau.clear();
        for (const auto& matiere_name : matiere_names) {
            materiau.addItem(QString::fromStdString(matiere_name));
        }
        materiau.setCurrentText("PVC");

    }

private:

    std::shared_ptr<bdd> database;

    QComboBox materiau;
    QComboBox *unite;

    QLineEdit debit;
    QLineEdit diametre;
    QLineEdit longueur;
    QLineEdit denivele;

    QLineEdit Perte;
    QLineEdit Piezo;
    QLineEdit Vitesse;

    QPushButton Calcul;

    ///
    /// @brief Gère l'événement de pression de touche du clavier
    ///
    /// @param event : Événement de pression de touche
    ///
    void keyPressEvent(QKeyEvent *event) override;

    ///
    /// @brief Change le champ actif pour le champ suivant
    ///
    void focusNextInput();

    ///
    /// @brief Change le champ actif pour le champ précédent
    ///
    void focusPreviousInput();

    ///
    /// @brief Calculer les résultats et les afficher dans le tableau
    ///
    void calculer();

    ///
    /// @brief Vérifier que tous les champs de saisie sont remplis
    ///
    void checkInputs();

    ///
    /// @brief Effacer les résultats dans le tableau
    ///
    void clearresult();

    void showdia();
};


#endif //TUBESIMPLE_TUBESIMPLE_H
