//
// Created by qraim on 05/04/23.
//

#include "tableau.h"


const int ROW_HEIGHT = 40;

void espacementColonne(QGridLayout *layout) {
    // Parcourt toutes les colonnes du layout
    for (int i = 0; i < layout->columnCount(); i++) {
        layout->setColumnStretch(i, 1); // Étire la colonne i avec une proportion de 1
    }
}


pertechargeherse::pertechargeherse(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent), database(db) {

    ligne = 1;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Inpout layout et widget
    QWidget *inputsWidget = new QWidget(this);
    QVBoxLayout *inputsLayout = new QVBoxLayout(inputsWidget);

    // Scroll Arae et widget
    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(scrollWidget);
    scrollWidget->setMinimumSize(sizeHint());

    gridLayout = new QGridLayout(scrollWidget);
    scrollWidget->setLayout(gridLayout);

    // Layout en dessous de la scroll Area
    QWidget *bottomWidget = new QWidget(this);
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);

    // Widget des Header
    QWidget *headersWidget = new QWidget(this);
    QGridLayout *headersGridLayout = new QGridLayout(headersWidget);

    // Ajout des titres des colonnes
    const QStringList headers = {"Numero", "Debit", "ΣDebit", "Diametre", "Longueur ", "Hauteur", "Vitesse", "Perte", "Piezo", "ΣPerte", "ΣPiezo"};

    for (int i = 0; i < headers.size(); ++i) {
        QLabel *label = new QLabel(headers[i], headersWidget);
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        label->setFixedHeight(40); // Fixe la taille
        headersGridLayout->addWidget(label, 0, i);
        headersGridLayout->setHorizontalSpacing(0); // On enleve les espaces
    }

    // Ajoute les unité dans le header
    const QStringList units = {"", "l/h", "l/h", "mm", "m", "m", "m/s", "m", "m", "m", "m"};

    for (int i = 0; i < units.size(); ++i) {
        QLabel *unitLabel = new QLabel(units[i], headersWidget);
        unitLabel->setAlignment(Qt::AlignCenter);
        unitLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        unitLabel->setFixedHeight(40); // Fixe la taille
        headersGridLayout->addWidget(unitLabel, 1, i); // Ajouter les étiquettes d'unité dans la deuxième ligne du headersGridLayout
    }




    // Ajouter le layout des inputs, la zone de scroll, et le layout du bas au layout principal
    mainLayout->addWidget(inputsWidget);
    mainLayout->addWidget(headersWidget);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(bottomWidget);

    Materiau = new QComboBox(this);
    Materiau->setFixedSize(75, 25);

    std::vector<std::string> matiere_names = database->getAllMatiereNames();
    for (const auto& matiere_name : matiere_names) {
        Materiau->addItem(QString::fromStdString(matiere_name));
    }

    inputsLayout->addWidget(Materiau);

    QLabel *Q = new QLabel("Debit (l/h)", this);
    QLabel *D = new QLabel("Diametre (mm)", this);
    QLabel *L = new QLabel("Longueur (m)", this);
    QLabel *H = new QLabel("Hauteur (m)", this);


    // Créer un QGridLayout pour la section des entrées
    QGridLayout *inputsGridLayout = new QGridLayout();
    inputsLayout->addLayout(inputsGridLayout);

    // Ajouter des espaces horizontaux pour centrer les champs d'entrée et les étiquettes
    inputsGridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0);
    inputsGridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 8);


    // Ajoute les labels à InputGridLayout
    inputsGridLayout->addWidget(Q, 0, 1);
    inputsGridLayout->addWidget(D, 0, 3);
    inputsGridLayout->addWidget(L, 0, 5);
    inputsGridLayout->addWidget(H, 0, 7);


    // Intialise les 4 entrées
    inputQ = new QLineEdit(this);
    inputD = new QLineEdit(this);
    inputL = new QLineEdit(this);
    inputH = new QLineEdit(this);

    inputQ->setMaximumWidth(75);
    inputD->setMaximumWidth(75);
    inputL->setMaximumWidth(75);
    inputH->setMaximumWidth(75);


    // AJout les champ d'entre à InputsGridLayout
    inputsGridLayout->addWidget(inputQ, 1, 1);
    inputsGridLayout->addWidget(inputD, 1, 3);
    inputsGridLayout->addWidget(inputL, 1, 5);
    inputsGridLayout->addWidget(inputH, 1, 7);

    inputQ->setAlignment(Qt::AlignRight);
    inputD->setAlignment(Qt::AlignRight);
    inputL->setAlignment(Qt::AlignRight);
    inputH->setAlignment(Qt::AlignRight);

    inputsGridLayout->setSpacing(5);

    // Appeler la fonction pour définir la largeur des colonnes pour inputsGridLayout
    espacementColonne(inputsGridLayout);

    // Appeler la fonction pour définir la largeur des colonnes pour gridLayout
    espacementColonne(gridLayout);

    // AJoute les 4 QLineEdits a BottomWidget
    sigmadebitcase = new QLineEdit(bottomWidget);
    sigmalongueurcase = new QLineEdit(bottomWidget);
    sigmapertecase = new QLineEdit(bottomWidget);
    sigmapiezocase = new QLineEdit(bottomWidget);

    sigmadebitcase->setReadOnly(true);
    sigmalongueurcase->setReadOnly(true);
    sigmapertecase->setReadOnly(true);
    sigmapiezocase->setReadOnly(true);

    // Fixe la taille des QlineEdit
    sigmadebitcase->setMaximumWidth(100);
    sigmalongueurcase->setMaximumWidth(100);
    sigmapertecase->setMaximumWidth(100);
    sigmapiezocase->setMaximumWidth(100);

    sigmadebitcase->setAlignment(Qt::AlignRight);
    sigmalongueurcase->setAlignment(Qt::AlignRight);
    sigmapertecase->setAlignment(Qt::AlignRight);
    sigmapiezocase->setAlignment(Qt::AlignRight);

    // Ajoute un labels pour chaque QLineEdit
    QLabel *label1 = new QLabel("Débit cumulé :", bottomWidget);
    QLabel *label2 = new QLabel("Volume cumulé :", bottomWidget);
    QLabel *label3 = new QLabel("Perte cumulé :", bottomWidget);
    QLabel *label4 = new QLabel("Piezo cumulé :", bottomWidget);

    // Crée un QHBoxLayout pour chaque QLineEdit et son label correspondant
    QHBoxLayout *hbox1 = new QHBoxLayout();
    QHBoxLayout *hbox2 = new QHBoxLayout();
    QHBoxLayout *hbox3 = new QHBoxLayout();
    QHBoxLayout *hbox4 = new QHBoxLayout();

    // Ajouter les étiquettes et les widgets QLineEdit aux QHBoxLayouts.
    hbox1->addWidget(label1);
    hbox1->addWidget(sigmadebitcase);
    hbox2->addWidget(label2);
    hbox2->addWidget(sigmalongueurcase);
    hbox3->addWidget(label3);
    hbox3->addWidget(sigmapertecase);
    hbox4->addWidget(label4);
    hbox4->addWidget(sigmapiezocase);

    // Ajouter les QHBoxLayouts au layout inférieur (bottomLayout)
    bottomLayout->addLayout(hbox1);
    bottomLayout->addLayout(hbox2);
    bottomLayout->addLayout(hbox3);
    bottomLayout->addLayout(hbox4);

    inputQ->setValidator(new QDoubleValidator(0.000001, 1000000, 6, this));
    inputD->setValidator(new QDoubleValidator(0.000001, 1000000, 6, this));
    inputL->setValidator(new QDoubleValidator(0.000001, 1000000, 6, this));
    inputH->setValidator(new QDoubleValidator(0.000001, 1000000, 6, this));

    inputQ->setFocus();

    // Définir l'espacement des inputsLayout et bottomLayout
    inputsLayout->setSpacing(10);
    bottomLayout->setSpacing(5);
}

