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
#include <set>
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

    matiere(){};
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
public:
    ///
    /// Constructeur de la classe 'bdd', prend un pointeur vers le widget parent en paramètre.
    ///
    bdd(QWidget *parent);

    ///
    /// Destructeur de la classe 'bdd'.
    ///
    ~bdd();

    ///
    /// @param m : Référence vers un objet 'matiere'
    /// @param bar : Pression en bar
    /// @return
    /// Renvoie un vecteur de paires contenant les diamètres internes et externes possibles pour la matière et la pression données
    ///
    std::vector<std::pair<int, float>> get_possible_tuyaux(const matiere& m, int bar);

    ///
    /// @param material_name : Nom de la matière
    /// @return
    /// Renvoie un tuple contenant les coefficients A, B et K de la matière spécifiée
    ///
    std::tuple<float, float, double> get_material_coefficients(const std::string& material_name);

    ///
    /// Affiche les tableaux des matériaux dans l'application
    ///
    void afficher_tableaux();

    ///
    /// @param material_name : Nom de la matière
    /// @return
    /// Renvoie un objet 'matiere' correspondant au nom de la matière donné
    ///
    matiere findMatiereByName(const std::string &material_name);

    ///
    /// @return
    /// Renvoie un vecteur contenant tous les noms des matériaux disponibles
    ///
    std::vector<std::string> getAllMatiereNames();

    ///
    /// @param material_name : Nom de la matière
    /// @return
    /// Renvoie un vecteur contenant toutes les pressions disponibles pour la matière spécifiée
    ///
    std::vector<int> getAllPressuresForMatiere(const std::string &material_name);

    std::vector<float> getInnerDiametersForMatiereAndPressure(const std::string &material_name, int pressure);

  private:


    std::vector<tableau> materials;

    std::tuple<std::string, int, int>find_matiere_pressure_and_outer_diameter(float inner_diameter);

    ///
    /// @brief Modifie les tableaux des matériaux
    ///
    void modifier_tableaux();

    ///
    /// @brief Ajoute des tableaux de matériaux
    ///
    void ajouter_tableaux();

    ///
    /// @brief Supprime des tableaux de matériaux
    ///
    void suprimmer_tableaux();

    ///
    /// @brief Affiche les tuyaux disponibles en fonction des critères sélectionnés
    ///
    void tuyau_dispo();

    ///
    /// @brief Trouve le tuyau approprié en fonction des critères sélectionnés
    ///
    void trouver_tuyau();

    ///
    /// @brief Affiche les coefficients du matériau sélectionné
    ///
    void montre_coef();

    void on_show_formula_button_clicked();

};


#endif //BDD_BDD_H


