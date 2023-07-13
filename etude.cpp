
#include "etude.h"

// Dimensions des cases en pixel
float hauteur = 30;
float largueur = 122.5;

etude::etude(std::shared_ptr<bdd> db, QWidget *parent)
        : QWidget(parent), database(db) {

    setWindowTitle(QString::fromStdString("Etude"));

    milieu = true;
    limitations = true;
    poste = true;
    premier = true;

    // MainLayout
    mainLayout = new QVBoxLayout();

    setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    // Init et Insert
    QPushButton *initButton = new QPushButton("Initialisation", this);
    QPushButton *divideButton = new QPushButton("Diviser", this);
    QPushButton *postebutton = new QPushButton("Poste", this);
    QPushButton *calcul = new QPushButton("Calcul", this);
    QPushButton *opti = new QPushButton("Optimiser", this);


    connect(initButton, &QPushButton::clicked, this, &etude::init);
    connect(divideButton, &QPushButton::clicked, this, &etude::divideData);
    connect(postebutton, &QPushButton::clicked, this, &etude::chooseCommandPost);
    connect(calcul, &QPushButton::clicked, this, &etude::appelSetDiametreDialog);
    connect(opti, &QPushButton::clicked, this, &etude::optimizeparcelle);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(initButton);
    buttonsLayout->addWidget(divideButton);
    buttonsLayout->addWidget(postebutton);
    buttonsLayout->addWidget(opti);
    buttonsLayout->addWidget(calcul);

    QPushButton *showButton = new QPushButton("Afficher", this);
    buttonsLayout->addWidget(showButton);

    // Connecter le bouton "Afficher" à un nouveau slot "showOptionsDialog"
    connect(showButton, &QPushButton::clicked, this, &etude::showOptionsDialog);

    mainLayout->insertLayout(0, buttonsLayout);

    // Entete scroll area
    QScrollArea *headerScrollArea = new QScrollArea(this);
    headerScrollArea->setFixedHeight(60);
    headerScrollArea->setWidgetResizable(true);
    headerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // Désactive le défilement horizontal
    headerScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // Désactive le défilement vertical

    QWidget *headerWidget = new QWidget();
    headerLayout = new QGridLayout(headerWidget);
    headerScrollArea->setWidget(headerWidget);

    // contenu Scroll Area
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    scrollAreaWidgetContents = new QWidget();
    gridLayout = new QGridLayout(scrollAreaWidgetContents);
    scrollArea->setWidget(scrollAreaWidgetContents);

    // Connection du défilement horizontal
    connect(scrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, headerScrollArea->horizontalScrollBar(), &QScrollBar::setValue);

    // Ajout des QScrollArea au layout principal
    mainLayout->addWidget(headerScrollArea);
    mainLayout->addWidget(scrollArea);

    // On crée les 40 colonnes
    for (int i = 0; i < 40; ++i) {
        gridLayout->setColumnStretch(i, 1);
        headerLayout->setColumnStretch(i, 1);
    }

    bottomlayout = new QGridLayout();


    QPushButton *save = new QPushButton("Sauvegarder", this);
    QPushButton *pdfbutton = new QPushButton("Export PDF", this);
    QPushButton *Debit = new QPushButton("Débit", this);
    QPushButton *Diametre = new QPushButton("Diametre", this);

    Debit->setFixedSize(140, 40);
    Diametre->setFixedSize(140, 40);
    save->setFixedSize(140, 40);
    pdfbutton->setFixedSize(140, 40);

    connect(save, &QPushButton::clicked, this, &etude::saveDataWrapper);
    connect(Debit, &QPushButton::clicked, this, &etude::changerDebitDialog);
    connect(Diametre, &QPushButton::clicked, this, &etude::changerDiametreDialog);
    connect(pdfbutton, &QPushButton::clicked, this, &etude::savePdf);

    bottomlayout->addWidget(save, 0, 4);
    bottomlayout->addWidget(pdfbutton, 0, 3);
    bottomlayout->addWidget(Debit, 0, 2);
    bottomlayout->addWidget(Diametre, 0, 1);
    mainLayout->addLayout(bottomlayout);


    setLayout(mainLayout);
}



void etude::showOptionsDialog() { // gerer les informations que l'ont souhaite afficher ou non

    // Créer un QDialog pour les options
    QDialog optionsDialog(this);
    optionsDialog.setWindowTitle("Options");

    QCheckBox *milieuHydroCheckBox = new QCheckBox("Milieu Hydro", this);
    milieuHydroCheckBox->setChecked(milieu);
    milieuHydroCheckBox->setStyleSheet("QCheckBox { color: green; }");

    QCheckBox *limitationParcelleCheckBox = new QCheckBox("Limitation parcelle", this);
    limitationParcelleCheckBox->setChecked(limitations);
    limitationParcelleCheckBox->setStyleSheet("QCheckBox { color: blue; }");

    QCheckBox *postecheckbox = new QCheckBox("Poste", this);
    postecheckbox->setChecked(poste);
    postecheckbox->setStyleSheet("QCheckBox { color:rgb(230, 255, 255) ; }");

    QCheckBox *premiere = new QCheckBox("Premier rang", this);
    premiere->setChecked(poste);
    premiere->setStyleSheet("QCheckBox { color : pink}");


    // Créer un layout pour le QDialog
    QVBoxLayout dialogLayout(&optionsDialog);
    dialogLayout.addWidget(milieuHydroCheckBox);
    dialogLayout.addWidget(limitationParcelleCheckBox);
    dialogLayout.addWidget(postecheckbox);
    dialogLayout.addWidget(premiere);

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
    poste = postecheckbox->isChecked();
    premier = premiere->isChecked();

    // Rafraichir le tableau pour prendre en compte les nouvelles options d'affichage
    rafraichirTableau();
}


void etude::init() {
    bool amont = true;

    QDialog choiceDialog(this);
    choiceDialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
    choiceDialog.setWindowTitle("Choisir la méthode d'importation");

    QVBoxLayout *choiceLayout = new QVBoxLayout(&choiceDialog);

    QRadioButton *pasteDataButton = new QRadioButton("Coller les données", &choiceDialog);
    QRadioButton *adddatas = new QRadioButton("Ajouter des données", &choiceDialog);
    QRadioButton *importFromFileButton = new QRadioButton("Importer depuis un fichier", &choiceDialog);

    choiceLayout->addWidget(pasteDataButton);
    choiceLayout->addWidget(adddatas);
    choiceLayout->addWidget(importFromFileButton);

    QHBoxLayout *choiceButtonLayout = new QHBoxLayout();
    QPushButton *choiceOkButton = new QPushButton("OK");
    QPushButton *choiceCancelButton = new QPushButton("Annuler");
    choiceButtonLayout->addWidget(choiceOkButton);
    choiceButtonLayout->addWidget(choiceCancelButton);
    choiceLayout->addLayout(choiceButtonLayout);

    connect(choiceOkButton, &QPushButton::clicked, &choiceDialog, &QDialog::accept);
    connect(choiceCancelButton, &QPushButton::clicked, &choiceDialog, &QDialog::reject);

    if (choiceDialog.exec() == QDialog::Accepted) {
        if (importFromFileButton->isChecked()) {
            loadDataWrapper();
        } else if (pasteDataButton->isChecked()) {
            QDialog dialog(this);
            dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
            dialog.setWindowTitle("Importer les données");

            QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

            QLabel *dataLabel = new QLabel("Collez vos données:");
            mainLayout->addWidget(dataLabel);

            QPlainTextEdit *dataEdit = new QPlainTextEdit();
            mainLayout->addWidget(dataEdit);

            QRadioButton *amontButton = new QRadioButton("Amont", &dialog);
            amontButton->setChecked(true);
            QRadioButton *avalButton = new QRadioButton("Aval", &dialog);

            mainLayout->addWidget(amontButton);
            mainLayout->addWidget(avalButton);


            QHBoxLayout *buttonLayout = new QHBoxLayout();
            QPushButton *okButton = new QPushButton("OK");
            QPushButton *cancelButton = new QPushButton("Annuler");
            buttonLayout->addWidget(okButton);
            buttonLayout->addWidget(cancelButton);
            mainLayout->addLayout(buttonLayout);

            connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
            connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

            if (dialog.exec() == QDialog::Accepted) {
                _Donnees.clear();
                _parcelles.clear();
                QString data = dataEdit->toPlainText();
                traitements(data);
                amont = amontButton->isChecked();
                QString nomParcelle = "parcelle 1";
                initCalcul();
                _parcelles.push_back(parcelle(_Donnees, 0, _Donnees.size(), database, nomParcelle, amont));
            }
        } else if(adddatas->isChecked()){
            ajouterDonnees();
        }
    }


    rafraichirTableau();

}


void etude::ajouterDonnees() {
    // Similaire à la partie "pasteDataButton->isChecked()" dans etude::init(),
    // mais cette fois, les nouvelles données sont ajoutées aux anciennes plutôt que de les remplacer.

    QDialog dialog(this);
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
    dialog.setWindowTitle("Ajouter des données");

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLabel *dataLabel = new QLabel("Collez vos données:");
    mainLayout->addWidget(dataLabel);

    QPlainTextEdit *dataEdit = new QPlainTextEdit();
    mainLayout->addWidget(dataEdit);

    QRadioButton *amontButton = new QRadioButton("Amont", &dialog);
    amontButton->setChecked(true);
    QRadioButton *avalButton = new QRadioButton("Aval", &dialog);

    mainLayout->addWidget(amontButton);
    mainLayout->addWidget(avalButton);


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
        int start = _Donnees.size() ;

        traitements(data);
        bool amont = amontButton->isChecked();

        // Ajouter une nouvelle parcelle avec les nouvelles données
        QString nomParcelle = "parcelle " + QString::number(_parcelles.size() + 1);
        int end = _Donnees.size();
        initCalcul();


        if (start < 0) start = 0;

        // Create the new parcel
        _parcelles.push_back(parcelle(_Donnees, start, end, database, nomParcelle, amont));
    }

}


