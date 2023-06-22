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
#include "tableau.h"

class parcelle {

public:

  parcelle()=default;

  parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db, QString nom, bool amont2 = true, std::string mat="PEHD");

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

  void herse(int ligne);
  void fromString(const std::string &s, std::shared_ptr<bdd> db);
  std::string toString() const;

public:
    int getDecalage() const;

    void setDecalage(int decalage);

    float getAspdebit() const;

    void setAspdebit(float aspdebit);

    float getAspinter() const;

    void setAspinter(float aspinter);

    float getAspinterdebut() const;

    void setAspinterdebut(float aspinterdebut);

private:
    std::shared_ptr<bdd> database;
  std::unique_ptr<pertechargeherse> hersealim;
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
  int _decalage;

  float aspdebit;
  float aspinter;
  float aspinterdebut;

  /// @brief permet de recalculer certaines colonnes si on change le débit d'une ligne
  void recalcul();

  /// @brief Permet de changer le diametre d'un asperseurs
  /// @param ligne index de la ligne
  /// @param colonne index de la colonne
  /// @param val valeur
  void updatediaasp(int ligne, int colonne, float val);

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

  /// @brief calcul la perte du peigne en mode aspersions
  /// @param indices ligne contenant des aspersseurs
  /// @param debit debit pour chaque aspersseurs
  /// @param distasp distance entre chaque aspersseurs
  /// @param a coefficient A de la matiere
  /// @param b coefficient B de la matiere
  /// @param k coefficient K de la matiere
  void calculperteherseasp(std::vector<int> &indice, float debit,
                           float distasp, float a, float b, double k);
  int trouvemilieuhydroasp();
};

#endif // TOUT_PARCELLE_H
