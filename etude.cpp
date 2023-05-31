
#include "etude.h"
float hauteur = 40;
float largueur = 150;
etude::etude(std::shared_ptr<bdd> db,QWidget *parent)
    : QWidget(parent),database(db)
{
  milieu = true;
  limitations = true;
  poste = true;

  // MainLayout
  mainLayout = new QVBoxLayout();

  setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");


  // Init et Insert
  QPushButton *initButton = new QPushButton("Init", this);
  QPushButton *divideButton = new QPushButton("Diviser", this);
  QPushButton *postebutton = new QPushButton("Poste", this);
  QPushButton *calcul = new QPushButton("calcul", this);

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
  QCheckBox *limitationParcelleCheckBox = new QCheckBox("Limitation parcelle", this);
  limitationParcelleCheckBox->setChecked(limitations);
  QCheckBox *postecheckbox = new QCheckBox("Poste", this);
  postecheckbox->setChecked(poste);
  // Créer un layout pour le QDialog
  QVBoxLayout dialogLayout(&optionsDialog);
  dialogLayout.addWidget(milieuHydroCheckBox);
  dialogLayout.addWidget(limitationParcelleCheckBox);
  dialogLayout.addWidget(postecheckbox);

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

  // Rafraichir le tableau pour prendre en compte les nouvelles options d'affichage
  rafraichirTableau();
}


void etude::init()
{
  _Donnees.clear();
  _parcelles.clear();

  QClipboard *clipboard = QApplication::clipboard();
  QString originalText = clipboard->text();

  if(!originalText.isEmpty()){
    traitements(originalText);
  } else {
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
      traitements(data);
    }
  }

  // Ajouter une parcelle avec toutes les données
  if (!_Donnees.empty()) {
    QString nomParcelle = "parcelle 1";
    initCalcul();
    _parcelles.push_back(parcelle(_Donnees, 0, _Donnees.size(),database, nomParcelle));
    rafraichirTableau();
  }
}


