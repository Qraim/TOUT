#include "MainWidow.h"


MainWindow::MainWindow(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent), database(db), Calcul("Calculer", this) {
    setFixedSize(500, 310);

    Materiau.setFixedWidth(80);
    Pression.setFixedWidth(80);
    debit.setFixedWidth(60);
    vitesse.setFixedWidth(60);
    longueur.setFixedWidth(60);
    denivele.setFixedWidth(60);
    Champresultat.setFixedWidth(100);
    ChampVitesse.setFixedWidth(500);
    Changer.setFixedWidth(80);



    std::vector<std::string> matiere_names = database->getAllMatiereNames();
    for (const auto& matiere_name : matiere_names) {
        Materiau.addItem(QString::fromStdString(matiere_name));
    }



    QGridLayout *gridLayout = new QGridLayout;

    // Matériau et Pression
    gridLayout->addWidget(new QLabel("Matériau : "), 0, 0);
    gridLayout->addWidget(&Materiau, 0, 1);
    gridLayout->addWidget(new QLabel("Pression : "), 1, 0);
    gridLayout->addWidget(&Pression, 1, 1);

    // Débit et Vitesse
    gridLayout->addWidget(new QLabel("Débit : "), 2, 0);
    gridLayout->addWidget(&debit, 2, 1);
    gridLayout->addWidget(new QLabel("m³/h"), 2, 2);
    gridLayout->addWidget(new QLabel("Vitesse : "), 3, 0);
    gridLayout->addWidget(&vitesse, 3, 1);
    gridLayout->addWidget(new QLabel("m/s"), 3, 2);

    // Longueur et Dénivelé
    gridLayout->addWidget(new QLabel("Longueur : "), 4, 0);
    gridLayout->addWidget(&longueur, 4, 1);
    gridLayout->addWidget(new QLabel("m"), 4, 2);
    gridLayout->addWidget(new QLabel("Dénivelé : "), 5, 0);
    gridLayout->addWidget(&denivele, 5, 1);
    gridLayout->addWidget(new QLabel("mm"), 5, 2);

    // Bouton Changer en haut à droite
    Changer.setText("Changer");
    Changer.setFixedWidth(80);
    gridLayout->addWidget(&Changer, 0, 3);

    // Bouton Calculer au milieu en bas
    Calcul.setFixedWidth(80);
    Calcul.setEnabled(false);
    gridLayout->addWidget(&Calcul, 6, 1);

    // Résultat sous le bouton Calculer
    gridLayout->addWidget(new QLabel("Résultat (en mm):"), 7, 0);
    gridLayout->addWidget(&Champresultat, 7, 1);

    Champligne.setReadOnly(true);
    Champligne.setFixedWidth(400);
    gridLayout->addWidget(&Champligne, 8, 0, 1, 4);

    Champligne.setReadOnly(true);
    Champligne.setFixedWidth(480);
    gridLayout->addWidget(&Champligne, 8, 0, 1, 4);

    ChampVitesse.setReadOnly(true);
    ChampVitesse.setFixedWidth(480);
    gridLayout->addWidget(&ChampVitesse, 9, 0, 1, 4);


    setLayout(gridLayout);

    Calcul.setEnabled(false);

    ChampVitesse.setReadOnly(true);
    ChampVitesse.setVisible(false);

    // Cache les champs servant dans l'autre mode
    longueur.setVisible(false);
    denivele.setVisible(false);

    // On relis les bouttons/champs aux fonctions
    connect(&Materiau, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateSecondComboBox);

    connect(&Calcul, &QPushButton::clicked, this, &MainWindow::onButtonClicked);

    connect(&debit, &QLineEdit::returnPressed, this, &MainWindow::onReturnPressed);
    connect(&longueur, &QLineEdit::returnPressed, this, &MainWindow::onReturnPressed);
    connect(&vitesse, &QLineEdit::returnPressed, this, &MainWindow::onReturnPressed);
    connect(&denivele, &QLineEdit::returnPressed, this, &MainWindow::onReturnPressed);

    connect(&debit, &QLineEdit::textChanged, this, &MainWindow::updateButtonState);
    connect(&vitesse, &QLineEdit::textChanged, this, &MainWindow::updateButtonState);
    connect(&longueur, &QLineEdit::textChanged, this, &MainWindow::updateButtonState);
    connect(&denivele, &QLineEdit::textChanged, this, &MainWindow::updateButtonState);
    connect(&Changer, &QPushButton::clicked, this, &MainWindow::onSwitchButtonClicked);

    // on déclenche les fonctions
    updateSecondComboBox(Materiau.currentIndex());
    updateButtonState();
}

