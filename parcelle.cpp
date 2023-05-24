//
// Created by Quentin Raimbault on 22/05/2023.
//

#include "parcelle.h"


parcelle::parcelle(std::vector<std::vector<float>> &data,int indexdebut, int indexfin,   std::shared_ptr<bdd> db, QString nom): database(db), _nom(nom) {
  // Redimensionner _Donnees pour qu'il ait la même taille que la plage de data que nous copions
  _indexdebut = indexdebut;
  _indexfin = indexfin;
  _Donnees.resize(indexfin - indexdebut);
  _diameters.resize(_Donnees.size());
  int cpt = 0;
  poste_de_commande=0;
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
  // Chercher l'indice où le débit cumulé dépasse la moitié du débit cumulé total
  float cumulatedDebit = 0;
  for(int i=0; i<_Donnees.size(); i++){
    if (_Donnees[i].size() < 11) {
      continue;
    }
    cumulatedDebit += _Donnees[i][10];
    if(cumulatedDebit >= totalDebit / 2){
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


void parcelle::calcul() {
  double a,b,k;

  a=1.75;
  b=-4.85;
  k=831743.11;

  _PerteCharge.resize(_Donnees.size()); // Assurez-vous que _PerteCharge a la même taille que _Donnees

  float denivele_gauche = 0;
  float denivele_droit = 0;
  float perted = 0;
  float perteg = 0;

  // Calcul de la perte de charge et du dénivelé du côté gauche du poste de commande
  for (size_t i = 0; i < poste_de_commande-_indexdebut; ++i) {
    float Dia = _diameters[i];
    float dLS = _Donnees[i][9] / 1000; // Convertir le débit en l/s
    float L = _Donnees[i][1];
    float perte = k * std::pow(dLS, a) * std::pow(Dia, b) * L;
    _PerteCharge[i] = perte;
    perteg += perte;

    // Calcul du dénivelé
    if(i == poste_de_commande-_indexdebut-1)
      denivele_gauche = _Donnees[0][3] - _Donnees[i][3];
  }

  // Calcul de la perte de charge et du dénivelé du côté droit du poste de commande
  for (size_t i = poste_de_commande-_indexdebut; i < _Donnees.size(); ++i) {
    float Dia = _diameters[i];
    float dLS = _Donnees[i][9] / 1000; // Convertir le débit en l/s
    float L = _Donnees[i][1];
    float perte = k * std::pow(dLS, a) * std::pow(Dia, b) * L;
    _PerteCharge[i] = perte;
    perted += perte;

    // Calcul du dénivelé
    if(i == _Donnees.size()-1)
        denivele_droit = _Donnees[i][3] - _Donnees[poste_de_commande-_indexdebut-1][3];
  }
  _Donnees[poste_de_commande-_indexdebut - 2][17] = perteg; // Stocke perteg à la ligne au-dessus du poste de commande
  _Donnees[poste_de_commande-_indexdebut - 2][18] = denivele_gauche; // Stocke denivele_gauche à la ligne au-dessus du poste de commande
  _Donnees[poste_de_commande-_indexdebut - 2][19] = denivele_gauche + perteg; // Stocke denivele_gauche à la ligne au-dessus du poste de commande

  _Donnees[poste_de_commande-_indexdebut][17] = perted; // Stocke perted à la ligne en dessous du poste de commande
  _Donnees[poste_de_commande-_indexdebut][18] = denivele_droit; // Stocke denivele_droit à la ligne en dessous du poste de commande
  _Donnees[poste_de_commande-_indexdebut][19] = denivele_droit + perted; // Stocke denivele_droit à la ligne en dessous du poste de commande

 }

void parcelle::setDiametreDialog() {
  QDialog diameterDialog;
  diameterDialog.setWindowTitle("Choix Diametre");

  QVBoxLayout dialogLayout;

  // Material Label and ComboBox
  QLabel materialLabel("Matiere");
  dialogLayout.addWidget(&materialLabel);

  QComboBox materialComboBox;
  std::vector<std::string> materials = database->getAllMatiereNames();
  for (const auto &material : materials) {
    materialComboBox.addItem(QString::fromStdString(material));
  }
  dialogLayout.addWidget(&materialComboBox);

  // Pressure Label and ComboBox
  QLabel pressureLabel("Pression");
  dialogLayout.addWidget(&pressureLabel);

  QComboBox pressureComboBox;
  dialogLayout.addWidget(&pressureComboBox);

  // Inner Diameter Label and ComboBox
  QLabel innerDiameterLabel("Diametre");
  dialogLayout.addWidget(&innerDiameterLabel);

  QComboBox innerDiameterComboBox;
  dialogLayout.addWidget(&innerDiameterComboBox);

  // Interval SpinBoxes with Labels
  QLabel startLabel("Départ");
  dialogLayout.addWidget(&startLabel);

  QSpinBox startSpinBox;
  startSpinBox.setRange(_indexdebut+1, _indexfin-1);
  dialogLayout.addWidget(&startSpinBox);

  QLabel endLabel("Arrive");
  dialogLayout.addWidget(&endLabel);

  QSpinBox endSpinBox;
  endSpinBox.setRange(_indexdebut+2, _indexfin);
  dialogLayout.addWidget(&endSpinBox);

  // Qlabel
  QLabel label("oui");
  dialogLayout.addWidget(&label);

  // Set button
  QPushButton setButton("Choix Intervale et Diametre");
  dialogLayout.addWidget(&setButton);



  diameterDialog.setLayout(&dialogLayout);

  // Connections
  QObject::connect(&materialComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   [this, &pressureComboBox, &materialComboBox](int index) {
                     QString selectedMaterial = materialComboBox.itemText(index);
                     std::vector<int> pressures = database->getAllPressuresForMatiere(selectedMaterial.toStdString());
                     pressureComboBox.clear();
                     for (int pressure : pressures) {
                       pressureComboBox.addItem(QString::number(pressure));
                     }
                   });

  QObject::connect(&pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                   [this, &materialComboBox, &innerDiameterComboBox, &pressureComboBox](int index) {
                     QString selectedMaterial = materialComboBox.currentText();
                     int selectedPressure = pressureComboBox.itemText(index).toInt();
                     std::vector<float> innerDiameters = database->getInnerDiametersForMatiereAndPressure(selectedMaterial.toStdString(), selectedPressure);
                     innerDiameterComboBox.clear();
                     for (float innerDiameter : innerDiameters) {
                       innerDiameterComboBox.addItem(QString::number(innerDiameter));
                     }
                   });

  QObject::connect(&setButton, &QPushButton::clicked, [this, &diameterDialog, &innerDiameterComboBox, &startSpinBox, &endSpinBox, &label]() {
    float diameter = innerDiameterComboBox.currentText().toFloat();

      int startIndex = startSpinBox.value() - _indexdebut - 1 ;
      int endIndex = endSpinBox.value() - _indexdebut;

      for(int i = startIndex; i < endIndex; i++) {
          _diameters[i] = diameter;
          _Donnees[i][15] = diameter;
      }

      int nonZeroCount = std::count_if(_diameters.begin(), _diameters.end(), [](float val) { return val != 0.0f; });

    QString text = QString::number(nonZeroCount) + "/" + QString::number(_diameters.size());

    label.setText(text);

    bool toutlestuyaux = true;

    for(int i=0; i<_diameters.size(); i++) {
      if(_diameters[i] == 0) {
        toutlestuyaux = false;
        break;
      }
    }

    if (toutlestuyaux) {
      calcul();
      diameterDialog.accept();
    }
  });


  QObject::connect(&startSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                   [&endSpinBox](int startValue) {
                     endSpinBox.setMinimum(startValue + 1);
                   });

  QObject::connect(&endSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                   [&startSpinBox](int endValue) {
                     startSpinBox.setMaximum(endValue - 1);
                   });
  diameterDialog.exec();
}

void parcelle::modifiedia(int index, float diameters){
  _diameters[index] = diameters;
  _Donnees[index][15] = diameters;
}
const QString &parcelle::getNom() const { return _nom; }
void parcelle::setNom(const QString &nom) { _nom = nom; }

int parcelle::getIndexdebut() const {
    return _indexdebut;
}

int parcelle::getIndexfin() const {
    return _indexfin;
}
