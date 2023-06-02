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

    parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db, QString nom, bool amont2 = true);

    ///
    /// @brief Récupère le milieu hydrologique.
    ///
    int getMilieuhydro() const;

    ///
    /// @brief Récupère les données associées.
    ///
    std::vector<std::vector<float>> &getDonnees() ;

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
    bool amont;
    float _debit;
    bool _calcul;

  public:
    bool isCalcul() const;
};

#endif // TOUT_PARCELLE_H
