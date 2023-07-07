//
// Created by Quentin Raimbault on 22/05/2023.
//

#include "parcelle.h"


parcelle::parcelle(std::vector<std::vector<float>> &data, int indexdebut, int indexfin, std::shared_ptr<bdd> db,
                   QString nom, bool amont2, std::string mat)
        : database(db), _nom(nom), amont(amont2), _indexdebut(indexdebut), _indexfin(indexfin), _calcul(false),
          _matiere(mat) {

    int range_size = indexfin - indexdebut;
    aspdebit = 0;
    aspinterdebut = 0;

    _Donnees.resize(range_size);
    _diameters.resize(range_size);

    hersealim = std::make_unique<pertechargeherse>(database, nullptr);

    std::copy(data.begin() + indexdebut, data.begin() + indexfin, _Donnees.begin());

    poste_de_commande = -1;

    milieuhydro = trouvemilieuhydro();

    _longueur = std::accumulate(_Donnees.begin(), _Donnees.end(), 0.0,
                                [](double sum, const std::vector<float> &v) { return sum + v[1]; });
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

std::vector<std::vector<float>> &parcelle::getDonnees() {
    return _Donnees;
}

int parcelle::trouvemilieuhydro() {
    // Vérifiez que _Donnees n'est pas vide
    if (_Donnees.empty()) {
        return 0;
    }

    // Calculer le débit cumulé total et trouver l'indice du milieu hydrologique en une seule boucle
    float debitTotal = 0;
    float cumulatedDebit = 0;
    int midHydroIndex = 0;

    for (int i = 0; i < _Donnees.size(); ++i) {
        const auto &donnee = _Donnees[i];
        if (donnee.size() < 11) {
            continue;
        }

        debitTotal += donnee[9];

    }


    float milieudebitotal = debitTotal /2;


    for (int i = 0; i < _Donnees.size(); ++i) {
        const auto &donnee = _Donnees[i];
        if (donnee.size() < 11) {
            continue;
        }

        cumulatedDebit += donnee[9];

        // Mettre à jour l'indice du milieu hydrologique lors du passage du débit cumulé à la moitié du débit total
        if (cumulatedDebit >= milieudebitotal ) {
            midHydroIndex = i;
            break;
        }
    }


    _debit = debitTotal;

    // Si le débit total est 0, retourner 0
    if (debitTotal == 0) {
        return 0;
    }

    return midHydroIndex;
}


void parcelle::calcul() {

    // Si poste_de_commande est à zéro, on sort de la fonction
    if (poste_de_commande<0 ||poste_de_commande> _Donnees.size() ){
        if (milieuhydro == 0) { // Si le milieu hydraulique est 0
            // Placer le poste de commande au milieu de la parcelle
            poste_de_commande = _Donnees.size() / 2;
        } else {
            // Utiliser le milieu hydraulique comme poste de commande
            poste_de_commande = milieuhydro +1;
        }
    }

    if (_matiere.empty()) _matiere = "PVC";

    // Coefficients pour le calcul de la perte de charge
    auto coefficients = database->get_material_coefficients(_matiere);
    float a = std::get<0>(coefficients);
    float b = std::get<1>(coefficients);
    double k = std::get<2>(coefficients);

    if (a == 0 || b == 0 || k == 0) {
        a = 1.75;
        b = -4.85;
        k = 8381743.11;
    }

    auto aspesseurs = trouveaspersseurs();

    if (!aspesseurs.empty()) {
        calculaspersseurs(aspesseurs, a, b, k);
        _calcul = true;
        return;
    }

    calcul_gauche(a, b, k);
    calcul_droit(a, b, k);
    calculdiametre(a, b, k);
    _calcul = true;
}

void parcelle::optimize(float aspdebit, float aspinterdebut){
    if (_matiere.empty()) _matiere = "PVC";

    // Coefficients pour le calcul de la perte de charge
    auto coefficients = database->get_material_coefficients(_matiere);
    float a = std::get<0>(coefficients);
    float b = std::get<1>(coefficients);
    double k = std::get<2>(coefficients);

    if (a == 0 || b == 0 || k == 0) {
        a = 1.75;
        b = -4.85;
        k = 8381743.11;
    }


    auto aspesseurs = trouveaspersseurs();

    if (!aspesseurs.empty()) {
        parcelle::aspdebit = aspdebit;
        parcelle::aspinterdebut = aspinterdebut;

        optimize_aspersseurs(aspesseurs, a, b, k);
        _calcul = true;
        return;
    }

    optimize_diameters_droit(a,b,k);
    optimize_diameters_gauche(a,b,k);
    calculdiametre(a,b,k);
}

void parcelle::optimize_aspersseurs(std::vector<int> indices, float a, float b, double k) {


    for(auto &it : _Donnees){
        if(it[2]==0){
            it[15] = 0; // On suprimme les diametres où il n'y a pas d'asperseurs
        }
    }

    int debut = poste_de_commande - _decalage;
    if (debut < 0) {
        return;
    }

    if(aspdebit!=0 && aspinterdebut!=0){
        _debit = 0;
        for (const auto &it: indices) {
            _debit += aspdebit * _Donnees[it][2];
        }

        _debit *= 1000;

        float debitConverti = (aspdebit * 1000) / 3600;

        milieuhydro = trouvemilieuhydroasp();

        std::sort(indices.begin(), indices.end());

        optimize_gauche_aspersseurs(indices, debitConverti, a, b, k);
        optimize_droit_aspersseurs(indices, debitConverti, a, b, k);
        return;
    }

    // Création de la boîte de dialogue
    QDialog dialog(nullptr);
    QFormLayout form(&dialog);

    QLineEdit *lineEditDebit = new QLineEdit(&dialog);
    lineEditDebit->setText(QString::number(0));
    form.addRow("Débit en m³/h:", lineEditDebit);


    QLineEdit *lineEditDistAsperseurs = new QLineEdit(&dialog);
    lineEditDistAsperseurs->setText(QString::number(0));
    form.addRow("Distance entre asperseurs:", lineEditDistAsperseurs);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    if (dialog.exec() == QDialog::Accepted) {
    aspdebit = lineEditDebit->text().toFloat();
    aspinterdebut = lineEditDistAsperseurs->text().toFloat();

    _debit = 0;
    for (const auto &it: indices) {
    _debit += aspdebit * _Donnees[it][2];
    }

    _debit *= 1000;

    float debitConverti = (aspdebit * 1000) / 3600;

    milieuhydro = trouvemilieuhydroasp();

    std::sort(indices.begin(), indices.end());

    optimize_gauche_aspersseurs(indices, debitConverti, a, b, k);
    optimize_droit_aspersseurs(indices, debitConverti, a, b, k);

    }
}


void parcelle::calculdiametre(float a, float b, double k) {
    float dia16 = 14;
    float dia20 = 17.6;
    float powDia16 = std::pow(dia16, b);
    float powDia20 = std::pow(dia20, b);

    for (int i = 0; i < _Donnees.size(); ++i) {
        // Ensure that _Donnees[i] has enough elements
        if (_Donnees[i].size() >= 31) {
            // Calcul du goutte à goutte
            float arosseurs = (_Donnees[i][8] != 0) ? _Donnees[i][1] / _Donnees[i][8] : 0;

            float debitLS = _Donnees[i][7] / 3600;

            float espacement = _Donnees[i][8];
            float perte16 = 0;
            float perte20 = 0;

            float calcDia16 = k * powDia16 * espacement;
            float calcDia20 = k * powDia20 * espacement;

            for (int j = 1; j <= arosseurs; ++j) {
                float debitPow = std::pow(debitLS * j, a);
                perte16 += debitPow * calcDia16;
                perte20 += debitPow * calcDia20;
            }

            float additional = amont ? _Donnees[i][14] : _Donnees[i][13];

            _Donnees[i][27] = perte16;
            _Donnees[i][28] = perte16 + additional;
            _Donnees[i][29] = perte20;
            _Donnees[i][30] = perte20 + additional;
        }
        else {
            //Gestion des erreurs
        }
    }

}


void parcelle::setDiametreDialog(std::string matiere) {

    _matiere = matiere;

    if (poste_de_commande < 0) {
        setPosteDeCommande(milieuhydro);
    }

    bool toutlestuyaux = true;


    for (auto &i: _Donnees) {
        if (i[15] == 0) {
            toutlestuyaux = false;
            break;
        }
    }

    std::vector<int> temp = trouveaspersseurs();

    if (!temp.empty()) {
        bool toutdiametre = true;
        for (auto &it: temp) {
            if (_Donnees[it][15] == 0) {
                toutdiametre = false;
                break;
            }
        }

        if (toutdiametre) {
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
    for (const auto &material: materials) {
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
    startSpinBox.setRange(_indexdebut + 1, _indexfin - 1);
    dialogLayout.addWidget(&startSpinBox);

    QLabel endLabel("Arrive");
    dialogLayout.addWidget(&endLabel);

    QSpinBox endSpinBox;
    endSpinBox.setRange(_indexdebut + 2, _indexfin);
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
                         std::vector<int> pressures = database->getAllPressuresForMatiere(
                                 selectedMaterial.toStdString());
                         pressureComboBox.clear();
                         for (int pressure: pressures) {
                             pressureComboBox.addItem(QString::number(pressure));
                         }
                     });

    QObject::connect(&pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     [this, &materialComboBox, &innerDiameterComboBox, &pressureComboBox](int index) {
                         QString selectedMaterial = materialComboBox.currentText();
                         int selectedPressure = pressureComboBox.itemText(index).toInt();
                         std::vector<float> innerDiameters = database->getInnerDiametersForMatiereAndPressure(
                                 selectedMaterial.toStdString(), selectedPressure);
                         innerDiameterComboBox.clear();
                         for (float innerDiameter: innerDiameters) {
                             innerDiameterComboBox.addItem(QString::number(innerDiameter));
                         }
                     });

    QObject::connect(&setButton, &QPushButton::clicked,
                     [this, &diameterDialog, &innerDiameterComboBox, &startSpinBox, &endSpinBox, &label, &materialComboBox]() {
                         float diameter = innerDiameterComboBox.currentText().toFloat();

                         _matiere = materialComboBox.currentText().toStdString();

                         int startIndex = startSpinBox.value() - _indexdebut - 1;
                         int endIndex = endSpinBox.value() - _indexdebut;

                         for (int i = startIndex; i < endIndex; i++) {
                             _diameters[i] = diameter;
                             _Donnees[i][15] = diameter;
                         }

                         endSpinBox.setValue(endSpinBox.value() + 1);
                         startSpinBox.setValue(endSpinBox.value());

                         int nonZeroCount = std::count_if(_diameters.begin(), _diameters.end(),
                                                          [](float val) { return val != 0.0f; });

                         QString text = QString::number(nonZeroCount) + "/" + QString::number(_diameters.size());

                         label.setText(text);

                         bool toutlestuyaux = true;

                         for (int i = 0; i < _diameters.size(); i++) {
                             if (_diameters[i] == 0) {
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

    if (materials.size() > 0) {
        materialComboBox.setCurrentIndex(3);
    }

    diameterDialog.exec();
}

void parcelle::modifiedia(int index, float diameters) {
    _Donnees[index][15] = diameters;
    auto indices = trouveaspersseurs();

    if(indices.size()>0){
        if(aspdebit !=0 && aspinterdebut!=0){
            bool toutdiametre = true;
            for (auto &it: indices) {
                if (_Donnees[it][15] == 0) {
                    toutdiametre = false;
                    break;
                }
            }

            if (toutdiametre) {
                calcul();
                return;
            }
        }
    } else {
        bool toutlestuyaux = true;

        for (int i = 0; i < _diameters.size(); i++) {
            if (_diameters[i] == 0) {
                toutlestuyaux = false;
                break;
            }
        }

        if (toutlestuyaux) {
            calcul();
        }
    }
}

void parcelle::modifiedebit(int index, float diameters) {

    _Donnees[index][9] = diameters;
    recalcul();
}

void parcelle::modifieinter(int index, int colonne, float diameters) {
    int ligne = index;
    if (index > _Donnees.size() - 1) {
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
    // Nettoyage des résultats de calculs précédents
    for (auto &ligne: _Donnees) {
        if (ligne.size() > 19) {
            std::fill(ligne.begin() + 17, ligne.begin() + 23, 0.0);
            if (trouveaspersseurs().empty()) {
                ligne[23] = 0.0;
            }
        }
    }

    // Mise à jour du poste de commande
    if (posteDeCommande == _indexdebut) {
        poste_de_commande = milieuhydro + _indexdebut + 1;
    } else if (posteDeCommande < _indexdebut) {
        poste_de_commande = posteDeCommande;
    } else {
        poste_de_commande = posteDeCommande - _indexdebut;
    }

    // Mise à jour des données en fonction du nouveau poste de commande
    int debut = poste_de_commande;
    int fin_droit = _Donnees.size() - 1;

    for (int i = fin_droit; i >= debut; --i) {
        if ((i == debut || i - 1 >= debut) && _Donnees.size() > i && _Donnees[i].size() >= 13 && _Donnees[i-1].size() >= 5) {
            _Donnees[i][amont ? 11 : 12] =  _Donnees[i][amont ? 3 : 4] - _Donnees[i - 1][amont ? 3 : 4] ;
        }
        else{
            // Gestion d'erreur si les données ne sont pas disponibles
        }
    }

    for (int i = 0; i < poste_de_commande; ++i) {
        if ((i + 1 < poste_de_commande) && _Donnees.size() > i && _Donnees[i].size() >= 13 && _Donnees[i+1].size() >= 5) {
            _Donnees[i][amont ? 11 : 12] = _Donnees[i][amont ? 3 : 4] - _Donnees[i + 1][amont ? 3 : 4];
        } else {
            if(_Donnees.size() > i && _Donnees[i].size() >= 13){
                _Donnees[i][amont ? 11 : 12] = 0;
            }
            else{
                // Gestion d'erreur si les données ne sont pas disponibles
            }
        }
    }

    auto indices = trouveaspersseurs();

    if(indices.size()>0){
        if(aspdebit !=0 && aspinterdebut!=0){
            bool toutdiametre = true;
            for (auto &it: indices) {
                if (_Donnees[it][15] == 0) {
                    toutdiametre = false;
                    break;
                }
            }

            if (toutdiametre) {
                calcul();
                return;
            }
        }
    } else {
        bool toutlestuyaux = true;

        for (int i = 0; i < _diameters.size(); i++) {
            if (_diameters[i] == 0) {
                toutlestuyaux = false;
                break;
            }
        }

        if (toutlestuyaux) {
            calcul();
        }
    }


}

int parcelle::getPosteDeCommande() const {
    return poste_de_commande;
}

int parcelle::getvraiindiceposte() {
    return poste_de_commande + _indexdebut;
}

bool parcelle::isCalcul() const {
    return _calcul;
}

const float pi = 3.14159265358979323846;

void parcelle::calcul_droit(float a, float b, double k) {
    const float PI = 3.14159265358979323846;
    int debut = poste_de_commande - _decalage;
    int fin_gauche = debut - 1;
    int fin_droit = _Donnees.size() - 1;
    float sigmadebit = 0, cumulperte = 0, cumulpiezo = 0;

    for (int i = fin_droit; i >= debut; --i) {
        if(i>_Donnees.size() && _Donnees[i].size() >=23){
            return;
        }
        float Dia = _Donnees[i][15]; // Diamètre en mm
        sigmadebit += _Donnees[i][9]; // Débit en l/h
        float L = _Donnees[i][amont ? 5 : 6]; // Longueur de la conduite en mètre

        // Calcul de la perte de charge
        float perte = k * std::pow(sigmadebit / 3600, a) * std::pow(Dia, b) * L;
        cumulperte += perte; // Cumul de la perte de charge

        float denivele = amont ? _Donnees[i][3] - _Donnees[std::max(0,fin_gauche)][3] : _Donnees[i][4] - _Donnees[std::max(0,fin_gauche)][4];

        float piezo = perte + (amont ? _Donnees[i][11] : _Donnees[i][12]);
        cumulpiezo += piezo;

        float debitM3S = sigmadebit / 3600 / 1000; // Conversion de l/h à m³/s
        float diametreM = Dia / 1000; // Conversion de mm à m
        float aire = PI * pow((diametreM / 2), 2); // Calcul de l'aire de la section de la conduite
        float vitesse = debitM3S / aire; // Calcul de la vitesse

        // Mise à jour des données de la conduite
        _Donnees[i][17] = denivele;
        _Donnees[i][18] = vitesse;
        _Donnees[i][19] = perte;
        _Donnees[i][20] = piezo;
        _Donnees[i][21] = cumulperte;
        _Donnees[i][22] = cumulpiezo;
    }
}


void parcelle::calcul_gauche(float a, float b, double k) {
    int debut = poste_de_commande;
    int fin_gauche = debut - 1;


    float sigmadebit = 0;
    float debit_gauche = 0;
    float piezo = 0;

    float cumulperte = 0;
    float cumulpiezo = 0;

    for (int i = 0; i < debut - _decalage; ++i) {
        if(i==-1 && _Donnees[i].size() >=23){
            return;
        }
        float Dia = _Donnees[i][15]; // Diamètre en mm
        sigmadebit += _Donnees[i][9]; // Débit en l/h
        float L = _Donnees[i][amont ? 5 : 6]; // Longueur de la conduite en mètre

        // Calcul de la perte de charge
        float perte = k * std::pow(sigmadebit / 3600, a) * std::pow(Dia, b) * L;
        cumulperte += perte; // Cumul de la perte de charge

        float denivele = amont ? _Donnees[i][3] - _Donnees[std::max(0,fin_gauche)][3] : _Donnees[i][4] - _Donnees[std::max(0,fin_gauche)][4];
        float piezo = perte + (amont ? _Donnees[i][11] : _Donnees[i][12]);
        cumulpiezo += piezo;

        float debitM3S = sigmadebit / 3600 / 1000; // Conversion de l/h à m³/s
        float diametreM = Dia / 1000; // Conversion de mm à m
        float aire = pi * pow((diametreM / 2), 2); // Calcul de l'aire de la section de la conduite
        float vitesse = debitM3S / aire; // Calcul de la vitesse

        // Mise à jour des données de la conduite
        _Donnees[i][17] = denivele;
        _Donnees[i][18] = vitesse;
        _Donnees[i][19] = perte;
        _Donnees[i][20] = piezo;
        _Donnees[i][21] = cumulperte;
        _Donnees[i][22] = cumulpiezo;
    }
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



void parcelle::calculpeigne(float a, float b, double k, float debitasp) {
    int debut = poste_de_commande - _decalage;
    int fin_droit = _Donnees.size() - 1;

    float perteg = 0;
    float sigmadebitg = 0;
    float intervalleg = 0;
    float perted = 0;
    float sigmadebitd = 0;
    float intervalled = 0;

    const float debit_scale = 1000.0f / 3600.0f;

    for (int i = 0; i < debut; ++i) {
        if(_Donnees.size() > i && _Donnees[i].size() >= 16){
            float interval = amont ? _Donnees[i][6] : _Donnees[i][5];
            float debit = _Donnees[i][2] * debitasp;

            if (_Donnees[i][2] != 0) {
                float Dia = _Donnees[i][15]; // Diamètre en mm
                sigmadebitg += debit; // Débit en M3/h
                float perte = k * std::pow(sigmadebitg * debit_scale, a) * std::pow(Dia, b) * interval;
                perteg += perte;
            }
            intervalleg += interval;
        }
        else{
            // Gestion d'erreur si les données ne sont pas disponibles
        }
    }

    for (int i = fin_droit; i >= debut; --i) {
        if(_Donnees.size() > i && _Donnees[i].size() >= 16){
            float interval = amont ? _Donnees[i][6] : _Donnees[i][5];
            float debit = _Donnees[i][2] * debitasp;

            if (_Donnees[i][2] != 0) {
                float Dia = _Donnees[i][15];    // Diamètre en mm
                sigmadebitd += debit; // Débit en l/h

                // Calcul de la perte de charge
                float perte = k * std::pow(sigmadebitd * debit_scale, a) * std::pow(Dia, b) * interval;
                perted += perte; // Cumul de la perte de charge
            }
            intervalled += interval;
        }
        else{
            // Gestion d'erreur si les données ne sont pas disponibles
        }
    }

}


void parcelle::calculaspersseurs(std::vector<int> &indices, float a, float b, double k) {

    for(auto &it : _Donnees){
        if(it[2]==0){
            it[15] = 0; // On suprimme les diametres où il n'y a pas d'asperseurs
        }
    }

    int debut = poste_de_commande - _decalage;
    if (debut < 0) {
        return;
    }

    if(aspdebit!=0 && aspinterdebut!=0){
        _debit = 0;
        for (const auto &it: indices) {
            _debit += aspdebit * _Donnees[it][2];
        }

        _debit *= 1000;

        float debitConverti = (aspdebit * 1000) / 3600;

        milieuhydro = trouvemilieuhydroasp();

        std::sort(indices.begin(), indices.end());

        calcul_gauche_aspersseurs(indices, debitConverti, a, b, k);
        calcul_droit_aspersseurs(indices, debitConverti, a, b, k);
        return;
    }

    // Création de la boîte de dialogue
    QDialog dialog(nullptr);
    QFormLayout form(&dialog);

    QLineEdit *lineEditDebit = new QLineEdit(&dialog);
    lineEditDebit->setText(QString::number(0));
    form.addRow("Débit en m³/h:", lineEditDebit);


    QLineEdit *lineEditDistAsperseurs = new QLineEdit(&dialog);
    lineEditDistAsperseurs->setText(QString::number(0));
    form.addRow("Distance entre asperseurs:", lineEditDistAsperseurs);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    if (dialog.exec() == QDialog::Accepted) {
        aspdebit = lineEditDebit->text().toFloat();
        aspinterdebut = lineEditDistAsperseurs->text().toFloat();

        _debit = 0;
        for (const auto &it: indices) {
            _debit += aspdebit * _Donnees[it][2];
        }

        _debit *= 1000;

        float debitConverti = (aspdebit * 1000) / 3600;

        milieuhydro = trouvemilieuhydroasp();

        std::sort(indices.begin(), indices.end());

        calcul_gauche_aspersseurs(indices, debitConverti, a, b, k);
        calcul_droit_aspersseurs(indices, debitConverti, a, b, k);

    }
}



void parcelle::SetAmont(bool tamont) {
    amont = tamont;
}

void parcelle::inverser() {

    // Inverser les lignes
    std::reverse(_Donnees.begin(), _Donnees.end());

    // Réinitialiser les valeurs après l'indice 8
    for (auto &row: _Donnees) {
        for (int i = 10; i < row.size(); ++i) {
            row[i] = 0.0f;  // Vous pouvez ajuster cette valeur en fonction de ce que vous voulez pour la réinitialisation
        }
    }

    float debitcumule = 0;

    float zamont;
    float zaval;

    float zamontO = _Donnees[0][3];
    float zavalO = _Donnees[0][4];

    for (int i = 0; i < _Donnees.size(); i++) {

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

void parcelle::recalcul() {

    float debitcumule = 0;

    float zamont;
    float zaval;

    float zamontO = _Donnees[0][3];
    float zavalO = _Donnees[0][4];

    for (int i = 0; i < _Donnees.size(); i++) {

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

int parcelle::trouvemilieuhydroasp() {
    // Vérifiez que _Donnees n'est pas vide
    if (_Donnees.empty()) {
        return 0;
    }

    // Diviser le débit total par deux en avance
    float debitMoitie = _debit / 2;

    // Chercher l'indice où le débit cumulé dépasse la moitié du débit cumulé total
    float cumulatedDebit = 0;
    for (int i = 0; i < _Donnees.size(); ++i) {
        const auto &donnee = _Donnees[i];
        if (donnee.size() < 11) {
            continue;
        }
        cumulatedDebit += donnee[2] * (aspdebit * 1000);
        if (cumulatedDebit >= debitMoitie) {
            return i;
        }
    }

    return 0;

}


void parcelle::choisirCote(int a) {
    _decalage = a;
}


#include <unordered_set>

void parcelle::calcul_gauche_aspersseurs(std::vector<int> &indices, float debit, float a, float b, double k) {
    const int debut = poste_de_commande - _decalage;
    std::unordered_set<int> indicesSet(indices.begin(), indices.end());

    std::vector<float> intervalles;
    float interval = 0;

    for (int i = debut - 1; i >= 0; i--) {
        interval += amont ? _Donnees[i][5] : _Donnees[i][6];
        if (indicesSet.count(i)) {
            intervalles.push_back(interval);
            interval = 0;
        }
    }

    float sigmadebit = 0;
    float cumulperte = 0;
    float cumulpiezo = 0;
    int compteur = intervalles.size() - 1;

    const float const_debitM3S = 1.0f / 3600 / 1000;
    const float const_pi = 3.14159265358979323846f;

    // Calcul de la perte de charge, de la vitesse et du dénivelé du côté gauche du poste de commande
    for (int i = 0; i < debut; ++i) {
        if (indicesSet.count(i) && _Donnees[i].size() >= 23 ) {

            const float Dia = _Donnees[i][15]; // Diamètre en mm
            sigmadebit += _Donnees[i][2] * debit; // Débit en l/s
            const float L = intervalles[compteur--]; // Longueur de la conduite en mètre

            // Calcul de la perte de charge
            const float perte = k * std::pow(sigmadebit, a) * std::pow(Dia, b) * L;

            int next_idx = *std::upper_bound(indices.begin(), indices.end(), i);

            if(next_idx>poste_de_commande)
                next_idx = poste_de_commande-1;

            const float denivele = amont ? _Donnees[next_idx][3] - _Donnees[i][3]
                                         : _Donnees[next_idx][4] - _Donnees[i][4];

            const float piezo = perte + (-denivele);
            cumulperte += perte; // Cumul de la perte de charge
            cumulpiezo += piezo;

            const float debitM3S = sigmadebit / 1000;
            const float diametreM = Dia / 1000;
            const float aire = const_pi * std::pow((diametreM / 2), 2);
            const float vitesse = debitM3S / aire;

            _Donnees[i][17] = -denivele;
            _Donnees[i][18] = vitesse;
            _Donnees[i][19] = perte;
            _Donnees[i][20] = piezo;
            _Donnees[i][21] = cumulperte;
            _Donnees[i][22] = cumulpiezo;
        } else {
            _Donnees[i][17] = 0;
            _Donnees[i][18] = 0;
            _Donnees[i][19] = 0;
            _Donnees[i][20] = 0;
            _Donnees[i][21] = 0;
            _Donnees[i][22] = 0;
        }
    }
}

void parcelle::herse(int ligne) {

    hersealim->refresh();
    int index = ligne;

    if (ligne >= _Donnees[0][0]) {
        index = ligne - _indexdebut;
    }

    if (_Donnees[index][2] <= 0) {
        return;
    }

    float denivele = 0;

    if (amont) {
        denivele = _Donnees[index][4] - _Donnees[index][3];
    } else {
        denivele = _Donnees[index][3] - _Donnees[index][4];
    }

    // Permet de lancer un calcul de herse sur la ligne avec un diametr" 55.4 de rentrer ainsi que le débit de la ligne
    hersealim->importdonees(aspdebit, 55.4, aspinterdebut, _Donnees[index], denivele);
    hersealim->show();
}


void parcelle::calcul_droit_aspersseurs(std::vector<int> &indices, float debit, float a, float b, double k) {
    const int debut = poste_de_commande - _decalage ;
    const int fin_droit = _Donnees.size() - 1;

    std::unordered_set<int> indicesSet(indices.begin(), indices.end());
    std::vector<float> intervalles;
    float interval = 0;

    for (int i = debut; i <= fin_droit; i++) {
        interval += amont ? _Donnees[i][5] : _Donnees[i][6];
        if (indicesSet.count(i)) {
            intervalles.push_back(interval);
            interval = 0;
        }
    }

    std::vector<int> reverse = indices;
    std::reverse(reverse.begin(), reverse.end());

    float sigmadebit = 0;
    float cumulperte = 0;
    float cumulpiezo = 0;
    int compteur = intervalles.size() - 1;

    const float const_debitM3S = 1.0f / 3600 * 1000;
    const float const_pi = 3.14159265358979323846f;

    // Calcul de la perte de charge, de la vitesse et du dénivelé du côté droit du poste de commande
    for (int i = fin_droit; i >= debut; --i) {
        if (indicesSet.count(i) && _Donnees[i].size() >= 23) {

            const float Dia = _Donnees[i][15]; // Diamètre en mm
            sigmadebit += _Donnees[i][2] * debit; // Débit en l/s
            const float L = intervalles[compteur--]; // Longueur de la conduite en mètre

            // Calcul de la perte de charge
            const float perte = k * std::pow(sigmadebit, a) * std::pow(Dia, b) * L;
            const float debitM3S = sigmadebit / 1000;
            const float diametreM = Dia / 1000;
            const float aire = const_pi * std::pow((diametreM / 2), 2);
            const float vitesse = debitM3S / aire;

            int nextIndex = *std::upper_bound(reverse.begin(), reverse.end(), i, std::greater<int>());

            if(nextIndex<poste_de_commande)
                nextIndex = poste_de_commande-1;

            const float denivele = amont ? _Donnees[i][3] - _Donnees[nextIndex][3]
                                         : _Donnees[i][4] - _Donnees[nextIndex][4];

            const float piezo = perte + (denivele);
            cumulperte += perte; // Cumul de la perte de charge
            cumulpiezo += piezo;

            _Donnees[i][17] = denivele;
            _Donnees[i][18] = vitesse;
            _Donnees[i][19] = perte;
            _Donnees[i][20] = piezo;
            _Donnees[i][21] = cumulperte;
            _Donnees[i][22] = cumulpiezo;
        } else {
            _Donnees[i][17] = 0;
            _Donnees[i][18] = 0;
            _Donnees[i][19] = 0;
            _Donnees[i][20] = 0;
            _Donnees[i][21] = 0;
            _Donnees[i][22] = 0;
        }
    }
}



std::string parcelle::toString() const {
    std::string s;

    for (const auto &row: _Donnees) {
        for (const auto &value: row) {
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

void parcelle::fromString(const std::string &s, std::shared_ptr<bdd> db) {
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

    // Chargement des autre données
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


float parcelle::getAspinterdebut() const {
    return aspinterdebut;
}

void parcelle::setAspinterdebut(float aspinterdebut) {
    parcelle::aspinterdebut = aspinterdebut;
}


int parcelle::nbasp() { // Compte le nombre d'asperseurs
    int nbasp = 0;
    for (auto &i: _Donnees) {
        nbasp += i[2];
    }
    return nbasp;
}

void parcelle::doubledebit(float multi){
    for(auto &datas : _Donnees){
        datas[9] *=multi;
    }
    recalcul();
    trouvemilieuhydro();
}

void parcelle::placerarrosseurs(int ligne, int nombre){
    if(nombre < 0)
        return;
    if(ligne-_indexdebut-1 > _Donnees.size()-1)
        return;

    _Donnees[ligne-1-_indexdebut][2] = nombre;
}

#include <map>

float parcelle::hectare(){
    std::map<float, int> freq;
    float max_val = 0;
    int max_count = -1;

    // find the most frequent value in _Donnees[i][amont ? 5 : 6]
    for(int i=0;i<_Donnees.size();i++){
        float val = _Donnees[i][amont ? 5 : 6];
        freq[val]++;

        if (freq[val] > max_count) {
            max_val = val;
            max_count = freq[val];
        }
    }

    // use the most frequent value for calculation
    float hectare = 0;
    for(int i=0;i<_Donnees.size();i++){
        hectare += _Donnees[i][1]*max_val;
    }

    return hectare/10000;
}



void parcelle::optimize_diameters_droit(float a, float b, double k) {
    const float PI = 3.14159265358979323846;
    int debut = poste_de_commande - _decalage;
    int fin_droit = _Donnees.size() - 1;
    float sigmadebit = 0.0f;

    std::vector<float> diametresDisponibles = {21.0f, 28.0f, 35.2f, 44.0f, 55.4f, 66.0f, 79.20f};

    int count = 0;
    for (int index = debut; index <= fin_droit; index++) {
        if (index >= _Donnees.size() || _Donnees[index].size() < 23){
            break;
        }
        ++count;
    }

    float limite_cumul_piezo = 2;
    float denivele_total_peigne = amont ? _Donnees[fin_droit][3] - _Donnees[debut][3] : _Donnees[fin_droit][4] - _Donnees[debut][4];
    float limite_cumul_perte = limite_cumul_piezo - denivele_total_peigne;
    float limite_perte_rang = limite_cumul_perte / count;

    for (int i = fin_droit; i >= debut; --i) {
        if(i >= _Donnees.size() && _Donnees[i].size() < 23){
            return;
        }

        float bestDiameter = _Donnees[i][15];
        float bestVitesse = std::numeric_limits<float>::max();
        float bestPerte = -1;

        sigmadebit += _Donnees[i][9]; // Débit en l/h

        for (float dia : diametresDisponibles) {
            float vitesse = calcul_vitesse(dia, sigmadebit, a, b, k);
            float perte = k * std::pow(sigmadebit/3600 , a) * std::pow(dia, b);

            float difference = limite_perte_rang - perte;

            if (vitesse < bestVitesse && vitesse < 2.0f && difference >= 0 && (bestPerte == -1 || difference < (limite_perte_rang - bestPerte)) && perte <limite_perte_rang) {
                bestDiameter = dia;
                bestVitesse = vitesse;
                bestPerte = perte;
            }
        }

        _Donnees[i][15] = bestDiameter;
    }

    calcul_droit(a,b,k);
}

float parcelle::calcul_vitesse(float dia, float sigmadebit, float a, float b, double k) {
    const float PI = 3.14159265358979323846;
    float debitM3S = sigmadebit / 3600 / 1000; // Conversion de l/h à m³/s
    float diametreM = dia / 1000; // Conversion de mm à m
    float aire = PI * pow((diametreM / 2), 2); // Calcul de l'aire de la section de la conduite
    float vitesse = debitM3S / aire; // Calcul de la vitesse
    return vitesse;
}

void parcelle::optimize_gauche_aspersseurs(std::vector<int> &indices, float debit, float a, float b, double k) {
    const int debut = poste_de_commande - _decalage;
    int fin_gauche = debut - 1;

    std::unordered_set<int> indicesSet(indices.begin(), indices.end());

    float sigmadebit = 0;

    std::vector<float> diametresDisponibles = {19.0f, 26.0f, 34.0f, 44.0f, 57.0f, 69.0f, 83.0f, 101.4f, 116.2f, 130.2f, 148.8f};

    int count = 0;
    for (int index : indices) {
        if (index >= 0 && index < fin_gauche) {
            ++count;
        }
    }


    float limite_cumul_piezo = 2;
    float denivele_total_peigne = amont ? _Donnees[indices[0]][3] - _Donnees[fin_gauche][3] : _Donnees[0][4] - _Donnees[fin_gauche][4];
    float limite_cumul_perte = limite_cumul_piezo - denivele_total_peigne;
    float limite_perte_rang = limite_cumul_perte / count;

    std::cout<<limite_perte_rang<<std::endl;

    std::vector<float> intervalles;
    float interval = 0;
    for (int i = debut - 1; i >= 0; i--) {
        interval += amont ? _Donnees[i][5] : _Donnees[i][6];
        if (indicesSet.count(i)) {
            intervalles.push_back(interval);
            interval = 0;
        }
    }

    int compteur = intervalles.size() - 1;


    for (int i = 0; i < debut; ++i) {
        if (indicesSet.count(i) && _Donnees[i].size() >= 23 ) {
            sigmadebit += _Donnees[i][2] * debit; // Débit en l/s

            float bestDiameter = _Donnees[i][15];
            float bestVitesse = std::numeric_limits<float>::max();
            float bestPerte = -1; // initialize bestPerte to -1
            const float L = intervalles[compteur--]; // Longueur de la conduite en mètre


            for (float dia : diametresDisponibles) {
                float vitesse = calcul_vitesse(dia, sigmadebit*3600, a, b, k);
                float perte = k * std::pow(sigmadebit , a) * std::pow(dia, b) * L;

                float difference = limite_perte_rang - perte;

                // Now the condition checks if the difference is smaller and positive (i.e., perte is closer to the limit but not over it)
                if (vitesse <= 2.0f && difference >= 0 && (bestPerte == -1 || difference < (limite_perte_rang - bestPerte))) {
                    bestPerte = perte;
                    bestDiameter = dia;
                    bestVitesse = vitesse;
                }


            }

            // Après avoir parcouru tous les diamètres disponibles, on fixe le diamètre de la conduite à la meilleure valeur trouvée.
            _Donnees[i][15] = bestDiameter;
            _Donnees[i][18] = bestVitesse;
        }
    }
    calcul_gauche_aspersseurs(indices,debit,a,b,k);
}




void parcelle::optimize_droit_aspersseurs(std::vector<int> &indices, float debit, float a, float b, double k) {
    const int debut = poste_de_commande - _decalage ;
    const int fin_droit = _Donnees.size() - 1;

    std::unordered_set<int> indicesSet(indices.begin(), indices.end());

    float sigmadebit = 0;

    std::vector<float> diametresDisponibles = {19.0f, 26.0f, 34.0f, 44.0f, 57.0f, 69.0f, 83.0f, 101.4f, 116.2f, 130.2f, 148.8f};

    int count = 0;
    for (int index : indices) {
        if (index >= debut && index <= fin_droit) {
            ++count;
        }
    }

    float limite_cumul_piezo = 2;
    float denivele_total_peigne = amont ? _Donnees[indices.back()][3] - _Donnees[debut][3] : _Donnees[fin_droit][4] - _Donnees[debut][4];
    float limite_cumul_perte = limite_cumul_piezo - denivele_total_peigne;
    float limite_perte_rang = limite_cumul_perte / count;

    std::cout<<limite_perte_rang<<std::endl;

    std::vector<float> intervalles;
    float interval = 0;
    for (int i = debut + 1; i <= fin_droit; i++) {
        interval += amont ? _Donnees[i][5] : _Donnees[i][6];
        if (indicesSet.count(i)) {
            intervalles.push_back(interval);
            interval = 0;
        }
    }

    int compteur = 0;

    for (int i = fin_droit; i >= debut; --i) {
        if (indicesSet.count(i) && _Donnees[i].size() >= 23 ) {
            sigmadebit += _Donnees[i][2] * debit; // Débit en l/s

            float bestDiameter = _Donnees[i][15];
            float bestVitesse = std::numeric_limits<float>::max();
            float bestPerte = -1;
            const float L = intervalles[compteur++];

            for (float dia : diametresDisponibles) {
                float vitesse = calcul_vitesse(dia, sigmadebit*3600, a, b, k);
                float perte = k * std::pow(sigmadebit , a) * std::pow(dia, b) * L;

                float difference = limite_perte_rang - perte;

                if (vitesse <= 2.0f && difference >= 0 && (bestPerte == -1 || difference < (limite_perte_rang - bestPerte))) {
                    bestPerte = perte;
                    bestDiameter = dia;
                    bestVitesse = vitesse;
                }
            }

            _Donnees[i][15] = bestDiameter;
            _Donnees[i][18] = bestVitesse;
        }
    }
    calcul_droit_aspersseurs(indices,debit,a,b,k);
}




void parcelle::optimize_diameters_gauche(float a, float b, double k) {
    const float PI = 3.14159265358979323846;
    int debut = poste_de_commande - _decalage;
    int fin_droit = _Donnees.size() - 1;
    float sigmadebit = 0.0f;

    std::vector<float> diametresDisponibles = {21.0f, 28.0f, 35.2f, 44.0f, 55.4f, 66.0f, 79.20f};

    int count = 0;
    for (int index = 0; index < debut; index++) {
        if (index >= _Donnees.size() || _Donnees[index].size() < 23){
            break;
        }
        ++count;
    }

    float limite_cumul_piezo = 2;
    float denivele_total_peigne = amont ? _Donnees[0][3] - _Donnees[debut][3] : _Donnees[0][4] - _Donnees[debut][4];
    float limite_cumul_perte = limite_cumul_piezo - denivele_total_peigne;
    float limite_perte_rang = limite_cumul_perte / count;

    for (int i = 0; i < debut - _decalage; i++) {
        if(i > _Donnees.size() && _Donnees[i].size() >= 23){
            return;
        }

        float bestDiameter = _Donnees[i][15];
        float bestVitesse = std::numeric_limits<float>::max();
        float bestPerte = -1;

        sigmadebit += _Donnees[i][9]; // Débit en l/h

        for (float dia : diametresDisponibles) {
            float vitesse = calcul_vitesse(dia, sigmadebit, a, b, k);
            float perte = k * std::pow(sigmadebit/3600 , a) * std::pow(dia, b);

            float difference = limite_perte_rang - perte;

            if (vitesse < bestVitesse && vitesse < 2.0f && difference >= 0 && (bestPerte == -1 || difference < (limite_perte_rang - bestPerte))) {
                bestDiameter = dia;
                bestVitesse = vitesse;
                bestPerte = perte;
                break;
            }
        }

        _Donnees[i][15] = bestDiameter;
    }

    calcul_gauche(a,b,k);
}


