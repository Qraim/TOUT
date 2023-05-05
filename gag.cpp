//
// Created by qraim on 07/04/23.
//

#include <QDateEdit>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include "gag.h"


gag::gag(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent), database(db){
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setWindowTitle(QString::fromStdString("Goutte à goutte"));
    setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    // Top layout
    QGridLayout *topLayout = new QGridLayout;
    topLayout->setSpacing(5);
    topLayout->setAlignment(Qt::AlignCenter);
    topLayout->setColumnStretch(10, 1);


    // Ajout des champs d'entrée et des étiquettes au QGridLayout fixeInputsLayout
    Materiau = new QComboBox(this);
    Materiau->setFixedSize(75, 25);

    std::vector<std::string> matiere_names = database->getAllMatiereNames();
    for (const auto& matiere_name : matiere_names) {
        Materiau->addItem(QString::fromStdString(matiere_name));
    }


    // QlineEdit et labels pour la section du haut
    Debit = new QLineEdit;
    Espacement = new QLineEdit;
    Diametre = new QLineEdit;
    Longueur = new QLineEdit;
    Hauteur = new QLineEdit;


    QLocale customLocale = QLocale::French;

    // On ajoute des validator pour restreindre l'input à un nombre
    QDoubleValidator *debitValidator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 2, this);
    debitValidator->setLocale(customLocale);
    Debit->setValidator(debitValidator);

    QDoubleValidator *espacementValidator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 2, this);
    espacementValidator->setLocale(customLocale);
    Espacement->setValidator(espacementValidator);

    QDoubleValidator *diametreValidator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 2, this);
    diametreValidator->setLocale(customLocale);
    Diametre->setValidator(diametreValidator);

    QDoubleValidator *longueurValidator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 2, this);
    longueurValidator->setLocale(customLocale);
    Longueur->setValidator(longueurValidator);

    QDoubleValidator *hauteurValidator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 2, this);
    hauteurValidator->setLocale(customLocale);
    Hauteur->setValidator(hauteurValidator);



    // Ajout des labels en dessous des inputs
    QLabel *labelNumero = new QLabel("Numéro");
    QLabel *labelDebit2 = new QLabel("Debit");
    QLabel *labelEspacement2 = new QLabel("Espacement");
    QLabel *labelDiametre2 = new QLabel("Diametre");
    QLabel *labelLongueur2 = new QLabel("Longueur");
    QLabel *labelHauteur2 = new QLabel("Hauteur");
    QLabel *labelPerte = new QLabel("J");
    QLabel *labelPiezo = new QLabel("Piezo");
    QLabel *labelSigmaPerte = new QLabel("ΣJ");
    QLabel *labelSigmaPiezo = new QLabel("ΣPiezo");

    topLayout->addWidget(labelNumero, 4, 0, Qt::AlignCenter);
    topLayout->addWidget(labelDebit2, 4, 1, Qt::AlignCenter);
    topLayout->addWidget(labelEspacement2, 4, 2, Qt::AlignCenter);
    topLayout->addWidget(labelDiametre2, 4, 3, Qt::AlignCenter);
    topLayout->addWidget(labelLongueur2, 4, 4, Qt::AlignCenter);
    topLayout->addWidget(labelHauteur2, 4, 5, Qt::AlignCenter);
    topLayout->addWidget(labelPerte, 4, 6, Qt::AlignCenter);
    topLayout->addWidget(labelPiezo, 4, 7, Qt::AlignCenter);
    topLayout->addWidget(labelSigmaPerte,4, 8, Qt::AlignCenter);
    topLayout->addWidget(labelSigmaPiezo,4, 9, Qt::AlignCenter);
    topLayout->addWidget(Materiau, 6, 0, Qt::AlignCenter);



    labelNumero->setAlignment(Qt::AlignCenter);
    labelDebit2->setAlignment(Qt::AlignCenter);
    labelEspacement2->setAlignment(Qt::AlignCenter);
    labelDiametre2->setAlignment(Qt::AlignCenter);
    labelLongueur2->setAlignment(Qt::AlignCenter);
    labelHauteur2->setAlignment(Qt::AlignCenter);
    labelPerte->setAlignment(Qt::AlignCenter);
    labelPiezo->setAlignment(Qt::AlignCenter);
    labelSigmaPerte->setAlignment(Qt::AlignCenter);
    labelSigmaPiezo->setAlignment(Qt::AlignCenter);


    Diametre->setAlignment(Qt::AlignCenter);
    Debit->setAlignment(Qt::AlignCenter);
    Espacement->setAlignment(Qt::AlignCenter);
    Longueur->setAlignment(Qt::AlignCenter);
    Hauteur->setAlignment(Qt::AlignCenter);

    // Ajout de label pour les unités

    QLabel *unitDebit2 = new QLabel("l/h");
    QLabel *unitEspacement2 = new QLabel("m");
    QLabel *unitDiametre2 = new QLabel("mm");
    QLabel *unitLongueur2 = new QLabel("m");
    QLabel *unitHauteur2 = new QLabel("m");
    QLabel *unitPerte = new QLabel("m");
    QLabel *unitPiezo = new QLabel("m");
    QLabel *unitSigmaPerte = new QLabel("m");
    QLabel *unitSigmaPiezo = new QLabel("m");

    unitDebit2->setAlignment(Qt::AlignCenter);
    unitEspacement2->setAlignment(Qt::AlignCenter);
    unitDiametre2->setAlignment(Qt::AlignCenter);
    unitLongueur2->setAlignment(Qt::AlignCenter);
    unitHauteur2->setAlignment(Qt::AlignCenter);
    unitPerte->setAlignment(Qt::AlignCenter);
    unitPiezo->setAlignment(Qt::AlignCenter);
    unitSigmaPerte->setAlignment(Qt::AlignCenter);
    unitSigmaPiezo->setAlignment(Qt::AlignCenter);


    // Ajout des unités au layout
    topLayout->addWidget(unitDebit2, 5, 1, Qt::AlignCenter);
    topLayout->addWidget(unitEspacement2, 5, 2, Qt::AlignCenter);
    topLayout->addWidget(unitDiametre2, 5, 3, Qt::AlignCenter);
    topLayout->addWidget(unitLongueur2, 5, 4, Qt::AlignCenter);
    topLayout->addWidget(unitHauteur2, 5, 5, Qt::AlignCenter);
    topLayout->addWidget(unitPerte, 5, 6, Qt::AlignCenter);
    topLayout->addWidget(unitPiezo, 5, 7, Qt::AlignCenter);
    topLayout->addWidget(unitSigmaPerte, 5, 8, Qt::AlignCenter);
    topLayout->addWidget(unitSigmaPiezo, 5, 9, Qt::AlignCenter);


    // Ajout des label et des QlineEdit au layout
    topLayout->addWidget(Debit, 6, 1, Qt::AlignCenter);
    topLayout->addWidget(Espacement, 6, 2, Qt::AlignCenter);
    topLayout->addWidget(Diametre, 6, 3, Qt::AlignCenter);
    topLayout->addWidget(Longueur, 6, 4, Qt::AlignCenter);
    topLayout->addWidget(Hauteur, 6, 5, Qt::AlignCenter);

    // On fixe la taille
    int fixedWidth = 181;
    int fixedHeight = 40;

    Debit->setFixedSize(fixedWidth,fixedHeight);

    Espacement->setFixedSize(fixedWidth,fixedHeight);
    Diametre->setFixedSize(fixedWidth,fixedHeight);

    Longueur->setFixedSize(fixedWidth,fixedHeight);
    Hauteur->setFixedSize(fixedWidth,fixedHeight);

    labelNumero->setFixedSize(fixedWidth, fixedHeight);
    labelDebit2->setFixedSize(fixedWidth, fixedHeight);
    labelEspacement2->setFixedSize(fixedWidth, fixedHeight);
    labelDiametre2->setFixedSize(fixedWidth, fixedHeight);
    labelLongueur2->setFixedSize(fixedWidth, fixedHeight);
    labelHauteur2->setFixedSize(fixedWidth, fixedHeight);
    labelPerte->setFixedSize(fixedWidth, fixedHeight);
    labelPiezo->setFixedSize(fixedWidth, fixedHeight);
    labelSigmaPerte->setFixedSize(fixedWidth, fixedHeight);
    labelSigmaPiezo->setFixedSize(fixedWidth, fixedHeight);
    unitDebit2->setFixedSize(fixedWidth, fixedHeight);
    unitEspacement2->setFixedSize(fixedWidth, fixedHeight);
    unitDiametre2->setFixedSize(fixedWidth, fixedHeight);
    unitLongueur2->setFixedSize(fixedWidth, fixedHeight);
    unitHauteur2->setFixedSize(fixedWidth, fixedHeight);
    unitPerte->setFixedSize(fixedWidth, fixedHeight);
    unitPiezo->setFixedSize(fixedWidth, fixedHeight);
    unitSigmaPerte->setFixedSize(fixedWidth, fixedHeight);
    unitSigmaPiezo->setFixedSize(fixedWidth, fixedHeight);
    Materiau->setFixedSize(fixedWidth, fixedHeight);




    mainLayout->addLayout(topLayout);

    // Section du milieu
    scrollArea = new QScrollArea;
    mainLayout->addWidget(scrollArea);

    QWidget *scrollAreaContents = new QWidget;
    scrollAreaLayout = new QGridLayout(scrollAreaContents);
    scrollArea->setWidget(scrollAreaContents);
    scrollArea->setWidgetResizable(true);

    // Section du bas
    QGridLayout *bottomLayout = new QGridLayout;
    bottomLayout->setSpacing(10);
    bottomLayout->setAlignment(Qt::AlignCenter);

    // QLineEdit et label pour la section du bas
    CumulLongueur = new QLineEdit;
    Cumulhauteur = new QLineEdit;
    CumulPerte = new QLineEdit;
    CumulPiezo = new QLineEdit;


    CumulLongueur->setFixedSize(fixedWidth, fixedHeight);
    Cumulhauteur->setFixedSize(fixedWidth, fixedHeight);
    CumulPerte->setFixedSize(fixedWidth, fixedHeight);
    CumulPiezo->setFixedSize(fixedWidth, fixedHeight);

    // Add spacer before the first QLineEdit

    bottomLayout->addWidget(CumulLongueur, 0, 3, Qt::AlignCenter); // Column 4 for Longueur
    bottomLayout->addWidget(Cumulhauteur, 0, 4, Qt::AlignCenter); // Column 6 for Hauteur

    // Add spacer between Hauteur and Perte

    bottomLayout->addWidget(CumulPerte, 0, 5, Qt::AlignCenter); // Column 9 for Perte

    // Add spacer between Perte and Piezo
    bottomLayout->addWidget(CumulPiezo, 0, 6, Qt::AlignCenter); // Column 11 for Piezo

    connect(Debit, &QLineEdit::returnPressed, this, &gag::AjoutDonnee);
    connect(Espacement, &QLineEdit::returnPressed, this, &gag::AjoutDonnee);
    connect(Diametre, &QLineEdit::returnPressed, this, &gag::AjoutDonnee);
    connect(Longueur, &QLineEdit::returnPressed, this, &gag::AjoutDonnee);
    connect(Hauteur, &QLineEdit::returnPressed, this, &gag::AjoutDonnee);

    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);

    Debit->installEventFilter(this);
    Espacement->installEventFilter(this);
    Diametre->installEventFilter(this);
    Longueur->installEventFilter(this);
    Hauteur->installEventFilter(this);


    // Create buttons
    QPushButton *button1 = new QPushButton("Export PDF");
    QPushButton *button2 = new QPushButton("Sauvergarder");
    QPushButton *button3 = new QPushButton("Charger");

    // Create the buttons for the bottom layout
    QPushButton *calculButton = new QPushButton("Calcul");
    QPushButton *effacerButton = new QPushButton("Effacer");
    QPushButton *modifierButton = new QPushButton("Modifier");
    QPushButton *recopier = new QPushButton("Recopier");
    QPushButton *reinitialiserButton = new QPushButton("Réinitialiser");

    connect(calculButton, &QPushButton::clicked, this, &gag::calcul);
    connect(effacerButton, &QPushButton::clicked, this, &gag::enleverLigne);
    connect(modifierButton, &QPushButton::clicked, this, &gag::showUpdateDialog);
    connect(reinitialiserButton, &QPushButton::clicked, this, &gag::refresh);
    connect(recopier, &QPushButton::clicked, this, &gag::recopiederniereligne);


    button1->setFixedSize(fixedWidth, fixedHeight);
    button2->setFixedSize(fixedWidth, fixedHeight);
    button3->setFixedSize(fixedWidth, fixedHeight);
    calculButton->setFixedSize(fixedWidth, fixedHeight);
    effacerButton->setFixedSize(fixedWidth, fixedHeight);
    modifierButton->setFixedSize(fixedWidth, fixedHeight);
    reinitialiserButton->setFixedSize(fixedWidth, fixedHeight);
    recopier->setFixedSize(fixedWidth, fixedHeight);

    // Add buttons to the bottomLayout

    // Add buttons to the bottomLayout
    bottomLayout->addWidget(calculButton, 1, 0, Qt::AlignCenter);
    bottomLayout->addWidget(effacerButton, 1, 1, Qt::AlignCenter);
    bottomLayout->addWidget(modifierButton, 1, 2, Qt::AlignCenter);
    bottomLayout->addWidget(recopier, 1, 3, Qt::AlignCenter);
    bottomLayout->addWidget(reinitialiserButton, 1, 4, Qt::AlignCenter);
    bottomLayout->addWidget(button1, 1, 5, Qt::AlignCenter);
    bottomLayout->addWidget(button2, 1, 6, Qt::AlignCenter);
    bottomLayout->addWidget(button3, 1, 7, Qt::AlignCenter);


    bottomLayout->setAlignment(Qt::AlignHCenter);

