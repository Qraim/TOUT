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



    /// @brief calcul les pertes de charges pour chaque ligne ou l'indice est contenu dans le vecteur indice
    /// @param indice vecteur des indices des lignes à calculer
    /// @param a coefficient A de la matiere choisis
    /// @param b coefficient B de la matiere choisis
    /// @param k coefficient K de la matiere choisis
    void calculaspersseurs(std::vector<int> &indice, float a, float b, double k);


    /// @brief trouve les lignes ou il y a un aspersseurs et renvoie les indices de celles ci dans un vecteur trié
    /// @return vector of int
    std::vector<int> trouveaspersseurs();

    /// @brief ajoute un diametre dans le vecteur _Donnees et diameter
    /// @param a diametre à ajouter
    void addDiameter(float a);

    /// @brief Foction permettant de changer la valeurs d'une ligne et d'une colonne spécifique
    /// @param text valeurs à changer
    /// @param row numero de ligne
    /// @param col numero de colonne
    void onLineEditChanged(const QString &text, int row, int col);

    /// @brief permet de choisir pour chaque ligne contennant un aspersseurs et pour chaque aspersseurs un diametre
    void showDialogWithTable();

    /// @brief permet de changer l'interval ciblé
    /// @param index
    /// @param colonne
    /// @param diameters
    void modifieinter(int index, int colonne, float diameters);


    /// @brief Liste des Setters
    /// @param amont
    void SetAmont(bool amont);
    void setDonnees(const std::vector<std::vector<float>> &donnees);
    void setMilieuhydro(int milieuhydro);
    void setIndexdebut(int indexdebut);
    void setIndexfin(int indexfin);
    void setLongueur(float longueur);
    void setMatiere(const std::string &matiere);
    void setAmont(bool amont);
    void setDebit(float debit);
    void setCalcul(bool calcul);

    /// @brief Liste des Getters
    /// @return
    std::vector<std::vector<float>> getTableValues();
    const std::vector<float> &getDiameters() const;
    const std::string &getMatiere() const;


    /// @brief calcul du peigne en asperssions
    /// @param a coefficient A de la matiere choisis
    /// @param b coefficient B de la matiere choisis
    /// @param k coefficient K de la matiere choisis
    /// @param debitgoutteur
    void calculpeigne(float a, float b, double k, float debitgoutteur);

    void inverser();

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

    void calculdiametre(float a, float b, double k);
};

#endif // TOUT_PARCELLE_H
