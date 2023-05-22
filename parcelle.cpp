//
// Created by Quentin Raimbault on 22/05/2023.
//

#include "parcelle.h"


parcelle::parcelle(std::vector<std::vector<float>> &data,int indexdebut, int indexfin) {
  // Redimensionner _Donnees pour qu'il ait la même taille que la plage de data que nous copions
  _Donnees.resize(indexfin - indexdebut);
  int cpt = 0;

  for(int i = indexdebut; i < indexfin;i++) {
    // Redimensionner chaque sous-vecteur pour qu'il ait la même taille que le sous-vecteur correspondant dans data
    _Donnees[cpt].resize(data[i].size());

    for(int j = 0; j < data[i].size();j++) {
      _Donnees[cpt][j] = data[i][j];
    }
    cpt++;
  }
  milieuhydro = trouvemilieuhydro();
}



const std::vector<std::vector<float>> &parcelle::getDonnees() const {
  return _Donnees;
}

int parcelle::trouvemilieuhydro() {
  // Vérifiez que _Donnees n'est pas vide
  if (_Donnees.empty()) {
    return 0;
  }

  // Calculer le débit cumulé total
  float totalDebit = 0;
  for (const auto& donnee : _Donnees) {
    if (donnee.size() < 11) {
      continue;
    }
    totalDebit += donnee[9];
  }
  std::cout << totalDebit << std::endl;

  // Chercher l'indice où le débit cumulé dépasse la moitié du débit cumulé total
  float cumulatedDebit = 0;
  for(int i=0; i<_Donnees.size(); i++){
    if (_Donnees[i].size() < 11) {
      continue;
    }
    cumulatedDebit += _Donnees[i][10];
    if(cumulatedDebit >= totalDebit / 2){
      std::cout << i << std::endl;

      return i;

    }
  }

  return 0;
}


int parcelle::getMilieuhydro() const { return milieuhydro; }
void parcelle::setPosteDeCommande(int posteDeCommande) {
  poste_de_commande = posteDeCommande;
}
int parcelle::getPosteDeCommande() const { return poste_de_commande; }