// Connect buttons to their respective slots (functions)
    connect(button1, &QPushButton::clicked, this, &gag::saveAsPdf);
    connect(button2, &QPushButton::clicked, this, &gag::saveDataWrapper);
    connect(button3, &QPushButton::clicked, this, &gag::loadDataWrapper);


    Debit->setFocus();

    // Get the available geometry of the screen
    QDesktopWidget desktop;
    QRect screenSize = desktop.availableGeometry(this);

    // Set the window size to the screen size
    this->setGeometry(screenSize);

}



void gag::saveAsPdf() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save as PDF", QDir::homePath(), "PDF Files (*.pdf)");

    if (!fileName.isEmpty()) {
        createPdfReport(fileName);
    }
}

void gag::saveDataWrapper() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Data", QDir::homePath(), "Data Files (*.dat)");

    if (!fileName.isEmpty()) {
        saveData(fileName);
    }
}

void gag::loadDataWrapper() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Data", QDir::homePath(), "Data Files (*.dat)");

    if (!fileName.isEmpty()) {
        loadData(fileName);
    }
}






bool gag::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_R) {
            recopiederniereligne();
            return true;
        } else if (keyEvent->key() == Qt::Key_M) {
            showUpdateDialog();
            return true;
        } else if (keyEvent->key() == Qt::Key_Z) {
            enleverLigne();
            return true;
        } else if (keyEvent->key() == Qt::Key_E) {
            _Donnees.clear();
            clear();
            return true;
        } else if (keyEvent->key() == Qt::Key_Control) {
            focusPreviousInput();
            return true;
        } else if (keyEvent->key() == Qt::Key_C) {
            calcul();
            return true;
        } else if (keyEvent->key() == Qt::Key_Tab) {
            if  (!Debit->text().isEmpty() && !Espacement->text().isEmpty() &&
                    !Diametre->text().isEmpty() && !Longueur->text().isEmpty() &&
                    !Hauteur->text().isEmpty()) {
                AjoutDonnee();
                return true;

            } else {
                keyEvent->ignore(); // Ignore tab key event to prevent natural action
                return true;
            }
        }
        // Ajoute le raccourci Ctrl + S pour enregistrer en PDF
        else if (keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_S && !(keyEvent->modifiers() & Qt::ShiftModifier)) {
            saveAsPdf();
            return true;
        }
        // Ajoute le raccourci Ctrl + Shift + S pour enregistrer les données
        else if (keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_S && keyEvent->modifiers() & Qt::ShiftModifier) {
            saveDataWrapper();
            return true;
        }
        // Ajoute le raccourci Ctrl + L pour charger les données
        else if (keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_L) {
            loadDataWrapper();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}


