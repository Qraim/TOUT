//
// Created by qraim on 05/04/23.
//

#include "tableau.h"
#include <QDir>
#include <QFileDialog>

const double LH_TO_M3S = 1.0 / (1000.0 * 3600.0);
const double LS_TO_M3S = 1.0 / 1000.0;
const double M3H_TO_M3S = 1.0 / 3600.0;

const int ROW_HEIGHT = 40;
const int VERTICAL_SPACING = 1;

float PI =3.14159265359;

void espacementColonne(QGridLayout *layout) {
  // Parcourt toutes les colonnes du layout
  for (int i = 0; i < layout->columnCount(); i++) {
    layout->setColumnStretch(i,
                             1); // Étire la colonne i avec une proportion de 1
  }
}

pertechargeherse::pertechargeherse(std::shared_ptr<bdd> db, QWidget *parent)
    : QWidget(parent), database(db) {

  setWindowTitle(QString::fromStdString("Herse d'alimentation"));

  setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

  ligne = 1;

  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // entete
  QWidget *headersWidget = new QWidget(this);
  QGridLayout *headersGridLayout = new QGridLayout(headersWidget);

  // Ajout des titres des colonnes
  const QStringList headers = {"Numero",    "Debit",   "ΣDebit",  "Diametre",
                                                             "Intervale ", "Hauteur", "Vitesse", "Perte",
                                "Piezo",     "ΣPerte",  "ΣPiezo"};

    for (int i = 0; i < headers.size(); ++i) {
      QLabel *label = new QLabel(headers[i], headersWidget);
      label->setAlignment(Qt::AlignCenter);
      label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      label->setFixedHeight(40); // Fixe la taille
      headersGridLayout->addWidget(label, 0, i);
      headersGridLayout->setHorizontalSpacing(0); // On enlève les espaces
    }

    // Ajoute les unités dans le header
    const QStringList units = {" ",   " ", " ", "mm", "m", "m",
                               "m/s", "m",   "m",   "m",  "m"};


    // 4 champ d'entré
    inputQ = new QLineEdit(this);
    inputD = new QLineEdit(this);
    inputL = new QLineEdit(this);
    inputH = new QLineEdit(this);

    inputQ->setMaximumWidth(75);
    inputD->setMaximumWidth(75);
    inputL->setMaximumWidth(75);
    inputH->setMaximumWidth(75);

    inputQ->setAlignment(Qt::AlignCenter);
    inputD->setAlignment(Qt::AlignCenter);
    inputL->setAlignment(Qt::AlignCenter);
    inputH->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < units.size(); ++i) {
          if(i!=1){
              QLabel *unitLabel = new QLabel(units[i], headersWidget);
              unitLabel->setAlignment(Qt::AlignCenter);
              unitLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
              unitLabel->setFixedHeight(40); // Fixe la taille
              headersGridLayout->addWidget(unitLabel, 1, i);
          } else {
              unite = new QComboBox(this);
              unite->addItem("  l/h");
              unite->addItem("  l/s");
              unite->addItem(" m3/h");
              unite->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
              unite->setFixedHeight(40); // Fixe la taille
              unite->setFixedWidth(100); // Fixe la taille
              headersGridLayout->addWidget(unite, 1, i);
              headersGridLayout->setAlignment(unite, Qt::AlignCenter); // Center the QComboBox
          }
    }

    // ajout des entrés
    headersGridLayout->addWidget(inputQ, 2, 1, Qt::AlignCenter);
    headersGridLayout->addWidget(inputD, 2, 3, Qt::AlignCenter);
    headersGridLayout->addWidget(inputL, 2, 4, Qt::AlignCenter);
    headersGridLayout->addWidget(inputH, 2, 5, Qt::AlignCenter);

    headersGridLayout->setHorizontalSpacing(0);

    mainLayout->addWidget(headersWidget);

    // Scroll Area
    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidget(scrollWidget);
    scrollWidget->setMinimumSize(sizeHint());

    gridLayout = new QGridLayout(scrollWidget);
    scrollWidget->setLayout(gridLayout);

    mainLayout->addWidget(scrollArea);

    // layout du bas
    QWidget *bottomWidget = new QWidget(this);
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomWidget);

    mainLayout->addWidget(bottomWidget);

    // Material ComboBox
    Materiau = new QComboBox(this);
    Materiau->setFixedSize(100, 40);

    std::vector<std::string> matiere_names = database->getAllMatiereNames();
    for (const auto &matiere_name : matiere_names) {
      Materiau->addItem(QString::fromStdString(matiere_name));
    }

    headersGridLayout->addWidget(Materiau, 2, 0, Qt::AlignCenter);

  // AJoute les 4 QLineEdits a BottomWidget
  sigmadebitcase = new QLineEdit(bottomWidget);
  sigmalongueurcase = new QLineEdit(bottomWidget);
  sigmapertecase = new QLineEdit(bottomWidget);
  sigmapiezocase = new QLineEdit(bottomWidget);

  sigmadebitcase->setReadOnly(true);
  sigmalongueurcase->setReadOnly(true);
  sigmapertecase->setReadOnly(true);
  sigmapiezocase->setReadOnly(true);

  sigmadebitcase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sigmalongueurcase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sigmapertecase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sigmapiezocase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QGridLayout *bottomGrid = new QGridLayout();
  bottomLayout->addLayout(bottomGrid);

  QSpacerItem *spacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer2 = new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer3 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer4 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer5 = new QSpacerItem(50, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer6 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer7 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
  QSpacerItem *spacer8 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

  bottomGrid->addItem(spacer1, 0, 0);
  bottomGrid->addItem(spacer2, 0, 3);
  bottomGrid->addItem(spacer3, 0, 6);
  bottomGrid->addItem(spacer4, 0, 9);
  bottomGrid->addItem(spacer5, 0, 1);
  bottomGrid->addItem(spacer6, 0, 5);
  bottomGrid->addItem(spacer7, 0, 7);
  bottomGrid->addItem(spacer8, 0, 8);

  // fixe la taille
  sigmadebitcase->setFixedSize(170, 40);
  sigmalongueurcase->setFixedSize(170, 40);
  sigmapertecase->setFixedSize(170, 40);
  sigmapiezocase->setFixedSize(170, 40);

  bottomGrid->addWidget(sigmadebitcase, 0, 2, Qt::AlignCenter);
  bottomGrid->addWidget(sigmalongueurcase, 0, 4, Qt::AlignCenter);
  bottomGrid->addWidget(sigmapertecase, 0, 10, Qt::AlignCenter);
  bottomGrid->addWidget(sigmapiezocase, 0, 11, Qt::AlignCenter);

  QPushButton *saveAsPdfButton = new QPushButton("Export PDF", this);

  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->addStretch();
  mainLayout->insertLayout(0, topLayout);

  QPushButton *saveDataButton = new QPushButton("Sauvergarder", this);
  QPushButton *loadDataButton = new QPushButton("Charger", this);

  saveAsPdfButton->setMaximumWidth(140);
  saveDataButton->setMaximumWidth(140);
  loadDataButton->setMaximumWidth(140);

  QPushButton *calculButton = new QPushButton("Calculer");
  QPushButton *insererbutton = new QPushButton("Insérer");
  QPushButton *effacerButton = new QPushButton("Effacer");
  QPushButton *modifierButton = new QPushButton("Modifier");
  QPushButton *recopier = new QPushButton("Recopier");
  QPushButton *reinitialiserButton = new QPushButton("Réinitialiser");
  QPushButton *editdiam = new QPushButton("Diametre");

  calculButton->setMaximumWidth(140);
  effacerButton->setMaximumWidth(140);
  modifierButton->setMaximumWidth(140);
  reinitialiserButton->setMaximumWidth(140);
  recopier->setMaximumWidth(140);
  editdiam->setMaximumWidth(140);

  connect(calculButton, &QPushButton::clicked, this, &pertechargeherse::calcul);
  connect(effacerButton, &QPushButton::clicked, this, &pertechargeherse::enleverLigne);
  connect(modifierButton, &QPushButton::clicked, this, &pertechargeherse::showUpdateDialog);
  connect(reinitialiserButton, &QPushButton::clicked, this, &pertechargeherse::refresh);
  connect(recopier, &QPushButton::clicked, this, &pertechargeherse::recopiederniereligne);
  connect(insererbutton, &QPushButton::clicked, this, &pertechargeherse::addRow);
  connect(editdiam, &QPushButton::clicked, this, &pertechargeherse::editDiameter);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();

  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(calculButton);
  buttonsLayout->addWidget(effacerButton);
  buttonsLayout->addWidget(modifierButton);
  buttonsLayout->addWidget(recopier);
  buttonsLayout->addWidget(editdiam);
  buttonsLayout->addWidget(insererbutton);
  buttonsLayout->addWidget(reinitialiserButton);
  buttonsLayout->addWidget(saveAsPdfButton);
  buttonsLayout->addWidget(saveDataButton);
  buttonsLayout->addWidget(loadDataButton);
  buttonsLayout->addStretch(1);

  connect(saveAsPdfButton, &QPushButton::clicked, this, &pertechargeherse::saveAsPdf);
  connect(saveDataButton, &QPushButton::clicked, this, &pertechargeherse::saveDataWrapper);
  connect(loadDataButton, &QPushButton::clicked, this, &pertechargeherse::loadDataWrapper);

  mainLayout->addLayout(buttonsLayout);

  inputQ->setFocus();

  bottomLayout->setSpacing(5);

  inputD->installEventFilter(this);
  inputQ->installEventFilter(this);
  inputH->installEventFilter(this);
  inputL->installEventFilter(this);

  QDesktopWidget desktop;
  QRect screenSize = desktop.availableGeometry(this);

  this->setGeometry(screenSize);
}