pertechargeherse::~pertechargeherse() {
    delete gridLayout;
    delete inputD;
    delete inputH;
    delete inputL;
    delete inputQ;
    delete Materiau;
    delete scrollWidget;
    delete scrollArea;
    delete sigmapiezocase;
    delete sigmapertecase;
    delete sigmalongueurcase;
    delete sigmadebitcase;
}


void pertechargeherse::AjoutDonne() {

    float debit = inputQ->text().toFloat(); // récupération des inputs
    float diametre = inputD->text().toFloat();
    float longueur = inputL->text().toFloat();
    float hauteur = inputH->text().toFloat();

    int numero = _Donnees.size()+1; // numéro

    std::vector<float> temp(11, 0.0f); // variable temporaire

    temp[0] = numero;
    temp[1] = debit;
    temp[3] = diametre;
    temp[4] = longueur;
    temp[5] = hauteur;

    _Donnees.push_back(temp); // mise dans le set des données

    AjoutLigne();

}

// La fonction "AjoutLigne" est appelée pour ajouter une nouvelle ligne au pertechargeherse.

void pertechargeherse::AjoutLigne() {

    if (_Donnees.empty()) { // Si le vecteur "_Donnees" est vide, sort de la fonction.
        return;
    }

    const std::vector<float> &rowData = _Donnees.back(); // Récupère les données de la dernière ligne.

    // On spécifie les colonnes que l'on veut remplir.
    const std::vector<int> indices = {0, 1, -1, 3, 4, 5, -1, -1, -1, -1, -1};

    for (int i = 0; i < indices.size(); ++i) {

        QLineEdit *lineEdit = new QLineEdit(scrollWidget);
        lineEdit->setReadOnly(true);
        lineEdit->setAlignment(Qt::AlignCenter);
        lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        lineEdit->setFixedHeight(40); // Fixe la taille de la ligne.
        lineEdit->setFixedWidth(200);

        if (indices[i] != -1) { // Si l'indice n'est pas égal à -1, on remplit la case correspondante avec la donnée.
            if (i == 0) {
                lineEdit->setText(QString::number(rowData[i], 'f', 0)); // Formatte la donnée en nombre entier s'il s'agit de la première colonne.
            } else {
                lineEdit->setText(QString::number(rowData[indices[i]], 'f', 2)); // Formatte la donnée en nombre décimal s'il s'agit d'une autre colonne.
            }
        } else {
            lineEdit->setText(""); // Si l'indice est égal à -1, on laisse la case vide.
        }

        gridLayout->addWidget(lineEdit, ligne, i); // Ajoute la case au layout.
    }

    ligne++; // On incrémente la valeur de "ligne" pour la prochaine ligne à ajouter.

    // On ajoute la taille de la barre de scrolling en fonction du nombre de lignes.
    int scrollWidgetHeight = ligne * ROW_HEIGHT;
    scrollWidget->setMinimumHeight(scrollWidgetHeight);
    scrollWidget->setMaximumHeight(scrollWidgetHeight);

    // On fixe l'espacement vertical et l'alignement.
    gridLayout->setVerticalSpacing(15);
    gridLayout->setAlignment(Qt::AlignTop);
}