void gag::AjoutDonnee() {
    // Vérifier si tous les champs sont remplis
    if (!Debit->text().isEmpty() && !Espacement->text().isEmpty() &&
        !Diametre->text().isEmpty() && !Longueur->text().isEmpty() &&
        !Hauteur->text().isEmpty()) {

        std::vector<float> temp;
        temp.resize(10);

        // Obtenir le numéro de la ligne en ajoutant 1 à la taille des données existantes
        float numero = _Donnees.size()+1;

        // Convertir les valeurs de texte en float et remplacer les virgules par des points
        QString debitText = Debit->text();
        debitText.replace(',', '.');
        float debit = debitText.toFloat();

        QString espacementText = Espacement->text();
        espacementText.replace(',', '.');
        float espacement = espacementText.toFloat();

        QString diametreText = Diametre->text();
        diametreText.replace(',', '.');
        float diametre = diametreText.toFloat();

        QString longueurText = Longueur->text();
        longueurText.replace(',', '.');
        float longueur = longueurText.toFloat();

        QString hauteurText = Hauteur->text();
        hauteurText.replace(',', '.');
        float hauteur = hauteurText.toFloat();

        // Stocker les valeurs dans le vecteur temporaire
        temp[0] = numero;
        temp[1] = debit;
        temp[2] = espacement;
        temp[3] = diametre;
        temp[4] = longueur;
        temp[5] = hauteur;

        // Ajouter le vecteur temporaire aux données
        _Donnees.push_back(temp);

        Debit->clear();
        Debit->setFocus();
        Espacement->clear();
        Diametre->clear();
        Longueur->clear();
        Hauteur->clear();

        // Ajouter une nouvelle ligne avec les données saisies
        AjoutLigne();

    } else {
        // Si tous les champs ne sont pas remplis, passez au champ suivant non rempli
        focusNextInput();
    }
}

