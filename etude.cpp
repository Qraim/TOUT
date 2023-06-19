
#include "etude.h"
float hauteur = 40;
float largueur = 150;
etude::etude(std::shared_ptr<bdd> db,QWidget *parent)
    : QWidget(parent),database(db)
{
  milieu = true;
  limitations = true;
  poste = true;
  premier = true;
  asp = false;
  // MainLayout
  mainLayout = new QVBoxLayout();

  setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");


  // Init et Insert
  QPushButton *initButton = new QPushButton("Init", this);
  QPushButton *divideButton = new QPushButton("Diviser", this);
  QPushButton *postebutton = new QPushButton("Poste", this);
  QPushButton *calcul = new QPushButton("Calcul", this);

  connect(initButton, &QPushButton::clicked, this, &etude::init);
  connect(divideButton, &QPushButton::clicked, this, &etude::divideData);
  connect(postebutton, &QPushButton::clicked, this, &etude::chooseCommandPost);
  connect(calcul, &QPushButton::clicked, this, &etude::appelSetDiametreDialog);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget(initButton);
  buttonsLayout->addWidget(divideButton);
  buttonsLayout->addWidget(postebutton);
  buttonsLayout->addWidget(calcul);

  QPushButton *showButton = new QPushButton("Afficher", this);
  buttonsLayout->addWidget(showButton);

  // Connecter le bouton "Afficher" à un nouveau slot "showOptionsDialog"
  connect(showButton, &QPushButton::clicked, this, &etude::showOptionsDialog);

  mainLayout->insertLayout(0, buttonsLayout);

  // QScrollArea
  scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  mainLayout->addWidget(scrollArea);

  scrollAreaWidgetContents = new QWidget();
  scrollArea->setWidget(scrollAreaWidgetContents);

  gridLayout = new QGridLayout(scrollAreaWidgetContents);

  // On crée les 20 colonnes
  for (int i = 0; i < 20; ++i) {
    gridLayout->setColumnStretch(i, 1);
  }

  setLayout(mainLayout);
}

void etude::showOptionsDialog() {

  // Créer un QDialog pour les options
  QDialog optionsDialog(this);
  optionsDialog.setWindowTitle("Options");

  QCheckBox *milieuHydroCheckBox = new QCheckBox("Milieu Hydro", this);
  milieuHydroCheckBox->setChecked(milieu);
  milieuHydroCheckBox->setStyleSheet("QCheckBox { color: orange; }");

  QCheckBox *limitationParcelleCheckBox = new QCheckBox("Limitation parcelle", this);
  limitationParcelleCheckBox->setChecked(limitations);
  limitationParcelleCheckBox->setStyleSheet("QCheckBox { color: blue; }");

  QCheckBox *postecheckbox = new QCheckBox("Poste", this);
  postecheckbox->setChecked(poste);
  postecheckbox->setStyleSheet("QCheckBox { color: red; }");

  QCheckBox *premiere = new QCheckBox("Premier rang", this);
  premiere->setChecked(poste);
  premiere->setStyleSheet("QCheckBox { color : pink}");

  QCheckBox *asper = new QCheckBox("Aspersions", this);
  asper->setChecked(asp);

  // Créer un layout pour le QDialog
  QVBoxLayout dialogLayout(&optionsDialog);
  dialogLayout.addWidget(milieuHydroCheckBox);
  dialogLayout.addWidget(limitationParcelleCheckBox);
  dialogLayout.addWidget(postecheckbox);
  dialogLayout.addWidget(premiere);
  dialogLayout.addWidget(asper);

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
  asp = asper->isChecked();

  // Rafraichir le tableau pour prendre en compte les nouvelles options d'affichage
  rafraichirTableau();
}



void etude::init()
{
  bool amont = true;
  _Donnees.clear();
  _parcelles.clear();


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
    QString data = dataEdit->toPlainText();
    traitements(data);
    amont = amontButton->isChecked();
  }


  // Ajouter une parcelle avec toutes les données
  if (!_Donnees.empty()) {
    QString nomParcelle = "parcelle 1";
    initCalcul();
    _parcelles.push_back(parcelle(_Donnees, 0, _Donnees.size(),database, nomParcelle, amont));
    rafraichirTableau();
  }
}



