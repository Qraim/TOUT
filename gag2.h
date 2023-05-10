
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


    void calcul();

    bool eventFilter(QObject *obj, QEvent *event);

    bool allInputsFilled();

    bool isTargetLineEdit(QLineEdit *lineEdit);

    void focusNextTargetChild();

    void focusPreviousTargetChild();
};

#endif // GAG2_H
