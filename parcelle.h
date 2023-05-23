//
// Created by Quentin Raimbault on 22/05/2023.
//

#ifndef TOUT_PARCELLE_H
#define TOUT_PARCELLE_H
#include <vector>
#include <iostream>
#include <QSpinBox>
#include <QSpinBox>
#include <QDialog>

#include "bdd.h"
class parcelle {

public:

  parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db);
  int getMilieuhydro() const;
  const std::vector<std::vector<float>> &getDonnees() const;
  void setPosteDeCommande(int posteDeCommande);
  int getPosteDeCommande() const;
  void modifiedia(int index, float diameters);
  void setDiametreDialog();

private:
  std::shared_ptr<bdd> database;

  int milieuhydro;
  int poste_de_commande;
  std::vector<std::vector<float>> _Donnees;
  std::vector<float> _PerteCharge; // Nouveau champ pour stocker les pertes de charge
  std::vector<float> _diameters;

  void calcul();
  int trouvemilieuhydro();
};

#endif // TOUT_PARCELLE_H
