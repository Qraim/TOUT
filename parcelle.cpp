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

void parcelle::setDonnees(const std::vector<std::vector<float>> &donnees) {
  _Donnees = donnees;
}

void parcelle::setMilieuhydro(int milieuhydro) {
  parcelle::milieuhydro = milieuhydro;
}
void parcelle::setIndexdebut(int indexdebut) { _indexdebut = indexdebut; }
void parcelle::setIndexfin(int indexfin) { _indexfin = indexfin; }
void parcelle::setLongueur(float longueur) { _longueur = longueur; }
void parcelle::setMatiere(const std::string &matiere) { _matiere = matiere; }
void parcelle::setAmont(bool amont) { parcelle::amont = amont; }
void parcelle::setDebit(float debit) { _debit = debit; }
void parcelle::setCalcul(bool calcul) { _calcul = calcul; }

std::vector<std::vector<float>> &parcelle::getDonnees(){
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
    cumulatedDebit += _Donnees[i][9];
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

  // Coefficients pour le calcul de la perte de charg
  std::tuple<float, float, double> coefficients = database->get_material_coefficients(_matiere);
  float a = std::get<0>(coefficients);
  float b = std::get<1>(coefficients);
  double k = std::get<2>(coefficients);


  auto aspesseurs = trouveaspersseurs();

  for(int i=0; i<_Donnees.size(); i++){
    std::cout << _Donnees[i][2] <<" "<< std::endl;
  }
  std::cout<<"-----------------------------CALCUL---------------------------------"<<std::endl;

  if(!aspesseurs.empty()){
    calculaspersseurs(aspesseurs,a,b,k);
    return;
  }


  if (a!= 0 && b!= 0 && k!=0){
    calcul_gauche(a,b,k);
    calcul_droit(a,b,k);
    calculdiametre(a,b,k);
  }

  _calcul = true;
}

void parcelle::calculdiametre(float a, float b, double k){
  for(int i=0;i<_Donnees.size();++i){
    //calcul du goutte à goutte
    float arosseurs = 0;


    if(_Donnees[i][8]!=0)
      arosseurs = _Donnees[i][1]/_Donnees[i][8];

    float debitLS = _Donnees[i][7]/3600;

    float dia16 = 14;
    float dia20 = 17.6;

    float espacement = _Donnees[i][8];
    float perte16 = 0;
    float perte20 = 0;

    for(int j=1;j<=arosseurs; ++j){
      perte16 += k * std::pow(debitLS*j, a) * std::pow(dia16, b) * espacement;
      perte20 += k * std::pow(debitLS*j, a) * std::pow(dia20, b) * espacement;
    }
    _Donnees[i][23] = perte16;
    _Donnees[i][25] = perte20;

    if(amont){
      _Donnees[i][24] = perte16 + _Donnees[i][14];
      _Donnees[i][26] = perte20 + _Donnees[i][14];

    } else {
      _Donnees[i][24] = perte16 + _Donnees[i][13];
      _Donnees[i][26] = perte20 + _Donnees[i][13];
    }


  }

}