void pertechargeherse::saveAsPdf() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save as PDF", QDir::homePath(), "PDF Files (*.pdf)");

  if (!fileName.isEmpty()) {
    createPdfReport(fileName);
  }
}

void pertechargeherse::saveDataWrapper() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save Data", QDir::homePath(), "Data Files (*.dat)");

  if (!fileName.isEmpty()) {
    saveData(fileName);
  }
}

void pertechargeherse::loadDataWrapper() {
  QString fileName = QFileDialog::getOpenFileName(
      this, "Load Data", QDir::homePath(), "Data Files (*.dat)");

  if (!fileName.isEmpty()) {
    loadData(fileName);
  }
}

void pertechargeherse::AjoutDonne() {

  QString debitText = inputQ->text();
  debitText.replace(',', '.');
  float debit = debitText.toFloat();

  QString diametreText = inputD->text();
  diametreText.replace(',', '.');
  float diametre = diametreText.toFloat();

  QString longueurtexte = inputL->text();
  longueurtexte.replace(',', '.');
  float longueur = longueurtexte.toFloat();

  QString hauteurtext = inputH->text();
  hauteurtext.replace(',', '.');
  float hauteur = hauteurtext.toFloat();

  int numero = _Donnees.size() + 1; // numéro

  std::vector<float> temp(11, 0.0f); // variable temporaire

  temp[0] = numero;
  temp[1] = debit;
  temp[3] = diametre;
  temp[4] = longueur;
  temp[5] = hauteur;

  _Donnees.push_back(temp); // mise dans le set des données

  inputD->clear();
  inputH->clear();
  inputL->clear();
  inputQ->clear();
  inputQ->setFocus();

  AjoutLigne();
}

bool pertechargeherse::focusNextPrevChild(bool next) {
  return false;
}


