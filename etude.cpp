
#include "etude.h"

etude::etude(std::shared_ptr<bdd> db,QWidget *parent)
    : QWidget(parent),database(db)
{
    // Create and configure mainLayout
    mainLayout = new QVBoxLayout();

    setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");


    // Create the Init and Insert buttons
    QPushButton *initButton = new QPushButton("Init", this);
    QPushButton *insertButton = new QPushButton("Insérer", this);

    // Connect the signals of the buttons to the init() and initCalcul() functions
    connect(initButton, &QPushButton::clicked, this, &etude::init);
    connect(insertButton, &QPushButton::clicked, this, &etude::initCalcul);

    // Create a QHBoxLayout to contain the Init and Insert buttons and add them to the layout
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(initButton);
    buttonsLayout->addWidget(insertButton);

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


// Méthode pour extraire les données du texte
void etude::init()
{
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
            rowData[5] = cols[7].toFloat(); // Intervale rang début
            rowData[6] = cols[8].toFloat(); // Intervale rang fin

            _Donnees.push_back(rowData);
        }
    }
    rafraichirTableau();
}

// La fonction "clearchild" supprime toutes les cases du tableau.
void etude::clearchild() {
    // Parcourt les éléments dans le gridLayout
    for (int i = 0; i < gridLayout->count(); ++i) {
        // Obtient l'élément à l'index i
        QLayoutItem *item = gridLayout->itemAt(i);

        // Supprime l'élément s'il existe
        if (item) {
            QWidget *widget = item->widget();
            if (widget) {
                // Supprime le widget de l'interface graphique
                widget->setParent(nullptr);
                // Supprime le widget de la mémoire
                delete widget;
            }
            // Supprime l'élément du layout
            gridLayout->removeItem(item);
            delete item;
        }
    }
    std::cout<<"Clear finit"<<std::endl;
}

// La fonction "rafraichirTableau" permet de mettre à jour le tableau
// en affichant les nouvelles données.
void etude::rafraichirTableau() {
    // Supprime toutes les cases du tableau.
    clearchild();

    // Initialise le numéro de ligne.
    int ligne = 1;

    // Ajoute les données au tableau.
    for (const std::vector<float> &donneesLigne : _Donnees) {

        // Détermine la couleur du texte.
        QString textColor = "QLineEdit { color: white; }";

        for (int i = 0; i < donneesLigne.size(); ++i) {

            // Crée une nouvelle ligne de texte.
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setStyleSheet(textColor);

            // Centre le texte dans la ligne de texte.
            lineEdit->setAlignment(Qt::AlignCenter);

            // Définit la politique de redimensionnement de la ligne de texte.
            lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            // Définit la hauteur fixe de la ligne de texte.
            lineEdit->setFixedHeight(40);

            lineEdit->setFixedWidth(100);
            QString formattedText = QString::number(donneesLigne[i], 'f', 2);
            lineEdit->setText(formattedText);

            // Définit l'espacement vertical du layout.
            gridLayout->setVerticalSpacing(1);

            gridLayout->addWidget(lineEdit, ligne, i);

            std::cout<<i<<std::endl;

        }

        // Incrémente le numéro de ligne.
        ligne++;
    }

    std::cout<<"insertion finit"<<std::endl;


    // Calcule la hauteur du widget de défilement et ajuste sa hauteur minimum et
    // maximum en conséquence.
    int scrollWidgetHeight = (ligne * 40) + ((ligne - 1) * 1);
    scrollAreaWidgetContents->setMinimumHeight(scrollWidgetHeight);
    scrollAreaWidgetContents->setMaximumHeight(scrollWidgetHeight);

    // Assure la visibilité de l'ensemble du widget de défilement.
    scrollArea->ensureVisible(0, scrollWidgetHeight);

    // Définit l'espacement vertical et l'alignement.
    gridLayout->setVerticalSpacing(1);
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

    float debit;
    float debitcumule;
    float peignezamont;
    float peignezaval;
    float lignerangamont;
    float lignerangaval;

    float zamont;
    float zaval;

    float zamontO = _Donnees[0][3];
    float zavalO =  _Donnees[0][4];


    for(int i=0;i<_Donnees.size();i++){

        zamont = _Donnees[i][3];
        zaval = _Donnees[i][4];

        _Donnees[i][10] = _Donnees[i][1] * (_Donnees[i][7]/_Donnees[i][8]);
        _Donnees[i][11] += _Donnees[i][10];

        _Donnees[i][12] = zamont - zamontO;
        _Donnees[i][13] = zaval - zavalO;


        _Donnees[i][14] = zamont - zaval;
        _Donnees[i][15] = zaval - zamont;

    }
    rafraichirTableau();
    std::cout<<"calcul finit"<<std::endl;
}