void MainWindow::onSwitchButtonClicked() {
    bool isVisible = longueur.isVisible();

    // Soit on cache ou non certains bouttons
    vitesse.setVisible(isVisible);
    longueur.setVisible(!isVisible);
    denivele.setVisible(!isVisible);

    debit.setFocus();

    updateButtonState();
}

void MainWindow::updateSecondComboBox(int index) {
    Pression.clear(); // On efface le contenu pour le remplacer

    // Get the selected material
    QString selected_material = Materiau.itemText(index);

    // Retrieve the corresponding material object
    matiere selected_matiere = database->findMatiereByName(selected_material.toStdString());

    // Fill the Pression ComboBox with available pressure values
    for (const auto& pression_obj : selected_matiere.pressions) {
        Pression.addItem(QString::number(pression_obj.bar));
    }
}


float MainWindow::calculer() {

    bool isVisible = longueur.isVisible();
    // lance le calcul en fonction du mode choisi
    if (!isVisible) {
        ChampVitesse.setVisible(false);
        return calculdebitvitesse();
    } else {
        return calcullongueurdeniv();
    }

}

float MainWindow::calcullongueurdeniv() {

    float k = 831743.11;
    float a = 1.75;
    float b = -4.75;

    float debits = (debit.text().toFloat() * 1000) / 3600;
    float deniveles = denivele.text().toFloat();
    float longueurs = longueur.text().toFloat();

    float vitesses = (debits / 3600) / ((std::pow((deniveles / 2000), 2)) * M_PI);
    float diametre = k * std::pow(debits, a) * std::pow(deniveles, b) * longueurs;

    // écris le champ vitesse
    setvitesse(vitesses);

    // récupere le bon tuyau
    Gettuyau(diametre);

    return roundf(diametre * 100) / 100;

}

float MainWindow::calculdebitvitesse() {

    int debits = debit.text().toInt();
    int vitesses = vitesse.text().toInt(); // Récupération des variables

    float diametre = std::sqrt((4 * debits) / (M_PI * vitesses)) * 1000; // formule pour le calcul du diametre

    Gettuyau(diametre);

    return roundf(diametre * 100) / 100; // Pour être sur deux décimales

}

std::map<float, float>
MainWindow::gettableau() { // Générateur du tableau en fonction des entrées utilisateur avec ( diametre exterieur, diametre interieur )
    QString str1 = Materiau.currentText();
    QString str2 = Pression.currentText();
    std::map<float, float> tableau;
    if (str1 == "PEBD") {
        tableau = {
                {16, 12},
                {20, 15.4},
                {25, 19.4},
                {32, 24.8}
        };
    } else if (str1 == "PEHD") {
        if (str2 == "6") {
            tableau = {
                    {25, 21},
                    {32, 28},
                    {40, 35.2},
                    {50, 44},
                    {63, 55.4},
                    {75, 66},
                    {90, 79.2}
            };
        } else if (str2 == "10") {
            tableau = {
                    {25,  20.4},
                    {32,  27.2},
                    {40,  34},
                    {50,  42.6},
                    {63,  53.6},
                    {75,  63.8},
                    {90,  76.6},
                    {110, 96.8},
                    {125, 110.2},
                    {140, 123.4},
                    {160, 141},
                    {200, 176.2},
                    {250, 220.2},
                    {315, 277.6},
                    {355, 312.8},
                    {400, 352.6},
                    {450, 396.6},
                    {500, 440.6},
                    {560, 493.6},
                    {630, 555.2},
                    {710, 625.8}
            };
        } else {
            tableau = {
                    {25,  19},
                    {32,  24.8},
                    {40,  31},
                    {50,  40.8},
                    {63,  51.4},
                    {75,  58.2},
                    {90,  73.6},
                    {110, 90},
                    {125, 102.2},
                    {140, 114.6},
                    {160, 130.8},
                    {200, 163.6},
                    {250, 204.6},
                    {315, 257.8},
                    {355, 290.6},
                    {400, 327.4}
            };
        }
    } else if (str1 == "PVC") {
        if (str2 == "8") {
            tableau = {
                    {25, 21.2},
                    {32, 28.4},
                    {40, 36.4},
                    {50, 46}
            };
        } else if (str2 == "10") {
            tableau = {
                    {25,  25},
                    {32,  32},
                    {40,  40},
                    {50,  50},
                    {63,  63},
                    {75,  69},
                    {90,  83},
                    {110, 101.4},
                    {125, 116.2},
                    {140, 130.2},
                    {160, 148.8},
                    {200, 186.2},
                    {250, 232.6},
                    {315, 293.2}
            };

        } else {
            tableau = {
                    {90,  85},
                    {110, 105.2},
                    {125, 133.8},
                    {140, 153},
                    {160, 191.2},
                    {200, 239},
                    {250, 301.2},
                    {315, 293.2}
            };
        }
    }
    return tableau;
}