// La fonction "focusPreviousInput" est appelée lorsqu'un raccourci clavier est utilisé pour passer au champ d'entrée précédent.
void pertechargeherse::focusPreviousInput() {
    // Si l'entrée "hauteur" a le focus, passe le focus à l'entrée "longueur".
    if (inputH->hasFocus()) {
        inputL->setFocus();
        return;
    }
        // Si l'entrée "longueur" a le focus, passe le focus à l'entrée "diametre".
    else if (inputL->hasFocus()) {
        inputD->setFocus();
        return;
    }
        // Si l'entrée "diametre" a le focus, passe le focus à l'entrée "debit".
    else if (inputD->hasFocus()){
        inputQ->setFocus();
    }
}


// La fonction "focusNextInput" est appelée lorsqu'un raccourci clavier est utilisé pour passer au champ d'entrée suivant.
void pertechargeherse::focusNextInput() {

    // Si l'entrée "debit" a le focus, passe le focus à l'entrée "diametre".
    if (inputQ->hasFocus()) {
        inputD->setFocus();
        return;
    }
        // Si l'entrée "diametre" a le focus, passe le focus à l'entrée "longueur".
    else if (inputD->hasFocus()) {
        inputL->setFocus();
        return;
    }
        // Si l'entrée "longueur" a le focus, passe le focus à l'entrée "hauteur".
    else if (inputL->hasFocus()){
        inputH->setFocus();
    }
}


