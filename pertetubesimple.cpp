//
// Created by qraim on 04/04/23.
//

#include "pertetubesimple.h"
#include <cmath>
#include <memory>


pertetubesimple::pertetubesimple(std::shared_ptr<bdd> db, QWidget *parent)
        : QWidget(parent), database(db) {

  setWindowTitle(QString::fromStdString("Tube simple"));
  setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");


    std::vector<std::string> matiere_names = database->getAllMatiereNames();
    for (const auto& matiere_name : matiere_names) {
        materiau.addItem(QString::fromStdString(matiere_name));
    }

    unite = new QComboBox(this);
    unite->addItem("m3/h");
    unite->addItem("l/h");
    unite->addItem("l/s");

    // Créé la grille
    QGridLayout *gridLayout = new QGridLayout;

    QLabel *titleLabel = new QLabel("Perte de charge en tube simple", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(titleLabel, 0, 0, 1, 5);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setColumnStretch(3, 1);
    gridLayout->setColumnStretch(4, 1);

    // On ajoute les éléments à leur place dans la grille
    gridLayout->addWidget(new QLabel("Materiau", this), 1, 0);
    gridLayout->addWidget(&materiau, 2, 0);
    gridLayout->addWidget(new QLabel("Debit", this), 1, 1);
    debit.setFixedWidth(110);
    gridLayout->addWidget(&debit, 2, 1);
    unite->setFixedWidth(90);
    gridLayout->addWidget(unite, 2, 2);
    gridLayout->addWidget(new QLabel("Diametre (mm)", this), 1, 3);
    diametre.setFixedWidth(120);
    gridLayout->addWidget(&diametre, 2, 3);
    gridLayout->addWidget(new QLabel("Longueur (m)", this), 1, 4);
    longueur.setFixedWidth(110);
    gridLayout->addWidget(&longueur, 2, 4);
    gridLayout->addWidget(new QLabel("Denivele (m)", this), 1, 5);
    denivele.setFixedWidth(110);
    gridLayout->addWidget(&denivele, 2, 5);

    QPushButton *dia = new QPushButton("Diametre");
    gridLayout->addWidget(dia, 0, 5);

    // La derniere ligne pour les résultats
    QHBoxLayout *resultLabelsLayout = new QHBoxLayout;
    QLabel *perteLabel = new QLabel("Pertes de charge (m)", this);
    QLabel *piezoLabel = new QLabel("Variation Piezo (m)", this);
    QLabel *vitesseLabel = new QLabel("Vitesse (m/s)", this);
    resultLabelsLayout->addWidget(perteLabel);
    resultLabelsLayout->addWidget(piezoLabel);
    resultLabelsLayout->addWidget(vitesseLabel);


    QHBoxLayout *resultFieldsLayout = new QHBoxLayout;
    Perte.setReadOnly(true);
    Piezo.setReadOnly(true);
    Vitesse.setReadOnly(true);
    resultFieldsLayout->addWidget(&Perte);
    resultFieldsLayout->addWidget(&Piezo);
    resultFieldsLayout->addWidget(&Vitesse);

    // Ajoute des champs non écrivable
    std::unique_ptr<QHBoxLayout> nonEditableFieldsLayout = std::make_unique<QHBoxLayout>();

    // Ajoute le bouton Calculer
    Calcul.setText("Calculer");
    QHBoxLayout *calculerLayout = new QHBoxLayout;
    Calcul.setFixedSize(120, 30);
    calculerLayout->addStretch(1);
    calculerLayout->addWidget(&Calcul);
    calculerLayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(gridLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(calculerLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(resultLabelsLayout);
    mainLayout->addLayout(resultFieldsLayout);


    // Fixe l'interface globale
    setLayout(mainLayout);

    Calcul.setDisabled(true);
    debit.hasFocus();

    connect(&Calcul, &QPushButton::clicked, this, &pertetubesimple::calculer);
    connect(&debit, &QLineEdit::textChanged, this, &pertetubesimple::checkInputs);
    connect(&diametre, &QLineEdit::textChanged, this, &pertetubesimple::checkInputs);
    connect(&longueur, &QLineEdit::textChanged, this, &pertetubesimple::checkInputs);
    connect(&denivele, &QLineEdit::textChanged, this, &pertetubesimple::checkInputs);
    connect(&materiau, &QComboBox::currentTextChanged, this, &pertetubesimple::checkInputs);
    connect(dia, &QPushButton::clicked, this, &pertetubesimple::showdia);


    QDoubleValidator *debitValidator = new QDoubleValidator(0.01, 1000000, 2, this);
    debit.setValidator(debitValidator);

    QDoubleValidator *diametreValidator = new QDoubleValidator(0.01, 1000000, 2, this);
    diametre.setValidator(diametreValidator);

    QDoubleValidator *longueurValidator = new QDoubleValidator(0.01, 1000000, 2, this);
    longueur.setValidator(longueurValidator);

    QDoubleValidator *deniveleValidator = new QDoubleValidator(0.01, 1000000, 2, this);
    denivele.setValidator(deniveleValidator);

}

pertetubesimple::~pertetubesimple() {
    delete debit.validator();
    delete diametre.validator();
    delete longueur.validator();
    delete denivele.validator();
}


void pertetubesimple::keyPressEvent(QKeyEvent *event) {

    // Si la touche "Entrée" est appuyée
    if(event->key() == Qt::Key_Return){

        // Si le bouton Calcul est actif, on clique dessus
        if(Calcul.isEnabled()){
            Calcul.click();
        }
            // Sinon, on passe à l'input suivant
        else {
            focusNextInput();
        }
    }
        // Si la touche "Ctrl" est appuyée, on passe à l'input précédent
    else if (event->key() == Qt::Key_Control) {
        focusPreviousInput();
        return;
    }
        // Si la touche "Tab" est appuyée, on passe à l'input suivant
    else if (event->key() == Qt::Key_Tab) {
        focusNextInput();
        return;
    }
        // Si la touche "Entrée" avec le modificateur est appuyée
    else if (event->modifiers() & Qt::Key_Return) {

        // Si le bouton Calcul est actif, on calcule les résultats
        if (Calcul.isEnabled()) {
            calculer();
            return;
        }
    }
}



void pertetubesimple::focusPreviousInput() {

    if (denivele.hasFocus()) {
        longueur.setFocus();
        return;
    } else if (longueur.hasFocus()) {
        diametre.setFocus();
        return;
    } else if (diametre.hasFocus()){
        debit.setFocus();
    }
}

void pertetubesimple::focusNextInput() {

    if (debit.hasFocus() ) {
        diametre.setFocus();
        return;
    } else if (diametre.hasFocus()) {
        longueur.setFocus();
        return;
    } else if (longueur.hasFocus()) {
        denivele.setFocus();
    }

}

void pertetubesimple::calculer() {

    int index = unite->currentIndex();
    float flowRate = 0;
    float dLS=0;

    
    QString debitText = debit.text();
    debitText.replace(',', '.');
    float D = debitText.toFloat();

    QString espacementText = diametre.text();
    espacementText.replace(',', '.');
    float Dia = espacementText.toFloat();

    QString diametreText = longueur.text();
    diametreText.replace(',', '.');
    float L = diametreText.toFloat();

    if(index==0){
        flowRate = D / 3600; // Convertit m³/h en m³/s
        dLS = D /3600 * 1000;
    }else if(index==1){
        flowRate = D/(1000*3600);
        dLS = D/3600;
    } else {
        flowRate = D/1000;
        dLS = D;
    }

    float deniveles = 0;
    double  k =0;
    float a=0;
    float b = 0 ;

    // Si l'input de dénivelé n'est pas vide, on le convertit en float et on le stocke dans la variable deniveles
    if(!denivele.text().isEmpty()){
        deniveles = denivele.text().toFloat();
    }

    float pipeDiameter = Dia / 1000; // Convertit mm en m

    float pipeArea = M_PI * pow(pipeDiameter / 2, 2); // Calcule l'aire de la section transversale du tuyau en m²
    float v = flowRate / pipeArea; // Calcule la vitesse d'écoulement en m/s

    std::tuple<float, float, double> coefficients = database->get_material_coefficients(materiau.currentText().toStdString());
    a = std::get<0>(coefficients);
    b = std::get<1>(coefficients);
    k = std::get<2>(coefficients);


    float pertecharge = k * pow(dLS, a) * pow(Dia, b) * L; // Calcule la perte de charge en Pa
    float variation = pertecharge+deniveles; // Calcule la variation de charge en Pa


    QString str = "";
    if (v > 2) { // Si la vitesse d'écoulement est supérieure à 2 m/s, on change la couleur de la QLineEdit "Vitesse" en rouge
        Vitesse.setStyleSheet("color: red;  background-color: white ");
        str = QString::number(v, 'f', 2);
    } else { // Sinon, on enlève la couleur rouge
        Vitesse.setStyleSheet("");
        str = QString::number(v, 'f', 2);
    }

    debit.clear();
    diametre.clear();
    longueur.clear();
    denivele.clear();

    Perte.setStyleSheet("color : white;");
    Vitesse.setText(str); // Met à jour le texte de la QLineEdit "Vitesse"
    Perte.setText(QString::number(pertecharge, 'f', 2)); // Met à jour le texte de la QLineEdit "Perte"
    Piezo.setText(QString::number(variation, 'f', 2)); // Met à jour le texte de la QLineEdit "Piezo"

}



// Cette fonction permet de vérifier que les champs d'entrée sont correctement remplis
void pertetubesimple::checkInputs() {

    // On réinitialise la couleur des champs d'entrée
    debit.setStyleSheet("");
    diametre.setStyleSheet("");
    longueur.setStyleSheet("");

    QString debitText = debit.text();
    debitText.replace(',', '.');
    float D = debitText.toFloat();

    QString espacementText = diametre.text();
    espacementText.replace(',', '.');
    float Dia = espacementText.toFloat();

    QString diametreText = longueur.text();
    diametreText.replace(',', '.');
    float L = diametreText.toFloat();

    // On vérifie si les valeurs des champs d'entrée sont correctes
    bool deb = D<=0;
    bool diam = Dia<=0;
    bool Long = L<=0;

    // Si les valeurs ne sont pas correctes, on met la couleur de fond en rouge
    if(deb)
    {
        debit.setStyleSheet("color: red;  background-color: white ");
    }
    if (diam)
    {
        diametre.setStyleSheet("color: red;  background-color: white ");
    }
    if (Long)
    {
        longueur.setStyleSheet("color: red;  background-color: white ");
    }

    // On vérifie si tous les champs sont remplis
    if (debit.text().isEmpty() || diametre.text().isEmpty() || longueur.text().isEmpty() ) {
        clearresult(); // On efface les résultats précédents
        Calcul.setDisabled(true); // On désactive le bouton de calcul
        return;
    } else if(!deb && !diam && !Long){
        Calcul.setDisabled(false); // On active le bouton de calcul
    }

}

void pertetubesimple::clearresult(){
    Vitesse.setText("");
    Piezo.setText("");
    Perte.setText("");
}


void pertetubesimple::showdia(){
    database->afficher_tableaux();
}