void MainWindow::Gettuyau(float diametre) {
    std::pair<int, float> closestDuo;
    float minValue = std::numeric_limits<float>::max();

    std::map<float, float> tableau = gettableau(); // récupération du tableau de diametres

    for (const auto &duo: tableau) { // ittération dans le tableau pour trouver le bon diametre
        if (duo.second > diametre && duo.second < minValue) {
            minValue = duo.second;
            closestDuo = duo;
            break; // On coupe la boucle quand on a trouvé
        }
    }

    // Maintenant, closestDuo contient le duo le plus proche et supérieur au diamètre
    QString str;
    if (closestDuo.first == 0 && closestDuo.second == 0) {
        str = "Calcul Invalide";
    } else {
        str = "Il faut un tuyau de " + Materiau.currentText() + " en " +
              Pression.currentText() + " bar, de [ " + QString::number(closestDuo.first) + " ; " +
              QString::number(closestDuo.second, 'f', 1) + " ] mm";
    }
    Champligne.setText(str);
}



void MainWindow::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Control) {
        Changer.click();
    }
        // Gérer la combinaison de touches Shift + Up && Shift + Down ou U
    else if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_Up || event->key() == Qt::Key_Up) {
        focusPreviousInput();
    } else if (event->modifiers() & Qt::ShiftModifier && event->key() == Qt::Key_Down || event->key() == Qt::Key_Down) {
        focusNextInput();
        // gérer l'appuie sur la touche entrée
    } else if (event->modifiers() & Qt::Key_Enter) {
        if (Calcul.isEnabled()) { // Si calcul est actif
            float result = calculer();
            Champresultat.setText(QString::number(result)); // Affichage du résultat
            return;
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::onButtonClicked() {
    float result = calculer();
    Champresultat.setText(QString::number(result)); // Affichage du résultat
}

void MainWindow::updateButtonState() {
    bool debitHasText = !debit.text().isEmpty();
    bool vitesseHasText = !vitesse.text().isEmpty();
    bool longueurHasText = !longueur.text().isEmpty();
    bool deniveleHasText = !denivele.text().isEmpty();

    // Verifier quel mode est actif et suivant les champs rempli, on l'active ou non
    if (debit.isVisible() && vitesse.isVisible()) {
        Calcul.setEnabled(debitHasText && vitesseHasText);
    } else {
        Calcul.setEnabled(longueurHasText && deniveleHasText && debitHasText);
    }
}


void MainWindow::onReturnPressed() {
    if (Calcul.isEnabled()) {
        float result = calculer();
        Champresultat.setText(QString::number(result)); // Affichage du résultat
        return;
    }

    bool isVisible = longueur.isVisible();

    if (debit.hasFocus() && !isVisible) {
        vitesse.setFocus();
        return;
    } else if (debit.hasFocus() && isVisible) {
        longueur.setFocus();
        return;
    }

    if (longueur.hasFocus()) {
        denivele.setFocus();
    }
}


void MainWindow::focusPreviousInput() {

    bool isVisible = longueur.isVisible();

    if (denivele.hasFocus()) {
        longueur.setFocus();
        return;
    } else if (longueur.hasFocus() || vitesse.hasFocus()) {
        debit.setFocus();
        return;
    }
}

void MainWindow::focusNextInput() {

    bool isVisible = longueur.isVisible();

    if (debit.hasFocus() && !isVisible) {
        vitesse.setFocus();
        return;
    } else if (debit.hasFocus() && isVisible) {
        longueur.setFocus();
        return;
    } else if (longueur.hasFocus()) {
        denivele.setFocus();
    }

}


void MainWindow::setvitesse(int vitesse) {
    // met le champ en visible
    ChampVitesse.setVisible(true);
    std::string str = "";
    // si la vitesse est > 2 on écris on rouge
    if (vitesse > 2) {
        ChampVitesse.setStyleSheet("color: red;  background-color: white ");
        str = "La vitesse est trop élevé ( " + std::to_string(vitesse) + " m/s)";
    } else {
        ChampVitesse.setStyleSheet("");
        str = "La vitesse sera de " + std::to_string(vitesse) + " m/s";
    }
    ChampVitesse.setText(QString::fromStdString(str));
}