void etude::traitements(QString data) {
    // Remplacer les virgules par des points
    data.replace(',', '.');

    QStringList lines = data.split("\n");
    for (int i = 0; i < lines.size() - 1; i++) {

        // Ignore les lignes qui commencent avec "Parcelle", "Intervalle", or "Nombre"
        if (lines[i].startsWith("Parcelle") || lines[i].startsWith("Intervalle") || lines[i].startsWith("Nombre")) {
            continue;
        }

        QStringList cols = lines[i].split(QRegExp("\\s+"));

        // Ignorer les lignes qui n'ont pas assez de colonnes
        if (cols.size() < 7) {
            continue;
        }

        std::vector<float> rowData(31);
        rowData[0] = cols[2].toInt(); // numero du rang
        rowData[1] = cols[3].toFloat(); // Longueur du rang
        rowData[2] = cols[4].toFloat(); // Nombre d'asperseurs
        rowData[3] = cols[5].toFloat(); // Zamont du rang
        rowData[4] = cols[6].toFloat(); // Zaval du rang
        if (i == 3) {
            rowData[5] = 0;// Intervale rang début
            rowData[6] = 0; // Intervale rang fin
        } else {
            if (cols.size() > 7) rowData[5] = cols[7].toFloat(); // Intervale rang début
            if (cols.size() > 8) rowData[6] = cols[8].toFloat(); // Intervale rang fin
        }
        _Donnees.push_back(rowData);
    }
}

void etude::updateAllLineEditsFromDonnees() {
    if (_parcelles.size() != 0) {
        updateDonnees(); // Met à jour les données
    }
    if (_Donnees.size() == 0) {
        return;
    }
    // Sauvegarder la position actuelle des barres de défilement
    int scrollPosVertical = scrollArea->verticalScrollBar()->value();
    int scrollPosHorizontal = scrollArea->horizontalScrollBar()->value();

    // Initialise le numéro de ligne.
    int ligne = 1;
    int totalRows = 0;
    std::vector<ParcelInfo> parcelInfos;
    for (auto &parcel: _parcelles) {
        ParcelInfo info;
        totalRows += parcel.getDonnees().size();
        const std::vector<std::vector<float>> &parcelData = parcel.getDonnees();
        info.milieuHydro = parcel.getMilieuhydro() + parcel.getIndexdebut() + 1;
        info.limiteParcelle = totalRows;
        info.commandPost = parcel.getPosteDeCommande() + parcel.getIndexdebut();
        info.nom = parcel.getNom();
        info.longueur = parcel.hectare();
        info.debit = parcel.getDebit();
        parcelInfos.push_back(info);
    }
    int parcelIndex = 0;

    // Parcourt les QLineEdit existants dans le tableau et actualise leur texte avec les données les plus récentes.
    for (int row = 1; row <= gridLayout->rowCount(); ++row) {
        for (int column = 0; column < gridLayout->columnCount(); ++column) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(row, column)->widget());
            if (lineEdit != nullptr) {
                const std::vector<float> &donneesLigne = _Donnees[row - 1];
                QString formattedText;
                if (column == 0) {
                    formattedText = QString::number(static_cast<int>(donneesLigne[column]));
                } else if (donneesLigne[column] == 0) {
                    formattedText = " ";
                } else {
                    formattedText = QString::number(donneesLigne[column], 'f', 2);
                }
                lineEdit->setText(formattedText);
            }
        }
    }

    // Met à jour les couleurs des QLineEdit existants dans le tableau.
    for (int row = 1; row <= gridLayout->rowCount(); ++row) {
        QString textColor = WHITE_TEXT;
        int distanceToNearestCommandPost = std::numeric_limits<int>::max();
        auto parcelInfoIt = std::find_if(parcelInfos.begin(), parcelInfos.end(),
                                         [row](const ParcelInfo &info) { return row <= info.limiteParcelle; });
        if (parcelInfoIt != parcelInfos.end()) {
            const ParcelInfo &info = *parcelInfoIt;
            if (row == info.commandPost && poste) {
                textColor = RED_TEXT; // Limite entre deux parcelles
            } else if (row == info.limiteParcelle && limitations) {
                textColor = BLUE_TEXT; // Poste de commande
            } else if (row == info.milieuHydro && milieu) {
                textColor = ORANGE_TEXT; // Milieu hydrolique
            } else if (row == 1 && premier) {
                textColor = PINK_TEXT; //premiere ligne d'une parcelle
            }
            distanceToNearestCommandPost = std::abs(row - info.commandPost);
        }
        for (int column = 0; column < gridLayout->columnCount(); ++column) {
            QLineEdit *lineEdit = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(row, column)->widget());
            if (lineEdit != nullptr) {
                lineEdit->setStyleSheet(textColor);
            }
        }
    }

    // Remet la position des barres de défilement.
    scrollArea->verticalScrollBar()->setValue(scrollPosVertical);
    scrollArea->horizontalScrollBar()->setValue(scrollPosHorizontal);
}

void etude::clearchild() {
    QLayoutItem *item;
    while ((item = gridLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->setParent(nullptr);
            delete widget;
        }
        delete item;
    }
    while ((item = headerLayout->takeAt(0)) != nullptr) {
        if (QWidget *widget = item->widget()) {
            widget->setParent(nullptr);
            delete widget;
        }
        delete item;
    }

}


