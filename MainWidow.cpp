#include "MainWidow.h"


float PI2 =3.14159265359;

MainWindow::MainWindow(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent), database(db), Calcul("Calculer", this) {
    setFixedSize(800, 600);

    setWindowTitle(QString::fromStdString("Calcul du diametre intérieur d'un tube simple"));
    setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

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

    Unite = new QComboBox();
    Unite->addItem("m3/h");
    Unite->addItem("l/h");
    Unite->addItem("l/s");
    //Unite->setFixedWidth(100);

    debitLabel = new QLabel("Débit : ");
    vitesseLabel = new QLabel("Vitesse : ");
    longueurLabel = new QLabel("Longueur : ");
    deniveleLabel = new QLabel("Dénivelé : ");
    vitesseUnite = new QLabel("m/s");
    longueurUnite = new QLabel("m");
    deniveleUnite = new QLabel("mm");


    // Débit et Vitesse
    gridLayout->addWidget(debitLabel, 2, 0);
    gridLayout->addWidget(&debit, 2, 1);
    gridLayout->addWidget(Unite, 2, 2);

    gridLayout->addWidget(vitesseLabel, 3, 0);
    gridLayout->addWidget(&vitesse, 3, 1);
    gridLayout->addWidget(vitesseUnite, 3, 2);

    // Longueur et Dénivelé
    gridLayout->addWidget(longueurLabel, 4, 0);
    gridLayout->addWidget(&longueur, 4, 1);
    gridLayout->addWidget(longueurUnite, 4, 2);
    gridLayout->addWidget(deniveleLabel, 5, 0);
    gridLayout->addWidget(&denivele, 5, 1);
    gridLayout->addWidget(deniveleUnite, 5, 2);

    // Bouton Changer en haut à droite
    Changer.setText("Changer");
    Changer.setFixedWidth(100);
    gridLayout->addWidget(&Changer, 0, 3);

    // Bouton Calculer au milieu en bas
    Calcul.setFixedWidth(100);
    Calcul.setEnabled(false);
    gridLayout->addWidget(&Calcul, 6, 1);

    // Résultat sous le bouton Calculer
    gridLayout->addWidget(new QLabel("Résultat (en mm):"), 7, 0);
    gridLayout->addWidget(&Champresultat, 7, 1);


    Champligne.setReadOnly(true);
    Champligne.setFixedWidth(800);
    Champligne.setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(&ChampVitesse, 8, 0, 1, 4);

    ChampVitesse.setReadOnly(true);
    ChampVitesse.setFixedWidth(800);
    ChampVitesse.setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(&Champligne, 9, 0, 1, 4);

    Champligne2.setReadOnly(true);
    Champligne2.setFixedWidth(800);
    Champligne2.setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(&Champligne2, 10, 0, 1, 4);

    // Configuration of the third QLineEdit widget (Champligne3)
    Champligne3.setReadOnly(true);
    Champligne3.setFixedWidth(800);
    Champligne3.setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(&Champligne3, 11, 0, 1, 4);

    setLayout(gridLayout);

    Calcul.setEnabled(false);

    ChampVitesse.setReadOnly(true);

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
    Changer.click();
    Changer.click();

}

void MainWindow::onSwitchButtonClicked() {
    bool isVisible = longueur.isVisible();

    // Soit on cache ou non certains bouttons
    vitesse.setVisible(isVisible);
    longueur.setVisible(!isVisible);
    denivele.setVisible(!isVisible);

    // Cacher ou afficher les labels et les unités en fonction de la visibilité des champs
    vitesseLabel->setVisible(isVisible);
    vitesseUnite->setVisible(isVisible);
    longueurLabel->setVisible(!isVisible);
    longueurUnite->setVisible(!isVisible);
    deniveleLabel->setVisible(!isVisible);
    deniveleUnite->setVisible(!isVisible);

    debit.setFocus();

    updateButtonState();
}


