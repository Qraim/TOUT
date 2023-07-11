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
#include "herse.h"

class parcelle {

public:

    parcelle()=default; // pour pouvoir instancier une parcelle sans parametres

    parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db, QString nom, bool amont2 = true, std::string mat="PEHD");


    void placerarrosseurs(int ligne, int nombre);

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
    /// @brief retourne le débit totql de la parcelle
    ///
    float getDebit() const;

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

    /// @brief permet de changer l'interval ciblé
    /// @param index
    /// @param colonne
    /// @param diameters
    void modifieinter(int index, int colonne, float diameters);


    /// @brief Liste des Setters
    /// @param amont
    void SetAmont(bool amont);

    /// @brief Liste des Getters
    /// @return
    const std::string &getMatiere() const;


    /// @brief retourne la parcelle
    void inverser();

    /// @brief  permet de choisir si le poste est du cote gauche ou droit
    /// @param a valeur du décalage du poste (0 pour gauche, 1 pour droite)
    void choisirCote(int a);

    /// @brief calcul la perte en diametre 16 ou 20 pour chaque ligne d'asperssions
    /// @param a coefficient A de la matiere choisis
    /// @param b coefficient B de la matiere choisis
    /// @param k coefficient K de la matiere choisis
    void calculdiametre(float a, float b, double k);

    /// @brief modifie le diametre de la ligne choisis
    /// @param index index de la ligne
    /// @param diameters valeurs du débit
    void modifiedebit(int index, float diameters);

    /// @brief lance le calcul de perte de la ligne ciblé dans une herse
    /// @param ligne
    void herse(int ligne);

    /// @brief convertit une parcelle en texte
    /// @return
    [[maybe_unused]] std::string toString() const;

    /// @brief renvoie l'indice du poste dans la parcelle en entiere
    /// @return
    int getvraiindiceposte();

    /// @brief renvoie le nombre d'aspersseurs
    /// @return
    int nbasp();

    /// @brief renvoie le désalage pour savoir si la ligne du poste est compté comme au dessus ou en dessous
    /// @return
    int getDecalage() const;

    /// @brief fixe le désalage pour savoir si la ligne du poste est compté comme au dessus ou en dessous
    /// @return
    void setDecalage(int decalage);

    /// @brief permet de recalculer certaines colonnes si on change le débit d'une ligne
    void recalcul();

    /// @brief calcul la perte du cote gauche du poste en mode aspersions
    /// @param indices ligne contenant des aspersseurs
    /// @param debit debit pour chaque aspersseurs
    /// @param a coefficient A de la matiere
    /// @param b coefficient B de la matiere
    /// @param k coefficient K de la matiere
    void calcul_gauche_aspersseurs(std::vector<int> &indices,float debit, float a, float b,
                                   double k);

    /// @brief calcul la perte du cote droit du poste en mode aspersions
    /// @param indices ligne contenant des aspersseurs
    /// @param debit debit pour chaque aspersseurs
    /// @param a coefficient A de la matiere
    /// @param b coefficient B de la matiere
    /// @param k coefficient K de la matiere
    void calcul_droit_aspersseurs(std::vector<int> &indices,float debit, float a, float b,
                                  double k);


    /// @brief calcul et renvoie le milieu hydrolique en aspersions
    /// @return int
    int trouvemilieuhydroasp();


    /// @brief Liste de getters/setters
    float getAspdebit() const;

    void setAspdebit(float d);

    float getAspinterdebut() const;

    void setAspinterdebut(float d);








    /// permet de changer le débit
    /// \param multi
    void doubledebit(float multi);

    /// calcul en hectare l'aire de la parcelle
    /// \return
    float hectare();

    ///
    /// \param diametres vecteur de diametres disponible
    /// \param piezo limite de piezo
    /// \param aspdebit débit par asperseurs
    /// \param aspinterdebut distance entre asperseurs
    void optimize(std::vector<float> diametres, float piezo=0,float aspdebit = 0, float aspinterdebut=0);



    /// Fonction optimisant le calcul pour 2 cotes dans deux mode différents (GAG, ASP)
    void optimize_diameters_droit(float a, float b, double k,float piezo,std::vector<float> diametresDisponibles);

    void optimize_diameters_gauche(float a, float b, double k,float piezo,std::vector<float> diametresDisponibles);

    float calcul_vitesse(float dia, float sigmadebit, float a, float b, double k);

    void optimize_aspersseurs(std::vector<int> indices, float a, float d, double k, float piezo,std::vector<float> diametresDisponibles);

    void optimize_gauche_aspersseurs(std::vector<int> &indices, float debit, float a, float b, double k,float piezo,std::vector<float> diametresDisponibles);

    void optimize_droit_aspersseurs(std::vector<int> &indices, float debit, float a, float b, double k,float piezo,std::vector<float> diametresDisponibles);

private:

    std::shared_ptr<bdd> database; // base de données
    std::unique_ptr<pertechargeherse> hersealim; // Calcul de ligne en ASP
    std::vector<std::vector<float>> _Donnees; // Données de la parcelle : rang ,etc
    QString _nom; // Nom de la parcelle
    int milieuhydro; // Milieu hydrolique
    int poste_de_commande; // Poste de commande
    int _indexdebut; // index de début de la parcelle
    int _indexfin;// index de fin de la parcelle
    float _longueur; // longueur de la parcelle
    std::string _matiere; // Matiere des tuyaux la parcelle
    bool amont; // Si la parcelle est en amont (true) ou en aval (false)
    float _debit; // Débit total de la parcelle
    bool _calcul; // Si les calculs sont fait
    int _decalage; // Si le poste de commande est sur le cote gauche ou droit
    float aspdebit; // Débit par asperseurs
    float aspinterdebut; // Distance entre asperseurs


};

#endif // TOUT_PARCELLE_H