void parcelle::setDiametreDialog(std::string matiere) {

  _matiere=matiere;

  if(poste_de_commande==0){
    setPosteDeCommande(milieuhydro);
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

void parcelle::modifiedebit(int index, float diameters){
  std::cout<<"on a modifié "<<std::endl;

  _Donnees[index][9] = diameters;
  recalcul();
}

void parcelle::modifieinter(int index,int colonne, float diameters){
  _Donnees[index][colonne] = diameters;
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
  for (auto& row : _Donnees) {
    if (row.size() > 19) {
      row[17] = 0.0;
      row[18] = 0.0;
      row[19] = 0.0; // Effacement des résultats de calculs
      row[20] = 0.0;
      row[21] = 0.0;
      row[22] = 0.0;
      row[23] = 0.0;
      row[24] = 0.0;
      row[25] = 0.0;
      row[26] = 0.0;
    }
  }

  if(posteDeCommande==_indexdebut)
    poste_de_commande=milieuhydro+_indexdebut+1;
  else
    poste_de_commande=posteDeCommande-_indexdebut;

  int debut = poste_de_commande;
  int fin_droit = _Donnees.size() - 1;

  for(int i = fin_droit; i >= debut; i--) {
    if(i == debut)
      _Donnees[i][amont ? 11 : 12] =   _Donnees[i-1][amont ? 3 : 4] - _Donnees[i][amont ? 3 : 4];
    else if(i - 1 >= debut)
      _Donnees[i][amont ? 11 : 12] =   _Donnees[i-1][amont ? 3 : 4] - _Donnees[i][amont ? 3 : 4];
  }

  for(int i = 0; i < poste_de_commande; ++i){
    if(i + 1 < poste_de_commande)
      _Donnees[i][amont ? 11 : 12] = _Donnees[i][amont ? 3 : 4] - _Donnees[i+1][amont ? 3 : 4];
    else
      _Donnees[i][amont ? 11 : 12] = 0;
  }

  /*for(auto &it : _Donnees[poste_de_commande]){
    std::cout<<it<<" ";
  }
  std::cout<<std::endl;*/
}

int parcelle::getPosteDeCommande() const {
  return poste_de_commande;
}
bool parcelle::isCalcul() const {
  return _calcul;
}

const float pi = 3.14159265358979323846;

void parcelle::calcul_droit(float a, float b, double k) {

  int debut = poste_de_commande - _decalage;
  int fin_gauche = debut - 1;
  int fin_droit = _Donnees.size() - 1;

  // Initialisation des variables nécessaires pour les calculs
  float denivele_droit = 0;
  float perted = 0, sigmadebit = 0;
  float vitesse_droite = 0;
  float debit_droit = 0;

  float piezo = 0;

  float cumulperte = 0;
  float cumulpiezo = 0;


  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté droit du poste de commande
  for (int i = fin_droit; i >=debut; --i) {

    float Dia = _diameters[i]; // Diamètre en mm
    sigmadebit += _Donnees[i][9]; // Débit en l/h
    float L = _Donnees[i][5]; // Longueur de la conduite en mètre

    // Calcul de la perte de charge
    float perte = k * std::pow(sigmadebit/3600, a) * std::pow(Dia, b) * L;

    float denivele = 0;

    if(fin_gauche<0)
      denivele = amont ?    _Donnees[i][3] - _Donnees[0][3]  :  _Donnees[i][4] - _Donnees[0][4] ;
    else
      denivele = amont ?    _Donnees[i][3] - _Donnees[fin_gauche][3]  :  _Donnees[i][4] - _Donnees[fin_gauche][4] ;


    piezo = perte + denivele ;
    cumulperte += perte; // Cumul de la perte de charge
    cumulpiezo += piezo;

    float debitM3S = sigmadebit /3600 / 1000;

    float diametreM = Dia/1000;

    float aire = pi * pow((diametreM/2), 2);

    float vitesse = debitM3S / aire;


    _Donnees[i][17] = denivele;
    _Donnees[i][18] = vitesse;
    _Donnees[i][19] = perte;
    _Donnees[i][20] = piezo;
    _Donnees[i][21] = cumulperte;
    _Donnees[i][22] = cumulpiezo;

    /*
        // Imprime les informations de débogage de manière structurée
        std::cout << "----- Information de la conduite (droite) -----" << std::endl;
        std::cout << "Tuyau: " << _Donnees[i][0] << std::endl;
        std::cout << "Diamètre (D): " << Dia << " mm" << std::endl;
        std::cout << "Débit cumulatif (sigmadebit): " << sigmadebit << " l/h" << std::endl;
        std::cout << "Intervale (L): " << L << " m" << std::endl;
        std::cout << "Coefficients: " << "a=" << a << ", b=" << b << ", k=" << k << std::endl;
        std::cout << "Perte de charge calculée : " << perte << std::endl;
        std::cout << "Perte de charge cumulée (perted): " << perted << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
    */
  }

  /* std::cout << "--------------------------------"<<std::endl;
   std::cout << "Débit cumulé droite : "<<debit_droit<<std::endl;
   std::cout << "--------------------------------"<<std::endl;*/


/*    // Calcul de la vitesse seulement pour le rang juste au-dessus du poste de commande
    float diameterMeters = _diameters[debut] / 1000; // Diametre en metres
    float flowRateLitersPerHour = debit_droit; // Debit en litres par heure
    float flowRateLitersPerSecond = flowRateLitersPerHour / 3600.0; // Convert l/h en l/s
    float flowRateCubicMetersPerSecond = flowRateLitersPerSecond / 1000; // Convert l/s en m³/s
    float pipeArea = pi * pow(diameterMeters / 2, 2);
    vitesse_droite = flowRateCubicMetersPerSecond / pipeArea;

    // Calcul du dénivelé
    denivele_droit =  amont ? _Donnees[fin_droit][3] - _Donnees[fin_gauche+1][3] : _Donnees[fin_droit][4] - _Donnees[fin_gauche+1][4];

    if(debut!=_Donnees.size()-1) {
        // Stockage des valeurs calculées dans le tableau _Donnees pour le côté droit
        _Donnees[debut][17] = perted;
        _Donnees[debut][18] = denivele_droit;
        _Donnees[debut][19] = denivele_droit + perted;
        _Donnees[debut+1][17] = vitesse_droite;
    }*/
}

void parcelle::calcul_gauche(float a, float b, double k) {
  int debut = poste_de_commande;
  int fin_gauche = debut - 1;


  float sigmadebit = 0;
  float debit_gauche = 0;
  float piezo = 0;

  float cumulperte = 0;
  float cumulpiezo = 0;

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
  for (int i = 0; i < poste_de_commande - _decalage; ++i) {

    float Dia = _diameters[i]; // Diamètre en mm
    sigmadebit += _Donnees[i][9]; // Débit en l/h
    float L = _Donnees[i][5]; // Longueur de la conduite en mètre

    // Calcul de la perte de charge
    float perte = k * std::pow(sigmadebit/3600, a) * std::pow(Dia, b) * L;

    float denivele = amont ?    _Donnees[i][3] - _Donnees[fin_gauche][3]   :    _Donnees[i][4] - _Donnees[fin_gauche][4] ;

    piezo = perte + denivele ;
    cumulperte += perte; // Cumul de la perte de charge
    cumulpiezo += piezo;

    float diametre = Dia/1000;

    float debitM3S = sigmadebit /3600 / 1000;

    float diametreM = Dia/1000;

    float aire = pi * pow((diametreM/2), 2);

    float vitesse = debitM3S / aire;

    _Donnees[i][17] = denivele;
    _Donnees[i][18] = vitesse;
    _Donnees[i][19] = perte;
    _Donnees[i][20] = piezo;
    _Donnees[i][21] = cumulperte;
    _Donnees[i][22] = cumulpiezo;

    // Imprime les informations de débogage de manière structurée
    /*std::cout << "----- Information de la conduite (gauche) -----" << std::endl;
    std::cout << "Tuyau: " << _Donnees[i][0] << std::endl;
    std::cout << "Diamètre (D): " << Dia << " mm" << std::endl;
    std::cout << "Débit cumulatif (sigmadebit): " << sigmadebit << " l/h" << std::endl;
    std::cout << "Intervale (L): " << L << " m" << std::endl;
    std::cout << "Coefficients: " << "a=" << a << ", b=" << b << ", k=" << k << std::endl;
    std::cout << "Perte de charge calculée : " << perte << std::endl;
    std::cout << "Perte de charge cumulée (perteg): " << perteg << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;*/
  }

/*  std::cout << "--------------------------------"<<std::endl;
  std::cout << "Débit cumulé gauche : "<<debit_gauche<<std::endl;
  std::cout << "--------------------------------"<<std::endl;*/
/*    // Calcul de la vitesse seulement pour le rang juste en dessous du poste de commande
    float diameterMeters = _diameters[poste_de_commande-1] / 1000; // Diametre en metres
    float flowRateLitersPerHour = debit_gauche; // Debit en litres par heure
    float flowRateLitersPerSecond = flowRateLitersPerHour / 3600.0; // Convert l/h en l/s
    float flowRateCubicMetersPerSecond = flowRateLitersPerSecond / 1000; // Convert l/s en m³/s
    float pipeArea = pi * pow(diameterMeters / 2, 2);
    vitesse_gauche = flowRateCubicMetersPerSecond / pipeArea;

    // Calcul du dénivelé
    denivele_gauche = amont ? _Donnees[0][3] - _Donnees[fin_gauche][3] : _Donnees[0][4] - _Donnees[fin_gauche][4];

    // Stockage des valeurs calculées dans le tableau _Donnees pour le côté gauche

    if(fin_gauche!=0 ){
        _Donnees[fin_gauche-1][17] = perteg ;
        _Donnees[fin_gauche-1][18] = denivele_gauche;
        _Donnees[fin_gauche-1][19] = denivele_gauche + perteg;
        if(debut-3>0){
            _Donnees[debut-3][17] = vitesse_gauche;
        }
    }*/
}

const std::vector<float> &parcelle::getDiameters() const { return _diameters; }
const std::string &parcelle::getMatiere() const { return _matiere; }

void parcelle::addDiameter(float a){
  if(a> 0 ){
    _diameters.push_back(a);
  }
}

std::vector<int> parcelle::trouveaspersseurs() {
  std::vector<int> result;
  for (int i = 0; i < _Donnees.size(); i++) {
    if (_Donnees[i][2] != 0) {
      result.push_back(i);
    }
  }

  // Trier le vecteur
  std::sort(result.begin(), result.end());

  return result;
}


#include <QInputDialog>

// Créez une fonction pour calculer la perte
float calculPerte(float debit, float Dia, float intervale, float a, float b, float k) {
  float perte = k * std::pow((debit), a) * std::pow(Dia, b) * intervale;
  std::cout<<perte<<std::endl;
  return perte;
}

void parcelle::calculpeigne(float a, float b, double k, float debitasp){
  int debut = poste_de_commande - _decalage;
  int fin_gauche = debut - 1;
  int fin_droit = _Donnees.size() - 1;

  float denivele_gauche = 0;
  float perteg = 0;
  float sigmadebit = 0;
  float debit_gauche = 0;
  float intervalle = 0;
  float debit_droit = 0;
  float perted=0;

  float vitesse_gauche = 0;
  for(int i=0; i< poste_de_commande - _decalage;i++){
    debit_gauche+=_Donnees[i][2]*debitasp;
  }

  for(int i=0; i<poste_de_commande - _decalage; i++){
    if(_Donnees[i][2]==0){
      if(amont){
        intervalle+=_Donnees[i][6];
      } else {
        intervalle+=_Donnees[i][5];
      }
      continue;
    }

    if(amont){
      intervalle+=_Donnees[i][6];
    } else {
      intervalle+=_Donnees[i][5];
    }

    float Dia = _diameters[i]; // Diamètre en mm
    sigmadebit += _Donnees[i][2]*debitasp; // Débit en M3/h

    float perte = k * std::pow((sigmadebit*1000)/3600, a) * std::pow(Dia, b) * intervalle;
    perteg += perte;
    intervalle=0;
  }


  for(int i = fin_droit; i >=debut; --i) {
    debit_droit += _Donnees[i][9];
  }

  sigmadebit=0;

  for (int i = fin_droit; i >=debut; --i) {

    if(_Donnees[i][2]==0){
      if(amont){
        intervalle+=_Donnees[i][6];
      } else {
        intervalle+=_Donnees[i][5];
      }
      continue;
    }

    if(amont){
      intervalle+=_Donnees[i][6];
    } else {
      intervalle+=_Donnees[i][5];
    }

    float Dia = _diameters[i];    // Diamètre en mm
    sigmadebit += _Donnees[i][2]*debitasp; // Débit en l/h

    // Calcul de la perte de charge
    float perte = k * std::pow(sigmadebit / 3600, a) * std::pow(Dia, b) * intervalle;
    perted += perte; // Cumul de la perte de charge
    intervalle=0;
  }


}

void parcelle::calculaspersseurs(std::vector<int> &indices, float a, float b, double k) {
  int compteur = 0;

  float piezod=0, piezog=0;
  float pertesPeigneGauche = 0;
  float pertesPeigneDroit = 0;

  int debut = poste_de_commande- _decalage;
  int fin_gauche = debut - 1;
  int fin_droit = _Donnees.size() - 1;

  // Création de la boîte de dialogue
  QDialog dialog(nullptr);
  QFormLayout form(&dialog);

  QLineEdit *lineEditDebit = new QLineEdit(&dialog);
  QString labelDebit = QString("Débit en m³/h:");
  form.addRow(labelDebit, lineEditDebit);

  QLineEdit *lineEditDistRang = new QLineEdit(&dialog);
  QString labelDistRang = QString("Distance entre rangs:");
  form.addRow(labelDistRang, lineEditDistRang);

  QLineEdit *lineEditDistAsperseurs = new QLineEdit(&dialog);
  QString labelDistAsperseurs = QString("Distance entre asperseurs:");
  form.addRow(labelDistAsperseurs, lineEditDistAsperseurs);

  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
  form.addRow(&buttonBox);

  QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
  QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

  if (dialog.exec() == QDialog::Accepted) {
    float debit = lineEditDebit->text().toFloat();
    float distRangs = lineEditDistRang->text().toFloat();
    float distAsperseurs = lineEditDistAsperseurs->text().toFloat();

    float debitConverti = (debit * 1000) / 3600;

    std::sort(indices.begin(), indices.end());
    std::ostringstream debugOutput;

    calcul_gauche_aspersseurs(indices,a,b,k);
    for(int i=0; i<_Donnees.size(); i++){
      std::cout << _Donnees[i][2] <<" "<< std::endl;
    }
    std::cout<<"-----------------------------CALCULGAUCHE---------------------------------"<<std::endl;

    calcul_droit_aspersseurs(indices,a,b,k);
    for(int i=0; i<_Donnees.size(); i++){
      std::cout << _Donnees[i][2] <<" "<< std::endl;
    }
    std::cout<<"-----------------------------CALCULDROIT---------------------------------"<<std::endl;


    calculperteherseasp(indices, debit, distAsperseurs, a,b,k);
    for(int i=0; i<_Donnees.size(); i++){
      std::cout << _Donnees[i][2] <<" "<< std::endl;
    }
    std::cout<<"-----------------------------CALCULPERTE---------------------------------"<<std::endl;


  }
}



void parcelle::onLineEditChanged(const QString &text, int row, int col) {
  bool ok;
  float value = text.toFloat(&ok);
  if (ok) { // Si la conversion est accepté
    _tableValues[row][col] = value;
  }
}

std::vector<std::vector<float>> parcelle::getTableValues() {
  return _tableValues;
}

void parcelle::SetAmont(bool tamont){
  amont = tamont;
}

void parcelle::inverser() {

  // Inverser les lignes
  std::reverse(_Donnees.begin(), _Donnees.end());

  // Réinitialiser les valeurs après l'indice 8
  for (auto& row : _Donnees) {
    for (int i = 9; i < row.size(); ++i) {
      row[i] = 0.0f;  // Vous pouvez ajuster cette valeur en fonction de ce que vous voulez pour la réinitialisation
    }
  }

  float debitcumule = 0;

  float zamont;
  float zaval;

  float zamontO = _Donnees[0][3];
  float zavalO =  _Donnees[0][4];


  for(int i=0;i<_Donnees.size();i++){

    zamont = _Donnees[i][3];
    zaval = _Donnees[i][4];

    debitcumule += _Donnees[i][9];

    _Donnees[i][10] = debitcumule;

    _Donnees[i][11] = zamont - zamontO;
    _Donnees[i][12] = zaval - zavalO;

    _Donnees[i][13] = zamont - zaval;
    _Donnees[i][14] = zaval - zamont;

  }
}

void parcelle::recalcul(){

  float debitcumule = 0;

  float zamont;
  float zaval;

  float zamontO = _Donnees[0][3];
  float zavalO =  _Donnees[0][4];

  for(int i=0;i<_Donnees.size();i++){

    zamont = _Donnees[i][3];
    zaval = _Donnees[i][4];

    debitcumule += _Donnees[i][9];

    _Donnees[i][10] = debitcumule;

    _Donnees[i][11] = zamont - zamontO;
    _Donnees[i][12] = zaval - zavalO;

    _Donnees[i][13] = zamont - zaval;
    _Donnees[i][14] = zaval - zamont;

  }
}


void parcelle::choisirCote(int a){
  _decalage = a;
}

void parcelle::updatediaasp(int ligne, int colonne, float val){
  if(val > 0 && ligne>0 && colonne >0){
    _Donnees[ligne][colonne] = val;
  }
}


void parcelle::calcul_gauche_aspersseurs(std::vector<int> &indices, float a, float b, double k) {
  int debut = poste_de_commande - _decalage;
  int fin_gauche = debut - 1;

  std::vector<float> intervalles;

  float interval;

  for(int i=poste_de_commande-_decalage-1; i>=0; i--) {
    if (std::binary_search(indices.begin(), indices.end(), _Donnees[i][0])) {
      intervalles.push_back(interval);
      if(amont){
        interval= _Donnees[i][5];

      } else {
        interval= _Donnees[i][6];

      }
    } else {
      if(amont){
        interval+= _Donnees[i][5];

      } else {
        interval+= _Donnees[i][6];

      }
    }

  }


  float sigmadebit = 0;
  float debit_gauche = 0;
  float piezo = 0;

  float cumulperte = 0;
  float cumulpiezo = 0;


  int compteur =intervalles.size()-1;

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
  for (int i = 0; i < poste_de_commande - _decalage; ++i) {
    if (std::binary_search(indices.begin(), indices.end(), i)) {

      float Dia = _diameters[i];    // Diamètre en mm
      sigmadebit += _Donnees[i][9]; // Débit en l/h
      float L = intervalles[compteur--];   // Longueur de la conduite en mètre

      // Calcul de la perte de charge
      float perte = k * std::pow(sigmadebit / 3600, a) * std::pow(Dia, b) * L;

      float denivele = amont ? _Donnees[i][3] - _Donnees[fin_gauche][3]
                             : _Donnees[i][4] - _Donnees[fin_gauche][4];

      piezo = perte + denivele;
      cumulperte += perte; // Cumul de la perte de charge
      cumulpiezo += piezo;

      float diametre = Dia / 1000;

      float debitM3S = sigmadebit / 3600 / 1000;

      float diametreM = Dia / 1000;

      float aire = pi * pow((diametreM / 2), 2);

      float vitesse = debitM3S / aire;

      _Donnees[i][17] = denivele;
      _Donnees[i][18] = vitesse;
      _Donnees[i][19] = perte;
      _Donnees[i][20] = piezo;
      _Donnees[i][21] = cumulperte;
      _Donnees[i][22] = cumulpiezo;
    }
  }
}



void parcelle::calcul_droit_aspersseurs(std::vector<int> &indices, float a, float b, double k) {
  int debut = poste_de_commande - _decalage;
  int fin_gauche = debut - 1;
  int fin_droit = _Donnees.size() - 1;

  std::vector<float> intervalles;

  float interval;

  for(int i=debut; i<=fin_droit; i++) {
    if (std::binary_search(indices.begin(), indices.end(), i)){
      intervalles.push_back(interval);
      if(amont){
        interval= _Donnees[i][5];

      } else {
        interval= _Donnees[i][6];

      }
    } else {
      if(amont){
        interval+= _Donnees[i][5];

      } else {
        interval+= _Donnees[i][6];

      }
    }

  }


  float sigmadebit = 0;
  float debit_gauche = 0;
  float piezo = 0;

  float cumulperte = 0;
  float cumulpiezo = 0;


  int compteur =intervalles.size()-1;

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
  for (int i = fin_droit; i >=debut; --i) {
    if (std::binary_search(indices.begin(), indices.end(), i)) {

      float Dia = _diameters[i];    // Diamètre en mm
      sigmadebit += _Donnees[i][9]; // Débit en l/h
      float L = intervalles[compteur--];   // Longueur de la conduite en mètre

      // Calcul de la perte de charge
      float perte = k * std::pow(sigmadebit / 3600, a) * std::pow(Dia, b) * L;

      float denivele = amont ? _Donnees[i][3] - _Donnees[fin_gauche][3]
                             : _Donnees[i][4] - _Donnees[fin_gauche][4];

      piezo = perte + denivele;
      cumulperte += perte; // Cumul de la perte de charge
      cumulpiezo += piezo;

      float debitM3S = sigmadebit / 3600 / 1000;

      float diametreM = Dia / 1000;

      float aire = pi * pow((diametreM / 2), 2);

      float vitesse = debitM3S / aire;

      _Donnees[i][17] = denivele;
      _Donnees[i][18] = vitesse;
      _Donnees[i][19] = perte;
      _Donnees[i][20] = piezo;
      _Donnees[i][21] = cumulperte;
      _Donnees[i][22] = cumulpiezo;
    }
  }
}

void parcelle::calculperteherseasp(std::vector<int> &indices, float debit, float distasp ,float a, float b, double k){
  float debitConverti = (debit * 1000) / 3600;

  for (auto &index : indices) {
    float pertes = 0;

    float nb = _Donnees[index][2];

    int nb_int = nb;

    std::vector<float> diametres(nb_int,55.4);

    for(int j=24; j<29; j++){
      for(int k=0; k<_Donnees[index][j]; k++){
        switch(j){
        case 24: diametres[k]=66; break;
        case 25: diametres[k]=55.4; break;
        case 26: diametres[k]=44; break;
        case 27: diametres[k]=35.2; break;
        case 28: diametres[k]=28; break;
        default: break;
        }
      }
    }


    float firstintervalle;


    if(_Donnees[index][23]==0){
      firstintervalle = 15;
    } else {
      firstintervalle = _Donnees[index][23];
    }


    float intervalle = _Donnees[index][6];
    float diametre = diametres[0];

    pertes = calculPerte(debitConverti, diametre, firstintervalle, a, b, k);

    for (int l = 1; l < nb_int; l++) {
      diametre = diametres[l];
      float newDebitConverti = debitConverti * (l+1);

      float perteAsperseur = k * std::pow(newDebitConverti, a) * std::pow(diametre, b) * distasp;
      pertes += perteAsperseur;

      std::cout << "j = " << a << ", diametre = " << diametre << ", debit converti = " << newDebitConverti << ", perte asperseur = " << perteAsperseur << std::endl;
    }

    std::cout<<"TAILLE DE TA RACE LE NAIN : "<< _Donnees[index].size()<<std::endl;

    _Donnees[index][29] = pertes;
    _Donnees[index][30] = amont ? pertes + _Donnees[index][14] : pertes + _Donnees[index][13];

    std::cout<<pertes<<std::endl;



  }
}