void etude::rafraichirTableau() {

    if (_parcelles.size() != 0) {
        updateDonnees(); // Met à jour les données
    }

    if (_Donnees.size() == 0) {
        return;
    }

    // Sauvegarder la position actuelle des barres de défilement
    int scrollPosVertical = scrollArea->verticalScrollBar()->value();
    int scrollPosHorizontal = scrollArea->horizontalScrollBar()->value();
    // Supprime toutes les cases du tableau.
    clearchild();


    bool amont = true;
    if (_parcelles.size() > 0)
        amont = _parcelles[0].isAmont();

    int column = 2;
    std::vector<int> result;

    for (int i = 0; i < _Donnees.size(); ++i) {
        if (_Donnees[i].size() > column)
            result.push_back(_Donnees[i][column]);
    }

    bool tout0 = std::all_of(result.begin(), result.end(), [](int i) { return i == 0; });

    // Initialize headers
    QStringList headers;
    // Initialize headers
    if (tout0) {
        // Description du vecteur _Donnees en mode GAG :

        // _Donnees[i][0] - "No rang" : Le numéro du rang, utilisé pour identifier chaque rang.
        // _Donnees[i][1] - "Long" : La longueur du rang.
        // _Donnees[i][2] - "NbAsp" : Le nombre d'aspersions pour le rang.
        // _Donnees[i][3] - "Zamont" : L'altitude du côté amont du rang.
        // _Donnees[i][4] - "Zaval" : L'altitude du côté aval du rang.
        // _Donnees[i][5] - "InterRangD" : L'intervalle entre les rangs sur le côté droit.
        // _Donnees[i][6] - "InterRangF" : L'intervalle entre les rangs sur le côté gauche.
        // _Donnees[i][7] - "DebitG" : Le débit de l'eau sur le côté gauche.
        // _Donnees[i][8] - "Espacement" : L'espacement entre les asperseurs.
        // _Donnees[i][9] - "Q Ligne" : Le débit total de la ligne.
        // _Donnees[i][10] - "Σ Q Ligne" : Le débit cumulé de la ligne.
        // _Donnees[i][11] - "PeigneZAm" : L'altitude du peigne côté amont.
        // _Donnees[i][12] - "PeigneZAv" : L'altitude du peigne côté aval.
        // _Donnees[i][13] - "DeltaLigneAm" : Le delta de la ligne côté amont.
        // _Donnees[i][14] - "DeltaLigneAv" : Le delta de la ligne côté aval.
        // _Donnees[i][15] - "Diametre" : Le diamètre du tuyau de la ligne.
        // _Donnees[i][16] - "Colonne vide."
        // _Donnees[i][17] - "Hauteur" : Le dénivelé de la ligne par rapport au poste.
        // _Donnees[i][18] - "Vitesse" : La vitesse de l'eau dans la ligne.
        // _Donnees[i][19] - "J Peigne" : La perte de charge du peigne.
        // _Donnees[i][20] - "P Peigne" : Le Piezo du peigne.
        // _Donnees[i][21] - "Σ J" : La perte de charge cumulée.
        // _Donnees[i][22] - "Σ P" : Le Piezo cumulée.
        // _Donnees[i][23] -  "Colonne vide."
        // _Donnees[i][24] - "Longueur" : La longueur totale de la ligne.
        // _Donnees[i][25] - "DeltaLigneAm" : Le delta de la ligne côté amont.
        // _Donnees[i][26] - "DeltaLigneAv" : Le delta de la ligne côté aval.
        // _Donnees[i][27] - "J Ø16/14" : La perte de charge pour un diamètre de 16/14.
        // _Donnees[i][28] - "P Ø16" : Le Piezo pour un diamètre de 16.
        // _Donnees[i][29] - "J Ø20/17.6 La perte de charge pour un diamètre de 20/17.6.
        // _Donnees[i][30] - "P Ø20" : Le Piezo pour un diamètre de 20.

        headers << "No rang" << "Long" << "NbAsp" << "Zamont" << "Zaval" << "InterRangD" << "InterRangF" << "DebitG"
                << "Espacement" << "Q Ligne" << "Σ Q Ligne" << "PeigneZAm" << "PeigneZAv"<<"DeltaLigneAm"<< "DeltaLigneAv" <<  "Diametre" << "Nom" << "Hauteur" << "Vitesse" << "J Peigne" << "P Peigne"
                << "Σ J" << "Σ P" <<" Ligne "<<"Longueur"<<"ΔLigneAm"<< "ΔLigneAv" <<"J Ø16/14" << "P Ø16" << "J Ø20/17.6" << "P Ø20";
    } else {
        for (int i = 0; i < _Donnees.size(); i++) {
            _Donnees[i].resize(24);
        }

        // Description du vecteur _Donnees en mode asperssions:

        // _Donnees[i][0] - "No rang" : Le numéro du rang, utilisé pour identifier chaque rang.
        // _Donnees[i][1] - "Long" : La longueur du rang.
        // _Donnees[i][2] - "NbAsp" : Le nombre d'aspersions pour le rang.
        // _Donnees[i][3] - "Zamont" : L'altitude du côté amont du rang.
        // _Donnees[i][4] - "Zaval" : L'altitude du côté aval du rang.
        // _Donnees[i][5] - "InterRangD" : L'intervalle entre les rangs sur le côté droit.
        // _Donnees[i][6] - "InterRangF" : L'intervalle entre les rangs sur le côté gauche.
        // _Donnees[i][7] - "DebitG" : Le débit de l'eau sur le côté gauche.
        // _Donnees[i][8] - "Espacement" : L'espacement entre les aspersions.
        // _Donnees[i][9] - "Q Ligne" : Le débit total de la ligne.
        // _Donnees[i][10] - "Σ Q Ligne" : Le débit cumulé de la ligne.
        // _Donnees[i][11] - "PeigneZAm" : L'altitude du peigne côté amont.
        // _Donnees[i][12] - "PeigneZAv" : L'altitude du peigne côté aval.
        // _Donnees[i][13] - "DeltaLigneAm" : Le delta de la ligne côté amont.
        // _Donnees[i][14] - "DeltaLigneAv" : Le delta de la ligne côté aval.
        // _Donnees[i][15] - "Diametre" : Le diamètre du tuyau sur la ligne.
        // _Donnees[i][16] - "Colonne Vide".
        // _Donnees[i][17] - "Hauteur" : Le dénivelé de la ligne par rapport au poste.
        // _Donnees[i][18] - "Vitesse" : La vitesse de l'eau dans la ligne.
        // _Donnees[i][19] - "J Peigne" : La perte de charge du peigne.
        // _Donnees[i][20] - "P Peigne" : Le Piezo du peigne.
        // _Donnees[i][21] - "Σ J" : La perte de charge cumulée.
        // _Donnees[i][22] - "Σ P" : Le Piezo cumulée.
        // _Donnees[i][23] - "Afficher" : Bouton permettant de lancer le calcul sur la ligne.

        headers << "No rang" << "Long" << "NbAsp" << "Zamont" << "Zaval" << "InterRangD" << "InterRangF" << "DebitG"
                << "Espacement" << "Q Ligne" << "Σ Q Ligne" << "PeigneZAm" << "PeigneZAv" << "ΔLigneAm"
                << "ΔLigneAv" << "Diametre" << "Nom" << "Hauteur" << "Vitesse" << "J Peigne" << "P Peigne"
                << "Σ J" << "Σ P" << "Afficher";
    }

    for (int i = 0; i < headers.size(); ++i) {
        QLabel *headerLabel = new QLabel(headers[i], this);
        headerLabel->setStyleSheet("QLabel { color: white; font-size: 22px;font-weight: bold; }");
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        headerLabel->setFixedHeight(hauteur);
        headerLabel->setFixedWidth(largueur);
        headerLayout->addWidget(headerLabel, 0, i);
    }

    // Initialise le numéro de ligne.
    int ligne = 1;

    // Récupérations des infos des parcelles
    int totalRows = 0;
    std::vector<ParcelInfo> parcelInfos;
    for (auto &parcel: _parcelles) {
        ParcelInfo info;
        totalRows += parcel.getDonnees().size();
        const std::vector<std::vector<float>> &parcelData = parcel.getDonnees();
        info.milieuHydro = parcel.getMilieuhydro() + parcel.getIndexdebut() + 1;
        info.limiteParcelle = totalRows;
        info.commandPost = parcel.getPosteDeCommande() + parcel.getIndexdebut();
        info.nom = parcel.getNom();
        info.longueur = parcel.hectare();
        info.debit = parcel.getDebit();
        parcelInfos.push_back(info);
    }

    int parcelIndex = 0;  // Ajoutez ceci avant la boucle sur _parcelles

    // Ajoute les données au tableau.
    for (const std::vector<float> &donneesLigne: _Donnees) {


        // Détermine la couleur du texte.
        QString textColor = WHITE_TEXT; // CSS définit dans etude.h
        int distanceToNearestCommandPost = std::numeric_limits<int>::max();

        // Trouvez la parcelle à laquelle cette ligne appartient
        auto parcelInfoIt = std::find_if(parcelInfos.begin(), parcelInfos.end(),
                                         [ligne](const ParcelInfo &info) { return ligne <= info.limiteParcelle; });

        if (parcelInfoIt != parcelInfos.end()) {
            const ParcelInfo &info = *parcelInfoIt;
            if (ligne == info.commandPost && poste ) {
                textColor = RED_TEXT; // Limite entre deux parcelles
            } else if (ligne == info.limiteParcelle && limitations) {
                textColor = BLUE_TEXT; // Poste de commande
            } else if (ligne == info.milieuHydro && milieu) {
                textColor = ORANGE_TEXT; // Milieu hydrolique
            } else if (donneesLigne[0] == 1 && premier) {
                textColor = PINK_TEXT; //premiere ligne d'une parcelle
            }
            distanceToNearestCommandPost = std::abs(ligne - info.commandPost);
        }


        for (int i = 0; i < donneesLigne.size(); ++i) {

            if (i == 2 && tout0) {
                continue;
            }

            if (i == 16 || (i == 23 && tout0)) {
                QLabel *lineEdit = new QLabel(this);
                lineEdit->setStyleSheet(textColor);
                lineEdit->setAlignment(Qt::AlignCenter);
                lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                lineEdit->setFixedHeight(hauteur);
                lineEdit->setFixedWidth(largueur);
                gridLayout->setVerticalSpacing(0);
                gridLayout->addWidget(lineEdit, ligne, i);
            } else {
                QLineEdit *lineEdit = new QLineEdit(this);
                lineEdit->setStyleSheet(textColor);
                lineEdit->setAlignment(Qt::AlignCenter);
                lineEdit->setReadOnly(true);
                lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                lineEdit->setFixedHeight(hauteur);
                lineEdit->setFixedWidth(largueur);

                QString formattedText;
                if (i == 0) {
                    formattedText = QString::number(static_cast<int>(donneesLigne[i]));
                } else if (donneesLigne[i] == 0) {
                    formattedText = " ";
                } else if (i == 17 || i == 19 || i == 20 || i == 21 || i == 22 || i==24 || i==25 || i == 26 || i == 27 || i == 28 || i == 29 || i==30) {
                    formattedText = QString::number(donneesLigne[i], 'f', 2) + "m";
                } else if (i == 18) {

                    formattedText = QString::number(donneesLigne[i], 'f', 2) + "m/s";
                } else {
                    formattedText = QString::number(donneesLigne[i], 'f', 2);
                }


                lineEdit->setText(formattedText);
                gridLayout->setVerticalSpacing(0);
                gridLayout->addWidget(lineEdit, ligne, i);
                // Ajoute un signal pour la 16ème/5éme/6éme colonne
                if ((i == 15 || i == 5 || i == 6 || i == 9) || ((i == 23 || i ==2) && !tout0)) {
                    lineEdit->installEventFilter(this);
                    lineEdit->setStyleSheet("QLineEdit { color: yellow;font-size: 20px }");
                    lineEdit->setReadOnly(false);
                    // Connecte textEdited signal comme avant
                    if (i == 15) {
                        lineEdit->setReadOnly(false);

                        if (_Donnees[ligne - 1][2] <= 0 && !tout0) {
                            lineEdit->setVisible(false);
                        }

                        connect(lineEdit, &QLineEdit::textEdited, [this, ligne](const QString &newDiameter) {
                            this->updateDiameter(ligne - 1, newDiameter);
                        });
                    } else if (i == 9) {
                        lineEdit->setReadOnly(false);

                        connect(lineEdit, &QLineEdit::textEdited, [this, ligne, i](const QString &newDiameter) {
                            this->updateDebit(ligne - 1, newDiameter);
                        });
                    } else if (i==5 || i==6){
                        int colonne = i;
                        connect(lineEdit, &QLineEdit::textEdited, [this, ligne, colonne](const QString &newDiameter) {
                            this->updateinterval(ligne - 1, colonne, newDiameter);
                        });
                    }
                    if (!tout0) {
                        if(i == 23){
                            if (_Donnees[ligne - 1][2] != 0) {

                                int parcelIndex = std::distance(parcelInfos.begin(), parcelInfoIt);
                                QPushButton *afficher = new QPushButton("Afficher");
                                afficher->setStyleSheet("border: 1px solid white;");
                                gridLayout->addWidget(afficher, ligne, i);
                                connect(afficher, &QPushButton::clicked, [this, ligne, parcelIndex]() {
                                    this->_parcelles[parcelIndex].herse(ligne - 1);
                                });

                            } else {
                                lineEdit->setVisible(false);
                                lineEdit->setReadOnly(false);
                            }
                        } else if (i == 2) {
                            int parcelIndex = std::distance(parcelInfos.begin(), parcelInfoIt);

                            connect(lineEdit, &QLineEdit::textEdited, [this, ligne, parcelIndex](const QString &newnombre) {
                                this->_parcelles[parcelIndex].placerarrosseurs(ligne, newnombre.toInt() );
                            });
                        }

                    }

                } else if (i == 18) {
                    if (donneesLigne[i] > 2) {
                        lineEdit->setStyleSheet("QLineEdit { color: red; }");
                    }
                }
            }
        }


        // Vérifie si cette ligne est une limite de parcelle
        if (parcelInfoIt != parcelInfos.end() && ligne == parcelInfoIt->limiteParcelle) {
            ParcelInfo &info = *parcelInfoIt;
            int parcelIndex = std::distance(parcelInfos.begin(), parcelInfoIt);

            if (ligne >= 5) {
                /* QLineEdit *parcelNameLineEdit = createLineEdit(info.nom, textColor, this, false);
                 // Connecte le signal textChanged au slot setNom
                 connect(parcelNameLineEdit, &QLineEdit::textChanged, [this, parcelIndex](const QString &newName) {
                     this->_parcelles[parcelIndex].setNom(newName);
                 });
                 gridLayout->addWidget(parcelNameLineEdit, ligne, 16);  // Ajoute le QLineEdit à la 16e colonne
                 */
                QPushButton *calculparcelle = new QPushButton("Calcul", nullptr);
                calculparcelle->setStyleSheet("QPushButton {"
                                              "background-color: blue;"
                                              "color: white;"
                                              "border: none;"
                                              "}");
                connect(calculparcelle, &QPushButton::clicked, [this, parcelIndex]() {
                    this->_parcelles[parcelIndex].calcul();
                    rafraichirTableau();
                });
                gridLayout->addWidget(calculparcelle, ligne, 16);  // Ajoute le QLineEdit à la 16e colonne

                // Crée un QLineEdit pour la longueur de la parcelle
                QLineEdit *parcelLengthLineEdit = createLineEdit(QString::number(info.longueur, 'f', 2) + " Hec",
                                                                 textColor, this);
                gridLayout->addWidget(parcelLengthLineEdit, ligne - 1, 16);  // Ajoute le QLineEdit à la 17e colonne

                // Crée un QLineEdit pour le debit de la parcelle
                QString text;
                if (info.debit > 1000) {
                    text = QString::number(info.debit / 1000, 'f', 2) + " m3/h";
                } else {
                    text = QString::number(info.debit, 'f', 2) + " L/h";
                }
                QLineEdit *parcelDebitLineEdit = createLineEdit(text, textColor, this);
                gridLayout->addWidget(parcelDebitLineEdit, ligne - 2, 16);  // Ajoute le QLineEdit à la 18e colonne



                QComboBox *setamont = new QComboBox();
                setamont->addItem("  Amont");
                setamont->addItem("  Aval");
                setamont->setStyleSheet("QComboBox { background-color: blue; color: white; }");

                if (this->_parcelles[parcelIndex].isAmont()) {
                    setamont->setCurrentIndex(0);  // Index 0 corresponds à "Amont"
                } else {
                    setamont->setCurrentIndex(1);  // Index 1 corresponds à "Aval"
                }

                connect(setamont, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, parcelIndex](int index) {
                    bool isAmont = (index == 0);
                    this->_parcelles[parcelIndex].SetAmont(isAmont);
                });

                gridLayout->addWidget(setamont, ligne - 3, 16);  // Ajoute le QLineEdit à la 16e colonne

                QPushButton *inverse = new QPushButton("Inverser");
                inverse->setStyleSheet("QPushButton { background-color: blue; color: white; }");
                connect(inverse, &QPushButton::clicked, [this, parcelIndex]() {
                    this->_parcelles[parcelIndex].inverser();
                    rafraichirTableau();
                });

                if (!tout0) {
                    QString nbasp = QString::number(_parcelles[parcelIndex].nbasp()) + " Asp";
                    QLineEdit *qelineeditnbasp = createLineEdit(nbasp, textColor, this);
                    gridLayout->addWidget(qelineeditnbasp, ligne - 5, 16);  // Ajoute le QLineEdit à la 18e colonne
                }

                if (ligne == info.commandPost && poste) {
                    inverse->setStyleSheet("QPushButton { background-color: #e6ffff; color: black; }");
                    setamont->setStyleSheet("QComboBox { background-color: #e6ffff; color: black; }");
                    calculparcelle->setStyleSheet("QPushButton {"
                                                  "background-color: #e6ffff;"
                                                  "color: black;"
                                                  "border: none;"
                                                  "}");
                }

                gridLayout->addWidget(inverse, ligne - 4, 16);  // Ajoute le QLineEdit à la 16e colonne
            }
        }

        // Incrémente le numéro de ligne.
        ligne++;
    }



    // Contrôle de visibilité des colonnes après avoir rempli la grille
    if (amont) {
        for (int i = 0; i < gridLayout->columnCount(); ++i) {
            if ( i == 6 || i == 12 || i == 13 || i == 25) {
                for (int j = 0; j < gridLayout->rowCount(); ++j) {
                    if (gridLayout->itemAtPosition(j, i) && gridLayout->itemAtPosition(j, i)->widget()) {
                        gridLayout->itemAtPosition(j, i)->widget()->setVisible(false);
                    }
                }
                // Cache les entetes
                if (headerLayout->itemAtPosition(0, i) && headerLayout->itemAtPosition(0, i)->widget()) {
                    // Cache les entetes
                    if (headerLayout->itemAtPosition(0, i)) {
                        QLabel *label = qobject_cast<QLabel *>(headerLayout->itemAtPosition(0, i)->widget());
                        if (label) {
                            label->setVisible(false);
                        }
                    }

                }
            }
        }
    } else {
        for (int i = 0; i < gridLayout->columnCount(); ++i) {
            if ( i == 5 || i == 11 || i == 14 || i==24) {
                for (int j = 0; j < gridLayout->rowCount(); ++j) {
                    if (gridLayout->itemAtPosition(j, i) && gridLayout->itemAtPosition(j, i)->widget()) {
                        gridLayout->itemAtPosition(j, i)->widget()->setVisible(false);
                    }
                }
                if (headerLayout->itemAtPosition(0, i) && headerLayout->itemAtPosition(0, i)->widget()) {
                    // cache les entetes
                    if (headerLayout->itemAtPosition(0, i)) {
                        QLabel *label = qobject_cast<QLabel *>(headerLayout->itemAtPosition(0, i)->widget());
                        if (label) {
                            label->setVisible(false);
                        }
                    }

                }
            }
        }
    }


    // Contrôle de visibilité des colonnes après avoir rempli la grille
    if (tout0) {

        int i = 2;
        for (int j = 0; j < gridLayout->rowCount(); ++j) {
            if (gridLayout->itemAtPosition(j, i) && gridLayout->itemAtPosition(j, i)->widget()) {
                // cache les entetes
                if (gridLayout->itemAtPosition(0, i)) {
                    QLineEdit *label = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(0, i)->widget());
                    if (label) {
                        label->setVisible(false);
                    }
                }

            }
            if (headerLayout->itemAtPosition(0, i)) {
                QLabel *label = qobject_cast<QLabel *>(headerLayout->itemAtPosition(0, i)->widget());
                if (label) {
                    label->setVisible(false);
                }
            }
        }
    } else {

        // Nous définissons un ensemble de colonnes que nous voulons cacher.
        // Cela facilite l'ajout ou la suppression de colonnes à l'avenir.
        std::set<int> colonnesACacher = {7, 8, 9, 10};
        // Nous parcourons chaque colonne, de 0 à 10 (inclus).
        for (int i = 7; i < 24; i++) {
            if (colonnesACacher.count(i) > 0) {
                for (int j = 0; j < gridLayout->rowCount(); ++j) {
                    if (gridLayout->itemAtPosition(j, i)) {
                        QWidget* widget = gridLayout->itemAtPosition(j, i)->widget();
                        if (widget) {
                            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
                            if (lineEdit) {
                                lineEdit->setVisible(false);
                            }
                            else {
                                widget->setVisible(false);
                            }
                        }
                    }
                }
                if (headerLayout->itemAtPosition(0, i)) {
                    QLabel *label = qobject_cast<QLabel *>(headerLayout->itemAtPosition(0, i)->widget());
                    if (label) {
                        label->setVisible(false);
                    }
                }
            }
        }

    }

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

    // Restaurer les positions des barres de défilement après le rafraîchissement
    scrollArea->verticalScrollBar()->setValue(scrollPosVertical);
    scrollArea->horizontalScrollBar()->setValue(scrollPosHorizontal);

    setTabOrderForLineEdits();

    saveDataWrapper();
}