void etude::traitements(QString data){
  // Remplacer les virgules par des points
  data.replace(',', '.');

  QStringList lines = data.split("\n");
  for (int i = 0; i < lines.size()-1; i++) {

    // Ignore lines that start with "Parcelle", "Intervalle", or "Nombre"
    if (lines[i].startsWith("Parcelle") || lines[i].startsWith("Intervalle") || lines[i].startsWith("Nombre")) {
      std::cout<<lines[i].toStdString()<<std::endl;
      continue;
    }

    QStringList cols = lines[i].split(QRegExp("\\s+"));

    // Ignorer les lignes qui n'ont pas assez de colonnes
    if (cols.size() < 7) {
      continue;
    }

    std::vector<float> rowData(27);
    rowData[0] = cols[2].toInt(); // numero du rang
    rowData[1] = cols[3].toFloat(); // Longueur du rang
    rowData[2] = cols[4].toFloat(); // Nombre d'asperseurs
    rowData[3] = cols[5].toFloat(); // Zamont du rang
    rowData[4] = cols[6].toFloat(); // Zaval du rang
    if(i==3){
      rowData[5] = 0;// Intervale rang début
      rowData[6] = 0; // Intervale rang fin
    } else {
      if (cols.size() > 7) rowData[5] = cols[7].toFloat(); // Intervale rang début
      if (cols.size() > 8) rowData[6] = cols[8].toFloat(); // Intervale rang fin
    }
    _Donnees.push_back(rowData);
  }
}



void etude::clearchild() {
  QLayoutItem *item;
  while ((item = gridLayout->takeAt(0)) != nullptr) {
    if (QWidget *widget = item->widget()) {
      widget->setParent(nullptr); // Disown the widget
      delete widget;  // Now it's safe to delete it
    }
    delete item;
  }
}




#include <QScrollBar>