// La fonction "keyPressEvent" gère les événements clavier dans la fenêtre.
void pertechargeherse::keyPressEvent(QKeyEvent *event) {

    // Si la touche Contrôle est appuyée, passe le focus à l'entrée précédente.
    if (event->key() == Qt::Key_Control) {
        focusPreviousInput();
        return;
    }

        // Si la touche Tab ou Entrée est appuyée.
    else if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Return) {

        // Si la touche Shift+Entrée est appuyée et que toutes les données sont remplies, effectue le calcul.
        if (event->modifiers() == Qt::ShiftModifier && event->key() == Qt::Key_Return && Allinputfill()) {
            calcul();
            return;
        }

            // Si Entrée est appuyée et que toutes les données sont remplies, ajoute les données.
        else if(event->key() == Qt::Key_Return && Allinputfill()){
            AjoutDonne();
        }

            // Sinon, passe le focus à l'entrée suivante.
        else {
            focusNextInput();
        }

        return;
    }

        // Si la touche E est appuyée, supprime toutes les données.
    else if(event->key() == Qt::Key_E){
        _Donnees.clear();
        clearchild();
    }

        // Si la touche R est appuyée et qu'il y a des données, recopie la dernière ligne.
    else if(event->key() == Qt::Key_R){
        if(_Donnees.size() > 0){
            recopiederniereligne();
        }
    }

        // Si la touche M est appuyée, ouvre la boîte de dialogue pour modifier une ligne.
    else if(event->key() == Qt::Key_M){
        showUpdateDialog();
    }

        // Si la touche Z est appuyée, enlève une ligne.
    else if(event->key() == Qt::Key_Z){
        enleverLigne();
    }

}


bool pertechargeherse::Allinputfill() {
    if (inputD->text().isEmpty() || inputQ->text().isEmpty() ||
        inputH->text().isEmpty() || inputL->text().isEmpty()) {
        return false; // Si au moins 1 est vide
    }
    return true; // Tout les inputs sont remplis
}


// La fonction "calcul" effectue des calculs sur les données du pertechargeherse.
void pertechargeherse::calcul() {

    // Initialise les paramètres.
    double k = 0;
    float a = 0;
    float b = 0;

    // Initialise les variables.
    float debit = 0;
    float debitM3 = 0; // Débit en m3/h
    float debitLs = 0; // Débit en l/s
    float diametre = 0;
    float longueur = 0;
    float hauteur = 0;
    float perteCharge = 0;
    float piezo = 0;
    float sigmaDebit = 0; // Cumul débit
    float vitesse = 0;
    float aireTuyau = 0;
    float sigmaPiezo = 0; // Cumul piezo
    float sigmaPerte = 0; // Cumul perte
    float sigmaLongueur = 0;

    // Calcule le cumul de débit.
    for (int i = 0; i < _Donnees.size(); ++i) {
        sigmaDebit += _Donnees[i][1];
        _Donnees[i][2] = sigmaDebit;
    }

    std::tuple<float, float, double> coefficients = database->get_material_coefficients(Materiau->currentText().toStdString());
    a = std::get<0>(coefficients);
    b = std::get<1>(coefficients);
    k = std::get<2>(coefficients);

    // Effectue les calculs pour chaque ligne de données.
    for (int i = 0; i < _Donnees.size(); ++i) {

        // Récupère les données de la ligne courante.
        sigmaDebit = _Donnees[i][2];
        diametre = _Donnees[i][3];
        longueur = _Donnees[i][4];
        hauteur = _Donnees[i][5];

        // Calcule l'aire du tuyau.
        aireTuyau = (M_PI * pow((diametre / 1000) / 2, 2));

        // Calcule le débit en m3/s.
        debitM3 = sigmaDebit * 0.000277778;

        // Calcule la vitesse.
        vitesse = debitM3 / aireTuyau;

        // Convertit le débit en l/s.
        float sigmaDebitLs = sigmaDebit / 3.6;

        // Calcule la perte de charge.
        perteCharge = k * pow(sigmaDebitLs, a) * pow(diametre, b) * longueur;

        // Calcule la hauteur piezométrique.
        piezo = perteCharge + hauteur;

        // Ajoute les données calculées au vecteur.
        _Donnees[i][6] = vitesse;
        _Donnees[i][7] = perteCharge;
        _Donnees[i][8] = piezo;

    }

    // Calcule les cumuls pour chaque ligne de données.
    for (int i = 0; i < _Donnees.size(); ++i) {
        sigmaPerte += _Donnees[i][7];
        sigmaPiezo += _Donnees[i][8];
        sigmaLongueur += _Donnees[i][4];
        _Donnees[i][9] = sigmaPerte;
        _Donnees[i][10] = sigmaPiezo;
    }

    // Affiche les résultats dans les cases correspondantes en arrondissant à deux chiffres après la virgule.
    sigmadebitcase->setText(QString::number(sigmaDebit, 'f', 2));
    sigmadebitcase->setAlignment(Qt::AlignCenter);
    sigmalongueurcase->setText(QString::number(sigmaLongueur, 'f', 2));
    sigmalongueurcase->setAlignment(Qt::AlignCenter);
    sigmapertecase->setText(QString::number(sigmaPerte, 'f', 2));
    sigmapertecase->setAlignment(Qt::AlignCenter);
    sigmapiezocase->setText(QString::number(sigmaPiezo, 'f', 2));
    sigmapiezocase->setAlignment(Qt::AlignCenter);

    // Rafraîchit le pertechargeherse.
    RafraichirTableau();
}