void gag::AjoutLigne() {
    // Ajouter une nouvelle ligne dans la zone de défilement avec les valeurs correspondantes
    int row = _Donnees.size(); // Obtenez le numéro de ligne pour la nouvelle ligne en ajoutant 1 à la taille du vecteur de données "_Donnees".
    const std::vector<float>& temp = _Donnees[row - 1]; // Copiez les valeurs de la dernière ligne pour les utiliser comme valeurs par défaut pour la nouvelle ligne.

    // Définir les propriétés pour les objets QLineEdit
    int fixedHeight = 40;
    Qt::Alignment alignment = Qt::AlignCenter;

    for (int col = 0; col < temp.size(); ++col) { // Pour chaque colonne
        // Créez un objet QLineEdit pour afficher la valeur et définir ses propriétés en lecture seule, son alignement et sa taille.
        QLineEdit *valueLabel = new QLineEdit(QString::number(temp[col]));
        valueLabel->setReadOnly(true);
        valueLabel->setAlignment(alignment);
        valueLabel->setFixedHeight(fixedHeight);
        valueLabel->setFixedWidth(180);
        valueLabel->setStyleSheet("color: yellow;"); // Changer la couleur du texte en bleu

        // Ajoutez l'objet QLineEdit dans la zone de défilement en utilisant le numéro de ligne et de colonne approprié.
        scrollAreaLayout->addWidget(valueLabel, row, col);
        scrollArea->ensureWidgetVisible(valueLabel);
    }

    // Set the vertical spacing and alignment.
    scrollAreaLayout->setVerticalSpacing(1);
    scrollAreaLayout->setAlignment(Qt::AlignTop);

    // Utilisez la variable fixedHeight pour déterminer la hauteur de la ligne.
    int ROW_HEIGHT = fixedHeight;

    int VERTICAL_SPACING = 15; // Utilisez la même valeur que celle définie pour l'espacement vertical dans scrollAreaLayout.
    int scrollWidgetHeight = (row * ROW_HEIGHT) + ((row - 1) * VERTICAL_SPACING);
    scrollArea->widget()->setMinimumHeight(scrollWidgetHeight);
    scrollArea->widget()->setMaximumHeight(scrollWidgetHeight);


    // Vérifier les propriétés des widgets de défilement (scrollArea et scrollAreaLayout) dans les deux classes pour s'assurer qu'elles sont identiques.

    Debit->setFocus(); // Définir le focus sur l'objet "Debit" après avoir ajouté la nouvelle ligne.
}




// La fonction "focusPreviousInput" est appelée lorsqu'un raccourci clavier est utilisé pour passer au champ d'entrée précédent.
void gag::focusPreviousInput() {
    // Si l'entrée "hauteur" a le focus, passe le focus à l'entrée "longueur".
    if (Hauteur->hasFocus()) {
        Longueur->setFocus();
        return;
    }
        // Si l'entrée "longueur" a le focus, passe le focus à l'entrée "diametre".
    else if (Longueur->hasFocus()) {
        Diametre->setFocus();
        return;
    }
        // Si l'entrée "diametre" a le focus, passe le focus à l'entrée "debit".
    else if (Diametre->hasFocus()){
        Espacement->setFocus();
        return;
    }

    else if (Espacement->hasFocus()){
        Debit->setFocus();
    }
}


// La fonction "focusNextInput" est appelée lorsqu'un raccourci clavier est utilisé pour passer au champ d'entrée suivant.
void gag::focusNextInput() {

    // Si l'entrée "hauteur" a le focus, passe le focus à l'entrée "longueur".
    if (Debit->hasFocus()) {
        Espacement->setFocus();
        return;
    }
        // Si l'entrée "longueur" a le focus, passe le focus à l'entrée "diametre".
    else if (Espacement->hasFocus()) {
        Diametre->setFocus();
        return;
    }
        // Si l'entrée "diametre" a le focus, passe le focus à l'entrée "debit".
    else if (Diametre->hasFocus()){
        Longueur->setFocus();
        return;
    }

    else if (Longueur->hasFocus()){
        Hauteur->setFocus();
    }
}



