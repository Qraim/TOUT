
#ifndef GAG2_H
#define GAG2_H


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
#include <QTimer>
#include <vector>
#include "bdd.h"


class gag2 : public QWidget {
Q_OBJECT


public:
    gag2(std::shared_ptr<bdd> db,QWidget *parent);

    void refresh(){
        _Longueur->clear();
        _Hauteur->clear();
        _Piezo->clear();
        _Perte->clear();

        Q->setFocus();

        Materiau->clear();

        std::vector<std::string> matiere_names = database->getAllMatiereNames();
        for (const auto& matiere_name : matiere_names) {
            Materiau->addItem(QString::fromStdString(matiere_name));
        }
        Materiau->setCurrentText("PVC");

    }

private :

    std::shared_ptr<bdd> database;


    QLineEdit *Q;
    QLineEdit *E;
    QLineEdit *D;
    QLineEdit *L;
    QLineEdit *H;

    QLineEdit *_Longueur;
    QLineEdit *_Hauteur;
    QLineEdit *_Perte;
    QLineEdit *_Piezo;

    QComboBox *unite;
    QComboBox *Materiau;

    ///
    /// @brief Effectue un calcul en fonction des valeurs entrées.
    ///
    void calcul();

    ///
    /// @brief Un filtre d'événement personnalisé pour gérer la navigation entre les widgets QLineEdit.
    ///
    bool eventFilter(QObject *obj, QEvent *event);

    ///
    /// @brief Vérifie si toutes les entrées sont remplies.
    ///
    bool allInputsFilled();

    ///
    /// @param lineedit : pointeur vers le QLineEdit trouvé
    /// @brief Vérifie si l'objet donné est un des widgets QLineEdit ciblés pour le remplissage.
    ///
    bool isTargetLineEdit(QLineEdit *lineEdit);

    ///
    /// @brief Met le focus sur le widget QLineEdit suivant.
    ///
    void focusNextTargetChild();

    ///
    /// @brief Met le focus sur le widget QLineEdit précédent.
    ///
    void focusPreviousTargetChild();
};

#endif // GAG2_H