void MainWindow::updateSecondComboBox(int index) {
    Pression.clear(); // Clear the contents to replace them

    // materiau selectionné
    QString selected_material = Materiau.itemText(index);

    // valeur de pressions
    std::vector<int> pressures = database->getAllPressuresForMatiere(selected_material.toStdString());

    // On remplis la comboox avec les pressions
    for (const auto& pressure : pressures) {
        Pression.addItem(QString::number(pressure));
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

    float a,b;
    double k;

    std::string material_name = Materiau.currentText().toStdString();
    auto coefficients = database->get_material_coefficients(material_name);

    a = std::get<0>(coefficients);
    b = std::get<1>(coefficients);
    k = std::get<2>(coefficients);

    QString debitText = debit.text();
    debitText.replace(',', '.');
    float debitValue = debitText.toFloat();

    QString unit = Unite->currentText();

    if (unit == "l/h") {
        debitValue = (debitValue / 1000); // Convert to m³/h
    } else if (unit == "l/s") {
        debitValue = (debitValue * 3.6); // Convert to m³/h
    } // else: m³/h, pas de conversion

    float debits = (debitValue * 1000) / 3600;


    QString espacementText = longueur.text();
    espacementText.replace(',', '.');
    float deniveles = espacementText.toFloat();

    QString diametreText = longueur.text();
    diametreText.replace(',', '.');
    float longueurs = diametreText.toFloat();

    float area = (PI2 * std::pow(deniveles, 2)) / 4; // Calculate the cross-sectional area
    float vitesses = debits / area; // Calculate the flow speed

    float diametre = k * std::pow(debits, a) * std::pow(deniveles, b) * longueurs;

    // écris le champ vitesse
    setvitesse(vitesses);

    // récupere le bon tuyau
    Gettuyau(diametre);

    return roundf(diametre * 100) / 100;

}

float MainWindow::calculdebitvitesse() {

    float debits = debit.text().toFloat(); // débit en m³/h
    float vitesses = vitesse.text().toFloat(); // vitesse en m/s

    float debits_m3s = debits / 3600; // Convertit m³/h en m³/s
    float diametre = std::sqrt((4 * debits_m3s) / (PI2 * vitesses));

    float diametre_mm = diametre * 1000; // convertit des m en mm

    Gettuyau(diametre_mm);

    return roundf(diametre_mm * 100) / 100;

}


void MainWindow::Gettuyau(float diametre) {
    std::pair<int, float> smallerDuo;
    std::pair<int, float> closestDuo;
    std::pair<int, float> largerDuo;
    int closestIndex = -1;

    QString selected_material = Materiau.currentText();
    QString selected_pressure = Pression.currentText();

    matiere selected_matiere = database->findMatiereByName(selected_material.toStdString());

    const pression* found_pression_obj = nullptr;

    for (const auto &pression_obj : selected_matiere.pressions) {
        if (pression_obj.bar == selected_pressure.toInt()) {
          found_pression_obj = &pression_obj;
          break;
        }
    }

    if (found_pression_obj != nullptr) {
        for (int i = 0; i < found_pression_obj->diametre.size(); ++i) {
          float current_diametre = found_pression_obj->diametre[i].second;
          if (current_diametre > diametre) {
            closestDuo = {found_pression_obj->diametre[i].first, current_diametre};
            closestIndex = i;
            break;
          }
        }

        if (closestIndex > 0) {
          smallerDuo = {found_pression_obj->diametre[closestIndex - 1].first, found_pression_obj->diametre[closestIndex - 1].second};
        }

        if (closestIndex + 1 < found_pression_obj->diametre.size()) {
          largerDuo = {found_pression_obj->diametre[closestIndex + 1].first, found_pression_obj->diametre[closestIndex + 1].second};
        }
    }

    QString str1, str2, str3;

    if (smallerDuo.first != 0 || smallerDuo.second != 0) {
        str1 = "Plus petit diamètre: [ " + QString::number(smallerDuo.first) + " ; " + QString::number(smallerDuo.second, 'f', 1) + " ] mm";
    }
    if (closestDuo.first != 0 || closestDuo.second != 0) {
        str2 = "Diamètre exact: [ " + QString::number(closestDuo.first) + " ; " + QString::number(closestDuo.second, 'f', 1) + " ] mm";
    }
    if (largerDuo.first != 0 || largerDuo.second != 0) {
        str3 = "Plus grand diamètre: [ " + QString::number(largerDuo.first) + " ; " + QString::number(largerDuo.second, 'f', 1) + " ] mm";
    }

    Champligne.setText(str1);
    Champligne2.setText(str2);
    Champligne3.setText(str3);
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
    // si la vitesse est > 2 on écrit en rouge
    if (vitesse > 2) {
        ChampVitesse.setStyleSheet("color: red;  background-color: white ");
        str = "La vitesse est trop élevé ( " + std::to_string(vitesse) + " m/s)";
    } else {
        ChampVitesse.setStyleSheet("");
        str = "La vitesse sera de " + std::to_string(vitesse) + " m/s";
    }
    ChampVitesse.setText(QString::fromStdString(str));
}