// La fonction "RafraichirTableau" permet de mettre à jour un pertechargeherse en affichant les nouvelles données.
void pertechargeherse::RafraichirTableau() {

    // Supprime toutes les cases du pertechargeherse.
    clearchild();

    // Initialise le numéro de ligne.
    ligne = 1;

    // Trouve l'index où le débit cumulé est supérieur à la moitié du débit cumulé total.
    float cumulDebit = _Donnees[_Donnees.size()-1][2];
    float moitieCumulDebit = cumulDebit / 2;
    float indexMilieuHydrolique = 0;

    for (int i = 0; i < _Donnees.size(); ++i) {
        if (_Donnees[i][2] > moitieCumulDebit) {
            indexMilieuHydrolique = i;
            break;
        }
    }

    // Ajoute les données au pertechargeherse.
    for (const std::vector<float> &donneesLigne : _Donnees) {

        // Les indices des colonnes à afficher.
        const std::vector<int> indicesColonnes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // Détermine si le texte doit être en rouge.
        bool texteRouge = (donneesLigne[6] > 2);

        for (int i = 0; i < indicesColonnes.size(); ++i) {

            // Crée une nouvelle ligne de texte.
            QLineEdit *lineEdit = new QLineEdit(scrollWidget);

            // Rend la ligne de texte en lecture seule.
            lineEdit->setReadOnly(true);

            // Centre le texte dans la ligne de texte.
            lineEdit->setAlignment(Qt::AlignCenter);

            // Définit la politique de redimensionnement de la ligne de texte.
            lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            // Définit la hauteur fixe de la ligne de texte.
            lineEdit->setFixedHeight(40);

            lineEdit->setFixedWidth(200);


            if (i == 0) {
                lineEdit->setText(QString::number(donneesLigne[i], 'f', 0));
            } else if (i == 6 && texteRouge) {
                lineEdit->setStyleSheet("QLineEdit { background-color : red; color : white; }");
                lineEdit->setText(QString::number(donneesLigne[indicesColonnes[i]], 'f', 2));
            } else {
                lineEdit->setText(QString::number(donneesLigne[indicesColonnes[i]], 'f', 2));
            }

            // Colorie la ligne en orange pour le milieu hydrolique.
            if (ligne - 1 == indexMilieuHydrolique) {
                lineEdit->setStyleSheet("QLineEdit { background-color : orange; }");
            }

            // Définit l'espacement vertical du layout.
            gridLayout->setVerticalSpacing(10);

            // Ajoute la ligne de texte au layout.
            gridLayout->addWidget(lineEdit, ligne, indicesColonnes[i]);
        }

        // Incrémente le numéro de ligne.
        ligne++;
    }

    // Calcule la hauteur du widget de défilement et ajuste sa hauteur minimum et maximum en conséquence.
    int scrollWidgetHeight = ligne * ROW_HEIGHT;
    scrollWidget->setMinimumHeight(scrollWidgetHeight);
    scrollWidget->setMaximumHeight(scrollWidgetHeight);

    gridLayout->setVerticalSpacing(15);
    gridLayout->setAlignment(Qt::AlignTop);
}



