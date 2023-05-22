
#include "etude.h"
float hauteur = 40;
float largueur = 150;
etude::etude(std::shared_ptr<bdd> db,QWidget *parent)
    : QWidget(parent),database(db)
{
    milieu = true;
    limitations = true;
    // Create and configure mainLayout
    mainLayout = new QVBoxLayout();

    setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");


    // Create the Init and Insert buttons
    QPushButton *initButton = new QPushButton("Init", this);
    QPushButton *insertButton = new QPushButton("Insérer", this);
    QPushButton *divideButton = new QPushButton("Diviser", this);

    // Connect the signals of the buttons to the init() and initCalcul() functions
    connect(initButton, &QPushButton::clicked, this, &etude::init);
    connect(insertButton, &QPushButton::clicked, this, &etude::initCalcul);
    connect(divideButton, &QPushButton::clicked, this, &etude::divideData);

    // Create a QHBoxLayout to contain the Init and Insert buttons and add them to the layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(initButton);
    buttonsLayout->addWidget(insertButton);
    buttonsLayout->addWidget(divideButton);

    QPushButton *showButton = new QPushButton("Afficher", this);
    buttonsLayout->addWidget(showButton);

    // Connecter le bouton "Afficher" à un nouveau slot "showOptionsDialog"
    connect(showButton, &QPushButton::clicked, this, &etude::showOptionsDialog);

    // Add the buttonsLayout to the beginning of the mainLayout
    mainLayout->insertLayout(0, buttonsLayout);

    // Create and configure the QScrollArea
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    // Create a widget to contain the contents of the QScrollArea
    scrollAreaWidgetContents = new QWidget();
    scrollArea->setWidget(scrollAreaWidgetContents);

    // Create and configure the QGridLayout
    gridLayout = new QGridLayout(scrollAreaWidgetContents);

    // Configure the QGridLayout to use 20 columns
    for (int i = 0; i < 20; ++i) {
        gridLayout->setColumnStretch(i, 1);
    }

    // Set the mainLayout as the layout for the etude class
    setLayout(mainLayout);
}

void etude::showOptionsDialog() {
    // Créer un QDialog pour les options
    QDialog optionsDialog(this);
    optionsDialog.setWindowTitle("Options");

    QCheckBox *milieuHydroCheckBox = new QCheckBox("Milieu Hydro", this);
    milieuHydroCheckBox->setChecked(milieu);
    QCheckBox *limitationParcelleCheckBox = new QCheckBox("Limitation parcelle", this);
    limitationParcelleCheckBox->setChecked(limitations);

    // Créer un layout pour le QDialog
    QVBoxLayout dialogLayout(&optionsDialog);
    dialogLayout.addWidget(milieuHydroCheckBox);
    dialogLayout.addWidget(limitationParcelleCheckBox);

    // Ajouter un bouton OK
    QPushButton okButton("OK");
    dialogLayout.addWidget(&okButton);

    // Connecter le bouton OK pour fermer le dialogue lorsque le bouton est cliqué
    connect(&okButton, &QPushButton::clicked, [&]() {
      optionsDialog.accept();
    });

    // Afficher le QDialog
    optionsDialog.exec();

    // Mettre à jour les options d'affichage selon les checkbox
    milieu = milieuHydroCheckBox->isChecked();
    limitations = limitationParcelleCheckBox->isChecked();

    // Rafraichir le tableau pour prendre en compte les nouvelles options d'affichage
    rafraichirTableau();
}


// Méthode pour extraire les données du texte
void etude::init()
{
    _Donnees.clear();
    _parcelles.clear();
    QDialog dialog(this);
    dialog.setWindowTitle("Importer les données");

        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLabel *dataLabel = new QLabel("Collez vos données:");
                        mainLayout->addWidget(dataLabel);

    QPlainTextEdit *dataEdit = new QPlainTextEdit();
    mainLayout->addWidget(dataEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Annuler");
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString data = dataEdit->toPlainText();

        // Remplacer les virgules par des points
        data.replace(',', '.');

        QStringList lines = data.split("\n");
        for (int i = 3; i < lines.size()-1; i++) {

            QStringList cols = lines[i].split(QRegExp("\\s+"));

            // Ignorer les lignes qui n'ont pas assez de colonnes
            if (cols.size() < 7) {
                continue;
            }

            std::cout<<cols[2].toFloat()<<" "<<cols[3].toFloat()<<" "<<cols[4].toFloat()<<" "<<cols[5].toFloat()<<" "<<cols[6].toFloat()<<" "<<cols[7].toFloat()<<std::endl;


            std::vector<float> rowData(20);
            rowData[0] = _Donnees.size()+1; // numero du rang
            rowData[1] = cols[3].toFloat(); // Longueur du rang
            rowData[2] = cols[4].toFloat(); // Nombre d'asperseu
            rowData[3] = cols[5].toFloat(); // Zamont du rang
            rowData[4] = cols[6].toFloat(); // Zaval du rang
            if(i==3){
                rowData[5] = 0;// Intervale rang début
                rowData[6] = 0; // Intervale rang fin
            } else {
                rowData[5] = cols[7].toFloat(); // Intervale rang début
                rowData[6] = cols[8].toFloat(); // Intervale rang fin
            }


            _Donnees.push_back(rowData);
        }
    }
    rafraichirTableau();
}

