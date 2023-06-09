///
/// Created by Quentin Raimbault on 22/05/2023.
///

#ifndef TOUT_PARCELLE_H
#define TOUT_PARCELLE_H
#include <vector>
#include <iostream>
#include <QSpinBox>
#include <QSpinBox>
#include <QDialog>
#include <QRadioButton>

#include "bdd.h"
class parcelle {

public:

    parcelle()=default;

    parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db, QString nom, bool amont2 = true);

    ///
    /// @brief Récupère le milieu hydrologique.
    ///
    int getMilieuhydro() const;

    ///
    /// @brief Récupère les données associées.
    ///
    std::vector<std::vector<float>> &getDonnees();

    ///
    /// @brief Récupère le poste de commande.
    ///
    int getPosteDeCommande() const;

    ///
    /// @param diameters : mesure du diametre en mm
    /// @param index : index de la ligne à modifier
    /// @brief Modifie le diamètre à l'index spécifié.
    ///
    void modifiedia(int index, float diameters);

    ///
    /// @brief Initialise la boîte de dialogue pour régler le diamètre.
    ///
    void setDiametreDialog();

    ///
    /// @brief Récupère le nom de la parcelle.
    ///
    const QString &getNom() const;

    ///
    /// @param nom : nom de la parcelle
    /// @brief Définit le nom.
    ///
    void setNom(const QString &nom);

    ///
    /// @brief Effectue un calcul en fonction des valeurs entrées.
    ///
    void calcul();

    ///
    /// @param posteDeCommande : Index du poste de commande
    /// @brief Définit le poste de commande.
    ///
    void setPosteDeCommande(int posteDeCommande);

    ///
    /// @brief Récupère l'index de début.
    ///
    int getIndexdebut() const;

    ///
    /// @brief Récupère l'index de fin.
    ///
    int getIndexfin() const;

    ///
    /// @brief retourne le bool amont pour gérer l'affichage
    /// @return
    bool isAmont() const;

    ///
    /// @brief Trouve et retourne le milieu hydrologique.
    ///
    int trouvemilieuhydro();

    ///
    /// @brief retourne la longueur de la parcelle
    ///
    float getLongueur() const;

    ///
    /// @brief retourne le débit totql de la parcelle
    ///
    float getDebit() const;

    /// @brief renvoie si le calcul à été effectué ou non
    /// @return
    bool isCalcul() const;

    /// @brief fixe le diametre des différents tuyaux
    /// @param matiere
    void setDiametreDialog(std::string matiere);

    /// @brief calcul la perte du cote droitedu poste de commande
    /// @param a coefficient A de la matiere choisis
    /// @param b coefficient B de la matiere choisis
    /// @param k coefficient K de la matiere choisis
    void calcul_droit(float a, float b, double k);

    /// @brief
    /// @param a coefficient A de la matiere choisis
    /// @param b coefficient B de la matiere choisis
    /// @param k coefficient K de la matiere choisis
    void calcul_gauche(float a, float b, double k);

    const std::vector<float> &getDiameters() const;
    const std::string &getMatiere() const;
    void setDonnees(const std::vector<std::vector<float>> &donnees);
    void setMilieuhydro(int milieuhydro);
    void setIndexdebut(int indexdebut);
    void setIndexfin(int indexfin);
    void setLongueur(float longueur);
    void setMatiere(const std::string &matiere);
    void setAmont(bool amont);
    void setDebit(float debit);
    void setCalcul(bool calcul);
    void calculaspersseurs(std::vector<int> &indice, float a, float b, double k);
    std::vector<int> trouveaspersseurs();
    void addDiameter(float a);

  private:

    std::shared_ptr<bdd> database;
    std::vector<std::vector<float>> _Donnees;
    std::vector<float> _diameters;
    QString _nom;
    int milieuhydro;
    int poste_de_commande;
    int _indexdebut;
    int _indexfin;
    float _longueur;
    std::string _matiere;
    bool amont;
    float _debit;
    bool _calcul;
    std::vector<std::vector<float>> _tableValues;

    std::vector<std::vector<float>> getTableValues();
    void onLineEditChanged(const QString &text, int row, int col);
    void showDialogWithTable();
};

#endif // TOUT_PARCELLE_H
