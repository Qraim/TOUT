//
// Created by Quentin Raimbault on 22/05/2023.
//

#include "parcelle.h"


parcelle::parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db, QString nom, bool amont2, std::string mat)
    : database(db), _nom(nom), amont(amont2), _indexdebut(indexdebut), _indexfin(indexfin), _calcul(false), _matiere(mat) {

  int range_size = indexfin - indexdebut;
  aspdebit = 0;
  aspinter = 0;
  aspinterdebut = 0;

  _Donnees.resize(range_size);
  _diameters.resize(range_size);

  hersealim = std::make_unique<pertechargeherse>(database, nullptr);

  std::copy(data.begin() + indexdebut, data.begin() + indexfin, _Donnees.begin());

  poste_de_commande = 0;

  milieuhydro = trouvemilieuhydro();

  _longueur = std::accumulate(_Donnees.begin(), _Donnees.end(), 0.0,
                              [](double sum, const std::vector<float>& v) { return sum + v[1]; });
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

  // Diviser le débit total par deux en avance
  float debitMoitie = debitTotal / 2;

  // Chercher l'indice où le débit cumulé dépasse la moitié du débit cumulé total
  float cumulatedDebit = 0;
  for(int i = 0; i < _Donnees.size(); ++i){
    const auto& donnee = _Donnees[i];
    if (donnee.size() < 11) {
      continue;
    }
    cumulatedDebit += donnee[9];
    if(cumulatedDebit >= debitMoitie){
      return i;
    }
  }

  return 0;
}


