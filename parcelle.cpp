//
// Created by Quentin Raimbault on 22/05/2023.
//

#include "parcelle.h"


parcelle::parcelle(std::vector<std::vector<float>> &data,int indexdebut, int indexfin,   std::shared_ptr<bdd> db, QString nom,bool amont2): database(db), _nom(nom) {
  _longueur=0;
  amont = amont2;
  _calcul = false;
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

void parcelle::calcul() {
  // Si poste_de_commande est à zéro, on sort de la fonction
  if(poste_de_commande==0)
    return;

  // Coefficients pour le calcul de la perte de charge
  double a=1.75, b=-4.85, k=831743.11;

  // Initialisation des variables nécessaires pour les calculs
  float denivele_gauche = 0, denivele_droit = 0;
  float perted = 0, perteg = 0, sigmadebit = 0;
  float vitesse_gauche = 0, vitesse_droite = 0;

  // Constante pour le calcul de l'aire de la conduite
  const float pi = 3.14159265358979323846;

  // Indices pour la première et la dernière ligne du côté gauche et du côté droit du poste de commande
  size_t debut = poste_de_commande-_indexdebut;
  size_t fin_gauche = debut - 1;
  size_t fin_droit = _Donnees.size() - 1;

  // Fonction lambda pour effectuer des calculs communs
  auto calculCommun = [&](size_t i) {
    float Dia = _diameters[i]; // Diamètre
    float debitlh = _Donnees[i][10]; // Débit
    sigmadebit+=debitlh; // Débit cumulé
    float L = _Donnees[i][1]; // Longueur de la conduite

    // Calcul de la perte de charge
    float perte = k * std::pow(sigmadebit/3600, a) * std::pow(Dia, b) * L;
    return perte;
  };

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
  for (size_t i = 0; i < debut+1; ++i) {
    auto perte = calculCommun(i);
    perteg += perte; // Cumul de la perte de charge
    // Calcul du dénivelé
    denivele_gauche = i == fin_gauche && amont ? _Donnees[0][3] - _Donnees[i][3] : _Donnees[0][4] - _Donnees[i][4];
  }

  // Calcule l'aire du tuyau.
  float aireTuyau = (pi * pow((_diameters[debut]/1000) / 2, 2));

  // Calcule la vitesse.
  vitesse_gauche = (_Donnees[debut][8]/(1000*3600)) / aireTuyau;

  for(auto i : _Donnees[debut]){
    std::cout<<i<<" ";
  }


  sigmadebit = 0; // Réinitialisation de la variable sigmadebit pour le côté droit

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté droit du poste de commande
  for (size_t i = debut+1; i < _Donnees.size(); ++i) {
    auto perte = calculCommun(i);
    perted += perte; // Cumul de la perte de charge
    // Calcul du dénivelé
    denivele_droit = i == fin_droit && amont ? _Donnees[i][3] - _Donnees[fin_gauche][3] : _Donnees[i][4] - _Donnees[fin_gauche][4];
  }

  aireTuyau = (pi * pow((_diameters[debut+1]/1000) / 2, 2));

  // Calcule la vitesse.
  vitesse_droite = (_Donnees[debut+1][8]/(1000*3600)) / aireTuyau;

  // Stockage des valeurs calculées dans le tableau _Donnees pour le côté gauche
  _Donnees[fin_gauche - 1][17] = perteg;
  _Donnees[fin_gauche - 1][18] = denivele_gauche;
  _Donnees[fin_gauche - 1][19] = denivele_gauche + perteg;
  _Donnees[debut - 3][17] = vitesse_gauche;

  // Stockage des valeurs calculées dans le tableau _Donnees pour le côté droit
  _Donnees[debut][17] = perted;
  _Donnees[debut][18] = denivele_droit;
  _Donnees[debut][19] = denivele_droit + perted;
  _Donnees[debut + 1][17] = vitesse_droite;

  _calcul = true;
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
  diameterDialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

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

const QString &parcelle::getNom() const {
  return _nom;
}

void parcelle::setNom(const QString &nom) {
  _nom = nom;
}

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

bool parcelle::isAmont() const {
  return amont;
}

int parcelle::getMilieuhydro() const {
  return milieuhydro;
}

void parcelle::setPosteDeCommande(int posteDeCommande) {
  poste_de_commande = posteDeCommande;
}

int parcelle::getPosteDeCommande() const {
  return poste_de_commande;
}
bool parcelle::isCalcul() const {
  return _calcul;
}