void pertechargeherse::AjoutLigne() {

  if (_Donnees.empty()) { // Si le vecteur "_Donnees" est vide, sort de la fonction.
    return;
  }

  const std::vector<float> &rowData =
      _Donnees.back(); // Récupère les données de la dernière ligne.

  // On spécifie les colonnes que l'on veut remplir.
  const std::vector<int> indices = {0, 1, -1, 3, 4, 5, -1, -1, -1, -1, -1};

  for (int i = 0; i < indices.size(); ++i) {

    QLineEdit *lineEdit = new QLineEdit(scrollWidget);
    // Make QLineEdit editable
    if (i == 1 || i == 3 || i == 4 || i == 5) {
              lineEdit->setReadOnly(false);
              lineEdit->installEventFilter(this); // Install event filter
    } else {
              lineEdit->setReadOnly(true);
    }

    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lineEdit->setFixedWidth(200);
    lineEdit->setFixedHeight(40);
    lineEdit->setStyleSheet("color: yellow;"); // Changer la couleur du texte en bleu

    if (indices[i] != -1) { // Si l'indice n'est pas égal à -1, on remplit la case correspondante avec la donnée.
              if (i == 0) {
                  lineEdit->setText(QString::number(static_cast<int>(rowData[i]))); // Formatte la donnée en nombre entier s'il s'agit de la première colonne.
              } else {
                  lineEdit->setText(
                      QString::number(rowData[indices[i]], 'f',
                                      2)); // Formatte la donnée en nombre décimal s'il s'agit d'une autre colonne.
              }
    } else {
              lineEdit->setText(""); // Si l'indice est égal à -1, on laisse la case vide.
    }

    // Définit l'espacement vertical du layout.
    gridLayout->setVerticalSpacing(VERTICAL_SPACING);

    // Ajoute la ligne de texte au layout.
    gridLayout->addWidget(lineEdit, ligne, i);
  }

  ligne++; // On incrémente la valeur de "ligne" pour la prochaine ligne à
  // ajouter.

  // Calcule la hauteur du widget de défilement et ajuste sa hauteur minimum et
  // maximum en conséquence.
  int scrollWidgetHeight =
      (ligne * ROW_HEIGHT) + ((ligne - 1) * VERTICAL_SPACING);
  scrollWidget->setMinimumHeight(scrollWidgetHeight);
  scrollWidget->setMaximumHeight(scrollWidgetHeight);

  scrollArea->ensureVisible(0, scrollWidgetHeight);

  // Set the vertical spacing and alignment.
  gridLayout->setVerticalSpacing(VERTICAL_SPACING);
  gridLayout->setAlignment(Qt::AlignTop);
}

void pertechargeherse::on_lineEdit_editingFinished(const QString &text, int row, int col) {

  bool ok;
  float value = text.toFloat(&ok);

  if (ok) {
    if (row >= 0 && row < _Donnees.size() && col >= 0 && col < _Donnees[row].size()) {
      _Donnees[row][col] = value;
    } else {
      qDebug() << "Error: Invalid row or col value.";
    }
  }
}




// La fonction "focusPreviousInput" est appelée lorsqu'un raccourci clavier est
// utilisé pour passer au champ d'entrée précédent.
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
  else if (inputD->hasFocus()) {
    inputQ->setFocus();
  }
}

// La fonction "focusNextInput" est appelée lorsqu'un raccourci clavier est
// utilisé pour passer au champ d'entrée suivant.
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
  else if (inputL->hasFocus()) {
    inputH->setFocus();
  }
}

void pertechargeherse::findWidgetIndex(QLineEdit *widget, int &row, int &col) {
  row = -1;
  col = -1;

  for (int i = 0; i < _Donnees.size(); ++i) {
    for (int j = 0; j < _Donnees[i].size(); ++j) {
      QLineEdit *currentLineEdit = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(i + 1, j)->widget());
      if (currentLineEdit == widget) {
        row = i;
        col = j;
        return;
      }
    }
  }
}


bool pertechargeherse::eventFilter(QObject *obj, QEvent *event) {


  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);

  // Add static variables to keep track of the initial QLineEdit and the mouse button state.
  static QLineEdit *initialLineEdit = nullptr;
  static bool leftMouseButtonPressed = false;
  static QPoint lastMousePos;


  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // Modifiers
    bool controlPressed = keyEvent->modifiers() & Qt::ControlModifier;
    bool shiftPressed = keyEvent->modifiers() & Qt::ShiftModifier;

    if (keyEvent->key() == Qt::Key_R) {
      recopiederniereligne();
      return true; // marqué l'event comme fait
    } else if (keyEvent->key() == Qt::Key_E) {
      _Donnees.clear();
      clearchild();
      return true;
    } else if (keyEvent->key() == Qt::Key_M) {
      showUpdateDialog();
      return true;

    } else if (keyEvent->key() == Qt::Key_Z) {
      enleverLigne();
      return true;

    }  else if (keyEvent->key() == Qt::Key_I) {
      addRow();
      return true;
    }

    // si M est appuyé ouvre la fenetre de modification
    else if(keyEvent->key() == Qt::Key_P) {
      importData();
      return true;
    }
    else if (keyEvent->key() == Qt::Key_D) {
      editDiameter();
      return true;
    }
    else if (keyEvent->key() == Qt::Key_C) {
      calcul();
      return true;

    } else if (keyEvent->key() == Qt::Key_Tab) {
      if(lineEdit){
        if (keyEvent->key() == Qt::Key_Tab) {
            int i, j;
            bool found = false;
            for (i = 0; i < _Donnees.size() && !found; ++i) {
                for (j = 0; j < _Donnees[i].size() && !found; ++j) {
                    QLineEdit *currentLineEdit = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(i + 1, j)->widget());
                    if (currentLineEdit == lineEdit) {
                        found = true;
                    }
                }
            }

            if (found) {
                --i; // ajustement de l'index
                on_lineEdit_editingFinished(lineEdit->text(), i, j-1);

                // On bouge à la ligne du dessous
                if (i + 1 < _Donnees.size()) {
                    QLineEdit *nextLineEdit = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(i + 2, j-1)->widget());
                    if (nextLineEdit) {
                        nextLineEdit->setFocus();
                        nextLineEdit->selectAll();
                    }
                } else {
                    // If the next row does not exist, stay in the same cell.
                    lineEdit->setFocus();
                    lineEdit->selectAll();
                }
                return true;
            }
        }
      }
      else if (Allinputfill()) {
        AjoutDonne();
        return true;
      } else {
        return true; // retire l'action naturel de tab
      }
    }
    // Save as PDF (Ctrl + S)
    else if (controlPressed && keyEvent->key() == Qt::Key_S && !shiftPressed) {
      saveAsPdf();
      return true;
    }
    // Save data (Ctrl + Shift + S)
    else if (controlPressed && keyEvent->key() == Qt::Key_S && shiftPressed) {
      saveDataWrapper();
      return true;
    }
    // Load data (Ctrl + L)
    else if (controlPressed && keyEvent->key() == Qt::Key_L && !shiftPressed) {
      loadDataWrapper();
      return true;
    }

  }
  else  // clique de souris dans la grille
    if (lineEdit) {
      if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            leftMouseButtonPressed = true;
            initialLineEdit = lineEdit;
            lastMousePos = mouseEvent->pos();
            QTimer::singleShot(0, lineEdit, &QLineEdit::selectAll);
            return false;
        }
      } else if (event->type() == QEvent::MouseMove && leftMouseButtonPressed) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (initialLineEdit && (mouseEvent->pos().y() - lastMousePos.y() > 10)) {
            lastMousePos = mouseEvent->pos();

            int iInitial, jInitial, iCurrent, jCurrent;
            findWidgetIndex(initialLineEdit, iInitial, jInitial);
            findWidgetIndex(lineEdit, iCurrent, jCurrent);

            if (jInitial == jCurrent && iCurrent > iInitial) {
                for (int i = iInitial + 1; i <= iCurrent; ++i) {
                    QLineEdit *currentLineEdit = qobject_cast<QLineEdit *>(gridLayout->itemAtPosition(i + 1, jInitial)->widget());
                    if (currentLineEdit) {
                        currentLineEdit->setText(initialLineEdit->text());
                    }
                }
            }
        }
        return false;
      } else if (event->type() == QEvent::MouseButtonRelease) {
        leftMouseButtonPressed = false;
        initialLineEdit = nullptr;
        return false;
      }
    }
  return QWidget::eventFilter(obj, event);
}