// La fonction "clearchild" permet de retirer toutes les cases dans un layout.

void pertechargeherse::clearchild() {
    QLayoutItem *enfant;

    // Tant qu'il y a encore des éléments dans le layout, retire-les.
    while ((enfant = gridLayout->takeAt(0)) != 0) {

        // Supprime le widget contenu dans l'élément layout.
        delete enfant->widget();

        // Supprime l'élément layout.
        delete enfant;
    }

    // Réinitialise les hauteurs minimum et maximum du widget de défilement.
    scrollWidget->setMinimumHeight(0);
    scrollWidget->setMaximumHeight(0);
}



void pertechargeherse::recopiederniereligne() { // Fonction déclenché par la touche 'R' qui permet de recopier la derniere ligne entrée

    int taille =_Donnees.size();
    std::vector<float> lastline = _Donnees[taille-1]; // Pour avoir la derniere ligne du vecteur
    std::vector<float> recopie(11, 0.0f);

    recopie[0] = lastline[0]+1; // Numéro
    recopie[1] = lastline[1];   // Debit
    recopie[3] = lastline[3];   // Diametre
    recopie[4] = lastline[4];   // Longueur
    recopie[5] = lastline[5];   // Hauteur

    _Donnees.push_back(recopie);
    AjoutLigne();
}


