
#include "gag2.h"

gag2::gag2(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent),database(db) {
  QGridLayout *gridLayout = new QGridLayout(this);
  setWindowTitle(QString::fromStdString("GAGV2"));

  setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");


  QStringList headers = {"Matiere" ,"Débit", "Espacement", "Diamètre", "Longueur", "Hauteur", "Perte", "Piezo"};
  QStringList units = {" ", "m", "m", "mm", "m", "m", "m"};

  for (int i = 0; i < headers.size(); ++i) {
    QLabel *headerLabel = new QLabel(headers[i], this);
    headerLabel->setFixedSize(200,30);
    headerLabel->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(headerLabel, 0, i);
  }

  Materiau = new QComboBox(this);
  gridLayout->addWidget(Materiau, 1, 0);

  unite = new QComboBox(this);
  unite->addItem("l/h");
  unite->addItem("l/s");
  unite->addItem("m3/h");
  gridLayout->addWidget(unite, 1, 1);

  for (int i = 2; i < units.size(); ++i) {
    QLabel *unitLabel = new QLabel(units[i], this);
    unitLabel->setFixedSize(200,40);
    unitLabel->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(unitLabel, 1, i);
  }

  Q = new QLineEdit(this);
  E = new QLineEdit(this);
  D = new QLineEdit(this);
  L = new QLineEdit(this);
  H = new QLineEdit(this);

  Q->setAlignment(Qt::AlignCenter);
  E->setAlignment(Qt::AlignCenter);
  D->setAlignment(Qt::AlignCenter);
  L->setAlignment(Qt::AlignCenter);
  H->setAlignment(Qt::AlignCenter);

  Q->setFixedSize(200,40);
  E->setFixedSize(200,40);
  D->setFixedSize(200,40);
  L->setFixedSize(200,40);
  H->setFixedSize(200,40);

  Q->installEventFilter(this);
  E->installEventFilter(this);
  D->installEventFilter(this);
  L->installEventFilter(this);
  H->installEventFilter(this);

  gridLayout->addWidget(Q, 2, 1);
  gridLayout->addWidget(E, 2, 2);
  gridLayout->addWidget(D, 2, 3);
  gridLayout->addWidget(L, 2, 4);
  gridLayout->addWidget(H, 2, 5);

  QPushButton *calculateButton = new QPushButton("Calculer", this);
  calculateButton->setFixedSize(200,40);
  connect(calculateButton, &QPushButton::clicked, this, &gag2::calcul);
  gridLayout->addWidget(calculateButton, 3, 3, Qt::AlignCenter);

  QPushButton *diametre = new QPushButton("Diametres", this);
  diametre->setFixedSize(200,40);
  connect(diametre, &QPushButton::clicked, [=]() {
      database->afficher_tableaux();
  });

  gridLayout->addWidget(diametre, 4, 0, Qt::AlignCenter);

  _Longueur = new QLineEdit(this);
  _Hauteur = new QLineEdit(this);
  _Perte = new QLineEdit(this);
  _Piezo = new QLineEdit(this);

  _Longueur->setFixedSize(200,40);
  _Hauteur->setFixedSize(200,40);
  _Perte->setFixedSize(200,40);
  _Piezo->setFixedSize(200,40);

  _Longueur->setAlignment(Qt::AlignCenter);
  _Hauteur->setAlignment(Qt::AlignCenter);
  _Piezo->setAlignment(Qt::AlignCenter);
  _Perte->setAlignment(Qt::AlignCenter);

  _Longueur->setReadOnly(true);
  _Hauteur->setReadOnly(true);
  _Perte->setReadOnly(true);
  _Piezo->setReadOnly(true);

  gridLayout->addWidget(_Longueur, 4, 4);
  gridLayout->addWidget(_Hauteur, 4, 5);
  gridLayout->addWidget(_Perte, 4, 6);
  gridLayout->addWidget(_Piezo, 4, 7);

}