void gag::calcul() {
    if(_Donnees.size()==0) return;

    // Initialise les paramètres.
    double k = 0;
    float a = 0;
    float b = 0;

    // Initialise les variables.
    float espacement = 0;
    float diametre = 0;
    float longueur = 0;
    float hauteur = 0;
    float perteCharge = 0;
    float piezo = 0;
    float sigmaDebit = 0; // Cumul débit
    float debitLS = 0;

    std::string material_name = Materiau->currentText().toStdString();
    auto coefficients = database->get_material_coefficients(material_name);

    a = std::get<0>(coefficients);
    b = std::get<1>(coefficients);
    k = std::get<2>(coefficients);

    float sigmaPiezo = 0; // Cumul piezo
    float sigmaPerte = 0; // Cumul perte
    float sigmaLongueur = 0;
    float sigmaHauteur = 0;

    // Effectue les calculs pour chaque ligne de données.
    for (int i = 0; i < _Donnees.size(); ++i) {

        // Récupère les données de la ligne courante.
        sigmaDebit += _Donnees[i][1];
        espacement = _Donnees[i][2];
        diametre = _Donnees[i][3];
        longueur = _Donnees[i][4];
        hauteur = _Donnees[i][5];

        debitLS = sigmaDebit / 3600;

        // Calcule la perte de charge.
        perteCharge = k * std::pow(debitLS, a) * std::pow(diametre, b) * longueur * espacement;

        // Calcule la hauteur piezométrique.
        piezo = perteCharge + hauteur;

        // Ajoute les données calculées au vecteur.
        _Donnees[i][6] = perteCharge;
        _Donnees[i][7] = piezo;

        // Calcule les cumuls pour chaque ligne de données.
        sigmaPiezo += piezo;
        sigmaPerte += perteCharge;
        sigmaLongueur += longueur;
        sigmaHauteur += hauteur;

        // Stocke les cumuls dans le vecteur.
        _Donnees[i][8] = sigmaPerte;
        _Donnees[i][9] = sigmaPiezo;
    }

    // Affiche les résultats dans les cases correspondantes en arrondissant à deux chiffres après la virgule.
    CumulLongueur->setText(QString::number(sigmaLongueur, 'f', 2));
    CumulLongueur->setAlignment(Qt::AlignCenter);
    Cumulhauteur->setText(QString::number(sigmaHauteur, 'f', 2));
    Cumulhauteur->setAlignment(Qt::AlignCenter);
    CumulPerte->setText(QString::number(sigmaPerte, 'f', 2));
    CumulPerte->setAlignment(Qt::AlignCenter);
    CumulPiezo->setText(QString::number(sigmaPiezo, 'f', 2));
    CumulPiezo->setAlignment(Qt::AlignCenter);

    RafraichirTableau();
}

/*void gag::calcul() {

    if(_Donnees.size()==0) return;

    // Initialise les paramètres.
    double k = 0;
    float a = 0;
    float b = 0;

    // Initialise les variables.
    float espacement = 0;
    float diametre = 0;
    float longueur = 0;
    float hauteur = 0;
    float perteCharge = 0;
    float piezo = 0;
    float sigmaDebit = 0; // Cumul débit
    float debitLS = 0;

    std::string material_name = Materiau->currentText().toStdString();
    auto coefficients = database->get_material_coefficients(material_name);

    a = std::get<0>(coefficients);
    b = std::get<1>(coefficients);
    k = std::get<2>(coefficients);

    float sigmaPiezo = 0; // Cumul piezo
    float sigmaPerte = 0; // Cumul perte
    float sigmaLongueur = 0;
    float sigmaHauteur = 0;

    // Effectue les calculs pour chaque ligne de données.
    for (int i = 0; i < _Donnees.size(); ++i) {

        // Récupère les données de la ligne courante.
        sigmaDebit += _Donnees[i][1];
        espacement = _Donnees[i][2];
        diametre = _Donnees[i][3];
        longueur = _Donnees[i][4];
        hauteur = _Donnees[i][5];

        debitLS = sigmaDebit / 3600;

        // Calcule le nombre de trous.
        int nombreDeTrous = longueur / espacement;

        // Calcule le débit par trou.
        float debitParTrou = debitLS / nombreDeTrous;

        // Calcule la perte de charge.
        perteCharge = k * std::pow(debitParTrou, a) * std::pow(diametre, b) * longueur;

        // Calcule la hauteur piezométrique.
        piezo = perteCharge + hauteur;

        // Ajoute les données calculées au vecteur.
        _Donnees[i][6] = perteCharge;
        _Donnees[i][7] = piezo;

        // Calcule les cumuls pour chaque ligne de données.
        sigmaPiezo += piezo;
        sigmaPerte += perteCharge;
        sigmaLongueur += longueur;
        sigmaHauteur += hauteur;

        // Stocke les cumuls dans le vecteur.
        _Donnees[i][8] = sigmaPerte;
        _Donnees[i][9] = sigmaPiezo;
    }

    // Affiche les résultats dans les cases correspondantes en arrondissant à deux chiffres après la virgule.
    CumulLongueur->setText(QString::number(sigmaLongueur, 'f', 2));
    CumulLongueur->setAlignment(Qt::AlignCenter);
    Cumulhauteur->setText(QString::number(sigmaHauteur, 'f', 2));
    Cumulhauteur->setAlignment(Qt::AlignCenter);
    CumulPerte->setText(QString::number(sigmaPerte, 'f', 2));
    CumulPerte->setAlignment(Qt::AlignCenter);
    CumulPiezo->setText(QString::number(sigmaPiezo, 'f', 2));
    CumulPiezo->setAlignment(Qt::AlignCenter);
    RafraichirTableau();
}
*/
void gag::clear(){
    // Supprime les widgets existants dans le layout de la scrollArea.
    QLayoutItem *item;
    while ((item = scrollAreaLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    Debit->clear();
    Espacement->clear();
    Diametre->clear();
    Longueur->clear();
    Hauteur->clear();


}

void gag::RafraichirTableau() {
    clear(); // Efface le contenu actuel de la zone de défilement.

    // Définit les propriétés des objets QLineEdit
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    int fixedHeight = 40;
    Qt::Alignment alignment = Qt::AlignCenter;

    // Parcourt le vecteur de données "_Donnees" et ajoute les données à la zone de défilement.
    for (int i = 0; i < _Donnees.size(); ++i) {
        for (int j = 0; j < _Donnees[i].size(); ++j) {
            QLineEdit *value;
            if (j == 0) { // Si nous sommes sur la première colonne, affichez la valeur en tant qu'entier.
                value = new QLineEdit(QString::number(static_cast<int>(_Donnees[i][j])));
            } else { // Sinon, affichez la valeur avec deux décimales.
                value = new QLineEdit(QString::number(_Donnees[i][j], 'f', 2));
            }
            value->setReadOnly(true);
            value->setFixedHeight(fixedHeight); // Définir la taille de la ligne.
            value->setSizePolicy(sizePolicy);
            value->setFixedWidth(180);
            value->setAlignment(alignment);
            if (j >= 1 && j <= 5) {
                QPalette palette;
                value->setStyleSheet("color : #eaff00");
            } else {
                value->setStyleSheet("color :  #64f5a3");

            }
            scrollAreaLayout->addWidget(value, i + 1, j, Qt::AlignTop);
        }
        // Définit l'espacement vertical et l'alignement pour la disposition de la zone de défilement.
        scrollAreaLayout->setVerticalSpacing(15);
        scrollAreaLayout->setAlignment(Qt::AlignTop);
    }
    // Set the vertical spacing and alignment.
    scrollAreaLayout->setVerticalSpacing(1);
    scrollAreaLayout->setAlignment(Qt::AlignTop);
}

void gag::keyPressEvent(QKeyEvent *event) {
    // Vérifie si la touche Shift est enfoncée et si la touche Entrée est également enfoncée.
    if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_Return) {
        if (!Debit->text().isEmpty() && !Espacement->text().isEmpty() &&
            !Diametre->text().isEmpty() && !Longueur->text().isEmpty() &&
            !Hauteur->text().isEmpty()) {
        // Appelle la fonction calcul().
             calcul();
        }
    } else if (event->key() == Qt::Key_R) {
        // Appelle la fonction recopie
        recopiederniereligne();
    } else if (event->key() == Qt::Key_C) {
        if (!Debit->text().isEmpty() && !Espacement->text().isEmpty() &&
            !Diametre->text().isEmpty() && !Longueur->text().isEmpty() &&
            !Hauteur->text().isEmpty()) {
             // Appelle la fonction calcul().
             calcul();
        }
    } else if (event->key() == Qt::Key_M) {
        // Appelle la fonction modifier
        showUpdateDialog();
    } else if (event->key() == Qt::Key_Z) {
        // Appelle la fonction enlever ligne
        enleverLigne();
    } else if (event->key() == Qt::Key_E) {
        // Efface les données et le tableau
        _Donnees.clear();
        clear();
    } else if (event->key() == Qt::Key_Control) {
        // Permet de changer la case active pour la precedente
        focusPreviousInput();
    }
        // Ajoute le raccourci Ctrl + S pour enregistrer en PDF
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_S && !(event->modifiers() & Qt::ShiftModifier)) {
        saveAsPdf();
    }
        // Ajoute le raccourci Ctrl + Shift + S pour enregistrer les données
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_S && event->modifiers() & Qt::ShiftModifier) {
        saveDataWrapper();
    }
        // Ajoute le raccourci Ctrl + L pour charger les données
    else if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_L) {
        loadDataWrapper();
    }
}