void pertechargeherse::keyPressEvent(QKeyEvent *event) {

  bool controlPressed = event->modifiers() & Qt::ControlModifier;
  bool shiftPressed = event->modifiers() & Qt::ShiftModifier;

  if (event->key() == Qt::Key_Tab) {
    event->ignore();
    return;
  }

  if (event->key() == Qt::Key_Control) {
    focusPreviousInput();
    return;
  }

  // si tab ou entrée est appuyé
  else if (event->key() == Qt::Key_Tab || event->key() == Qt::Key_Return) {

    if (shiftPressed && event->key() == Qt::Key_Return && Allinputfill()) {
      calcul();
      return;
    }
    else if (event->key() == Qt::Key_Return && Allinputfill()) {
      AjoutDonne();
    }

    else if (event->key() == Qt::Key_Tab && Allinputfill()){

      event->accept();

      AjoutDonne();
      return;
    }

    // changer le champ d'entré
    else {
      focusNextInput();
    }

  }

  // E pour effacer toutes les données
  else if (event->key() == Qt::Key_E) {
    _Donnees.clear();
    clearchild();
  }

  else if (event->key() == Qt::Key_C ) {
    calcul();
  }

  // R copie la derniere ligne
  else if (event->key() == Qt::Key_R) {
    if (_Donnees.size() > 0) {
      recopiederniereligne();
    }
  }

  // si M est appuyé ouvre la fenetre de modification
  else if (event->key() == Qt::Key_M) {
    showUpdateDialog();
  }

  // si M est appuyé ouvre la fenetre de modification
  else if (event->key() == Qt::Key_P) {
    importData();
  }

  else if (event->key() == Qt::Key_D) {
    editDiameter();
  }

  // si Z est appuyé ouvre la fenetre pour retirer une ligne
  else if (event->key() == Qt::Key_Z) {
    enleverLigne();
  }

  else if (event->key() == Qt::Key_I) {
    addRow();
  }

  // Save as PDF (Ctrl + S)
  else if (controlPressed && event->key() == Qt::Key_S && !shiftPressed) {
    saveAsPdf();
    return;
  }

  // Save data (Ctrl + Shift + S)
  else if (controlPressed && event->key() == Qt::Key_S && shiftPressed) {
    saveDataWrapper();
    return;
  }

  // Load data (Ctrl + L)
  else if (controlPressed && event->key() == Qt::Key_L && !shiftPressed) {
    loadDataWrapper();
    return;
  }
}

bool pertechargeherse::Allinputfill() {
  if (inputD->text().isEmpty() || inputQ->text().isEmpty() ||
      inputH->text().isEmpty() || inputL->text().isEmpty()) {
    return false; // Si au moins 1 est vide
  }
  return true; // Tout les inputs sont remplis
}