// Ouvre une boîte de dialogue permettant à l'utilisateur de modifier les données d'une ligne dans le pertechargeherse
void pertechargeherse::showUpdateDialog() {
    // Crée une nouvelle fenêtre de dialogue en tant que sous-fenêtre de la fenêtre principale de l'application
    QDialog *updateDialog = new QDialog(this);
    // Définit le titre de la fenêtre de dialogue
    updateDialog->setWindowTitle("Modifier ligne");

    // Crée un formulaire dans la fenêtre de dialogue pour saisir les nouvelles données de la ligne
    QFormLayout *formLayout = new QFormLayout(updateDialog);

    // Ajoute un champ pour entrer le numéro de ligne à modifier
    QLineEdit *rowNumberLineEdit = new QLineEdit(updateDialog);
    rowNumberLineEdit->setValidator(new QIntValidator(0, _Donnees.size() - 1,updateDialog)); // Permet de limiter la saisie à un nombre entier valide
    formLayout->addRow("Ligne :", rowNumberLineEdit);

    // Ajoute un champ pour entrer le nouveau débit de la ligne
    QLineEdit *debitLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Debit:", debitLineEdit);

    // Ajoute un champ pour entrer le nouveau diamètre de la ligne
    QLineEdit *diameterLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Diametre :", diameterLineEdit);

    // Ajoute un champ pour entrer la nouvelle longueur de la ligne
    QLineEdit *lengthLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Longueur :", lengthLineEdit);

    // Ajoute un champ pour entrer la nouvelle hauteur de la ligne
    QLineEdit *heightLineEdit = new QLineEdit(updateDialog);
    formLayout->addRow("Hauteur :", heightLineEdit);

    // Ajoute un bouton pour valider les nouvelles données et fermer la fenêtre de dialogue
    QPushButton *updateButton = new QPushButton("Modifier", updateDialog);
    formLayout->addWidget(updateButton);

    // Fonction qui permet de définir le comportement à adopter lorsque l'utilisateur appuie sur la touche "Entrée" dans un champ de saisie
    auto handleEnterKeyPress = [](QLineEdit *current, QLineEdit *next) {
        QObject::connect(current, &QLineEdit::returnPressed, [current, next]() {
            // Si le champ en cours de saisie est rempli et que le champ suivant (s'il y en a un) est également rempli, déplace le curseur vers le champ suivant
            if (!current->text().isEmpty() && (next == nullptr || !next->text().isEmpty())) {
                if (next) {
                    next->setFocus();
                }
            }
        });
    };

    // Associe la fonction handleEnterKeyPress aux champs de saisie pour gérer les appuis sur la touche "Entrée"
    handleEnterKeyPress(rowNumberLineEdit, debitLineEdit);
    handleEnterKeyPress(debitLineEdit, diameterLineEdit);
    handleEnterKeyPress(diameterLineEdit, lengthLineEdit);
    handleEnterKeyPress(lengthLineEdit, heightLineEdit);
    handleEnterKeyPress(heightLineEdit, nullptr); // Le dernier champ n'a pas de champ suivant, donc next est nul


    // Fonction qui met à jour les données de la ligne avec les nouvelles données saisies par l'utilisateur et ferme la fenêtre de dialogue
    auto updateDataAndClose = [this, rowNumberLineEdit, debitLineEdit, diameterLineEdit, lengthLineEdit, heightLineEdit, updateDialog]() {
        // Récupère
        // Récupère les nouvelles données saisies par l'utilisateur
        int rowNumber = rowNumberLineEdit->text().toInt();
        float debit = debitLineEdit->text().toFloat();
        float diameter = diameterLineEdit->text().toFloat();
        float length = lengthLineEdit->text().toFloat();
        float height = heightLineEdit->text().toFloat();

        // Met à jour les données de la ligne avec les nouvelles données saisies
        updateData(rowNumber, debit, diameter, length, height);
        // Ferme la fenêtre de dialogue
        updateDialog->close();
    };

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
// puis met à jour les données de la ligne correspondante dans un pertechargeherse.
void pertechargeherse::updateData(int numeroLigne, float debit, float diametre, float longueur, float hauteur) {

    // Vérifie si le numéro de ligne est valide (c'est-à-dire s'il se trouve dans la plage d'index des données).
    if (numeroLigne >= 0 && numeroLigne < _Donnees.size()) {
        // Décrémente le numéro de ligne de 1 pour correspondre à l'indice de pertechargeherse (qui commence à 0).
        numeroLigne -= 1;

        // Met à jour les données de la ligne avec les nouvelles valeurs fournies.
        _Donnees[numeroLigne][1] = debit;
        _Donnees[numeroLigne][3] = diametre;
        _Donnees[numeroLigne][4] = longueur;
        _Donnees[numeroLigne][5] = hauteur;

        // Recalculer et rafraîchir le pertechargeherse.
        calcul();
        RafraichirTableau();
    }
}


void pertechargeherse::enleverLigne() {
    // Crée une nouvelle fenêtre de dialogue pour demander le numéro de la ligne à supprimer
    QDialog *supprimerDialog = new QDialog(this);
    supprimerDialog->setWindowTitle("Supprimer une ligne");
    supprimerDialog->setFixedSize(300, 100);

    // Ajoute un label et un champ de saisie pour entrer le numéro de la ligne à supprimer
    QLabel *numeroLigneLabel = new QLabel("Numéro de ligne :", supprimerDialog);
    QLineEdit *numeroLigneLineEdit = new QLineEdit(supprimerDialog);
    numeroLigneLineEdit->setValidator(new QIntValidator(1, _Donnees.size(), supprimerDialog)); // Permet de limiter la saisie à un nombre entier valide
    numeroLigneLineEdit->setFixedWidth(50);

    // Ajoute un bouton pour confirmer la suppression de la ligne
    QPushButton *supprimerButton = new QPushButton("Supprimer", supprimerDialog);
    connect(supprimerButton, &QPushButton::clicked, [this, numeroLigneLineEdit, supprimerDialog]() {
        int numeroLigne = numeroLigneLineEdit->text().toInt() - 1;
        if (numeroLigne >= 0 && numeroLigne < _Donnees.size()) {
            _Donnees.erase(_Donnees.begin() + numeroLigne); // Supprime la ligne du pertechargeherse
            // Réaffecte les numéros de ligne à partir de la ligne supprimée
            for (int i = numeroLigne; i < _Donnees.size(); ++i) {
                _Donnees[i][0] = i + 1;
            }
            RafraichirTableau(); // Actualise l'affichage du pertechargeherse
        }
        supprimerDialog->close();
    });

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