QLineEdit *etude::createLineEdit(const QString &text, const QString &style, QWidget *parent, bool readOnly) {
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setStyleSheet(style);
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setReadOnly(readOnly);
    lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lineEdit->setFixedHeight(hauteur);
    lineEdit->setFixedWidth(largueur);
    lineEdit->setText(text);
    return lineEdit;
}

void etude::initCalcul() {

    int column = 2;  // colonnes des aspersseurs
    std::vector<int> result;  // vecteur des résultats

    for (int i = 0; i < _Donnees.size(); ++i) {
        if (_Donnees[i].size() > column)
            result.push_back(_Donnees[i][column]);
    }

    bool tout0 = std::all_of(result.begin(), result.end(), [](int i) { return i == 0; });

    if (!tout0) {
        for (int i = 0; i < _Donnees.size(); i++) {
            _Donnees[i].resize(24);
        }
        milieu = false;
        calcul();
        return;
    } else {
        for (int i = 0; i < _Donnees.size(); i++) {
            _Donnees[i].resize(31);
        }
    }

    // Crée un QDialog pour recueillir les valeurs.
    QDialog dialog(this);
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
    dialog.setWindowTitle("Entrer les valeurs");

    // Crée un bouton OK.
    QPushButton okButton("OK");

    // Crée des boutons radio pour les valeurs par défaut
    QRadioButton default1Button("2.2L / 0.6m");
    default1Button.setChecked(true);
    QRadioButton default2Button("1.6L / 0.5m");

    QComboBox matierebox(this);

    QLineEdit *litre = new QLineEdit();
    litre->setFixedSize(60, 20);  // Ajoute une taille fixe au QLineEdit
    QLineEdit *espacement = new QLineEdit();
    espacement->setFixedSize(60, 20);  // Ajoute une taille fixe au QLineEdit

    QLabel *labellitre = new QLabel("Debit");
    QLabel *labelespacement = new QLabel("Espacement");

    matierebox.setFixedSize(60, 20);

    std::vector<std::string> matiere_names = database->getAllMatiereNames();
    for (const auto &matiere_name: matiere_names) {
        matierebox.addItem(QString::fromStdString(matiere_name));
    }

    // Crée un layout pour le QDialog.
    QFormLayout formLayout(&dialog);
    formLayout.addRow(&default1Button);
    formLayout.addRow(&default2Button);

    // Ajoute les QLineEdit et les labels au layout
    QHBoxLayout *litreLayout = new QHBoxLayout;
    litreLayout->addWidget(labellitre);
    litreLayout->addWidget(litre);
    formLayout.addRow(litreLayout);

    QHBoxLayout *espacementLayout = new QHBoxLayout;
    espacementLayout->addWidget(labelespacement);
    espacementLayout->addWidget(espacement);
    formLayout.addRow(espacementLayout);

    formLayout.addWidget(&okButton);

    // Connecte le signal du bouton OK pour appeler la fonction 'calcul' lorsque le bouton est cliqué.
    connect(&okButton, &QPushButton::clicked, [&]() {
        _matiere = matierebox.currentText().toStdString();
        float debitGoutteur = 0.0f;
        float espacementGoutteur = 0.0f;

        if (!litre->text().isEmpty() && !espacement->text().isEmpty()) {
            debitGoutteur = litre->text().replace(",", ".").toFloat();
            espacementGoutteur = espacement->text().replace(",", ".").toFloat();
        } else if (default1Button.isChecked()) {
            debitGoutteur = 2.2f;
            espacementGoutteur = 0.6f;
        } else if (default2Button.isChecked()) {
            debitGoutteur = 1.6f;
            espacementGoutteur = 0.5f;
        }

        for (int i = 0; i < _Donnees.size(); i++) {
            _Donnees[i][7] = debitGoutteur;
            _Donnees[i][8] = espacementGoutteur;
        }
        calcul();
        dialog.accept();
    });

    // Affiche le QDialog.
    dialog.exec();
}


