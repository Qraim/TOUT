//
// Created by qraim on 11/04/23.
//

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <tuple>
#include <iostream>
#include <QString>
#include <QKeyEvent>
#include <QDialog>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <map>
#include <QString>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <QStandardItemModel>
#include<bits/stdc++.h>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QMainWindow>
#include <QStackedWidget>

#ifndef BDD_BDD_H
#define BDD_BDD_H


struct pression {
    int bar;
    std::vector<std::pair<int, float>> diametre;

    pression(int b, std::vector<std::pair<int, float>> d) : bar(b), diametre(d) {}
};

struct matiere {
    std::string nom;
    float a, b;
    double k;
    std::vector<pression> pressions;

    matiere(std::string n, float a_, float b_, double k_, std::vector<pression> p) : nom(n), a(a_), b(b_), k(k_), pressions(p) {}
};


struct tableau {
    std::string nom;
    std::vector<matiere> matieres;

    tableau(const std::string& n, const std::vector<matiere>& p)
            : nom(n), matieres(p) {}
};



class bdd : public QWidget {
Q_OBJECT
public :
    bdd(QWidget *parent);
    ~bdd();
     std::string find_matiere_for_diametre(float diametre_interieur);

    std::vector<std::pair<int, float>> get_possible_tuyaux(const matiere& m, int bar);

    std::tuple<float, float, float> get_material_coefficients(const std::string& material_name);

    std::pair<std::string, float> find_matiere_and_outer_diameter(float inner_diameter);

private :
    std::vector<tableau> materials;

    void afficher_tableaux();

    void modifier_tableaux();

    void ajouter_tableaux();

    void suprimmer_tableaux();

    void tuyau_dispo();

    void trouver_tuyau();

    void montre_coef();

   };


#endif //BDD_BDD_H


