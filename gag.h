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


#include <vector>
#include "bdd.h"

// Modify the class definition in your gag.h file
class gag : public QWidget {
Q_OBJECT

public:
    explicit gag(std::shared_ptr<bdd> db,QWidget *parent = nullptr);

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

    void keyPressEvent(QKeyEvent *event) override;
    
    void AjoutDonnee();

    void calcul();

    void focusPreviousInput();

    void focusNextInput();

    void RafraichirTableau();

    void recopiederniereligne();

    void AjoutLigne();

    void updateData(int numeroLigne, float debit,float espacement, float diametre, float longueur, float hauteur);

    void showUpdateDialog();

    void enleverLigne();

    void clear();
};



#endif //GAG_GAG_H