void etude::calcul() {

    float debitcumule = 0;

    float zamont;
    float zaval;

    float zamontO = _Donnees[0][3];
    float zavalO = _Donnees[0][4];

    bool tout0 = true;
    for (int i = 0; i < _Donnees.size(); i++) {
        if (_Donnees[i][2] != 0) {
            tout0 = false;
        }

    }

    for (int i = 0; i < _Donnees.size(); i++) {

        zamont = _Donnees[i][3];
        zaval = _Donnees[i][4];
        _Donnees[i][11] = zamont - zamontO;
        _Donnees[i][12] = zaval - zavalO;

        _Donnees[i][13] = zamont - zaval;

        _Donnees[i][14] = zaval - zamont;

        if(tout0){
            _Donnees[i][24] = _Donnees[i][1];
            _Donnees[i][25] = zamont - zaval;
            _Donnees[i][26] = zaval - zamont;
            _Donnees[i][9] = _Donnees[i][1] * (_Donnees[i][7] / _Donnees[i][8]);
            debitcumule += _Donnees[i][9];
            _Donnees[i][10] = debitcumule;
        }

    }
}

#include <QMessageBox>

void etude::divideData(){
    int cpt=0;

    QMessageBox::StandardButton reply;
    if (_parcelles.size() > 1) {
        reply = QMessageBox::question(this, "Alert", "Êtes vous sûr de vouloir séparer les parcelles",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    std::string matiere = "";


    for (auto &parcel: _parcelles) {

        bool tout0 = true;
        for (int i = 0; i < _Donnees.size(); i++) {
            for (int j = 0; j < _Donnees[i].size(); j++) {
                if (_Donnees[i][j] == 0) {
                    tout0 = false;
                }
            }
        }

        std::vector<std::vector<float>> parcelData = parcel.getDonnees();
        for (auto &row: parcelData) {
            if (row.size() > 19) {
                row[15] = 0.0;
                row[16] = 0.0;
                row[17] = 0.0;
                row[18] = 0.0;
                row[19] = 0.0;
                row[20] = 0.0;
                row[21] = 0.0;
                row[22] = 0.0;
                row[23] = 0.0;
                if (tout0) {
                    row[24] = 0.0;
                    row[25] = 0.0;
                    row[26] = 0.0;
                }
            }
        }
        _Donnees.insert(_Donnees.end(), parcelData.begin(), parcelData.end());
    }


    bool amont = _parcelles[0].isAmont();
    updateDonnees();
    _parcelles.clear();

     QDialog dialog(this);
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
    dialog.setWindowTitle("Diviser les données");

    QRadioButton autoButton("Automatique");
    QRadioButton manualButton("Manuel");
    QLineEdit startIndexLineEdit;
    QLineEdit endIndexLineEdit;
    QLineEdit nombreparcelleLineEdit;

    QLabel remainingLinesLabel;

    QFormLayout formLayout(&dialog);
    formLayout.addRow(&autoButton);
    formLayout.addRow(&manualButton);

    formLayout.addRow("Nombre de parcelles:", &nombreparcelleLineEdit);
    formLayout.addRow("Index de début:", &startIndexLineEdit);
    formLayout.addRow("Index de fin:", &endIndexLineEdit);
    formLayout.addRow("Lignes restantes:", &remainingLinesLabel);

    autoButton.setChecked(true);
    startIndexLineEdit.setEnabled(false);
    endIndexLineEdit.setEnabled(false);

    connect(&autoButton, &QRadioButton::clicked, [&]() {
        startIndexLineEdit.setEnabled(false);
        endIndexLineEdit.setEnabled(false);
        nombreparcelleLineEdit.setEnabled(true);
    });

    connect(&manualButton, &QRadioButton::clicked, [&]() {
        startIndexLineEdit.setEnabled(true);
        endIndexLineEdit.setEnabled(true);
        nombreparcelleLineEdit.setEnabled(false);

    });

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    formLayout.addWidget(buttonBox);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);


    int dialogResult = dialog.exec();


    int nextStartIndex = 0;

    while (nextStartIndex < _Donnees.size()) {
        remainingLinesLabel.setText(QString::number(_Donnees.size() - nextStartIndex));

        if ( dialogResult== QDialog::Accepted) {
            if (autoButton.isChecked()) {
                int nombreparcelle = nombreparcelleLineEdit.text().toInt();

                // Calcule la longueur totale des rangs
                float longueurtotale = 0;
                for (const auto &row: _Donnees) {
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
                for (int i = 0; i < _Donnees.size(); ++i) {
                    currentLength += _Donnees[i][1];
                    if ((currentLength >= longueurcible && i != startIndex) || i == _Donnees.size() - 1) {
                        endIndex = i;
                        QString nomParcelle = QString("parcelle %1").arg(_parcelles.size() + 1);
                        _parcelles.push_back(parcelle(_Donnees, startIndex, endIndex + 1, database, nomParcelle,
                                                      amont, matiere));
                        startIndex = i + 1;
                        currentLength = 0;
                    }
                }
                nextStartIndex = _Donnees.size();
            } else if (manualButton.isChecked()) {
                int startIndex = startIndexLineEdit.text().toInt() - 1;
                int endIndex = endIndexLineEdit.text().toInt() - 1;

                if (startIndex >= nextStartIndex && endIndex < _Donnees.size() && startIndex < endIndex) {

                    QString nomParcelle = QString("parcelle %1").arg(_parcelles.size() + 1);
                    _parcelles.push_back(parcelle(_Donnees, startIndex, endIndex + 1, database, nomParcelle));

                    nextStartIndex = endIndex + 1;

                    startIndexLineEdit.setText(QString::number(endIndex + 2));
                    endIndexLineEdit.setText(QString::number(endIndex + 3));

                } else {
                }
            }
        } else {
            break;
        }
    }



    rafraichirTableau();
}

void etude::setTabOrderForLineEdits() {
    QLineEdit *previousLineEdit = nullptr;
    QLineEdit *firstLineEdit = nullptr;

    int column = 2;
    std::vector<int> result;
    for (int i = 0; i < _Donnees.size(); ++i) {
        if (_Donnees[i].size() > column)
            result.push_back(_Donnees[i][column]);
    }
    bool tout0 = std::all_of(result.begin(), result.end(), [](int i) { return i == 0; });

    for (int j = 0; j < gridLayout->columnCount(); ++j) {
        if (j == 15 || j == 5 || j == 6 || j == 9 || ((j == 23 || j == 2) && !tout0)) {
            for (int i = 0; i < gridLayout->rowCount(); ++i) {
                QLayoutItem *item = gridLayout->itemAtPosition(i, j);
                if (item) {
                    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(item->widget());
                    if (lineEdit) {
                        if (!firstLineEdit) {
                            firstLineEdit = lineEdit;
                        }
                        if (previousLineEdit) {
                            QWidget::setTabOrder(previousLineEdit, lineEdit);
                        }
                        previousLineEdit = lineEdit;
                    }
                }
            }
        }
    }

    // reviens au premier LineEdit
    if (previousLineEdit && firstLineEdit) {
        QWidget::setTabOrder(previousLineEdit, firstLineEdit);
    }
}




void etude::updateDonnees() {
    // Effacer _Donnees
    _Donnees.clear();

    // Parcourir chaque parcelle
    for (auto &parcel: _parcelles) {
        // Obtenir les données de la parcelle
        const std::vector<std::vector<float>> &parcelData = parcel.getDonnees();

        // Ajouter les données de la parcelle à _Donnees
        _Donnees.insert(_Donnees.end(), parcelData.begin(), parcelData.end());
    }
}


void etude::chooseCommandPost() {
    // Créer un QDialog pour le choix du poste de commande
    QDialog dialog(this);
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    dialog.setWindowTitle("Choisir le poste de commande");

    // Créer un layout pour le QDialog
    QVBoxLayout dialogLayout(&dialog);

    // Créer deux QMap pour mapper les QComboBox aux parcelles correspondantes
    QMap<QComboBox *, parcelle *> comboBoxToParcelMap;
    QMap<QComboBox *, parcelle *> sideComboBoxToParcelMap;

    int globalIndex = 1; // L'index commence à 1

    for (auto &parcel: _parcelles) {
        const std::vector<std::vector<float>> &parcelData = parcel.getDonnees();
        if (!parcelData.empty() && !parcelData[0].empty()) {
            QComboBox *rangeIndexComboBox = new QComboBox(&dialog);
            QComboBox *sideComboBox = new QComboBox(&dialog);

            // Ajouter les options pour choisir le côté
            sideComboBox->addItem("Au dessus");
            sideComboBox->addItem("En dessous");

            int defaultIndex = -1;
            int commandPost = parcel.getPosteDeCommande() + parcel.getIndexdebut();
            int decalage = parcel.getDecalage();


            if(decalage==0){
                sideComboBox->setCurrentIndex(0);
            } else {
                sideComboBox->setCurrentIndex(1);

            }

            for (size_t i = 1; i < parcelData.size() + 1; ++i) {
                if(i != 0) {
                    rangeIndexComboBox->addItem(QString::number(globalIndex));
                } else {
                    rangeIndexComboBox->addItem("");
                }
                ++globalIndex;
            }

            if (commandPost != -1 && commandPost != 0) {
                defaultIndex = commandPost;
            }

            if (defaultIndex != -1) {
                rangeIndexComboBox->setCurrentIndex(rangeIndexComboBox->findText(QString::number(defaultIndex)));
            } else {
                rangeIndexComboBox->setCurrentIndex(rangeIndexComboBox->findText(""));
            }


            // Ajouter les comboboxes au layout du QDialog
            QHBoxLayout *parcelLayout = new QHBoxLayout();
            parcelLayout->addWidget(rangeIndexComboBox);
            parcelLayout->addWidget(sideComboBox);
            dialogLayout.addLayout(parcelLayout);

            // Ajouter les comboboxes et la parcelle correspondante à la QMap
            comboBoxToParcelMap.insert(rangeIndexComboBox, &parcel);
            sideComboBoxToParcelMap.insert(sideComboBox, &parcel);
        }
    }

    // Créer un bouton OK
    QPushButton okButton("OK", &dialog);
    dialogLayout.addWidget(&okButton);

    // Connecter le bouton OK pour mettre à jour le poste de commande lorsque le bouton est cliqué
    connect(&okButton, &QPushButton::clicked, [&]() {
        // Pour chaque combobox dans la QMap
        for (auto &comboBox: comboBoxToParcelMap.keys()) {
            // Mettre à jour le poste de commande avec la valeur sélectionnée dans la combobox
            int rangeIndex = comboBox->currentText().toInt(); // Soustraire 1 pour obtenir l'index d'origine
            parcelle *selectedParcel = comboBoxToParcelMap.value(comboBox);
            if (selectedParcel) {
                if (rangeIndex == 0) { // Si l'utilisateur n'a pas entré de valeur
                    int milieuHydro = selectedParcel->getMilieuhydro();
                    if (milieuHydro == 0) { // Si le milieu hydraulique est 0
                        // Placer le poste de commande au milieu de la parcelle
                        rangeIndex = selectedParcel->getDonnees().size() / 2;
                    } else {
                        // Utiliser le milieu hydraulique comme poste de commande
                        rangeIndex = milieuHydro;
                    }
                }
                selectedParcel->setPosteDeCommande(rangeIndex);
            }
        }

        for (auto &comboBox: sideComboBoxToParcelMap.keys()) {
            // Mettre à jour le côté avec la valeur sélectionnée dans la combobox
            QString side = comboBox->currentText();
            parcelle *selectedParcel = sideComboBoxToParcelMap.value(comboBox);
            if (selectedParcel) {
                selectedParcel->choisirCote(side == "Au dessus" ? 0 : 1);
            }
        }

        updateDonnees();
        rafraichirTableau();
        dialog.accept();
    });

    // Afficher le QDialog
    dialog.setLayout(&dialogLayout);
    dialog.exec();
}


void etude::appelSetDiametreDialog() {
    QDialog dialog;
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    QVBoxLayout layout;
    QComboBox comboBox;
    QPushButton launchButton("Lancer");

    // On remplit avec les noms des parcelles
    for (size_t i = 0; i < _parcelles.size(); ++i) {
        comboBox.addItem(_parcelles[i].getNom());
    }

    layout.addWidget(&comboBox);
    layout.addWidget(&launchButton);
    dialog.setLayout(&layout);

    QObject::connect(&launchButton, &QPushButton::clicked,
                     [this, &comboBox, &dialog]() {
                         int index = comboBox.currentIndex();
                         if (index >= 0 && index < _parcelles.size()) {
                             _parcelles[index].setDiametreDialog(_matiere);
                             updateDonnees();
                             rafraichirTableau();
                         }
                         dialog.accept(); // ferme le dialogue
                     });

    dialog.exec();
}

void etude::updateDiameter(int row, const QString &newDiameter) {
    float diameter = newDiameter.toFloat();
    for (auto &parcel: _parcelles) {
        if (row < parcel.getDonnees().size()) {
            parcel.modifiedia(row, diameter);
            return;
        }
        row -= parcel.getDonnees().size();
    }
}

void etude::updateDebit(int row, const QString &newDiameter) {
    float diameter = newDiameter.toFloat();
    for (auto &parcel: _parcelles) {
        if (row < parcel.getDonnees().size()) {
            parcel.modifiedebit(row, diameter);
            return;
        }
        row -= parcel.getDonnees().size();
    }
}

void etude::updateinterval(int row, int ligne, const QString &newDiameter) {

    QString temp = newDiameter;
    float diameter = temp.replace(",", ".").toFloat();

    for (auto &parcel: _parcelles) {
        if (row < parcel.getDonnees().size()) {
            parcel.modifieinter(row, ligne, diameter);
            return;
        }
        row -= parcel.getDonnees().size();
    }
}

#include <QTimer>
#include <QFileDialog>
#include <QDir>


void etude::saveDataWrapper() {
    if (_currentFileName.empty()) {
        QString fileName = QFileDialog::getSaveFileName(
                this, "Save Data", QDir::homePath(), "Data Files (*.dat)");
        if (!fileName.isEmpty()) {
            _currentFileName = fileName.toStdString();
        }
    }
    if (!_currentFileName.empty()) {
        saveToFile(_currentFileName);
    }
}

void etude::loadDataWrapper() {
    QString fileName = QFileDialog::getOpenFileName(
            this, "Load Data", QDir::homePath(), "Data Files (*.dat)");

    if (!fileName.isEmpty()) {
        readFromFile(fileName.toStdString());
    }
}

bool etude::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::FocusIn) {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
        if (lineEdit) {
            QTimer::singleShot(0, lineEdit, SLOT(selectAll()));
        }
    } else if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_D) {
            changerDiametreDialog();
            return true;
        }
        if (keyEvent->key() == Qt::Key_S) {
            saveDataWrapper();
            return true;
        }
        if (keyEvent->key() == Qt::Key_O) {
            changerDebitDialog();
            rafraichirTableau();
            return true;
        }
        if (keyEvent->key() == Qt::Key_L) {
            loadDataWrapper();
            return true;
        }

    }
    return QObject::eventFilter(obj, event);
}