// La fonction "calcul" effectue des calculs sur les données du
// pertechargeherse.
void pertechargeherse::calcul() {

  if (_Donnees.size() == 0) {
    return;
  }

  int unit = unite->currentIndex();

  // Initialise les paramètres.
  double k = 0;
  float a = 0;
  float b = 0;

  // Initialise les variables.
  float debitM3 = 0; // Débit en m3/h
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
  float sigmaDebitLs = 0;
  std::tuple<float, float, double> coefficients =
      database->get_material_coefficients(
          Materiau->currentText().toStdString());
  a = std::get<0>(coefficients);
  b = std::get<1>(coefficients);
  k = std::get<2>(coefficients);

  // Effectue les calculs pour chaque ligne de données.
  for (int i = 0; i < _Donnees.size(); ++i) {

    // Calcule le cumul de débit.
    sigmaDebit += _Donnees[i][1];
    _Donnees[i][2] = sigmaDebit;

    // Récupère les données de la ligne courante.
    diametre = _Donnees[i][3];
    longueur = _Donnees[i][4];
    hauteur = _Donnees[i][5];

    if(unit==0){ // l/h
      // Calcule le débit en m3/s.
      debitM3 = sigmaDebit / 1000 /3600;

      // Convertit le débit en l/s.
      sigmaDebitLs = sigmaDebit / 3600;
    } else if(unit==1){ // l/s
      sigmaDebitLs = sigmaDebit;

      // Calcule le débit en m3/s.
      debitM3 = sigmaDebit / 1000;
    } else if(unit==2){ // m3/h

      sigmaDebitLs = sigmaDebit / 3.6;

      // Calcule le débit en m3/s.
      debitM3 = sigmaDebit / 3600;
    }

    // Calcule l'aire du tuyau.
    aireTuyau = (PI * pow((diametre/1000) / 2, 2));

    // Calcule la vitesse.
    vitesse = debitM3 / aireTuyau;

    // Calcule la perte de charge.
    perteCharge = k * pow(sigmaDebitLs, a) * pow(diametre, b) * longueur;

    // Calcule la hauteur piezométrique.
    piezo = perteCharge + hauteur;

    // Ajoute les données calculées au vecteur.
    _Donnees[i][6] = vitesse;
    _Donnees[i][7] = perteCharge;
    _Donnees[i][8] = piezo;

    // Calcule les cumuls pour chaque ligne de données.
    sigmaPerte += _Donnees[i][7];
    sigmaPiezo += _Donnees[i][8];
    sigmaLongueur += _Donnees[i][4];
    _Donnees[i][9] = sigmaPerte;
    _Donnees[i][10] = sigmaPiezo;
  }

  // Affiche les résultats dans les cases correspondantes en arrondissant à deux
  // chiffres après la virgule.
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


// La fonction "RafraichirTableau" permet de mettre à jour un pertechargeherse
// en affichant les nouvelles données.
void pertechargeherse::RafraichirTableau() {
  // Supprime toutes les cases du pertechargeherse.
  clearchild();

  // Initialise le numéro de ligne.
  ligne = 1;

  // Trouve l'index où le débit cumulé est supérieur à la moitié du débit cumulé
  // total.
  float cumulDebit = _Donnees[_Donnees.size() - 1][2];
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

    // Détermine la couleur du texte.
    QString textColor;
    for (int i = 0; i < indicesColonnes.size(); ++i) {

      if(i == 1 || i == 3 || i == 4 || i == 5){
        textColor = "QLineEdit { color: #eaff00; }";
      }
      else{
        textColor = "QLineEdit { color: #64f5a3; }";
      }

      // Crée une nouvelle ligne de texte.
      QLineEdit *lineEdit = new QLineEdit(scrollWidget);
      lineEdit->setStyleSheet(textColor);

      if (i == 1 || i == 3 || i == 4 || i == 5) {
        lineEdit->setReadOnly(false);

        lineEdit->installEventFilter(this); // Install event filter

      } else {
        lineEdit->setReadOnly(true);
      }
      // Centre le texte dans la ligne de texte.
      lineEdit->setAlignment(Qt::AlignCenter);

      // Définit la politique de redimensionnement de la ligne de texte.
      lineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

      // Définit la hauteur fixe de la ligne de texte.
      lineEdit->setFixedHeight(ROW_HEIGHT);

      lineEdit->setFixedWidth(200);

      if (i == 0) {
        lineEdit->setText(QString::number(static_cast<int>(donneesLigne[i])));
      } else {
        QString formattedText = QString::number(donneesLigne[indicesColonnes[i]], 'f', 2);
        if (i == 6 && donneesLigne[6] > 2) {
            lineEdit->setStyleSheet("QLineEdit { background-color : red; color : white; }");
        }
        lineEdit->setText(formattedText);
      }



      // Colorie la ligne en orange pour le milieu hydrolique.
      if (ligne - 1 == indexMilieuHydrolique) {
        lineEdit->setStyleSheet("QLineEdit { background-color : orange; }");
      }
      // Définit l'espacement vertical du layout.
      gridLayout->setVerticalSpacing(VERTICAL_SPACING);
      gridLayout->addWidget(lineEdit, ligne, indicesColonnes[i]);



    }

    // Incrémente le numéro de ligne.
    ligne++;
  }

  // Calcule la hauteur du widget de défilement et ajuste sa hauteur minimum et
  // maximum en conséquence.
  int scrollWidgetHeight =
      (ligne * ROW_HEIGHT) + ((ligne - 1) * VERTICAL_SPACING);
  scrollWidget->setMinimumHeight(scrollWidgetHeight);
  scrollWidget->setMaximumHeight(scrollWidgetHeight);

  scrollArea->ensureVisible(0, scrollWidgetHeight);


  // Set the vertical spacing and alignment.
  gridLayout->setVerticalSpacing(VERTICAL_SPACING);
  gridLayout->setAlignment(Qt::AlignTop);
}

// La fonction "clearchild" permet de retirer toutes les cases dans un layout.

void pertechargeherse::clearchild() {
  ligne = 1;
  QLayoutItem *enfant;

  // Tant qu'il y a encore des éléments dans le layout, retire-les.
  while ((enfant = gridLayout->takeAt(0)) != nullptr) {

    // Supprime le widget contenu dans l'élément layout.
    delete enfant->widget();

    // Supprime l'élément layout.
    delete enfant;
  }

  inputH->clear();
  inputQ->clear();
  inputD->clear();
  inputL->clear();

}

void pertechargeherse::
recopiederniereligne() { // Fonction déclenche par la touche 'R' qui permet
  // de recopier la derniere ligne entrée
  if(_Donnees.size()==0){
    return;
  }

  int taille = _Donnees.size();
  std::vector<float> lastline =
      _Donnees[taille - 1]; // Pour avoir la derniere ligne du vecteur
  std::vector<float> recopie(11, 0.0f);

  recopie[0] = lastline[0] + 1; // Numéro
  recopie[1] = lastline[1];     // Debit
  recopie[3] = lastline[3];     // Diametre
  recopie[4] = lastline[4];     // Longueur
  recopie[5] = lastline[5];     // Hauteur

  _Donnees.push_back(recopie);
  AjoutLigne();
}