void parcelle::calcul() {
  // Si poste_de_commande est à zéro, on sort de la fonction
  if(poste_de_commande==0)
    return;

  if(_matiere.empty()){
    _matiere="PVC";
  }

  // Coefficients pour le calcul de la perte de charg
  std::tuple<float, float, double> coefficients = database->get_material_coefficients(_matiere);
  float a = std::get<0>(coefficients);
  float b = std::get<1>(coefficients);
  double k = std::get<2>(coefficients);

  auto aspesseurs = trouveaspersseurs();

  /*
  for(int i=0; i<_Donnees.size(); i++){
    std::cout << _Donnees[i][2] <<" "<< std::endl;
  }
  std::cout<<"-----------------------------CALCUL---------------------------------"<<std::endl;
  */

  if(!aspesseurs.empty()){
    calculaspersseurs(aspesseurs,a,b,k);
    _calcul = true;
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
  float dia16 = 14;
  float dia20 = 17.6;
  float powDia16 = std::pow(dia16, b);
  float powDia20 = std::pow(dia20, b);

  for(int i=0; i<_Donnees.size(); ++i){
    // Calcul du goutte à goutte
    float arosseurs = _Donnees[i][8] != 0 ? _Donnees[i][1] / _Donnees[i][8] : 0;

    float debitLS = _Donnees[i][7] / 3600;

    float espacement = _Donnees[i][8];
    float perte16 = 0;
    float perte20 = 0;

    for(int j=1; j <= arosseurs; ++j){
      float debitPow = std::pow(debitLS * j, a);
      perte16 += k * debitPow * powDia16 * espacement;
      perte20 += k * debitPow * powDia20 * espacement;
    }

    float additional = amont ? _Donnees[i][14] : _Donnees[i][13];

    _Donnees[i][23] = perte16;
    _Donnees[i][24] = perte16 + additional;
    _Donnees[i][25] = perte20;
    _Donnees[i][26] = perte20 + additional;
  }
}


void parcelle::setDiametreDialog(std::string matiere) {

  _matiere=matiere;

  if(poste_de_commande==0){
    setPosteDeCommande(milieuhydro);
  }

  bool toutlestuyaux = true;


  for(auto &i : _Donnees){
      if(i[15]==0){
          toutlestuyaux =false;
          break;
      }
  }

  std::vector<int> temp = trouveaspersseurs();

  if(!temp.empty()) {
    bool toutdiametre = true;
    for(auto& it : temp) {
      if(_Donnees[it][15]==0){
        toutdiametre=false;
        break;
      }
    }

    if(toutdiametre){
      calcul();
      return;
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

  // Label
  QLabel pressureLabel("Pression");
  dialogLayout.addWidget(&pressureLabel);

  QComboBox pressureComboBox;
  dialogLayout.addWidget(&pressureComboBox);

  // Combobox
  QLabel innerDiameterLabel("Diametre");
  dialogLayout.addWidget(&innerDiameterLabel);

  QComboBox innerDiameterComboBox;
  dialogLayout.addWidget(&innerDiameterComboBox);

  // Choix des rangs
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

  // Boutton
  QPushButton setButton("Choix Intervale et Diametre");
  dialogLayout.addWidget(&setButton);

  diameterDialog.setLayout(&dialogLayout);

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

  QObject::connect(&setButton, &QPushButton::clicked, [this, &diameterDialog, &innerDiameterComboBox, &startSpinBox, &endSpinBox, &label, &materialComboBox]() {
    float diameter = innerDiameterComboBox.currentText().toFloat();

    _matiere = materialComboBox.currentText().toStdString();

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
  _Donnees[index][15] = diameters;
}

void parcelle::modifiedebit(int index, float diameters){

  _Donnees[index][9] = diameters;
  recalcul();
}

void parcelle::modifieinter(int index,int colonne, float diameters){
  int ligne = index;
  if(index > _Donnees.size()-1){
    ligne = index - _indexdebut;
  }
  _Donnees[ligne][colonne] = diameters;
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
      if(trouveaspersseurs().size()==0){
        row[23] = 0.0;
      }

    }
  }

  if(posteDeCommande==_indexdebut){
      poste_de_commande=milieuhydro+_indexdebut+1;
  } else if(posteDeCommande<_indexdebut){
        poste_de_commande=posteDeCommande;
  }else
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

int parcelle::getvraiindiceposte(){
    return poste_de_commande+_indexdebut;
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

    float Dia = _Donnees[i][15]; // Diamètre en mm
    sigmadebit += _Donnees[i][9]; // Débit en l/h
    float L = _Donnees[i][5]; // Longueur de la conduite en mètre

    // Calcul de la perte de charge
    float perte = k * std::pow(sigmadebit/3600, a) * std::pow(Dia, b) * L;

    float denivele = 0;

    if(fin_gauche<0)
      denivele = amont ?    _Donnees[i][3] - _Donnees[0][3]  :  _Donnees[i][4] - _Donnees[0][4] ;
    else
      denivele = amont ?    _Donnees[i][3] - _Donnees[fin_gauche][3]  :  _Donnees[i][4] - _Donnees[fin_gauche][4] ;


    if(i-1 >= debut)
        piezo = perte + (amont ?    _Donnees[i][3] - _Donnees[i-1][3]  :  _Donnees[i][4] - _Donnees[i-1][4]) ;
    else
        piezo = perte + (amont ?    _Donnees[i][3] - _Donnees[debut][3]  :  _Donnees[i][4] - _Donnees[debut][4]) ;

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

    float Dia = _Donnees[i][15]; // Diamètre en mm
    sigmadebit += _Donnees[i][9]; // Débit en l/h
    float L = _Donnees[i][5]; // Longueur de la conduite en mètre

    // Calcul de la perte de charge
    float perte = k * std::pow(sigmadebit/3600, a) * std::pow(Dia, b) * L;

    float denivele = amont ?    _Donnees[i][3] - _Donnees[fin_gauche][3]   :    _Donnees[i][4] - _Donnees[fin_gauche][4] ;

    if(i+1 < poste_de_commande)
        piezo = perte + (amont ?    _Donnees[i][3] - _Donnees[i+1][3]  :  _Donnees[i][4] - _Donnees[i+1][4]) ;
    else
        piezo = perte + (amont ?    _Donnees[i][3] - _Donnees[poste_de_commande-1][3]  :  _Donnees[i][4] - _Donnees[poste_de_commande-1][4]) ;

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
/*

    // Imprime les informations de débogage de manière structurée
    std::cout << "----- Information de la conduite (gauche) -----" << std::endl;
    std::cout << "Tuyau: " << _Donnees[i][0] << std::endl;
    std::cout << "Diamètre (D): " << Dia << " mm" << std::endl;
    std::cout << "Débit cumulatif (sigmadebit): " << sigmadebit << " l/h" << std::endl;
    std::cout << "Intervale (L): " << L << " m" << std::endl;
    std::cout << "Coefficients: " << "a=" << a << ", b=" << b << ", k=" << k << std::endl;
    std::cout << "Perte de charge calculée : " << perte << std::endl;
    std::cout << "Perte de charge cumulée (perteg): " << cumulperte << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
*/
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

const std::string &parcelle::getMatiere() const { return _matiere; }


std::vector<int> parcelle::trouveaspersseurs() {
  std::vector<int> result;
  for (int i = 0; i < _Donnees.size(); i++) {
    if (_Donnees[i][2] != 0) {
      result.push_back(i);
    }
  }

  // Trier le vecteur en ordre croissant
  std::sort(result.begin(), result.end());

  return result;
}


#include <QInputDialog>

/*// Créez une fonction pour calculer la perte
float calculPerte(float debit, float Dia, float intervale, float a, float b, float k) {
  float perte = k * std::pow((debit), a) * std::pow(Dia, b) * intervale;
  //std::cout<<perte<<std::endl;
  return perte;
}*/

void parcelle::calculpeigne(float a, float b, double k, float debitasp){
  int debut = poste_de_commande - _decalage;
  int fin_droit = _Donnees.size() - 1;

  float perteg = 0;
  float sigmadebitg = 0;
  float intervalleg = 0;
  float perted = 0;
  float sigmadebitd = 0;
  float intervalled = 0;

  const float debit_scale = 1000.0f / 3600.0f;

  for(int i=0; i< debut; ++i){
    float interval = amont ? _Donnees[i][6] : _Donnees[i][5];
    float debit = _Donnees[i][2] * debitasp;

    if(_Donnees[i][2] != 0){
      float Dia = _Donnees[i][15]; // Diamètre en mm
      sigmadebitg += debit; // Débit en M3/h
      float perte = k * std::pow(sigmadebitg * debit_scale, a) * std::pow(Dia, b) * interval;
      perteg += perte;
    }
    intervalleg += interval;
  }

  for(int i = fin_droit; i >= debut; --i) {
    float interval = amont ? _Donnees[i][6] : _Donnees[i][5];
    float debit = _Donnees[i][2] * debitasp;

    if(_Donnees[i][2] != 0){
      float Dia = _Donnees[i][15];    // Diamètre en mm
      sigmadebitd += debit; // Débit en l/h

      // Calcul de la perte de charge
      float perte = k * std::pow(sigmadebitd * debit_scale, a) * std::pow(Dia, b) * interval;
      perted += perte; // Cumul de la perte de charge
    }
    intervalled += interval;
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

  if(debut<0){
    return;
  }

  // Création de la boîte de dialogue
  QDialog dialog(nullptr);
  QFormLayout form(&dialog);

  QLineEdit *lineEditDebit = new QLineEdit(&dialog);
  if(aspdebit!=0){
    lineEditDebit->setText(QString::number(aspdebit));
  }
  QString labelDebit = QString("Débit en m³/h:");
  form.addRow(labelDebit, lineEditDebit);

  QLineEdit *lineEditDistRang = new QLineEdit(&dialog);
  dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    if(aspinter!=0){
    lineEditDistRang->setText(QString::number(aspinter));
  }
  QString labelDistRang = QString("Distance entre rangs:");
  form.addRow(labelDistRang, lineEditDistRang);

  QLineEdit *lineEditDistAsperseurs = new QLineEdit(&dialog);
  if(aspinterdebut!=0){
    lineEditDistAsperseurs->setText(QString::number(aspinterdebut));
  }
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

    aspdebit = debit;
    aspinter = distRangs;
    aspinterdebut = distAsperseurs;

    _debit = 0;

    for(auto it : indices){
      _debit += aspdebit * _Donnees[it][2];
    }

    _debit = (_debit)*1000;

    float debitConverti = (debit * 1000) / 3600;

    milieuhydro = trouvemilieuhydroasp();

    std::sort(indices.begin(), indices.end());
    std::ostringstream debugOutput;

    calcul_gauche_aspersseurs(indices,debitConverti, a,b,k);
    /*for(int i=0; i<_Donnees.size(); i++){
      std::cout << _Donnees[i][2] <<" "<< std::endl;
    }
    std::cout<<"-----------------------------CALCULGAUCHE---------------------------------"<<std::endl;
*/

    calcul_droit_aspersseurs(indices,debitConverti,a,b,k);
    /* for(int i=0; i<_Donnees.size(); i++){
       std::cout << _Donnees[i][2] <<" "<< std::endl;
     }
     std::cout<<"-----------------------------CALCULDROIT---------------------------------"<<std::endl;
 */

  }
}



void parcelle::SetAmont(bool tamont){
  amont = tamont;
}

void parcelle::inverser() {

  // Inverser les lignes
  std::reverse(_Donnees.begin(), _Donnees.end());

  // Réinitialiser les valeurs après l'indice 8
  for (auto& row : _Donnees) {
    for (int i = 10; i < row.size(); ++i) {
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

int parcelle::trouvemilieuhydroasp(){
  // Vérifiez que _Donnees n'est pas vide
  if (_Donnees.empty()) {
    return 0;
  }

  // Diviser le débit total par deux en avance
  float debitMoitie = _debit / 2;

  // Chercher l'indice où le débit cumulé dépasse la moitié du débit cumulé total
  float cumulatedDebit = 0;
  for(int i = 0; i < _Donnees.size(); ++i){
    const auto& donnee = _Donnees[i];
    if (donnee.size() < 11) {
      continue;
    }
    cumulatedDebit += donnee[2] * (aspdebit*1000);
    if(cumulatedDebit >= debitMoitie){
      return i;
    }
  }

  return 0;

}


void parcelle::choisirCote(int a){
  _decalage = a;
}


#include <unordered_set>

void parcelle::calcul_gauche_aspersseurs(std::vector<int> &indices,float debit, float a, float b, double k) {
  int debut = poste_de_commande - _decalage;
  int fin_gauche = debut - 1;

  std::vector<float> intervalles;

  std::unordered_set<int> indicesSet(indices.begin(), indices.end());

  float interval = 0;

  for(int i=poste_de_commande-_decalage-1; i>=0; i--) {
    if (indicesSet.count(i)) {
      intervalles.push_back(interval);
      interval= amont ? _Donnees[i][5] : _Donnees[i][6];
    } else {
      interval += amont ? _Donnees[i][5] : _Donnees[i][6];
    }
  }

  std::vector<int> sorted_indices(indices.begin(), indices.end());
  std::sort(sorted_indices.begin(), sorted_indices.end());

  float sigmadebit = 0;
  float debit_gauche = 0;
  float piezo = 0;

  float cumulperte = 0;
  float cumulpiezo = 0;

  int compteur =intervalles.size()-1;

  float const_debitM3S = 1.0f / 3600 / 1000;
  float const_pi = 3.14159265358979323846f;

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
  for (int i = 0; i < poste_de_commande - _decalage; ++i) {
    if (indicesSet.count(i)) {

      float Dia = _Donnees[i][15]; // Diamètre en mm
      sigmadebit += _Donnees[i][2]*debit; // Débit en l/s
      float L = intervalles[compteur--]; // Longueur de la conduite en mètre
      // Calcul de la perte de charge
      float perte = k * std::pow(sigmadebit, a) * std::pow(Dia, b) * L;

      int next_idx = 0;

      auto it = std::upper_bound(sorted_indices.begin(), sorted_indices.end(), i);
      next_idx = (it != sorted_indices.end() && *it > poste_de_commande) ? fin_gauche :*it;

      if(next_idx > poste_de_commande || next_idx < 0)
        next_idx = poste_de_commande-1;

      float denivele = amont ? _Donnees[i][3] - _Donnees[next_idx][3]
                             : _Donnees[i][4] - _Donnees[next_idx][4];

/*
      // Affichage des valeurs
      std::cout << "Diamètre (mm) : " << Dia <<" ";
      std::cout << "Sigma Débit (l/h) : " << sigmadebit << " ";
      std::cout << "a "<<a <<"b "<<b <<"k "<< k << " ";
      std::cout << "Longueur (m) : " << L << std::endl;
      std::cout << "Matiere : " << _matiere << std::endl;
      std::cout<<"-------------------------" << std::endl;
*/

      piezo = perte + denivele;
      cumulperte += perte; // Cumul de la perte de charge
      cumulpiezo += piezo;

      float debitM3S = sigmadebit / 1000;

      float diametreM = Dia / 1000;

      float aire = const_pi * std::pow((diametreM / 2), 2);

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

void parcelle::herse(int ligne){

  hersealim->refresh();
  int index = ligne;

  if(ligne >= _Donnees[0][0]){
    index = ligne - _indexdebut;
  }

  if(_Donnees[index][2]<=0){
    return;
  }

  float denivele = 0;

  if(amont){
    denivele = _Donnees[index][4]-_Donnees[index][3];
  } else {
    denivele = _Donnees[index][3]-_Donnees[index][4];
  }

  hersealim->importdonees(aspdebit, 55.4, aspinterdebut, _Donnees[index], denivele);
  hersealim->show();
}




void parcelle::calcul_droit_aspersseurs(std::vector<int> &indices, float debit,float a, float b, double k) {
  int debut = poste_de_commande - _decalage;
  int fin_droit = _Donnees.size() - 1;

  std::vector<float> intervalles;
  std::unordered_set<int> indicesSet(indices.begin(), indices.end());

  float interval = 0;
  for(int i=debut; i<=fin_droit; i++) {
    if (indicesSet.count(i)){
      intervalles.push_back(interval);
      interval = amont ? _Donnees[i][5] : _Donnees[i][6];
    } else {
      interval += amont ? _Donnees[i][5] : _Donnees[i][6];
    }
  }

  std::vector<int> sorted_indices(indices.begin(), indices.end());
  std::sort(sorted_indices.rbegin(), sorted_indices.rend()); // on le trie dans l'ordre croissant


  float sigmadebit = 0;
  float piezo = 0;

  float cumulperte = 0;
  float cumulpiezo = 0;

  int compteur =intervalles.size()-1;
  float const_debitM3S = 1.0f / 3600 * 1000;
  float const_pi = 3.14159265358979323846f;

  // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
  for (int i = fin_droit; i >= debut; --i) {
    if (indicesSet.count(i)) {
      float Dia = _Donnees[i][15]; // Diamètre en mm
      sigmadebit += _Donnees[i][2]*debit; // Débit en l/s
      float L = intervalles[compteur--]; // Longueur de la conduite en mètre

      // Calcul de la perte de charge
      float perte = k * std::pow(sigmadebit, a) * std::pow(Dia, b) * L;

      float debitM3S = sigmadebit/1000;

      float diametreM = Dia / 1000;

      float aire = const_pi * std::pow((diametreM / 2), 2);

      float vitesse = debitM3S / aire;

/*      // Affichage des valeurs
      std::cout << "Diamètre (mm) : " << Dia <<" ";
      std::cout << "Sigma Débit (l/h) : " << sigmadebit << " ";
      std::cout << "a "<<a <<" b "<<b <<" k "<< k << " ";
      std::cout << "Longueur (m) : " << L << " ";
      std::cout << "perte (m) : " << perte << " ";
      std::cout << "vitesse (m/s) : " << vitesse << std::endl;
      std::cout<<"-------------------------" << std::endl;*/

      auto nextIndexIter = std::find_if(indices.rbegin(), indices.rend(), [&](int index) { return index < i; });
      int nextIndex = (nextIndexIter != indices.rend() && *nextIndexIter > debut) ? *nextIndexIter : debut;

      if(nextIndex < 0 || nextIndex >= _Donnees.size())
        nextIndex = debut;

      float denivele = amont ? _Donnees[nextIndex][3] - _Donnees[i][3]
                             : _Donnees[nextIndex][4] - _Donnees[i][4];

      piezo = perte + denivele;
      cumulperte += perte; // Cumul de la perte de charge
      cumulpiezo += piezo;

      _Donnees[i][17] = -denivele;
      _Donnees[i][18] = vitesse;
      _Donnees[i][19] = perte;
      _Donnees[i][20] = piezo;
      _Donnees[i][21] = cumulperte;
      _Donnees[i][22] = cumulpiezo;
    }
  }
}



std::string parcelle::toString() const {
  std::string s;

  for (const auto &row : _Donnees) {
    for (const auto &value : row) {
      s += std::to_string(value) + ",";
    }
    s.back() = ';';
  }


  // on sauvegarde les données importantes
  s += _nom.toStdString() + ";";
  s += std::to_string(milieuhydro) + ";";
  s += std::to_string(poste_de_commande) + ";";
  s += std::to_string(_indexdebut) + ";";
  s += std::to_string(_indexfin) + ";";
  s += std::to_string(_longueur) + ";";
  s += _matiere + ";";
  s += std::to_string(amont) + ";";
  s += std::to_string(_debit) + ";";
  s += std::to_string(_calcul) + ";";
  s += std::to_string(_decalage);

  return s;
}

void parcelle::fromString(const std::string& s,std::shared_ptr<bdd> db) {
  database = db;
  std::istringstream ss(s);
  std::string item;

  // Charge les données passer en parametre dans la parcelle
  _Donnees.clear();
  while (std::getline(ss, item, ';')) {
    std::istringstream iss(item);
    std::string value;
    std::vector<float> row;
    while (std::getline(iss, value, ',')) {
      row.push_back(std::stof(value));
    }
    _Donnees.push_back(row);
  }

  // Load other data from string
  std::getline(ss, item, ';');
  _nom = QString::fromStdString(item);

  std::getline(ss, item, ';');
  milieuhydro = std::stoi(item);

  std::getline(ss, item, ';');
  poste_de_commande = std::stoi(item);

  std::getline(ss, item, ';');
  _indexdebut = std::stoi(item);

  std::getline(ss, item, ';');
  _indexfin = std::stoi(item);

  std::getline(ss, item, ';');
  _longueur = std::stof(item);

  std::getline(ss, item, ';');
  _matiere = item;

  std::getline(ss, item, ';');
  amont = std::stoi(item);

  std::getline(ss, item, ';');
  _debit = std::stof(item);

  std::getline(ss, item, ';');
  _calcul = std::stoi(item);

  std::getline(ss, item);
  _decalage = std::stoi(item);
}

int parcelle::getDecalage() const {
    return _decalage;
}

void parcelle::setDecalage(int decalage) {
    _decalage = decalage;
}

float parcelle::getAspdebit() const {
    return aspdebit;
}

void parcelle::setAspdebit(float aspdebit) {
    parcelle::aspdebit = aspdebit;
}

float parcelle::getAspinter() const {
    return aspinter;
}

void parcelle::setAspinter(float aspinter) {
    parcelle::aspinter = aspinter;
}

float parcelle::getAspinterdebut() const {
    return aspinterdebut;
}

void parcelle::setAspinterdebut(float aspinterdebut) {
    parcelle::aspinterdebut = aspinterdebut;
}