void etude::keyPressEvent(QKeyEvent *event) {

    double scaleFactor = 1.2;  // Changez-le selon vos besoins
    double scaleDownFactor = 1 / scaleFactor;
    QSize newSize;
    switch (event->key()) {
        case Qt::Key_D:
            changerDiametreDialog();
            break;
        case Qt::Key_S:
            saveDataWrapper();
            break;
        case Qt::Key_O:
            changerDebitDialog();
            rafraichirTableau();
            break;
        case Qt::Key_L:
            loadDataWrapper();
            break;
        case Qt::Key_P:
            savePdf();
            break;
        case Qt::Key_A:
            optimizeparcelle();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}


void etude::modifierdiametre(int debut, int fin, float dia) {
    if (debut > fin) {
        std::swap(debut, fin);
    }
    int i = 0;
    for (auto &parcelle: _parcelles) {
        auto &datas = parcelle.getDonnees();
        for (int j = 0; j < datas.size(); j++) {
            if (i >= debut && i <= fin) {
                parcelle.modifiedia(j, dia);
                datas[j][15] = dia;
            }
            i++;
        }
    }

    rafraichirTableau();
}


void etude::modifierdebit(int debut, int fin, float dia) {
    if (debut > fin) {
        std::swap(debut, fin);
    }
    int i = 0;
    for (auto &parcelle: _parcelles) {
        auto &datas = parcelle.getDonnees();
        for (int j = 0; j < datas.size(); j++) {
            if (i >= debut && i <= fin) {
                parcelle.modifiedebit(j, dia);
                datas[j][9] = dia;

            }
            i++;
        }
    }
    rafraichirTableau();
}

[[maybe_unused]] void etude::modifierarro(int ligne, float nombre) {
    int i = 0;

    for (auto &parcelle: _parcelles) {
        auto &datas = parcelle.getDonnees();
        for (int j = 0; j < datas.size(); j++) {
            if(i==(ligne-1)){
                datas[i][2] = nombre;
                break;
            }
            i++;
        }

    }
    rafraichirTableau();
}

void etude::doubledebit(float multi) {
    for (auto &parcelle: _parcelles) {
        parcelle.doubledebit(multi);
    }
    rafraichirTableau();
}

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QIntValidator>


void etude::changerDiametreDialog() {
    QDialog dialog(nullptr);
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    dialog.setWindowTitle("Changer diametre");

    std::string d = std::to_string(_Donnees.size());
    QLineEdit indiceDebutLineEdit(QString::fromStdString("1"));
    QLineEdit indiceFinLineEdit(QString::fromStdString(d));
    QLineEdit diametreLineEdit;

    QPushButton button("Appliquer");

    QVBoxLayout layout;
    layout.addWidget(new QLabel("Indice de début:"));
    layout.addWidget(&indiceDebutLineEdit);
    layout.addWidget(new QLabel("Indice de fin:"));
    layout.addWidget(&indiceFinLineEdit);
    layout.addWidget(new QLabel("Nouveau diametre:"));
    layout.addWidget(&diametreLineEdit);
    layout.addWidget(&button);
    dialog.setLayout(&layout);

    QObject::connect(&button, &QPushButton::clicked, [&]() {
        int debut = indiceDebutLineEdit.text().toInt() - 1;
        int fin = indiceFinLineEdit.text().toInt() - 1;
        float diametre = diametreLineEdit.text().toFloat();
        modifierdiametre(debut, fin, diametre);
        dialog.close();
    });

    dialog.exec();
}

void etude::changerDebitDialog() {
    QDialog dialog(nullptr);
    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
    dialog.setWindowTitle("Changer debit");

    // Créer les QLineEdit
    std::string d = std::to_string(_Donnees.size());
    QLineEdit indiceDebutLineEdit(QString::fromStdString("1"));
    QLineEdit indiceFinLineEdit(QString::fromStdString(d));
    QLineEdit diametreLineEdit;
    QLineEdit multiplicateurLineEdit; // Ajouter un QLineEdit pour le multiplicateur

    QPushButton button("Appliquer");
    QPushButton doubleButton("Multiplier"); // Changer le texte du bouton à "Multiplier"

    QVBoxLayout layout;
    layout.addWidget(new QLabel("Indice de début:"));
    layout.addWidget(&indiceDebutLineEdit);
    layout.addWidget(new QLabel("Indice de fin:"));
    layout.addWidget(&indiceFinLineEdit);
    layout.addWidget(new QLabel("Nouveau débit:"));
    layout.addWidget(&diametreLineEdit);
    layout.addWidget(new QLabel("Multiplicateur:")); // Ajouter un QLabel pour le multiplicateur
    layout.addWidget(&multiplicateurLineEdit); // Ajouter le QLineEdit pour le multiplicateur
    layout.addWidget(&button);

    // Ajoute le bouton "Multiplier" au layout
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addStretch(1);  // Force le bouton à se déplacer à droite
    topLayout->addWidget(&doubleButton);
    layout.insertLayout(0, topLayout); // Ajoute le layout en haut du QVBoxLayout

    dialog.setLayout(&layout);

    // Connecte le bouton "Multiplier" à la fonction doubledebit
    QObject::connect(&doubleButton, &QPushButton::clicked, this, [&]() {
        float multiplicateur = multiplicateurLineEdit.text().toFloat();
        doubledebit(multiplicateur);
    });

    // Quand le bouton est clické, appelle la fonction avec les paramètres
    QObject::connect(&button, &QPushButton::clicked, [&]() {
        int debut = indiceDebutLineEdit.text().toInt() - 1;
        int fin = indiceFinLineEdit.text().toInt() - 1;
        float diametre = diametreLineEdit.text().toFloat();
        modifierdebit(debut, fin, diametre);
        dialog.close();
    });

    dialog.exec();
}



void etude::refresh() {
    clearchild();
    _Donnees.clear();
    _parcelles.clear();
    milieu = true;
    limitations = true;
    poste = true;
}


#include <QPainter>
#include <QPdfWriter>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>


void etude::exportPdf(const QString &fileName) {

    std::vector<std::pair<int, int>> postes;

    for (auto &i: _parcelles) {
        postes.push_back(std::make_pair(i.getvraiindiceposte(), i.getDecalage()));
    }

    std::vector<int> parcelles;

    for (auto &i: _parcelles) {
        parcelles.push_back(i.getIndexfin());
    }

    QDialog inputDialog;
    inputDialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    inputDialog.setWindowTitle("Entrez les informations");

    QLabel *nomLabel = new QLabel("Nom:");
    QLabel *prenomLabel = new QLabel("Prénom:");
    QLabel *referenceLabel = new QLabel("Référence:");
    QLabel *dateLabel = new QLabel("Date:");

    QLineEdit *nomLineEdit = new QLineEdit;
    QLineEdit *prenomLineEdit = new QLineEdit;
    QLineEdit *referenceLineEdit = new QLineEdit;
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate());

    QDialogButtonBox *buttonBox =
            new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
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

    const int lineHeight = 300;
    const int footerHeight = 500;

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

    int column = 2;
    std::vector<int> result;

    for (int i = 0; i < _Donnees.size(); ++i) {
        if (_Donnees[i].size() > column)
            result.push_back(_Donnees[i][column]);
    }

    bool tout0 = std::all_of(result.begin(), result.end(), [](int i) { return i == 0; });
    QStringList headers;
    std::vector<int> indices;
    QVector<int> columnWidth;

    // Définir la taille de police pour l'en-tête
    QFont contentfont = painter.font();

    if (tout0) {
        if (_parcelles[0].isAmont()) {
            headers << "Num" << "Long" << "Zamont" << "InterRangD" << "DebitG" << "Espacement" << "Q Ligne"
                    << "Σ Q Ligne" << "PeigneZam" << "Diametre" << "DenivelePeigne" << "Vitesse" << "Perte J"
                    << "Piezo P" << "Σ Perte J" << "Σ Piezo P";
            indices = {0, 1, 3, 5, 7, 8, 9, 10, 11, 15, 17, 18, 19, 20, 21, 22};

        } else {
            headers << "Num" << "Long" << "Zaval" << "InterRangF" << "DebitG" << "Espacement" << "Q Ligne"
                    << "Σ Q Ligne" << "PeigneZav" << "Diametre" << "DenivelePeigne" << "Vitesse" << "Perte J"
                    << "Piezo P" << "Σ Perte J" << "Σ Piezo P";
            indices = {0, 1, 4, 6, 7, 8, 9, 10, 12, 15, 17, 18, 19, 20, 21, 22};
        }
        contentfont.setPointSize(8); // change this to desired font size

        columnWidth = {400, 600, 600, 600, 500, 600, 600, 600, 650, 600, 600, 600, 600, 600, 600, 600};
    } else {
        if (_parcelles[0].isAmont()) {
            headers << "Num" << "Long" << "NbAsp" << "Zamont" << "InterRangD" << "PeigneZam" << "Diametre"
                    << "DenivelePeigne" << "Vitesse" << "Perte J" << "Piezo P" << "Σ Perte J" << "Σ Piezo P";
            indices = {0, 1, 2, 3, 5, 11, 15, 17, 18, 19, 20, 21, 22};
        } else {
            headers << "Num" << "Long" << "NbAsp" << "Zaval" << "InterRangF" << "PeigneZav" << "Diametre"
                    << "DenivelePeigne" << "Vitesse" << "Perte J" << "Piezo P" << "Σ Perte J" << "Σ Piezo P";
            indices = {0, 1, 2, 4, 6, 12, 15, 17, 18, 19, 20, 21, 22};
        }
        contentfont.setPointSize(10);
        columnWidth = {400, 600, 600, 600, 900, 900, 800, 1100, 650, 600, 600, 800, 800};
    }

    painter.setFont(contentfont);


    int tableWidth = 0;
    for (int width: columnWidth) {
        tableWidth += width;
    }

    // Définir la taille de police pour l'en-tête
    QFont headerFont = painter.font();
    if (tout0) {
        headerFont.setPointSize(6);

    } else {
        headerFont.setPointSize(8);

    }
    painter.setFont(headerFont);

    int availablePageHeight = pdfWriter.height() - lineHeight * 4 - footerHeight;
    int rowsPerPage = availablePageHeight / lineHeight;
    int pageCount = ceil((double) _Donnees.size() / rowsPerPage);
    int currentY = 0;
    int currentX = 0;

    int yOffset = lineHeight * 5;
    int currentpage = 1;

    painter.drawText(0, lineHeight, QString("Nom: %1").arg(nom));
    painter.drawText(0, lineHeight * 2, QString("Prénom: %1").arg(prenom));
    painter.drawText(0, lineHeight * 3, QString("Référence: %1").arg(reference));
    painter.drawText(pdfWriter.width() - 1000, lineHeight - 200, QString("Date: %1").arg(date));
    if (!tout0) {
        painter.drawText(pdfWriter.width() - 1000, lineHeight,
                         QString::number(_parcelles[0].getAspdebit()) + QString("m3/h"));
        painter.drawText(pdfWriter.width() - 1000, lineHeight + 200,
                         QString::number(_parcelles[0].getAspinterdebut()) + QString("m Entre ASP"));
    }

    auto drawHeaderAndLines = [&](int yOffset) {
        int xPos = 0;
        for (int i = 0; i < headers.size(); ++i) {
            QRect headerRect(xPos, yOffset - lineHeight, columnWidth[i], lineHeight);
            painter.drawText(headerRect, Qt::AlignCenter, headers[i]);
            xPos += columnWidth[i];
        }

        yOffset += lineHeight;

        // Dessiner une ligne horizontale pour séparer l'en-tête et les nombres
        painter.setPen(QPen(Qt::black, 1));
        painter.drawLine(0, yOffset - lineHeight, tableWidth, yOffset - lineHeight);

        // Détermine le nombre de ligne par page
        int maxRowsPerPage = (pdfWriter.height() - yOffset - 150) / lineHeight;

        // Calculez le nombre de lignes restantes pour déterminer si vous êtes sur
        // la dernière page
        int remainingRows = _Donnees.size() - (currentpage - 1) * maxRowsPerPage;
        bool isLastPage = (remainingRows <= maxRowsPerPage);

        // Ajustez la hauteur des lignes verticales pour la dernière page en
        // fonction du nombre de lignes restantes
        int verticalLinesHeight = isLastPage ? (lineHeight * (remainingRows - 1))
                                             : (lineHeight * (maxRowsPerPage - 1));

        // Dessinez des lignes verticales pour séparer les colonnes
        xPos = 0;
        for (int i = 0; i < columnWidth.size(); ++i) {
            painter.drawLine(xPos, yOffset - lineHeight, xPos,
                             yOffset + verticalLinesHeight);
            xPos += columnWidth[i];
        }
        painter.drawLine(xPos, yOffset - lineHeight, xPos,
                         yOffset + verticalLinesHeight);

        // Réinitialiser la taille de police pour le reste du contenu
        painter.setFont(font);

        return yOffset;
    };

    yOffset = drawHeaderAndLines(yOffset);

    painter.setFont(headerFont);

    for (const std::vector<float> &donneesLigne: _Donnees) {

        painter.setFont(headerFont);

        if (std::find(parcelles.begin(), parcelles.end(), donneesLigne[0]) != parcelles.end()) {
            painter.setPen(QPen(QColor("#0000ff"), 30));  // adjust thickness as needed
            painter.drawLine(0, yOffset, tableWidth, yOffset);
            painter.setPen(QColor(Qt::black));
        }

        // Regarde si la ligne doit etre surligné
        auto foundPoste = std::find_if(postes.begin(), postes.end(), [&](const std::pair<int, int> &pair) {
            return pair.first == donneesLigne[0];
        });
        if (foundPoste != postes.end()) {
            int secondValue = foundPoste->second;
            painter.setPen(QPen(QColor("#e6ffff"), 40));  // adjust thickness as needed
            painter.drawLine(0, yOffset - (lineHeight * secondValue), tableWidth, yOffset - (lineHeight * secondValue));
            painter.setPen(QColor(Qt::black));
        }


        int xPos = 0;
        int cpt = 0;
        for (auto &i: indices) {

            if(i==15 && !tout0)
                if(donneesLigne[2]==0)
                    continue;


            QString cellText;
            if (donneesLigne[i] == 0) {
                cellText = QString(" ");
            } else {
                cellText = QString::number(donneesLigne[i], 'f', 2);
            }

            if (i == 0) {
                cellText = QString::number(donneesLigne[i], 'f', 0);
            } else if (i == 18) {
                cellText += "m/s";
            } else if (i == 17 || i == 19 || i == 20 || i == 21 || i == 22) {
                cellText += "m";

            }

            if (donneesLigne[i] == 0) {
                cellText = QString(" ");
            }

            QRect cellRect(xPos, yOffset - lineHeight, columnWidth[cpt], lineHeight);
            painter.drawText(cellRect, Qt::AlignCenter, cellText);
            xPos += columnWidth[cpt++];

        }

        yOffset += lineHeight;

        if (yOffset > pdfWriter.height() - 2 * lineHeight) {
            painter.setFont(headerFont);

            // Desinne le bas de page
            QString footerText = QString("Page %1").arg(currentpage);
            QRect footerRect(0, pdfWriter.height() - lineHeight, pdfWriter.width(),
                             lineHeight);
            painter.drawText(footerRect, Qt::AlignCenter, footerText);
            QString referenceText = QString("Reference: %1").arg(reference);
            QRect referenceRect(pdfWriter.width() - 1500,
                                pdfWriter.height() - lineHeight, 1500, lineHeight);
            painter.drawText(referenceRect, Qt::AlignCenter, referenceText);

            // Créé une nouvelle page
            pdfWriter.newPage();
            yOffset = lineHeight * 2;
            yOffset = drawHeaderAndLines(yOffset);
            currentpage++;
        }
    }
    // Ajouter un pied de page sur la dernière page
    QString footerText = QString("Page %1").arg(currentpage);
    painter.drawText(
            QRect(0, pdfWriter.height() - lineHeight, pdfWriter.width(), lineHeight),
            Qt::AlignCenter, footerText);
    painter.drawText(QRect(0, pdfWriter.height() - lineHeight,
                           pdfWriter.width() - 20, lineHeight),
                     Qt::AlignRight, QString("Référence: %1").arg(reference));

    painter.end();

}


