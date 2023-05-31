//
// Created by Quentin Raimbault on 22/05/2023.
//

#include "parcelle.h"


parcelle::parcelle(std::vector<std::vector<float>> &data,int indexdebut, int indexfin,   std::shared_ptr<bdd> db, QString nom): database(db), _nom(nom) {
  _longueur=0;
  amont = true;
  _indexdebut = indexdebut;
  _indexfin = indexfin;

  // Redimensionner _Donnees pour qu'il ait la même taille que la plage de data que nous copions
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

  for(auto it : _Donnees) {
    _longueur+= it[1];
  }

}

std::vector<std::vector<float>> &parcelle::getDonnees() {
  return _Donnees;
}

int parcelle::trouvemilieuhydro() {
  // Vérifiez que _Donnees n'est pas vide
  if (_Donnees.empty()) {
    return 0;
  }

  // Calculer le débit cumulé total
  float debitTotal = 0;
  for (const auto& donnee : _Donnees) {
    if (donnee.size() < 11) {
      continue;
    }
    debitTotal += donnee[9];
  }
  _debit = debitTotal;

  // Chercher l'indice où le débit cumulé dépasse la moitié du débit cumulé total
  float cumulatedDebit = 0;
  for(int i=0; i<_Donnees.size(); i++){
    if (_Donnees[i].size() < 11) {
      continue;
    }
    cumulatedDebit += _Donnees[i][10];
    if(cumulatedDebit >= debitTotal / 2){
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
  if(poste_de_commande==0)
    return;

  double a,b,k;

  a=1.75;
  b=-4.85;
  k=831743.11;

  float denivele_gauche = 0;
  float denivele_droit = 0;
  float perted = 0;
  float perteg = 0;
  float sigmadebit = 0;

  float vitesse_gauche = 0; // Nouvelle variable pour stocker la vitesse à gauche
  float vitesse_droite = 0; // Nouvelle variable pour stocker la vitesse à droite

  const float pi = 3.14159265358979323846;


  // Calcul de la perte de charge et du dénivelé du côté gauche du poste de commande
  for (size_t i = 0; i < poste_de_commande-_indexdebut; ++i) {
    float Dia = _diameters[i];
    float debitlh = _Donnees[i][9]; // Convertir le débit en l/s
    sigmadebit+=debitlh;
    float L = _Donnees[i][8];
    float perte = k * std::pow(sigmadebit/3600, a) * std::pow(Dia, b) * L;
    perteg += perte;

    // Calcul de la vitesse
    float A = pi * std::pow(Dia/2, 2); // Calcul de l'aire en m²
    vitesse_gauche = ((sigmadebit * 1000) / 3600 ) / A; // Calcul de la vitesse en m/s

    // Calcul du dénivelé
    if(i == poste_de_commande-_indexdebut-1 && amont)
      denivele_gauche = _Donnees[0][3] - _Donnees[i][3];
    else
      denivele_gauche = _Donnees[0][4] - _Donnees[i][4];

  }

  sigmadebit = 0;

  // Calcul de la perte de charge et du dénivelé du côté droit du poste de commande
  for (size_t i = poste_de_commande-_indexdebut; i < _Donnees.size(); ++i) {
    float Dia = _diameters[i]; // mm
    float debitlh = _Donnees[i][9]; // l/h
    sigmadebit += debitlh;
    float L = _Donnees[i][8]; // m
    float perte = k * std::pow(sigmadebit / 3600, a) * std::pow(Dia, b) * L;
    perted += perte;

    // Calcul de la vitesse
    float A = pi * std::pow(Dia/2, 2); // Calcul de l'aire en m²
    vitesse_droite = ((sigmadebit * 1000) / 3600) / A; // Calcul de la vitesse en m/s

    // Calcul du dénivelé
    if(i == _Donnees.size()-1 && amont)
      denivele_droit = _Donnees[i][3] - _Donnees[poste_de_commande-_indexdebut-1][3];
    else
      denivele_droit = _Donnees[i][4] - _Donnees[poste_de_commande-_indexdebut-1][4];

  }

  std::cout<<"Gauche : " << vitesse_gauche<<std::endl;
  std::cout<<"Droite : " << vitesse_droite<<std::endl;

  _Donnees[poste_de_commande-_indexdebut - 2][17] = perteg; // Stocke perteg à la ligne au-dessus du poste de commande
  _Donnees[poste_de_commande-_indexdebut - 3][17] = vitesse_gauche; // Stocke perteg à la ligne au-dessus du poste de commande
  _Donnees[poste_de_commande-_indexdebut - 2][18] = denivele_gauche; // Stocke denivele_gauche à la ligne au-dessus du poste de commande
  _Donnees[poste_de_commande-_indexdebut - 2][19] = denivele_gauche + perteg; // Stocke piezo à la ligne au-dessus du poste de commande

  _Donnees[poste_de_commande-_indexdebut][17] = perted; // Stocke perted à la ligne en dessous du poste de commande
  _Donnees[poste_de_commande-_indexdebut+1][17] = vitesse_droite; // Stocke perted à la ligne en dessous du poste de commande
  _Donnees[poste_de_commande-_indexdebut][18] = denivele_droit; // Stocke denivele_droit à la ligne en dessous du poste de commande
  _Donnees[poste_de_commande-_indexdebut][19] = denivele_droit + perted; // Stocke denivele_droit à la ligne en dessous du poste de commande

}

void parcelle::setDiametreDialog() {

  if(poste_de_commande==0){
    setPosteDeCommande(trouvemilieuhydro());
  }

  bool toutlestuyaux = true;

  for(int i=0; i<_diameters.size(); i++) {
    if(_diameters[i] == 0) {
      toutlestuyaux = false;
      break;
    }
  }

  if (toutlestuyaux) {
    calcul();
    return;
  }

  QDialog diameterDialog;
  diameterDialog.setWindowTitle("Choix Diametre");

  QVBoxLayout dialogLayout;

  // Amont/Aval Radio Buttons
  QRadioButton *amontButton = new QRadioButton("Amont", &diameterDialog);
  QRadioButton *avalButton = new QRadioButton("Aval", &diameterDialog);
  amontButton->setChecked(true);  // Amont is default

  // Add these to the layout
  dialogLayout.addWidget(amontButton);
  dialogLayout.addWidget(avalButton);


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
  QLabel label("");
  dialogLayout.addWidget(&label);

  int nonZeroCount = std::count_if(_diameters.begin(), _diameters.end(), [](float val) { return val != 0.0f; });

  QString text = QString::number(nonZeroCount) + "/" + QString::number(_diameters.size());

  label.setText(text);

  // Set button
  QPushButton setButton("Choix Intervale et Diametre");
  dialogLayout.addWidget(&setButton);



  diameterDialog.setLayout(&dialogLayout);

  // Connect radio button signals to update the 'amont' variable
  QObject::connect(amontButton, &QRadioButton::toggled, [&](bool checked) {
    if (checked) {
      amont = true;
    }
  });

  QObject::connect(avalButton, &QRadioButton::toggled, [&](bool checked) {
    if (checked) {
      amont = false;
    }
  });



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

    endSpinBox.setValue(endSpinBox.value() + 1);
    startSpinBox.setValue(endSpinBox.value());

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

  if(materials.size() > 0) {
    materialComboBox.setCurrentIndex(3);
  }

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

float parcelle::getLongueur() const {
  return _longueur;
}

float parcelle::getDebit() const {
  return _debit;
}