void gag::recopiederniereligne() { // Fonction déclenché par la touche 'R' qui permet de recopier la derniere ligne entrée

    if (_Donnees.size()==0) {
        return;
    }
    int taille =_Donnees.size();
    std::vector<float> lastline = _Donnees[taille-1]; // Pour avoir la derniere ligne du vecteur
    std::vector<float> recopie(10, 0.0f);

    recopie[0] = lastline[0]+1; // Numéro
    recopie[1] = lastline[1];   // Debit
    recopie[2] = lastline[2];   // Espacement
    recopie[3] = lastline[3];   // Diametre
    recopie[4] = lastline[4];   // Longueur
    recopie[5] = lastline[5];   // Hauteur

    _Donnees.push_back(recopie);
    AjoutLigne();
}

// Ouvre une boîte de dialogue permettant à l'utilisateur de modifier les données d'une ligne dans le tableau
void gag::showUpdateDialog() {
    // Crée une nouvelle fenêtre de dialogue en tant que sous-fenêtre de la fenêtre principale de l'application
    QDialog *updateDialog = new QDialog(this);
    // Définit le titre de la fenêtre de dialogue
    updateDialog->setWindowTitle("Modifier ligne");

    // Crée un formulaire dans la fenêtre de dialogue pour saisir les nouvelles données de la ligne
    QFormLayout *formLayout = new QFormLayout(updateDialog);

    // Ajoute un champ pour entrer le numéro de ligne à modifier
    QLineEdit *rowNumberLineEdit = new QLineEdit(updateDialog);
    rowNumberLineEdit->setValidator(new QIntValidator(0, _Donnees.size() - 1,updateDialog)); // Permet de limiter la saisie à un nombre entier valide
    formLayout->addRow("Ligne : ", rowNumberLineEdit);

    // Ajoute un champ pour entrer le nouveau débit de la ligne
    QLineEdit *debitLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Debit : ", debitLineEdit);

    // Ajoute un champ pour entrer le nouvel espacement de la ligne
    QLineEdit *espacementLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Espacement : ", espacementLineEdit);

    // Ajoute un champ pour entrer le nouveau diamètre de la ligne
    QLineEdit *diameterLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Diametre : ", diameterLineEdit);

    // Ajoute un champ pour entrer la nouvelle longueur de la ligne
    QLineEdit *lengthLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Longueur : ", lengthLineEdit);

    // Ajoute un champ pour entrer la nouvelle hauteur de la ligne
    QLineEdit *heightLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Hauteur : ", heightLineEdit);

    // Ajoute un bouton pour valider les nouvelles données et fermer la fenêtre de dialogue
    QPushButton *updateButton = new QPushButton("Modifier", updateDialog);
    formLayout->addWidget(updateButton);

    // Fonction qui permet de définir le comportement à adopter lorsque l'utilisateur appuie sur la touche "Entrée" dans un champ de saisie
    auto handleEnterKeyPress = [this](QLineEdit *current, QLineEdit *next, std::function<void()> lastAction = nullptr) {
        QObject::connect(current, &QLineEdit::returnPressed, [this, current, next, lastAction]() {
            // Si le champ en cours de saisie est rempli et que le champ suivant (s'il y en a un) est vide, déplace le curseur vers le champ suivant
            if (!current->text().isEmpty() && (next == nullptr || next->text().isEmpty())) {
                if (next) {
                    next->setFocus();
                } else if (lastAction) {
                    lastAction();
                }
            }
        });
    };


    // Fonction qui met à jour les données de la ligne avec les nouvelles données saisies par l'utilisateur et ferme la fenêtre de dialogue
    auto updateDataAndClose = [this, rowNumberLineEdit, debitLineEdit,espacementLineEdit, diameterLineEdit, lengthLineEdit, heightLineEdit, updateDialog]() {
        // Récupère
        // Récupère les nouvelles données saisies par l'utilisateur
        int rowNumber = rowNumberLineEdit->text().toInt();
        float debit = debitLineEdit->text().toFloat();
        float espacement = espacementLineEdit->text().toFloat();
        float diameter = diameterLineEdit->text().toFloat();
        float length = lengthLineEdit->text().toFloat();
        float height = heightLineEdit->text().toFloat();

        // Met à jour les données de la ligne avec les nouvelles données saisies
        updateData(rowNumber, debit, espacement, diameter, length, height);
        // Ferme la fenêtre de dialogue
        updateDialog->close();
    };

    // Associe la fonction handleEnterKeyPress aux champs de saisie pour gérer les appuis sur la touche "Entrée"
    handleEnterKeyPress(rowNumberLineEdit, debitLineEdit);
    handleEnterKeyPress(debitLineEdit,espacementLineEdit);
    handleEnterKeyPress(espacementLineEdit, diameterLineEdit);
    handleEnterKeyPress(diameterLineEdit, lengthLineEdit);
    handleEnterKeyPress(lengthLineEdit, heightLineEdit);
    handleEnterKeyPress(heightLineEdit, nullptr, updateDataAndClose); // Le dernier champ n'a pas de champ suivant, donc next est nul

    // Associe la fonction updateDataAndClose au bouton "Update" pour mettre à jour les données de la ligne et fermer la fenêtre de dialogue lorsque l'utilisateur clique sur le bouton
    connect(updateButton, &QPushButton::clicked, updateDataAndClose);

    // Ajoute un raccourci clavier pour valider les nouvelles données et fermer la fenêtre de dialogue lorsque l'utilisateur appuie sur la touche "Entrée"
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), updateDialog);
    QShortcut *returnShortcut = new QShortcut(QKeySequence(Qt::Key_Return), updateDialog);
    connect(enterShortcut, &QShortcut::activated, updateDataAndClose);
    connect(returnShortcut, &QShortcut::activated, updateDataAndClose);

    // Définit le layout du formulaire comme layout de la fenêtre de dialogue et affiche la fenêtre de dialogue modale
    updateDialog->setLayout(formLayout);
    updateDialog->exec();
}