void etude::savePdf() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save PDF"),
                                                    QDir::homePath(),
                                                    tr("PDF Files (*.pdf)"));

    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty()) {
            fileName.append(".pdf");
        }
        exportPdf(fileName);
    }
}


void etude::saveToFile(const std::string &filename) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        return;
    }

    // Ecriture
    for (const auto &row: _Donnees) {
        for (float value: row) {
            outFile << value << " ";
        }
        outFile << "\n";
    }

    // Délimiteur
    outFile << "---\n";

    // On écrit les attributs nécéssaires à la sauvegarde
    for (const auto &p: _parcelles) {
        outFile << p.getNom().toStdString() << " "
                << p.getPosteDeCommande() << " "
                << p.getIndexdebut() << " "
                << p.getIndexfin() << " "
                << p.getDecalage() << " "
                << p.getAspdebit() << " "
                << p.isAmont() << " "
                << p.getAspinterdebut() << "\n";
    }
    outFile.close();
}


void etude::readFromFile(const std::string &filename) {

    refresh();

    _currentFileName = filename;

    std::ifstream inFile(filename);
    if (!inFile) {
        return;
    }

    std::vector<std::vector<float>> data;
    std::string line;
    while (std::getline(inFile, line) && (line != "---\r" && line != "---")) {
        std::istringstream iss(line);

        std::vector<float> row;
        float value;
        while (iss >> value) {
            row.push_back(value);
        }
        data.push_back(row);
    }

    _Donnees = data;

    // Lecture des données
    std::getline(inFile, line);
    while (!line.empty()) {

        // remplace un espace par un _ pour ne pas avoir d'erreur dans le nom lié à " "
        std::replace(line.begin(), line.begin() + 10, ' ', '_');

        std::istringstream iss(line);
        std::string nom;
        int poste_de_commande, indexdebut, indexfin, decalage;
        bool amont;
        float aspdebit, aspinter, aspinterdebut;

        if (!(iss >> nom >> poste_de_commande >> indexdebut >> indexfin >> decalage >> aspdebit  >> amont
                  >> aspinterdebut)) {
            break; // Erreur dans la lecture
        }
        parcelle p = parcelle(_Donnees, indexdebut, indexfin, database, QString::fromStdString(nom).replace("_", " "),
                              amont);

        p.setPosteDeCommande(poste_de_commande);
        p.setDecalage(decalage);
        p.setAspdebit(aspdebit);
        p.setAspinterdebut(aspinterdebut);

        p.calcul();

        _parcelles.push_back(std::move(p));
        std::getline(inFile, line);
    }

    inFile.close();

}
#include <QGroupBox>

