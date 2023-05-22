//
// Created by Quentin Raimbault on 22/05/2023.
//

#ifndef TOUT_PARCELLE_H
#define TOUT_PARCELLE_H
#include <vector>
#include <iostream>

class parcelle {
public:
  parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin);
  int getMilieuhydro() const;
  const std::vector<std::vector<float>> &getDonnees() const;
  void setPosteDeCommande(int posteDeCommande);
  int getPosteDeCommande() const;

private:
  int milieuhydro;
  int poste_de_commande;
  std::vector<std::vector<float>> _Donnees;
  void calcul();
  int trouvemilieuhydro();

public:
};

#endif // TOUT_PARCELLE_H