// La fonction "updateData" prend en entrée un numéro de ligne, un débit, un diamètre, une longueur et une hauteur,
// puis met à jour les données de la ligne correspondante dans un tableau.
void gag::updateData(int numeroLigne, float debit,float espacement, float diametre, float longueur, float hauteur) {

    // Vérifie si le numéro de ligne est valide (c'est-à-dire s'il se trouve dans la plage d'index des données).
    if (numeroLigne >= 0 && numeroLigne < _Donnees.size()) {
        // Décrémente le numéro de ligne de 1 pour correspondre à l'indice de tableau (qui commence à 0).
        numeroLigne -= 1;

        // Met à jour les données de la ligne avec les nouvelles valeurs fournies.
        _Donnees[numeroLigne][1] = debit;
        _Donnees[numeroLigne][2] = espacement;
        _Donnees[numeroLigne][3] = diametre;
        _Donnees[numeroLigne][4] = longueur;
        _Donnees[numeroLigne][5] = hauteur;

        // Recalculer et rafraîchir le tableau.
        calcul();
        RafraichirTableau();
    }
}


void gag::enleverLigne() {
    // Crée une nouvelle fenêtre de dialogue pour demander le numéro de la ligne à supprimer
    QDialog *supprimerDialog = new QDialog(this);
    supprimerDialog->setWindowTitle("Supprimer une ligne");
    supprimerDialog->setFixedSize(300, 100);

    // Ajoute un label et un champ de saisie pour entrer le numéro de la ligne à supprimer
    QLabel *numeroLigneLabel = new QLabel("Numéro de ligne : ", supprimerDialog);
    QLineEdit *numeroLigneLineEdit = new QLineEdit(supprimerDialog);
    numeroLigneLineEdit->setValidator(new QIntValidator(1, _Donnees.size(), supprimerDialog)); // Permet de limiter la saisie à un nombre entier valide
    numeroLigneLineEdit->setFixedWidth(50);

    QPushButton *supprimerButton = new QPushButton("Supprimer", supprimerDialog);

    // Crée un slot pour gérer la suppression de la ligne
    auto supprimerLigneSlot = [this, numeroLigneLineEdit, supprimerDialog]() {
        int numeroLigne = numeroLigneLineEdit->text().toInt() - 1;
        if (numeroLigne >= 0 && numeroLigne < _Donnees.size()) {
            _Donnees.erase(_Donnees.begin() + numeroLigne); // Supprime la ligne du tableau
            // Réaffecte les numéros de ligne à partir de la ligne supprimée
            for (int i = numeroLigne; i < _Donnees.size(); ++i) {
                _Donnees[i][0] = i + 1;
            }
            RafraichirTableau(); // Actualise l'affichage du tableau
        }
        supprimerDialog->close();
    };

    // Connecte les signaux aux slots
    connect(supprimerButton, &QPushButton::clicked, supprimerLigneSlot);
    connect(numeroLigneLineEdit, &QLineEdit::returnPressed, supprimerLigneSlot);


    // Ajoute un layout à la fenêtre de dialogue
    QVBoxLayout *layout = new QVBoxLayout(supprimerDialog);
    layout->addWidget(numeroLigneLabel);
    layout->addWidget(numeroLigneLineEdit);
    layout->addWidget(supprimerButton);
    layout->setAlignment(Qt::AlignCenter);

    // Affiche la fenêtre de dialogue
    supprimerDialog->exec();

    calcul();
    RafraichirTableau();
}


void gag::saveData(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Cannot open file for writing");
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_9);

    for (const std::vector<float> &row : _Donnees) {
        for (float value : row) {
            out << value;
        }
    }
}