void etude::optimizeparcelle(){
    updateDonnees();

    for(auto &i : _parcelles)
        if(i.getPosteDeCommande()<=0){
            chooseCommandPost();
            break;
        }

    int column = 2;
    std::vector<int> result;

    for (int i = 0; i < _Donnees.size(); ++i) {
        if (_Donnees[i].size() > column)
            result.push_back(_Donnees[i][column]);
    }

    bool tout0 = std::all_of(result.begin(), result.end(), [](int i) { return i == 0; });


    // Création de la boîte de dialogue
    QDialog dialog(nullptr);
    QFormLayout form(&dialog);


    QLineEdit *lineEditDebit = new QLineEdit(&dialog);
    lineEditDebit->setText(QString::number(0));
    lineEditDebit->setVisible(false);

    QLineEdit *lineEditDistAsperseurs = new QLineEdit(&dialog);
    lineEditDistAsperseurs->setText(QString::number(0));
    lineEditDistAsperseurs->setVisible(false);
    if(!tout0){

        lineEditDistAsperseurs->setVisible(true);
        lineEditDebit->setVisible(true);

        form.addRow("Débit en m³/h:", lineEditDebit);
        form.addRow("Distance entre asperseurs:", lineEditDistAsperseurs);
    }

    QLineEdit *LineEditPiezo = new QLineEdit(&dialog);
    LineEditPiezo->setText(QString::number(0));
    form.addRow("Piezo Ciblé:", LineEditPiezo);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    if (dialog.exec() == QDialog::Accepted) {

        float aspdebit = lineEditDebit->text().replace(",",".").toFloat();
        float aspinterdebut = lineEditDistAsperseurs->text().replace(",",".").toFloat();
        float piezo = LineEditPiezo->text().replace(",",".").toFloat();

        if(tout0){

            std::vector<float> diametresDisponibles = database->getInnerDiametersForMatiereAndPressure("PEHD",6 );

            QDialog *dialog = new QDialog();
            dialog->setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
            QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
            QVector<QCheckBox*> checkboxes;

            // Crée un QHBoxLayout pour organiser les checkboxes en horizontal
            QHBoxLayout *checkboxLayout = new QHBoxLayout();
            mainLayout->addLayout(checkboxLayout);

            // Ajout des checkboxes pour chaque diamètre
            for (float diameter : diametresDisponibles) {
                QCheckBox *checkbox = new QCheckBox(QString::number(diameter), dialog);
                checkbox->setChecked(true);

                // Crée un QGroupBox pour chaque checkbox
                QGroupBox *groupbox = new QGroupBox(dialog);
                QVBoxLayout *groupboxLayout = new QVBoxLayout(groupbox);
                groupboxLayout->addWidget(checkbox);
                checkboxLayout->addWidget(groupbox);

                checkboxes.push_back(checkbox);
            }

            // Ajout du bouton "OK"
            QPushButton *button = new QPushButton("OK", dialog);
            mainLayout->addWidget(button);

            // Connecte le signal du bouton au slot de la dialogue
            QObject::connect(button, &QPushButton::clicked, dialog, &QDialog::accept);

            // Affiche le dialogue modale
            dialog->exec();


            // Récupère les diamètres sélectionnés
            std::vector<float> diametresChoisis;
            for (QCheckBox *checkbox : checkboxes) {
                if (checkbox->isChecked()) {
                    diametresChoisis.push_back(checkbox->text().toFloat());
                }
            }

            // Vous pouvez maintenant utiliser diametresChoisis pour vos calculs
            // Supprime la boîte de dialogue
            delete dialog;

            for(auto &parcel : _parcelles)
                parcel.optimize(diametresChoisis,piezo);
            rafraichirTableau();
            return;
        }

        std::vector<float> diametresDisponibles = database->getInnerDiametersForMatiereAndPressure("PVC",10 );

        QDialog *dialog = new QDialog();
        dialog->setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");
        QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
        QVector<QCheckBox*> checkboxes;

        // Crée un QHBoxLayout pour organiser les checkboxes en horizontal
        QHBoxLayout *checkboxLayout = new QHBoxLayout();
        mainLayout->addLayout(checkboxLayout);

        // Ajout des checkboxes pour chaque diamètre
        for (float diameter : diametresDisponibles) {
            QCheckBox *checkbox = new QCheckBox(QString::number(diameter), dialog);
            checkbox->setChecked(true);

            // Crée un QGroupBox pour chaque checkbox
            QGroupBox *groupbox = new QGroupBox(dialog);
            QVBoxLayout *groupboxLayout = new QVBoxLayout(groupbox);
            groupboxLayout->addWidget(checkbox);
            checkboxLayout->addWidget(groupbox);

            checkboxes.push_back(checkbox);
        }

        // Ajout du bouton "OK"
        QPushButton *button = new QPushButton("OK", dialog);
        mainLayout->addWidget(button);

        // Connecte le signal du bouton au slot de la dialogue
        QObject::connect(button, &QPushButton::clicked, dialog, &QDialog::accept);

        // Affiche le dialogue modale
        dialog->exec();

        // Récupère les diamètres sélectionnés
        std::vector<float> diametresChoisis;
        for (QCheckBox *checkbox : checkboxes) {
            if (checkbox->isChecked()) {
                diametresChoisis.push_back(checkbox->text().toFloat());
            }
        }

        for(auto &parcel : _parcelles)
            parcel.optimize(diametresDisponibles,piezo,aspdebit,aspinterdebut);
        rafraichirTableau();

    }

}