void etude::traitements(QString data){
  // Remplacer les virgules par des points
  data.replace(',', '.');

  QStringList lines = data.split("\n");
  for (int i = 3; i < lines.size()-1; i++) {

    QStringList cols = lines[i].split(QRegExp("\\s+"));

    // Ignorer les lignes qui n'ont pas assez de colonnes
    if (cols.size() < 7) {
      continue;
    }

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

void etude::clearchild() {
  QLayoutItem *item;
  while ((item = gridLayout->takeAt(0)) != nullptr) {
    delete item->widget();
    delete item;
  }
}

#include <QScrollBar>

void etude::rafraichirTableau() {

  if(_parcelles.size()!=0){
    updateDonnees(); // Met à jour les données
  }

  // Sauvegarder la position actuelle des barres de défilement
  int scrollPosVertical = scrollArea->verticalScrollBar()->value();
  int scrollPosHorizontal = scrollArea->horizontalScrollBar()->value();
  // Supprime toutes les cases du tableau.
  clearchild();

  // Initialize headers
  QStringList headers = {"Num", "Long","NbAsp", "Zamont", "Zaval", "InterD", "InterF","DebitG","Esp","DebitC","DebitL","PeigneZAm","PeigneZAv","DRAm","DRAv","Diametre", "Nom", "Perte/Vitesse", "Denivele", "Piezo"};

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
  std::vector<int> commandPosts;
  std::vector<QString> nom;
  std::vector<float> longueurs;
  std::vector<float> debits;

  int totalRows = 0;
  for (auto& parcel : _parcelles) {
    const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();
    totalRows += parcelData.size();
    nom.push_back(parcel.getNom());
    longueurs.push_back(parcel.getLongueur());
    milieuxHydro.push_back(totalRows - parcelData.size() / 2);
    limitesParcelles.push_back(totalRows);
    debits.push_back(parcel.getDebit());
    if(parcel.getPosteDeCommande()!=0){
      commandPosts.push_back(parcel.getPosteDeCommande());
    }
  }
  int parcelIndex = 0;  // Ajoutez ceci avant la boucle sur _parcelles

  // Ajoute les données au tableau.
  for (const std::vector<float> &donneesLigne : _Donnees) {

    // Détermine la couleur du texte.
    QString textColor = "QLineEdit { color: white; }";
    if (std::find(commandPosts.begin(), commandPosts.end(), ligne) != commandPosts.end() && poste) {
      textColor = "QLineEdit { color: white;background-color : red; }"; // Poste de commande
    } else if (std::find(milieuxHydro.begin(), milieuxHydro.end(), ligne) != milieuxHydro.end() && milieu) {
      textColor = "QLineEdit { color: white; background-color : orange; }"; // Milieu hydrolique
    } else if (std::find(limitesParcelles.begin(), limitesParcelles.end(), ligne) != limitesParcelles.end() && limitations) {
      textColor = "QLineEdit { color: white;background-color : blue; }"; // Limite entre deux parcelles
    }

    // Vérifiez si la ligne actuelle est à un ou deux postes de commande.
    int distanceToNearestCommandPost = std::numeric_limits<int>::max();
    for (int commandPost : commandPosts) {
      int distance = abs(ligne - commandPost);
      if (distance < distanceToNearestCommandPost) {
        distanceToNearestCommandPost = distance;
      }
    }


    for (int i = 0; i < donneesLigne.size(); ++i) {
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
        } else if(i>15 && i != 18 && i != 19){
          formattedText = (donneesLigne[i] == 0.0f) ? "" : QString::number(donneesLigne[i], 'f', 2);
        } else if (i == 18 || i == 19) { // Pour les colonnes 18 et 19
          formattedText = (donneesLigne[i] == 0.0f) ? "" : QString::number(donneesLigne[i], 'f', 2) + "m"; // Ajoutez "m" si la valeur n'est pas vide
        } else if (i == 17) {
          QString suffix;
          if (distanceToNearestCommandPost == 1) {
            suffix = "m";
          } else if (distanceToNearestCommandPost == 2) {
            suffix = "m/s";
          }
          formattedText = (donneesLigne[i] == 0.0f) ? "" : QString::number(donneesLigne[i], 'f', 2) + suffix;
        }else {
          formattedText = QString::number(donneesLigne[i], 'f', 2);
        }


        lineEdit->setText(formattedText);
        gridLayout->setVerticalSpacing(0);
        gridLayout->addWidget(lineEdit, ligne, i);
        // Ajoute un signal pour la 16ème colonne
        if (i == 15) {
          lineEdit->installEventFilter(this);
          lineEdit->setStyleSheet("QLineEdit { color: yellow; }");
          lineEdit->setReadOnly(false);
          // Connecte textEdited signal comme avant
          connect(lineEdit, &QLineEdit::textEdited, [this, ligne](const QString& newDiameter) {
            this->updateDiameter(ligne-1, newDiameter);
          });

          if(limitesParcelles.size()!=0){
            // Trouvez la dernière ligne de la parcelle actuelle
            int lastRowOfParcel = limitesParcelles[parcelIndex];
            // Si la ligne actuelle est la dernière ligne de la parcelle, connectez le signal editingFinished() à la méthode calcul()
            if (ligne == lastRowOfParcel) {
              connect(lineEdit, &QLineEdit::editingFinished, [this, parcelIndex] {
                this->_parcelles[parcelIndex].calcul();
              });
            }
          }

        }
      }
    }

    // Vérifie si cette ligne est un milieu hydraulique
    if (std::find(limitesParcelles.begin(), limitesParcelles.end(), ligne) != limitesParcelles.end()) {
      QLineEdit *parcelNameLineEdit = new QLineEdit(this);
      parcelNameLineEdit->setAlignment(Qt::AlignCenter);
      QString parcelName = nom[parcelIndex];
      parcelNameLineEdit->setText(parcelName);
      parcelNameLineEdit->setFixedHeight(hauteur);
      parcelNameLineEdit->setFixedWidth(largueur);
      parcelNameLineEdit->setStyleSheet(textColor);

      // Connecte le signal textChanged au slot setNom
      connect(parcelNameLineEdit, &QLineEdit::textChanged, [this, parcelIndex](const QString& newName) {
        this->_parcelles[parcelIndex].setNom(newName);
      });
      gridLayout->addWidget(parcelNameLineEdit, ligne, 16);  // Ajoute le QLineEdit à la 16e colonne

      // Crée un QLineEdit pour la longueur de la parcelle
      QLineEdit *parcelLengthLineEdit = new QLineEdit(this);
      parcelLengthLineEdit->setAlignment(Qt::AlignCenter);
      QString parcelLength = QString::number(longueurs[parcelIndex], 'f', 2) + " m";  // Converti la longueur en QString
      parcelLengthLineEdit->setText(parcelLength);
      parcelLengthLineEdit->setFixedHeight(hauteur);
      parcelLengthLineEdit->setFixedWidth(largueur);
      parcelLengthLineEdit->setStyleSheet(textColor);

      // Crée un QLineEdit pour le debit de la parcelle
      QLineEdit *parcelDebitLineEdit = new QLineEdit(this);
      parcelDebitLineEdit->setAlignment(Qt::AlignCenter);
      QString text;
      if(debits[parcelIndex]>1000){
        text = QString::number(debits[parcelIndex]/1000, 'f', 2) + " m3/h";
      } else {
        text = QString::number(debits[parcelIndex], 'f', 2) + " L/h";

      }
      parcelDebitLineEdit->setText(text);
      parcelDebitLineEdit->setFixedHeight(hauteur);
      parcelDebitLineEdit->setFixedWidth(largueur);
      parcelDebitLineEdit->setStyleSheet(textColor);

      gridLayout->addWidget(parcelLengthLineEdit, ligne, 17);  // Ajoute le QLineEdit à la 17e colonne
      gridLayout->addWidget(parcelDebitLineEdit, ligne, 18);  // Ajoute le QLineEdit à la 17e colonne

      parcelIndex++;
    }

    // Incrémente le numéro de ligne.
    ligne++;
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
}

void etude::initCalcul() {

  // Crée un QDialog pour recueillir les valeurs.
  QDialog dialog(this);
  dialog.setWindowTitle("Entrer les valeurs");

  // Crée un bouton OK.
  QPushButton okButton("OK");

  // Crée des boutons radio pour les valeurs par défaut
  QRadioButton default1Button("2.2L / 0.6m");
  QRadioButton default2Button("1.6L / 0.5m");

  // Crée un layout pour le QDialog.
  QFormLayout formLayout(&dialog);
  formLayout.addRow(&default1Button);
  formLayout.addRow(&default2Button);
  formLayout.addWidget(&okButton);

  // Connecte le signal du bouton OK pour appeler la fonction 'calcul' lorsque le bouton est cliqué.
  connect(&okButton, &QPushButton::clicked, [&]() {
    float debitGoutteur = 0.0f;
    float espacementGoutteur = 0.0f;

    if (default1Button.isChecked()) {
      debitGoutteur = 2.2f;
      espacementGoutteur = 0.6f;
    }
    else if (default2Button.isChecked()) {
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

void etude::divideData() {


  if(_parcelles.size()!=0){
    updateDonnees();
    _parcelles.clear();
    for (auto& parcel : _parcelles) {
      // Obtenir les données de la parcelle
      std::vector<std::vector<float>> parcelData = parcel.getDonnees();

      // Mettre à zéro les colonnes 15, 16, 17, 18, 19 de chaque ligne de données
      for (auto& row : parcelData) {
        if (row.size() > 19) { // Vérifie que les indices à zéro sont valides
          row[15] = 0.0;
          row[16] = 0.0;
          row[17] = 0.0;
          row[18] = 0.0;
          row[19] = 0.0;
        }
      }

      // Ajouter les données de la parcelle à _Donnees
      _Donnees.insert(_Donnees.end(), parcelData.begin(), parcelData.end());
    }

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
        QString nomParcelle = QString("parcelle %1").arg(_parcelles.size() + 1);
        _parcelles.push_back(parcelle(_Donnees, startIndex, endIndex + 1,database, nomParcelle));
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


void etude::chooseCommandPost() {
  // Créer un QDialog pour le choix du poste de commande
  QDialog dialog(this);
  dialog.setWindowTitle("Choisir le poste de commande");

  // Créer un layout pour le QDialog
  QVBoxLayout dialogLayout(&dialog);

  // Créer une QMap pour mapper le numéro de rang à l'index dans le vecteur de données
  QMap<QComboBox*, parcelle*> comboBoxToParcelMap;

  for (auto& parcel : _parcelles) {
    const std::vector<std::vector<float>>& parcelData = parcel.getDonnees();
    if (!parcelData.empty() && !parcelData[0].empty()) {
      QComboBox* rangeNumberComboBox = new QComboBox(&dialog);

      // Map from range number to the index of that range in parcelData
      QMap<int, int> rangeToIndexMap;
      int hydroIndex = parcel.getMilieuhydro();
      int defaultIndex = -1;

      for (size_t i = 0; i < parcelData.size(); ++i) {
        int rangeNumber = static_cast<int>(parcelData[i][0]);
        rangeNumberComboBox->addItem(QString::number(rangeNumber));
        rangeToIndexMap.insert(rangeNumber, i);
      }

      // Récupère le numéro de rang correspondant à l'index du milieu hydrolique
      int rangeNumberHydro = static_cast<int>(parcelData[hydroIndex][0]);

      // Récupère l'index de ce numéro de rang dans la comboBox
      defaultIndex = rangeNumberComboBox->findText(QString::number(rangeNumberHydro));

      if (defaultIndex != -1) {
        rangeNumberComboBox->setCurrentIndex(defaultIndex);
      }


      // Ajouter la combobox au layout du QDialog
      dialogLayout.addWidget(rangeNumberComboBox);

      // Ajouter la combobox et la parcelle correspondante à la QMap
      comboBoxToParcelMap.insert(rangeNumberComboBox, &parcel);
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
      int rangeNumber = comboBox->currentText().toInt();
      parcelle* selectedParcel = comboBoxToParcelMap.value(comboBox);
      if (selectedParcel) {
        selectedParcel->setPosteDeCommande(rangeNumber);
      }
    }
    rafraichirTableau();
    dialog.accept();
  });

  // Afficher le QDialog
  dialog.setLayout(&dialogLayout);
  dialog.exec();
}


void etude::appelSetDiametreDialog() {
  QDialog dialog;
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
                       _parcelles[index].setDiametreDialog();
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

    // Handle KeyPress event as before
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
    }
  }
  // Standard event processing
  return QObject::eventFilter(obj, event);
}



QLineEdit* etude::findNextDiameterLineEdit(int startRow)
{
  int totalRows = gridLayout->rowCount();

  // Commence la recherche
  for (int row = startRow; row < totalRows; ++row)
  {
    // Prend les widgets de la 15eme colonne
    QWidget* widget = gridLayout->itemAtPosition(row, 15)->widget();

    // SI c'est un QLineEdit
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(widget);
    if (lineEdit)
    {
      // Trouvé donc retourné
      return lineEdit;
    }
  }

  // Pas trouvé
  return nullptr;
}

void etude::modifierdiametre(int debut, int fin, float dia){
  if(debut > fin){
    std::swap(debut, fin); // Swap if the indices are not in ascending order
  }
  int i = 0 ;
  for(auto & parcelle : _parcelles){
    auto & datas = parcelle.getDonnees();
    for(int j = 0; j < datas.size();j++){
      if(i>=debut && i<=fin){
        //datas[j][15] = dia;
        parcelle.modifiedia(j,dia);
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
  dialog.setWindowTitle("Changer diametre");

  // Validations
  auto intValidator = new QIntValidator(0, 100);
  auto doubleValidator = new QDoubleValidator(0.0, 100.0, 2);

  // Create and setup the line edits
  std::string d = std::to_string(_Donnees.size()) ;
  QLineEdit indiceDebutLineEdit(QString::fromStdString("0"));
  indiceDebutLineEdit.setValidator(intValidator);
  QLineEdit indiceFinLineEdit(QString::fromStdString(d));
  indiceFinLineEdit.setValidator(intValidator);
  QLineEdit diametreLineEdit;
  diametreLineEdit.setValidator(doubleValidator);

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
    int debut = indiceDebutLineEdit.text().toInt();
    int fin = indiceFinLineEdit.text().toInt();
    float diametre = diametreLineEdit.text().toFloat();
    modifierdiametre(debut, fin, diametre);
    dialog.close();
  });

  dialog.exec();
}