void etude::rafraichirTableau() {

  if(_parcelles.size()!=0){
    updateDonnees(); // Met à jour les données
  }

  if(_Donnees.size()==0){
    return;
  }

  // Sauvegarder la position actuelle des barres de défilement
  int scrollPosVertical = scrollArea->verticalScrollBar()->value();
  int scrollPosHorizontal = scrollArea->horizontalScrollBar()->value();
  // Supprime toutes les cases du tableau.
  clearchild();

  bool amont = _parcelles[0].isAmont();

  int column = 2;  // The column you want
  std::vector<int> result;  // The vector to hold the column

  for (int i = 0; i < _Donnees.size(); ++i) {
    if(_Donnees[i].size() > column)  // Checking if the column index is within the range of inner vector
      result.push_back(_Donnees[i][column]);
  }

  bool tout0 = std::all_of(result.begin(), result.end(), [](int i) { return i == 0; });


  // Initialize headers
  QStringList headers;
  // Initialize headers
  if(tout0) {
    headers << "Num" << "Long" << "NbAsp" << "Zamont" << "Zaval" << "InterD" << "InterF" << "DebitG" << "Espacement" << "Debit Ligne" << "Debit Cumul" << "PeigneZAm" << "PeigneZAv" << "DeltaLigneAm" << "DeltaLigneAv" << "Diametre" << "Nom" << "DenivelePeigne" << "Vitesse" << "Perte J" << "Piezo P" << "Cumul Perte" << "Cumul Piezo"<<"Ø16/14"<<"Piezo Ø16"<<"Ø20/17.6"<<"Piezo Ø20";
  } else {
    for(int i = 0; i <_Donnees.size();i++) {
      _Donnees[i].resize(31);
    }
    headers << "Num" << "Long" << "NbAsp" << "Zamont" << "Zaval" << "InterD" << "InterF" << "DebitG" << "Espacement" << "Debit Ligne" << "Debit Cumul" << "PeigneZAm" << "PeigneZAv" << "DeltaLigneAm" << "DeltaLigneAv" << "Diametre" << "Nom" << "DenivelePeigne" << "Vitesse" <<"Perte J" << "Piezo P" << "Cumul Perte" << "Cumul Piezo"<<"1er Inter"<<"Ø75"<<"Ø63"<<"Ø50"<<"Ø40"<<"Ø32"<<"Perte J"<< "Piezo P";
  }


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

  int totalRows = 0;
  std::vector<ParcelInfo> parcelInfos;
  for (auto& parcel : _parcelles) {
    ParcelInfo info;
    totalRows += parcel.getDonnees().size();
    const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();
    info.milieuHydro = parcel.getMilieuhydro()+parcel.getIndexdebut()+1;
    info.limiteParcelle = totalRows;
    info.commandPost = parcel.getPosteDeCommande() + parcel.getIndexdebut();
    info.nom = parcel.getNom();
    info.longueur = parcel.getLongueur();
    info.debit = parcel.getDebit();
    parcelInfos.push_back(info);
  }

  int parcelIndex = 0;  // Ajoutez ceci avant la boucle sur _parcelles



  // Ajoute les données au tableau.
  for (const std::vector<float> &donneesLigne : _Donnees) {

    if( donneesLigne[2]==0 && asp ){
      ligne++;
      continue;
    }

    // Détermine la couleur du texte.
    QString textColor = WHITE_TEXT;
    int distanceToNearestCommandPost = std::numeric_limits<int>::max();

    // Trouvez la parcelle à laquelle cette ligne appartient
    auto parcelInfoIt = std::find_if(parcelInfos.begin(), parcelInfos.end(),
                                     [ligne](const ParcelInfo& info) { return ligne <= info.limiteParcelle; });

    if (parcelInfoIt != parcelInfos.end()) {
      const ParcelInfo& info = *parcelInfoIt;
      if (ligne == info.commandPost && poste) {
        textColor = RED_TEXT; // Poste de commande
      } else if (ligne == info.milieuHydro && milieu) {
        textColor = ORANGE_TEXT; // Milieu hydrolique
      } else if (ligne == info.limiteParcelle && limitations) {
        textColor = BLUE_TEXT; // Limite entre deux parcelles
      } else if (donneesLigne[0]==1 && premier){
        textColor = PINK_TEXT; //premiere ligne d'une parcelle
      }
      distanceToNearestCommandPost = std::abs(ligne - info.commandPost);
    }


    for (int i = 0; i < donneesLigne.size(); ++i) {

      if(i==2 && tout0){
        continue;
      }

      if(i==16){
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
        } else if(i ==17 || i == 19 || i == 20 || i == 21 || i == 22 ){
          formattedText = QString::number(donneesLigne[i], 'f', 2) + "m";
        } else if(i ==18 ){

          formattedText = QString::number(donneesLigne[i], 'f', 2) + "m/s";
        }else {
          formattedText = QString::number(donneesLigne[i], 'f', 2);
        }



        lineEdit->setText(formattedText);
        gridLayout->setVerticalSpacing(0);
        gridLayout->addWidget(lineEdit, ligne, i);
        // Ajoute un signal pour la 16ème/5éme/6éme colonne
        if ((i == 15 || i ==5 || i==6 || i ==9) || (i==24 || i==25 || i==26 || i ==27 || i==23 || i==28 && !tout0)) {
          lineEdit->installEventFilter(this);
          lineEdit->setStyleSheet("QLineEdit { color: yellow; }");
          lineEdit->setReadOnly(false);
          // Connecte textEdited signal comme avant
          if(i==15){
            connect(lineEdit, &QLineEdit::textEdited, [this, ligne](const QString& newDiameter) {
              this->updateDiameter(ligne-1, newDiameter);
            });
          } else if (i==9){
            connect(lineEdit, &QLineEdit::textEdited, [this, ligne, i](const QString& newDiameter) {
              this->updateDebit(ligne-1, newDiameter);
            });
          } if(i==24 || i==25 || i==26 || i ==27 || i==23 || i==28 && !tout0){
            if(_Donnees[ligne-1][2]!=0){
              connect(lineEdit, &QLineEdit::textEdited, [this, ligne, i](const QString& newDiameter) {
                this->updateinterval(ligne-1,i, newDiameter);
              });
            } else {
              lineEdit->setStyleSheet(textColor);
              lineEdit->setReadOnly(true);
            }

          } else {
            int colonne = i;
            connect(lineEdit, &QLineEdit::textEdited, [this, ligne, colonne](const QString& newDiameter) {
              this->updateinterval(ligne-1,colonne, newDiameter);
            });
          }

          if(!parcelInfos.empty()){
            // Trouvez la dernière ligne de la parcelle actuelle
            int lastRowOfParcel = parcelInfoIt->limiteParcelle;
            // Si la ligne actuelle est la dernière ligne de la parcelle, connectez le signal editingFinished() à la méthode calcul()
            if (ligne == lastRowOfParcel) {
              int parcelIndex = std::distance(parcelInfos.begin(), parcelInfoIt);
              connect(lineEdit, &QLineEdit::editingFinished, [this, parcelIndex] {
                this->_parcelles[parcelIndex].calcul();
              });
            }
          }
        } else if(i==18){
          if(donneesLigne[i] > 2){
            lineEdit->setStyleSheet("QLineEdit { color: red; }");
          }
        }
      }
    }


    // Vérifie si cette ligne est une limite de parcelle
    if (parcelInfoIt != parcelInfos.end() && ligne == parcelInfoIt->limiteParcelle) {
      ParcelInfo& info = *parcelInfoIt;
      int parcelIndex = std::distance(parcelInfos.begin(), parcelInfoIt);

      if(ligne >=5){
        QLineEdit *parcelNameLineEdit = createLineEdit(info.nom, textColor, this, false);
        // Connecte le signal textChanged au slot setNom
        connect(parcelNameLineEdit, &QLineEdit::textChanged, [this, parcelIndex](const QString& newName) {
          this->_parcelles[parcelIndex].setNom(newName);
        });
        gridLayout->addWidget(parcelNameLineEdit, ligne, 16);  // Ajoute le QLineEdit à la 16e colonne

        // Crée un QLineEdit pour la longueur de la parcelle
        QLineEdit *parcelLengthLineEdit = createLineEdit(QString::number(info.longueur, 'f', 2) + " m", textColor, this);
        gridLayout->addWidget(parcelLengthLineEdit, ligne - 1, 16);  // Ajoute le QLineEdit à la 17e colonne

        // Crée un QLineEdit pour le debit de la parcelle
        QString text;
        if(info.debit > 1000) {
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

        if(this->_parcelles[parcelIndex].isAmont()) {
          setamont->setCurrentIndex(0);  // Index 0 corresponds à "Amont"
        } else {
          setamont->setCurrentIndex(1);  // Index 1 corresponds à "Aval"
        }

        connect(setamont, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, parcelIndex](int index) {
          bool isAmont = (index == 0);
          this->_parcelles[parcelIndex].SetAmont(isAmont);
        });

        gridLayout->addWidget(setamont, ligne-3, 16);  // Ajoute le QLineEdit à la 16e colonne

        QPushButton *inverse = new QPushButton("Inverser");
        inverse->setStyleSheet("QPushButton { background-color: blue; color: white; }");
        connect(inverse, &QPushButton::clicked, [this, parcelIndex]() {
          this->_parcelles[parcelIndex].inverser();
          rafraichirTableau();
        });

        gridLayout->addWidget(inverse, ligne-4, 16);  // Ajoute le QLineEdit à la 16e colonne
      }


    }

    // Incrémente le numéro de ligne.
    ligne++;
  }

  // Contrôle de visibilité des colonnes après avoir rempli la grille
  if (amont) {
    for (int i = 0; i < gridLayout->columnCount(); ++i) {
      if (i == 4 || i == 6 || i ==12 || i==13) {
        for (int j = 0; j < gridLayout->rowCount(); ++j) {
          if (gridLayout->itemAtPosition(j, i) && gridLayout->itemAtPosition(j, i)->widget()) {
            gridLayout->itemAtPosition(j, i)->widget()->setVisible(false);
          }
        }
      }
    }
  } else {
    for (int i = 0; i < gridLayout->columnCount(); ++i) {
      if (i == 3 || i == 5  || i ==11 || i ==14) {
        for (int j = 0; j < gridLayout->rowCount(); ++j) {
          if (gridLayout->itemAtPosition(j, i) && gridLayout->itemAtPosition(j, i)->widget()) {
            gridLayout->itemAtPosition(j, i)->widget()->setVisible(false);
          }
        }
      }
    }
  }

  // Contrôle de visibilité des colonnes après avoir rempli la grille
  if (tout0) {
    int i=2;
    for (int j = 0; j < gridLayout->rowCount(); ++j) {
      if (gridLayout->itemAtPosition(j, i) && gridLayout->itemAtPosition(j, i)->widget()) {
        gridLayout->itemAtPosition(j, i)->widget()->setVisible(false);
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
}

QLineEdit* etude::createLineEdit(const QString& text, const QString& style, QWidget* parent, bool readOnly) {
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
  for (const auto &matiere_name : matiere_names) {
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

    if(!litre->text().isEmpty() && !espacement->text().isEmpty()){
      debitGoutteur = litre->text().replace(",",".").toFloat();
      espacementGoutteur = espacement->text().replace(",",".").toFloat();
    } else if (default1Button.isChecked()) {
      debitGoutteur = 2.2f;
      espacementGoutteur = 0.6f;
    } else if (default2Button.isChecked()) {
      debitGoutteur = 1.6f;
      espacementGoutteur = 0.5f;
    }

    for(int i=0;i<_Donnees.size();i++){
      _Donnees[i][7] = debitGoutteur;
      _Donnees[i][8] = espacementGoutteur;
    }
    calcul();
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
}

#include <QMessageBox>

void etude::divideData() {

  QMessageBox::StandardButton reply;
  if(_parcelles.size()>1){
    reply = QMessageBox::question(this, "Alert", "Êtes vous sûr de vouloir séparer les parcelles", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
      return;
  }

  if(!_parcelles.empty()){
    updateDonnees();
    _parcelles.clear();
  }

  for (auto& parcel : _parcelles) {
    std::vector<std::vector<float>> parcelData = parcel.getDonnees();
    for (auto& row : parcelData) {
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
        row[24] = 0.0;
        row[25] = 0.0;
        row[26] = 0.0;

      }
    }
    _Donnees.insert(_Donnees.end(), parcelData.begin(), parcelData.end());
  }

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

  // Création du bouton OK et Cancel
  QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
  formLayout.addWidget(&buttonBox);
  connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
  connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

  int nextStartIndex = 0;

  while (nextStartIndex < _Donnees.size()) {
    remainingLinesLabel.setText(QString::number(_Donnees.size() - nextStartIndex));

    if (dialog.exec() == QDialog::Accepted) {
      if (autoButton.isChecked()) {
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
            QString nomParcelle = QString("parcelle %1").arg(_parcelles.size() + 1);
            _parcelles.push_back(parcelle(_Donnees, startIndex, endIndex + 1,database, nomParcelle, _parcelles[0].isAmont()));
            startIndex = i + 1;
            currentLength = 0;
          }
        }
        nextStartIndex = _Donnees.size();
      } else if (manualButton.isChecked()) {
        int startIndex = startIndexLineEdit.text().toInt()-1;
        int endIndex = endIndexLineEdit.text().toInt()-1;

        if (startIndex >= nextStartIndex && endIndex < _Donnees.size() && startIndex < endIndex) {

          QString nomParcelle = QString("parcelle %1").arg(_parcelles.size() + 1);
          _parcelles.push_back(parcelle(_Donnees, startIndex, endIndex + 1,database, nomParcelle));

          nextStartIndex = endIndex + 1;

          startIndexLineEdit.setText(QString::number(endIndex+2));
          endIndexLineEdit.setText(QString::number(endIndex+3));

        } else {
        }
      }
    } else {
      break;
    }
  }
  rafraichirTableau();
}

void etude::setTabOrderForLineEdits(){
  QLineEdit* previousLineEdit = nullptr;
  QLineEdit* firstLineEdit = qobject_cast<QLineEdit*> (gridLayout->itemAtPosition(0, 15)->widget());

  // Iterate only through the 16th column
  for (int i = 0; i < gridLayout->rowCount(); ++i) {
    QLayoutItem* item = gridLayout->itemAtPosition(i, 15); // 16 is the column index
    if (item) {
      QLineEdit* lineEdit = qobject_cast<QLineEdit*>(item->widget());
      // Check if the item is a QLineEdit and is editable
      if (lineEdit && !lineEdit->isReadOnly()) {
        // If this is not the first QLineEdit found
        if (previousLineEdit) {
          QWidget::setTabOrder(previousLineEdit, lineEdit);
        }
        previousLineEdit = lineEdit;
      }
    }
  }
  if(previousLineEdit) {
    QWidget::setTabOrder(previousLineEdit, firstLineEdit);
  }
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


void etude::chooseCommandPost() {
  // Créer un QDialog pour le choix du poste de commande
  QDialog dialog(this);
  dialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

  dialog.setWindowTitle("Choisir le poste de commande");

  // Créer un layout pour le QDialog
  QVBoxLayout dialogLayout(&dialog);

  // Créer deux QMap pour mapper les QComboBox aux parcelles correspondantes
  QMap<QComboBox*, parcelle*> comboBoxToParcelMap;
  QMap<QComboBox*, parcelle*> sideComboBoxToParcelMap;

  int globalIndex = 0;

  for (auto& parcel : _parcelles) {
    const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();
    if (!parcelData.empty() && !parcelData[0].empty()) {
      QComboBox* rangeIndexComboBox = new QComboBox(&dialog);
      QComboBox* sideComboBox = new QComboBox(&dialog);

      // Ajouter les options pour choisir le côté
      sideComboBox->addItem("Au dessus");
      sideComboBox->addItem("En dessous");

      int defaultIndex = -1;
      int commandPost = parcel.getPosteDeCommande() + parcel.getIndexdebut();

      for (size_t i = 0; i < parcelData.size(); ++i) {
        rangeIndexComboBox->addItem(QString::number(globalIndex));
        ++globalIndex;
      }

      if (commandPost != -1) {
        defaultIndex = commandPost;
      }

      if (defaultIndex != -1) {
        rangeIndexComboBox->setCurrentIndex(rangeIndexComboBox->findText(QString::number(defaultIndex)));
      }

      // Ajouter les comboboxes au layout du QDialog
      QHBoxLayout* parcelLayout = new QHBoxLayout();
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
    for (auto comboBox : comboBoxToParcelMap.keys()) {
      // Mettre à jour le poste de commande avec la valeur sélectionnée dans la combobox
      int rangeIndex = comboBox->currentText().toInt();
      parcelle* selectedParcel = comboBoxToParcelMap.value(comboBox);
      if (selectedParcel) {
        selectedParcel->setPosteDeCommande(rangeIndex);
      }
    }

    // Pour chaque combobox dans la QMap
    for (auto comboBox : sideComboBoxToParcelMap.keys()) {
      // Mettre à jour le côté avec la valeur sélectionnée dans la combobox
      QString side = comboBox->currentText();
      parcelle* selectedParcel = sideComboBoxToParcelMap.value(comboBox);
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

  // Connect the launchButton signal to the setDiametreDialog function of the selected Parcelle
  QObject::connect(&launchButton, &QPushButton::clicked,
                   [this, &comboBox]() {
                     int index = comboBox.currentIndex();
                     if (index >= 0 && index < _parcelles.size()) {
                       _parcelles[index].setDiametreDialog(_matiere);
                       updateDonnees();
                       rafraichirTableau();
                     }
                   });
  dialog.exec();
}

void etude::updateDiameter(int row, const QString& newDiameter) {
  float diameter = newDiameter.toFloat();
  for (auto& parcel : _parcelles) {
    if(row < parcel.getDonnees().size()) {
      parcel.modifiedia(row, diameter);
      return;
    }
    row -= parcel.getDonnees().size();
  }
}

void etude::updateDebit(int row, const QString& newDiameter) {
  float diameter = newDiameter.toFloat();
  for (auto& parcel : _parcelles) {
    if(row < parcel.getDonnees().size()) {
      parcel.modifiedebit(row, diameter);
      return;
    }
    row -= parcel.getDonnees().size();
  }
}

void etude::updateinterval(int row, int ligne,const QString& newDiameter) {
  float diameter = newDiameter.toFloat();
  for (auto& parcel : _parcelles) {
    if(row < parcel.getDonnees().size()) {

      parcel.modifieinter(row,ligne,newDiameter.toFloat());
      return;
    }
    row -= parcel.getDonnees().size();
  }
}

#include <QTimer>

bool etude::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::FocusIn) {
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
    if (lineEdit) {
      // Use a singleShot QTimer to delay the selectAll() call
      QTimer::singleShot(0, lineEdit, SLOT(selectAll()));
    }
  } else if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_D) {
      changerDiametreDialog();
      return true;
    }
    if (keyEvent->key() == Qt::Key_S) {
      savePdf();
      return true;
    }
    if (keyEvent->key() == Qt::Key_O) {
      changerDebitDialog();
      rafraichirTableau();
      return true;
    }
    /*// Handle KeyPress event as before
    if (keyEvent->key() == Qt::Key_Tab) {
      QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
      if (lineEdit) {
        int currentRow = gridLayout->indexOf(lineEdit) / gridLayout->columnCount();
        QLineEdit* nextLineEdit = findNextDiameterLineEdit(currentRow + 1);
        if (nextLineEdit) {
          nextLineEdit->setFocus();
          nextLineEdit->selectAll();
          return true;  // Mark event as handled
        } else {
          return true; // Add condition for the last QLineEdit
        }
      }
    }*/
  }
  // Standard event processing
  return QObject::eventFilter(obj, event);
}



QLineEdit* etude::findNextDiameterLineEdit(int currentRow) {
  int nextRow = currentRow ;

  if (nextRow >= gridLayout->rowCount()) {
    return nullptr;  // If there is no next row
  }

  QLayoutItem* item = gridLayout->itemAtPosition(nextRow, 15);  // 15 is the column index
  if (item) {
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(item->widget());
    if (lineEdit && !lineEdit->isReadOnly()) {  // Make sure it's an editable QLineEdit
      return lineEdit;
    }
  }

  return nullptr;  // If no next editable QLineEdit is found
}



void etude::modifierdiametre(int debut, int fin, float dia){
  if(debut > fin){
    std::swap(debut, fin);
  }
  int i = 0 ;
  for(auto & parcelle : _parcelles){
    auto & datas = parcelle.getDonnees();
    for(int j = 0; j < datas.size();j++){
      if(i>=debut && i<=fin){
        parcelle.modifiedia(j,dia);
        datas[j][15] = dia;
      }
      i++;
    }
  }
  rafraichirTableau();
}


void etude::modifierdebit(int debut, int fin, float dia){
  if(debut > fin){
    std::swap(debut, fin);
  }
  int i = 0 ;
  for(auto & parcelle : _parcelles){
    auto & datas = parcelle.getDonnees();
    for(int j = 0; j < datas.size();j++){
      if(i>=debut && i<=fin){
        parcelle.modifiedebit(j,dia);
        datas[j][9] = dia;
        //
        //  PENSER A RECALCULER
        //
        //
      }
      i++;
    }
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


  // Create and setup the line edits
  std::string d = std::to_string(_Donnees.size()) ;
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

  // When the button is clicked, get the data and call modifierdiametre
  QObject::connect(&button, &QPushButton::clicked, [&]() {
    int debut = indiceDebutLineEdit.text().toInt()-1;
    int fin = indiceFinLineEdit.text().toInt()-1;
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


  // Create and setup the line edits
  std::string d = std::to_string(_Donnees.size()) ;
  QLineEdit indiceDebutLineEdit(QString::fromStdString("1"));
  QLineEdit indiceFinLineEdit(QString::fromStdString(d));
  QLineEdit diametreLineEdit;

  QPushButton button("Appliquer");

  QVBoxLayout layout;
  layout.addWidget(new QLabel("Indice de début:"));
  layout.addWidget(&indiceDebutLineEdit);
  layout.addWidget(new QLabel("Indice de fin:"));
  layout.addWidget(&indiceFinLineEdit);
  layout.addWidget(new QLabel("Nouveau débit:"));
  layout.addWidget(&diametreLineEdit);
  layout.addWidget(&button);
  dialog.setLayout(&layout);

  // When the button is clicked, get the data and call modifierdiametre
  QObject::connect(&button, &QPushButton::clicked, [&]() {
    int debut = indiceDebutLineEdit.text().toInt()-1;
    int fin = indiceFinLineEdit.text().toInt()-1;
    float diametre = diametreLineEdit.text().toFloat();
    modifierdebit(debut, fin, diametre);
    dialog.close();
  });

  dialog.exec();
}


void etude::refresh(){
  clearchild();
  _Donnees.clear();
  _parcelles.clear();
  milieu = true;
  limitations = true;
  poste = true;
}


#include <QPdfWriter>
#include <QPainter>
#include <QTextDocument>
#include <QTextTable>
#include <QTextCursor>



void etude::exportPdf(const QString& fileName) {
  QPdfWriter writer(fileName);

  // Configure the page layout to landscape
  QPageLayout layout;
  layout.setOrientation(QPageLayout::Landscape);
  writer.setPageLayout(layout);

  QPainter painter(&writer);


  QTextDocument doc;
  QTextCursor cursor(&doc);
  QTextTableFormat tableFormat;

  tableFormat.setHeaderRowCount(1);
  tableFormat.setAlignment(Qt::AlignCenter);
  tableFormat.setCellPadding(10);


  int totalRows = 0;
  std::vector<ParcelInfo> parcelInfos;
  for (auto& parcel : _parcelles) {
    ParcelInfo info;
    totalRows += parcel.getDonnees().size();
    const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();
    info.milieuHydro = parcel.getMilieuhydro()+parcel.getIndexdebut()+1;
    info.limiteParcelle = totalRows;
    info.commandPost = parcel.getPosteDeCommande();
    info.nom = parcel.getNom();
    info.longueur = parcel.getLongueur();
    info.debit = parcel.getDebit();


    parcelInfos.push_back(info);
  }

  // Initialize headers
  QStringList headers = {"Num", "Long","NbAsp", "Zamont", "Zaval", "InterD", "InterF","DebitG","Esp","DebitC","DebitL","PeigneZAm","PeigneZAv","DRAm","DRAv","Diametre", "Nom", "Perte/Vitesse", "Denivele", "Piezo"};

  QTextTable *table = cursor.insertTable(_Donnees.size() + 1, headers.size(), tableFormat);

  // Set header
  for (int i = 0; i < headers.size(); ++i) {
    table->cellAt(0, i).firstCursorPosition().insertText(headers[i]);
  }

  // Set table data
  for (int i = 0; i < _Donnees.size(); ++i) {
    for (int j = 0; j < _Donnees[i].size(); ++j) {
      QTextTableCell cell = table->cellAt(i + 1, j);

      QTextCharFormat format = cell.format();

      // Set cell background color based on data
      QString textColor = "white";
      int distanceToNearestCommandPost = std::numeric_limits<int>::max();

      // Find which parcel this row belongs to
      auto parcelInfoIt = std::find_if(parcelInfos.begin(), parcelInfos.end(),
                                       [i](const ParcelInfo& info) { return i <= info.limiteParcelle; });

      if (parcelInfoIt != parcelInfos.end()) {
        const ParcelInfo& info = *parcelInfoIt;
        if (i == info.commandPost && poste) {
          textColor = "red"; // Command post
        } else if (i == info.milieuHydro && milieu) {
          textColor = "orange"; // Hydro milieux
        } else if (i == info.limiteParcelle && limitations) {
          textColor = "blue"; // Parcel limits
        }
      }

      if (textColor == "red") {
        format.setBackground(Qt::red);
      } else if (textColor == "orange") {
        format.setBackground(Qt::yellow);
      } else if (textColor == "blue") {
        format.setBackground(Qt::blue);
      }

      cell.setFormat(format);

      cell.firstCursorPosition().insertText(QString::number(_Donnees[i][j]));
    }
  }

  doc.drawContents(&painter);
}

#include <QFileDialog>

void etude::savePdf() {
  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save PDF"),
                                                  QDir::homePath(),
                                                  tr("PDF Files (*.pdf)"));

  if (!fileName.isEmpty()) {
    if (QFileInfo(fileName).suffix().isEmpty()) { // If user didn't specify .pdf, add it
      fileName.append(".pdf");
    }
    exportPdf(fileName);
  }
}


// Enregistrement des données de l'étude dans un fichier
void etude::saveToFile(const std::string& filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
    return;
  }

  for (parcelle p : _parcelles) {
    // Write the basic parcelle data
    file << p.getNom().toStdString() << ',';
    file << p.getMilieuhydro() << ',';
    file << p.getPosteDeCommande() << ',';
    file << p.getIndexdebut() << ',';
    file << p.getIndexfin() << ',';
    file << p.getLongueur() << ',';
    file << p.getMatiere() << ',';
    file << p.isAmont() << ',';
    file << p.getDebit() << ',';
    file << p.isCalcul() << ',';

    // Write _Donnees
    auto& donnees = p.getDonnees();
    for (size_t i = 0; i < donnees.size(); ++i) {
      for (auto& d : donnees[i]) {
        file << d << ';';
      }
      if (i < donnees.size() - 1) {
        file << ',';
      }
    }
    file << ',';

    // Write _diameters
    const auto& diameters = p.getDiameters();
    for (size_t i = 0; i < diameters.size(); ++i) {
      file << diameters[i];
      if (i < diameters.size() - 1) {
        file << ';';
      }
    }
    file << '\n';
  }

  file.close();
}


void etude::loadFromFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);

    parcelle p;

    // Read the basic parcelle data
    std::string item;
    std::getline(iss, item, ',');
    p.setNom(QString::fromStdString(item));

    std::getline(iss, item, ',');
    p.setMilieuhydro(std::stoi(item));

    std::getline(iss, item, ',');
    p.setPosteDeCommande(std::stoi(item));

    std::getline(iss, item, ',');
    p.setIndexdebut(std::stoi(item));

    std::getline(iss, item, ',');
    p.setIndexfin(std::stoi(item));

    std::getline(iss, item, ',');
    p.setLongueur(std::stof(item));

    std::getline(iss, item, ',');
    p.setMatiere(item);

    std::getline(iss, item, ',');
    p.setAmont(std::stoi(item));

    std::getline(iss, item, ',');
    p.setDebit(std::stof(item));

    std::getline(iss, item, ',');
    p.setCalcul(std::stoi(item));

    // Read _Donnees
    std::getline(iss, item, ',');
    std::istringstream issDonnees(item);
    std::vector<std::vector<float>> donnees; // Vector of vectors to hold the read data
    std::string donneeRow;
    while (std::getline(issDonnees, donneeRow, ';')) {
      std::vector<float> donneesCol; // Vector to hold each row of data
      std::istringstream issDonneesRow(donneeRow);
      std::string donnee;
      while (std::getline(issDonneesRow, donnee, ',')) {
        donneesCol.push_back(std::stof(donnee));
      }
      donnees.push_back(donneesCol);
    }
    p.setDonnees(donnees);
    // Read _diameters
    // This part remains same as previous response
    std::getline(iss, item, ',');
    std::istringstream issDiameters(item);
    std::string dia;
    while (std::getline(issDiameters, dia, ';')) {
      p.addDiameter(std::stof(dia));
    }
    // Add the constructed parcelle to the list
    _parcelles.push_back(p);
  }
  file.close();
}