void etude::clearchild() {
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    std::cout<<"Clear finit"<<std::endl;
}


// La fonction "rafraichirTableau" permet de mettre à jour le tableau
// en affichant les nouvelles données.
void etude::rafraichirTableau() {

    if(_parcelles.size()!=0){
        updateDonnees(); // Met à jour les données
    }

    // Supprime toutes les cases du tableau.
    clearchild();

    // Initialize headers
    QStringList headers = {"Num", "Long","NbAsp", "Zamont", "Zaval", "InterD", "InterF","DebitG","Esp","DebitL","DebitC","PeigneZAm","PeigneZAv","oui","non"}; // Add as many headers as you need

    for (int i = 0; i < headers.size(); ++i) {
        QLabel *headerLabel = new QLabel(headers[i], this);
        headerLabel->setStyleSheet("QLabel { color: white; }");
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        headerLabel->setFixedHeight(hauteur);
        headerLabel->setFixedWidth(largueur);
        gridLayout->addWidget(headerLabel, 0, i);
    }

    // Initialise le numéro de ligne.
    int ligne = 1;

    std::vector<int> milieuxHydro;
    std::vector<int> limitesParcelles;
    int totalRows = 0;
    for (auto& parcel : _parcelles) {
        const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();
        totalRows += parcelData.size();
        milieuxHydro.push_back(totalRows - parcelData.size() / 2);
        limitesParcelles.push_back(totalRows);
    }
    // Ajoute les données au tableau.
    for (const std::vector<float> &donneesLigne : _Donnees) {

        // Détermine la couleur du texte.
        QString textColor = "QLineEdit { color: white; }";
        if (std::find(milieuxHydro.begin(), milieuxHydro.end(), ligne) != milieuxHydro.end() && milieu) {
            textColor = "QLineEdit { color: white; background-color : orange; }"; // Milieu hydrolique
        } else if (std::find(limitesParcelles.begin(), limitesParcelles.end(), ligne) != limitesParcelles.end() && limitations) {
            textColor = "QLineEdit { color: white;background-color : blue; }"; // Limite entre deux parcelles
        }

        for (int i = 0; i < donneesLigne.size(); ++i) {

            // Crée une nouvelle ligne de texte.
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setStyleSheet(textColor);

            // Centre le texte dans la ligne de texte.
            lineEdit->setAlignment(Qt::AlignCenter);

            // Définit la politique de redimensionnement de la ligne de texte.
            lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Définit la hauteur fixe de la ligne de texte.
            lineEdit->setFixedHeight(hauteur);

            lineEdit->setFixedWidth(largueur);
            QString formattedText = QString::number(donneesLigne[i], 'f', 2);
            lineEdit->setText(formattedText);

            // Définit l'espacement vertical du layout.
            gridLayout->setVerticalSpacing(0);

            gridLayout->addWidget(lineEdit, ligne, i);

        }

        // Incrémente le numéro de ligne.
        ligne++;
    }

    std::cout<<"insertion finit"<<std::endl;


    // Calcule la hauteur du widget de défilement et ajuste sa hauteur minimum et
    // maximum en conséquence.
    int scrollWidgetHeight = (ligne * hauteur) + ((ligne - 1) * 1);
    scrollAreaWidgetContents->setMinimumHeight(scrollWidgetHeight);
    scrollAreaWidgetContents->setMaximumHeight(scrollWidgetHeight);

    // Assure la visibilité de l'ensemble du widget de défilement.
    scrollArea->ensureVisible(0, scrollWidgetHeight);

    // Définit l'espacement vertical et l'alignement.
    gridLayout->setVerticalSpacing(0);
    gridLayout->setAlignment(Qt::AlignTop);
    std::cout<<"rafraichir finit"<<std::endl;

}