void gag2::calcul() {

  QString debitText = Q->text();
  debitText.replace(',', '.');
  float debit = debitText.toFloat();

  QString espacementText = E->text();
  espacementText.replace(',', '.');
  float espacement = espacementText.toFloat();

  QString diametreText = D->text();
  diametreText.replace(',', '.');
  float diametre = diametreText.toFloat();

  QString longueurText = L->text();
  longueurText.replace(',', '.');
  float longueur = longueurText.toFloat();

  QString hauteurText = H->text();
  hauteurText.replace(',', '.');
  float hauteur = hauteurText.toFloat();

  // Initialise les paramètres.
  double k = 0;
  double a = 0;
  double b = 0;

  std::string material_name = Materiau->currentText().toStdString();
  auto coefficients = database->get_material_coefficients(material_name);

  a = std::get<0>(coefficients);
  b = std::get<1>(coefficients);
  k = std::get<2>(coefficients);

  float arosseurs = 0 ;

  if(espacement!=0)
    arosseurs = longueur / espacement ;
  else
    return;

  double perte = 0;
  double piezo = 0;

  int index = 0;
  index = unite->currentIndex();

  float debitLS = 0;

  if (index == 0) {  // l/h
    debitLS = (debit ) / 3600;
  } else if (index == 1) {  // l/s
    debitLS = (debit);
  } else if (index == 2) {  // m3/h
    debitLS = (debit ) / 3.6;
  }

  for (int i = 1; i <= arosseurs; i++) {

    double pressureLossPerSegment = k * std::pow(debitLS*i, a) * std::pow(diametre, b) * espacement;  // formule de perte de charge
    perte += pressureLossPerSegment;
  }

  piezo += perte + hauteur;

  _Longueur->setText(QString::number(longueur));
  _Hauteur->setText(QString::number(hauteur));
  _Perte->setText(QString::number(perte, 'f', 2));
  _Piezo->setText(QString::number(piezo, 'f', 2));

  D->setFocus(); // On remet le focus sur le premier input

}

bool gag2::eventFilter(QObject *obj, QEvent *event) {
  QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);

  if (lineEdit && isTargetLineEdit(lineEdit)) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

      if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
        if (allInputsFilled()) {
          calcul();
          return true;
        } else {
          focusNextTargetChild();
          return true;
        }
      } else if (keyEvent->key() == Qt::Key_Control) {
        focusPreviousTargetChild();
        return true;
      }else if (keyEvent->key() == Qt::Key_E) {
        refresh();
        return true;
      }
    }
  } else if (event->type() == QEvent::KeyPress){
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->key() == Qt::Key_E) {
      refresh();
      return true;
    }
  }

  return QWidget::eventFilter(obj, event);
}

bool gag2::isTargetLineEdit(QLineEdit *lineEdit) {
  return lineEdit == Q || lineEdit == E || lineEdit == D || lineEdit == L || lineEdit == H;
}


bool gag2::allInputsFilled() {
  return !Q->text().isEmpty() && !E->text().isEmpty() && !D->text().isEmpty() &&
         !L->text().isEmpty() && !H->text().isEmpty();
}


void gag2::focusNextTargetChild() {
  QLineEdit *nextLineEdit = nullptr;

  if (Q->hasFocus()) {
    nextLineEdit = E;
  } else if (E->hasFocus()) {
    nextLineEdit = D;
  } else if (D->hasFocus()) {
    nextLineEdit = L;
  } else if (L->hasFocus()) {
    nextLineEdit = H;
  } else if (H->hasFocus()) {
    nextLineEdit = Q;
  }

  if (nextLineEdit) {
    nextLineEdit->setFocus();
    nextLineEdit->selectAll();
  }
}

void gag2::focusPreviousTargetChild() {
  QLineEdit *prevLineEdit = nullptr;

  if (Q->hasFocus()) {
    prevLineEdit = H;
  } else if (E->hasFocus()) {
    prevLineEdit = Q;
  } else if (D->hasFocus()) {
    prevLineEdit = E;
  } else if (L->hasFocus()) {
    prevLineEdit = D;
  } else if (H->hasFocus()) {
    prevLineEdit = L;
  }

  if (prevLineEdit) {
    prevLineEdit->setFocus();
    prevLineEdit->selectAll();
  }
}