void gag::loadData(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Cannot open file for reading");
        return;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_9);

    _Donnees.clear();

    while (!in.atEnd()) {
        std::vector<float> row;
        for (int i = 0; i < 10; ++i) {
            float value;
            in >> value;
            row.push_back(value);
        }
        _Donnees.push_back(row);
    }

    // Rafraichis le tableau et lance le calcul
    calcul();
    RafraichirTableau();
}


void gag::createPdfReport(const QString &fileName) {
    calcul();
    // Création des QDialog pour les entrées de l'utilisateur
    QDialog inputDialog;
    inputDialog.setWindowTitle("Entrez les informations");

    QLabel *nomLabel = new QLabel("Nom : ");
    QLabel *prenomLabel = new QLabel("Prénom : ");
    QLabel *referenceLabel = new QLabel("Référence : ");
    QLabel *dateLabel = new QLabel("Date : ");

    QLineEdit *nomLineEdit = new QLineEdit;
    QLineEdit *prenomLineEdit = new QLineEdit;
    QLineEdit *referenceLineEdit = new QLineEdit;
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate());

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &inputDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &inputDialog, &QDialog::reject);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(nomLabel, nomLineEdit);
    layout->addRow(prenomLabel, prenomLineEdit);
    layout->addRow(referenceLabel, referenceLineEdit);
    layout->addRow(dateLabel, dateEdit);
    layout->addWidget(buttonBox);

    inputDialog.setLayout(layout);

    if (inputDialog.exec() == QDialog::Rejected) {
        return;
    }

    QString nom = nomLineEdit->text();
    QString prenom = prenomLineEdit->text();
    QString reference = referenceLineEdit->text();
    QString date = QLocale().toString(dateEdit->date(), QLocale::ShortFormat);

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&pdfWriter);
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    const int lineHeight = 300;

    painter.drawText(0, lineHeight, QString("Nom: %1").arg(nom));
    painter.drawText(0, lineHeight * 2, QString("Prénom: %1").arg(prenom));
    painter.drawText(0, lineHeight * 3, QString("Référence: %1").arg(reference));
    painter.drawText(pdfWriter.width() - 1000, lineHeight-200, QString("Date: %1").arg(date));

    // Entete
    const QStringList headerLabels = {
            "Index", "Debit", "Espacement", "Diametre", "Longueur", "Hauteur", "Perte", "Piezo", "Cumul Perte", "Cumul Piezo"
    };

    // Espacement des colonnes
    QVector<int> columnWidths = {600, 600, 1000, 1000, 1000, 800, 800, 800, 1200, 1200};
    int tableWidth = 0;
    for (int width : columnWidths) {
        tableWidth += width;
    }

    int yOffset = lineHeight * 5;
    int currentPage = 1;

    auto drawHeaderAndLines = [&](int yOffset) {
        int xPos = 0;
        for (int i = 0; i < headerLabels.size(); ++i)
        {
            QRect headerRect(xPos, yOffset - lineHeight, columnWidths[i], lineHeight);
            painter.drawText(headerRect, Qt::AlignCenter, headerLabels[i]);
            xPos += columnWidths[i];
        }

        yOffset += lineHeight;

        // Ligne horizontal de l'entete
        painter.setPen(QPen(Qt::black, 1));
        painter.drawLine(0, yOffset - lineHeight, tableWidth, yOffset - lineHeight);

        // determine le nombre de lignes par page
        int maxRowsPerPage = (pdfWriter.height() - yOffset - 150) / lineHeight;

        // Lignes verticales
        xPos = 0;
        for (int i = 0; i < columnWidths.size(); ++i) {
            painter.drawLine(xPos, yOffset - lineHeight, xPos, yOffset + (lineHeight * (maxRowsPerPage - 1)));
            xPos += columnWidths[i];
        }
        painter.drawLine(xPos, yOffset - lineHeight, xPos, yOffset + (lineHeight * (maxRowsPerPage - 1)));

        painter.setFont(font);

        return yOffset;
    };

    yOffset = drawHeaderAndLines(yOffset);

    for (const std::vector<float> &donneesLigne : _Donnees) {
        int xPos = 0;

        for (int i = 0; i < donneesLigne.size(); ++i) {
            QString cellText;
            if (i == 0) {
                cellText = QString::number(donneesLigne[i], 'f', 0);
            } else {
                cellText = QString::number(donneesLigne[i], 'f', 2);
            }

            QRect cellRect(xPos, yOffset - lineHeight, columnWidths[i], lineHeight);
            painter.drawText(cellRect, Qt::AlignCenter, cellText);
            xPos += columnWidths[i];
        }

        yOffset += lineHeight;

        if (yOffset > pdfWriter.height() - 2 * lineHeight) {
            // Dessine le bas de page
            QString footerText = QString("Page %1").arg(currentPage);
            QRect footerRect(0, pdfWriter.height() - lineHeight, pdfWriter.width(), lineHeight);
            painter.drawText(footerRect, Qt::AlignCenter, footerText);
            QString referenceText = QString("Reference: %1").arg(reference);
            QRect referenceRect(pdfWriter.width() - 1500, pdfWriter.height() - lineHeight, 1500, lineHeight);
            painter.drawText(referenceRect, Qt::AlignCenter, referenceText);

            // Nouvelle page
            pdfWriter.newPage();
            yOffset = lineHeight * 2;
            yOffset = drawHeaderAndLines(yOffset);
            currentPage++;
        }
    }

    // Ajoute un pied de page à la derniere page
    QString footerText = QString("Page %1").arg(currentPage);
    painter.drawText(QRect(0, pdfWriter.height() - lineHeight, pdfWriter.width(), lineHeight), Qt::AlignCenter, footerText);
    painter.drawText(QRect(0, pdfWriter.height() - lineHeight, pdfWriter.width() - 20, lineHeight), Qt::AlignRight, QString("Référence: %1").arg(reference));

    painter.end();
}