void etude::initCalcul() {
    // Crée un QDialog pour recueillir les valeurs.
    QDialog dialog(this);
    dialog.setWindowTitle("Entrer les valeurs");

    // Crée deux QLineEdit pour les valeurs.
    QLineEdit debitGoutteurLineEdit;
    QLineEdit espacementGoutteurLineEdit;

    // Crée un bouton OK.
    QPushButton okButton("OK");

    // Crée un layout pour le QDialog.
    QFormLayout formLayout(&dialog);
    formLayout.addRow("Débit goutteur:", &debitGoutteurLineEdit);
        formLayout.addRow("Espacement goutteur:", &espacementGoutteurLineEdit);
    formLayout.addWidget(&okButton);

    // Connecte le signal du bouton OK pour appeler la fonction 'calcul' lorsque le bouton est cliqué.
    connect(&okButton, &QPushButton::clicked, [&]() {
        QString debitGoutteurStr = debitGoutteurLineEdit.text().replace(",", ".");
        QString espacementGoutteurStr = espacementGoutteurLineEdit.text().replace(",", ".");

        float debitGoutteur = debitGoutteurStr.toFloat();
        float espacementGoutteur = espacementGoutteurStr.toFloat();

        for(int i=0;i<_Donnees.size();i++){
            _Donnees[i][7] = debitGoutteur;
            _Donnees[i][8] = espacementGoutteur;
        }
        calcul();
        std::cout<<"calcul finit"<<std::endl;
        dialog.accept();
    });

    // Affiche le QDialog.
    dialog.exec();
}


void etude::calcul(){

    float debitcumule = 0;

    float zamont;
    float zaval;

    float zamontO = _Donnees[0][3];
    float zavalO =  _Donnees[0][4];


    for(int i=0;i<_Donnees.size();i++){

        zamont = _Donnees[i][3];
        zaval = _Donnees[i][4];

        _Donnees[i][9] = _Donnees[i][1] * (_Donnees[i][7]/_Donnees[i][8]);
        debitcumule += _Donnees[i][9];

        _Donnees[i][10] = debitcumule;

        _Donnees[i][11] = zamont - zamontO;
        _Donnees[i][12] = zaval - zavalO;

        _Donnees[i][13] = zamont - zaval;
        _Donnees[i][14] = zaval - zamont;

    }
    rafraichirTableau();
    std::cout<<"calcul finit"<<std::endl;
}

void etude::divideData() {


    if(_parcelles.size()!=0){
        updateDonnees();
        _parcelles.clear();

    }


    // Crée un QDialog pour recueillir le nombre de parties.
    QDialog dialog(this);
    dialog.setWindowTitle("Entrer le nombre de parties");

    // Crée un QLineEdit pour le nombre.
    QLineEdit nombreparcelleLineEdit;

    // Crée un bouton OK.
    QPushButton okButton("OK");

    // Crée un layout pour le QDialog.
    QFormLayout formLayout(&dialog);
    formLayout.addRow("Nombre de parties:", &nombreparcelleLineEdit);
    formLayout.addWidget(&okButton);

    // Connecte le signal du bouton OK pour appeler la fonction de division lorsque le bouton est cliqué.
    connect(&okButton, &QPushButton::clicked, [&]() {
      int nombreparcelle = nombreparcelleLineEdit.text().toInt();

      // Calcule la longueur totale des rangs
      float longueurtotale = 0;
      for(const auto& row : _Donnees){
        longueurtotale += row[1];
      }

      // La longueur cible pour chaque parcelle
      float longueurcible = longueurtotale / nombreparcelle;

      // Indices de début et de fin pour chaque parcelle
      int startIndex = 0;
      int endIndex = 0;

      // Longueur cumulée pour la parcelle actuelle
      float currentLength = 0;

      // Création des parcelles
      for(int i = 0; i < _Donnees.size(); ++i){
        currentLength += _Donnees[i][1];
        if((currentLength >= longueurcible && i != startIndex) || i == _Donnees.size() - 1){
          endIndex = i;
          _parcelles.push_back(parcelle(_Donnees, startIndex, endIndex + 1));
          startIndex = i + 1;
          currentLength = 0;
        }
      }

      rafraichirTableau();
      dialog.accept();
    });

    // Affiche le QDialog.
    dialog.exec();
}



void etude::updateDonnees() {
    // Effacer _Donnees
    _Donnees.clear();

    // Parcourir chaque parcelle
    for (auto& parcel : _parcelles) {
        // Obtenir les données de la parcelle
        const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();

        // Ajouter les données de la parcelle à _Donnees
        _Donnees.insert(_Donnees.end(), parcelData.begin(), parcelData.end());
    }
}