// Ouvre une boîte de dialogue permettant à l'utilisateur de modifier les
// données d'une ligne dans le pertechargeherse
void pertechargeherse::showUpdateDialog() {
  // Crée une nouvelle fenêtre de dialogue en tant que sous-fenêtre de la
  // fenêtre principale de l'application
  QDialog *updateDialog = new QDialog(this);
  // Définit le titre de la fenêtre de dialogue
  updateDialog->setWindowTitle("Modifier ligne");

  // Crée un formulaire dans la fenêtre de dialogue pour saisir les nouvelles
  // données de la ligne
  QFormLayout *formLayout = new QFormLayout(updateDialog);

  // Ajoute un champ pour entrer le numéro de ligne à modifier
  QLineEdit *rowNumberLineEdit = new QLineEdit(updateDialog);
  rowNumberLineEdit->setValidator(new QIntValidator(
      0, _Donnees.size() - 1,
      updateDialog)); // Permet de limiter la saisie à un nombre entier valide
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

  // Ajoute un bouton pour valider les nouvelles données et fermer la fenêtre de
  // dialogue
  QPushButton *updateButton = new QPushButton("Modifier", updateDialog);
  formLayout->addWidget(updateButton);

  // Fonction qui permet de définir le comportement à adopter lorsque
  // l'utilisateur appuie sur la touche "Entrée" dans un champ de saisie
  auto handleEnterKeyPress = [this](QLineEdit *current, QLineEdit *next) {
      QObject::connect(current, &QLineEdit::returnPressed, [this, current, next]() {
          // Si le champ en cours de saisie est rempli et que le champ suivant (s'il
          // y en a un) est également rempli, déplace le curseur vers le champ
          // suivant
          if (!current->text().isEmpty() &&
              (next == nullptr || !next->text().isEmpty())) {
              if (next) {
                  next->setFocus();
              }
          }
      });
  };



  // Associe la fonction handleEnterKeyPress aux champs de saisie pour gérer les
  // appuis sur la touche "Entrée"
  handleEnterKeyPress(rowNumberLineEdit, debitLineEdit);
  handleEnterKeyPress(debitLineEdit, diameterLineEdit);
  handleEnterKeyPress(diameterLineEdit, lengthLineEdit);
  handleEnterKeyPress(lengthLineEdit, heightLineEdit);
  handleEnterKeyPress(
      heightLineEdit,
      nullptr); // Le dernier champ n'a pas de champ suivant, donc next est nul

  // Fonction qui met à jour les données de la ligne avec les nouvelles données
  // saisies par l'utilisateur et ferme la fenêtre de dialogue
  auto updateDataAndClose = [this, rowNumberLineEdit, debitLineEdit,
      diameterLineEdit, lengthLineEdit, heightLineEdit,
      updateDialog]() {
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

  // Associe la fonction updateDataAndClose au bouton "Update" pour mettre à
  // jour les données de la ligne et fermer la fenêtre de dialogue lorsque
  // l'utilisateur clique sur le bouton
  connect(updateButton, &QPushButton::clicked, updateDataAndClose);

  // Ajoute un raccourci clavier pour valider les nouvelles données et fermer la
  // fenêtre de dialogue lorsque l'utilisateur appuie sur la touche "Entrée"
  QShortcut *enterShortcut =
      new QShortcut(QKeySequence(Qt::Key_Enter), updateDialog);
  QShortcut *returnShortcut =
      new QShortcut(QKeySequence(Qt::Key_Return), updateDialog);
  connect(enterShortcut, &QShortcut::activated, updateDataAndClose);
  connect(returnShortcut, &QShortcut::activated, updateDataAndClose);

  // Définit le layout du formulaire comme layout de la fenêtre de dialogue et
  // affiche la fenêtre de dialogue modale
  updateDialog->setLayout(formLayout);
  updateDialog->exec();
}

// La fonction "updateData" prend en entrée un numéro de ligne, un débit, un
// diamètre, une longueur et une hauteur, puis met à jour les données de la
// ligne correspondante dans un pertechargeherse.
void pertechargeherse::updateData(int numeroLigne, float debit, float diametre,
                                  float longueur, float hauteur) {

  // Vérifie si le numéro de ligne est valide (c'est-à-dire s'il se trouve dans
  // la plage d'index des données).
  if (numeroLigne >= 0 && numeroLigne < _Donnees.size()) {
    // Décrémente le numéro de ligne de 1 pour correspondre à l'indice de
    // pertechargeherse (qui commence à 0).
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
  // Crée une nouvelle fenêtre de dialogue pour demander le numéro de la ligne à
  // supprimer
  QDialog *supprimerDialog = new QDialog(this);
  supprimerDialog->setWindowTitle("Supprimer une ligne");
  supprimerDialog->setFixedSize(300, 100);

  // Ajoute un label et un champ de saisie pour entrer le numéro de la ligne à
  // supprimer
  QLabel *numeroLigneLabel = new QLabel("Numéro de ligne :", supprimerDialog);
  QLineEdit *numeroLigneLineEdit = new QLineEdit(supprimerDialog);
  numeroLigneLineEdit->setValidator(new QIntValidator(
      1, _Donnees.size(), supprimerDialog)); // Permet de limiter la saisie à un
  // nombre entier valide
  numeroLigneLineEdit->setFixedWidth(50);

  // Ajoute un bouton pour confirmer la suppression de la ligne
  QPushButton *supprimerButton = new QPushButton("Supprimer", supprimerDialog);
  connect(supprimerButton, &QPushButton::clicked,
          [this, numeroLigneLineEdit, supprimerDialog]() {
            int numeroLigne = numeroLigneLineEdit->text().toInt() - 1;
            if (numeroLigne >= 0 && numeroLigne < _Donnees.size()) {
              _Donnees.erase(
                  _Donnees.begin() +
                  numeroLigne); // Supprime la ligne du pertechargeherse
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

#include <QPainter>
#include <QPdfWriter>

#include <QDateEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

void pertechargeherse::createPdfReport(const QString &fileName) {
  calcul();
  // Créez QDialog pour la saisie de l'utilisateur
  QDialog inputDialog;
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
  connect(buttonBox, &QDialogButtonBox::accepted, &inputDialog,
          &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, &inputDialog,
          &QDialog::reject);

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
  painter.drawText(pdfWriter.width() - 1000, lineHeight - 200,
                   QString("Date: %1").arg(date));

  const QStringList headerLabels = {
      "Index",   "Debit", "Cumul", "Diametre",    "Longueur",   "Hauteur",
      "Vitesse", "Perte", "Piezo", "Cumul Perte", "Cumul Piezo"};

  QVector<int> columnWidths = {600, 800, 800, 1000, 1000, 800,
                               800, 800, 800, 1000, 1000};
  int tableWidth = 0;
  for (int width : columnWidths) {
    tableWidth += width;
  }

  // Définir la taille de police pour l'en-tête
  QFont headerFont = painter.font();
  headerFont.setPointSize(10);
  painter.setFont(headerFont);

  int yOffset = lineHeight * 5;
  int currentPage = 1;

  auto drawHeaderAndLines = [&](int yOffset) {
    int xPos = 0;
    for (int i = 0; i < headerLabels.size(); ++i) {
      QRect headerRect(xPos, yOffset - lineHeight, columnWidths[i], lineHeight);
      painter.drawText(headerRect, Qt::AlignCenter, headerLabels[i]);
      xPos += columnWidths[i];
    }

    yOffset += lineHeight;

    // Dessiner une ligne horizontale pour séparer l'en-tête et les nombres
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(0, yOffset - lineHeight, tableWidth, yOffset - lineHeight);

    // Détermine le nombre de ligne par page
    int maxRowsPerPage = (pdfWriter.height() - yOffset - 150) / lineHeight;

    // Calculez le nombre de lignes restantes pour déterminer si vous êtes sur
    // la dernière page
    int remainingRows = _Donnees.size() - (currentPage - 1) * maxRowsPerPage;
    bool isLastPage = (remainingRows <= maxRowsPerPage);

    // Ajustez la hauteur des lignes verticales pour la dernière page en
    // fonction du nombre de lignes restantes
    int verticalLinesHeight = isLastPage ? (lineHeight * (remainingRows - 1))
                                         : (lineHeight * (maxRowsPerPage - 1));

    // Dessinez des lignes verticales pour séparer les colonnes
    xPos = 0;
    for (int i = 0; i < columnWidths.size(); ++i) {
      painter.drawLine(xPos, yOffset - lineHeight, xPos,
                       yOffset + verticalLinesHeight);
      xPos += columnWidths[i];
    }
    painter.drawLine(xPos, yOffset - lineHeight, xPos,
                     yOffset + verticalLinesHeight);

    // Réinitialiser la taille de police pour le reste du contenu
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
      // Desinne le bas de page
      QString footerText = QString("Page %1").arg(currentPage);
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
      currentPage++;
    }
  }

  // Ajouter un pied de page sur la dernière page
  QString footerText = QString("Page %1").arg(currentPage);
  painter.drawText(
      QRect(0, pdfWriter.height() - lineHeight, pdfWriter.width(), lineHeight),
      Qt::AlignCenter, footerText);
  painter.drawText(QRect(0, pdfWriter.height() - lineHeight,
                         pdfWriter.width() - 20, lineHeight),
                   Qt::AlignRight, QString("Référence: %1").arg(reference));

  painter.end();
}

#include <QDataStream>
#include <QFile>

void pertechargeherse::saveData(const QString &fileName) {
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

void pertechargeherse::loadData(const QString &fileName) {
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
    for (int i = 0; i < 11; ++i) {
      float value;
      in >> value;
      row.push_back(value);
    }
    _Donnees.push_back(row);
  }

  RafraichirTableau();
  calcul();
}

#include <QSpinBox>

void pertechargeherse::addRow() {

  if(_Donnees.size()==0) return;
  // Ouvrir le QDialog pour l'ajout d'une ligne
  QDialog addRowDialog;
  addRowDialog.setWindowTitle("Ajouter une ligne");
  addRowDialog.setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

  // Créer les widgets
  QSpinBox *positionSpinBox = new QSpinBox(&addRowDialog);
  positionSpinBox->setRange(1, _Donnees.size() + 1);
  positionSpinBox->setValue(_Donnees.size() + 1);

  QLineEdit *debitSpinBox = new QLineEdit(&addRowDialog);
  QLineEdit *diametreSpinBox = new QLineEdit(&addRowDialog);
  QLineEdit *longueurSpinBox = new QLineEdit(&addRowDialog);
  QLineEdit *hauteurSpinBox = new QLineEdit(&addRowDialog);

  QPushButton *okButton = new QPushButton("OK", &addRowDialog);
  QPushButton *cancelButton = new QPushButton("Annuler", &addRowDialog);

  // Organiser les widgets dans une disposition
  QFormLayout *layout = new QFormLayout(&addRowDialog);
  layout->addRow("Position : ", positionSpinBox);
  layout->addRow("Débit : ", debitSpinBox);
      layout->addRow("Diamètre : ", diametreSpinBox);
      layout->addRow("Longueur : ", longueurSpinBox);
  layout->addRow("Hauteur : ", hauteurSpinBox);
  layout->addWidget(okButton);
  layout->addWidget(cancelButton);

  // Connecter le bouton OK à la fonction pour insérer la nouvelle ligne
  QObject::connect(okButton, &QPushButton::clicked, [this, &addRowDialog, positionSpinBox, debitSpinBox, diametreSpinBox, longueurSpinBox, hauteurSpinBox]() {
      // Obtenir la position souhaitée pour la nouvelle ligne
      int position = positionSpinBox->value() - 1;

      QString debitText = debitSpinBox->text();
      debitText.replace(',', '.');
      float debit = debitText.toFloat();

      QString diametreText = diametreSpinBox->text();
      diametreText.replace(',', '.');
      float diametre = diametreText.toFloat();

      QString longueurtexte = longueurSpinBox->text();
      longueurtexte.replace(',', '.');
      float longueur = longueurtexte.toFloat();

      QString hauteurtext = hauteurSpinBox->text();
      hauteurtext.replace(',', '.');
      float hauteur = hauteurtext.toFloat();

      // Créez une nouvelle ligne avec les valeurs entrées par l'utilisateur
      std::vector<float> newRow(11, 0.0);
      newRow[0] = static_cast<float>(position + 1);
      newRow[1] = static_cast<float>(debit);
      newRow[3] = static_cast<float>(diametre);
      newRow[4] = static_cast<float>(longueur);
      newRow[5] = static_cast<float>(hauteur);

      // Insérer la nouvelle ligne à la position souhaitée
      this->insererLigne(position, newRow);

      // Fermer le dialogue
      addRowDialog.accept();
  });

  // Connecter le bouton Annuler pour fermer le dialogue
  QObject::connect(cancelButton, &QPushButton::clicked, &addRowDialog, &QDialog::reject);

  // Afficher le dialogue et attendre la confirmation de l'utilisateur
  addRowDialog.exec();
}



void pertechargeherse::insererLigne(int position, const std::vector<float>& newRow) {
  // Créer un nouveau vecteur pour stocker les données modifiées
  std::vector<std::vector<float>> newDonnees;

  // Copier les éléments existants et insérer la nouvelle ligne à la position donnée
  for (size_t i = 0; i < _Donnees.size() + 1; ++i) {
    if (i == position) {
      newDonnees.push_back(newRow);
    } else if (i > position) {
      std::vector<float> updatedRow = _Donnees[i - 1];
      updatedRow[0] = static_cast<float>(i + 1);
      newDonnees.push_back(updatedRow);
    } else {
      newDonnees.push_back(_Donnees[i]);
    }
  }

  // Remplacer les anciennes données par les nouvelles
  _Donnees = newDonnees;

  // Recalculer les données et mettre à jour le tableau
  calcul();
}


void pertechargeherse::editDiameter() {
  QDialog dialog(this);
  dialog.setWindowTitle("Nouveau diametre");

  QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

  QHBoxLayout *diameterLayout = new QHBoxLayout();
  QLabel *diameterLabel = new QLabel("Diametre : ");
  QLineEdit *diameterEdit = new QLineEdit();
  diameterEdit->setValidator(new QDoubleValidator());
  diameterLayout->addWidget(diameterLabel);
  diameterLayout->addWidget(diameterEdit);
  mainLayout->addLayout(diameterLayout);

  QHBoxLayout *lineLayout = new QHBoxLayout();
  QLabel *startLineLabel = new QLabel("Départ : ");
  QLineEdit *startLineEdit = new QLineEdit("1");
  startLineEdit->setValidator(new QIntValidator());
  QLabel *endLineLabel = new QLabel("Fin : ");
  QLineEdit *endLineEdit = new QLineEdit(QString::number(_Donnees.size()));
  endLineEdit->setValidator(new QIntValidator());
  lineLayout->addWidget(startLineLabel);
  lineLayout->addWidget(startLineEdit);
  lineLayout->addWidget(endLineLabel);
  lineLayout->addWidget(endLineEdit);
  mainLayout->addLayout(lineLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *okButton = new QPushButton("OK");
  QPushButton *cancelButton = new QPushButton("Cancel");
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
  mainLayout->addLayout(buttonLayout);

  connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
  connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

  if (dialog.exec() == QDialog::Accepted) {
    double diameter = diameterEdit->text().toDouble();
    int startLine = startLineEdit->text().toInt();
    int endLine = endLineEdit->text().toInt();

    for (int i = startLine - 1; i < endLine && i < _Donnees.size(); i++) {
      _Donnees[i][3] = diameter;
    }
  }
  calcul();
}

// fonctionnalité abandonné
void pertechargeherse::importData() {
  QDialog dialog(this);
  dialog.setWindowTitle("Import Data");

  QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

  QLabel *dataLabel = new QLabel("Paste your data:");
  mainLayout->addWidget(dataLabel);

  QPlainTextEdit *dataEdit = new QPlainTextEdit();
  mainLayout->addWidget(dataEdit);

  QHBoxLayout *diameterLayout = new QHBoxLayout();
  QLabel *diameterLabel = new QLabel("Diameter:");
  QLineEdit *diameterEdit = new QLineEdit();
  diameterEdit->setValidator(new QDoubleValidator());
  diameterLayout->addWidget(diameterLabel);
  diameterLayout->addWidget(diameterEdit);
  mainLayout->addLayout(diameterLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  QPushButton *okButton = new QPushButton("OK");
  QPushButton *cancelButton = new QPushButton("Cancel");
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
  mainLayout->addLayout(buttonLayout);

  connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
  connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

  if (dialog.exec() == QDialog::Accepted) {
    if(diameterEdit->text().isEmpty()) return;

    QString data = dataEdit->toPlainText();
    data.replace(",", ".");
    float diameter = diameterEdit->text().toFloat();

    if (diameter == 0) return;

    QStringList lines = data.split("\n");
    for (int i = 0; i < lines.size(); i++) {

      QStringList cols = lines[i].split(QRegExp("\\s+"));
      if (cols.size() >= 11) {
        for(int j=0;j<cols.size();j++){
            std::cout<<cols[j].toStdString()<<" ";
        }
        std::cout<<" "<<std::endl;

        float col6 = cols[7].toFloat();
        float col11 = cols[11].toFloat();

        if(i==0){
            col6 = 0;
            col11 = cols[11].toFloat();
        }

        std::vector<float> rowData(11);
        rowData[0] = i + 1;
        rowData[1] = col11;
        rowData[2] = 0;  // sigmaDebit
        rowData[3] = diameter;
        rowData[4] = col6;
        rowData[5] = 0;  // hauteur
        rowData[6] = 0;  // vitesse
        rowData[7] = 0;  // perteCharge
        rowData[8] = 0;  // piezo
        rowData[9] = 0;  // sigmaPerte
        rowData[10] = 0; // sigmaPiezo

        _Donnees.push_back(rowData);
      }
    }
    calcul();
  }
}
