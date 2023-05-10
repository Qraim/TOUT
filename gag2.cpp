
#include "gag2.h"

gag2::gag2(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent),database(db) {
    QGridLayout *gridLayout = new QGridLayout(this);

    QStringList headers = {" " ,"Débit", "Espacement", "Diamètre", "Longueur", "Hauteur", "Perte", "Piezo"};
    QStringList units = {" ", "m", "mm", "m", "m", "m", "m"};

    for (int i = 0; i < headers.size(); ++i) {
        QLabel *headerLabel = new QLabel(headers[i], this);
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
        unitLabel->setAlignment(Qt::AlignCenter);
        gridLayout->addWidget(unitLabel, 1, i);
    }

    Q = new QLineEdit(this);
    E = new QLineEdit(this);
    D = new QLineEdit(this);
    L = new QLineEdit(this);
    H = new QLineEdit(this);

    gridLayout->addWidget(Q, 2, 1);
    gridLayout->addWidget(E, 2, 2);
    gridLayout->addWidget(D, 2, 3);
    gridLayout->addWidget(L, 2, 4);
    gridLayout->addWidget(H, 2, 5);

    QPushButton *calculateButton = new QPushButton("Calculer", this);
    connect(calculateButton, &QPushButton::clicked, this, &gag2::calcul);
    gridLayout->addWidget(calculateButton, 3, 0, 1, -1, Qt::AlignCenter);

    _Longueur = new QLineEdit(this);
    _Hauteur = new QLineEdit(this);
    _Perte = new QLineEdit(this);
    _Piezo = new QLineEdit(this);

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

    float arosseurs = longueur/espacement;

    float perte = 0;
    float piezo = 0;

    int index = 0;
    index = unite->currentIndex();

    float debitLS = 0;

    for(int i=0; i<arosseurs; i++){

        if(index==0){ // l/h
            debitLS = (debit*i) / 3600;

        } else if(index==1){ // l/s
            debitLS = (debit*i);

        } else if(index==2){ // m3/h
            debitLS = (debit*i) / 3.6;
        }

        perte+= k * std::pow(debitLS,a) * std::pow(diametre, b) * longueur ;
        piezo+= perte + hauteur;

    }
    _Longueur->setText(QString::number(longueur * arosseurs));
    _Hauteur->setText(QString::number(hauteur * arosseurs));
    _Perte->setText(QString::number(perte));
    _Piezo->setText(QString::number(piezo));
}

bool gag2::eventFilter(QObject *obj, QEvent *event) {
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);

    if (lineEdit && isTargetLineEdit(lineEdit)) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            // Modifier keys status
            bool controlPressed = keyEvent->modifiers() & Qt::ControlModifier;

            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                if (allInputsFilled()) {
                    calcul();
                    return true;
                } else {
                    focusNextTargetChild();
                    return true;
                }
            } else if (controlPressed && keyEvent->key() == Qt::Key_Left) {
                focusPreviousTargetChild();
                return true;
            }
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


