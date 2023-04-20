#include "MainWidow.h"


MainWindow::MainWindow(std::shared_ptr<bdd> db,QWidget *parent) : QWidget(parent), database(db), Calcul("Calculer", this) {
    setFixedSize(500, 310);

    setWindowTitle(QString::fromStdString("Calcul du diametre intérieur d'un tube simple"));

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
    Pression.clear(); // Clear the contents to replace them

    // Get the selected material
    QString selected_material = Materiau.itemText(index);

    // Get all the different pressure values for the selected material
    std::vector<int> pressures = database->getAllPressuresForMatiere(selected_material.toStdString());

    // Fill the Pression ComboBox with available pressure values
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
    float debits = (debitText.toFloat()*1000)/3600;

    QString espacementText = longueur.text();
    espacementText.replace(',', '.');
    float deniveles = espacementText.toFloat();

    QString diametreText = longueur.text();
    diametreText.replace(',', '.');
    float longueurs = diametreText.toFloat();

    float vitesses = (debits / 3600) / ((std::pow((deniveles / 2000), 2)) * M_PI);
    float diametre = k * std::pow(debits, a) * std::pow(deniveles, b) * longueurs;

    // écris le champ vitesse
    setvitesse(vitesses);

    // récupere le bon tuyau
    Gettuyau(diametre);

    return roundf(diametre * 100) / 100;

}

float MainWindow::calculdebitvitesse() {

    float debits = debit.text().toFloat(); // Flow rate in m³/h
    float vitesses = vitesse.text().toFloat(); // Flow velocity in m/s

    float debits_m3s = debits / 3600; // Convert m³/h to m³/s
    float diametre = std::sqrt((4 * debits_m3s) / (M_PI * vitesses)); // formula for calculating the diameter in meters

    float diametre_mm = diametre * 1000; // Convert diameter from meters to millimeters

    Gettuyau(diametre_mm);

    return roundf(diametre_mm * 100) / 100; // For two decimal places

}


void MainWindow::Gettuyau(float diametre) {
    std::pair<int, float> closestDuo;
    float minValue = std::numeric_limits<float>::max();

    QString selected_material = Materiau.currentText();
    QString selected_pressure = Pression.currentText();

    // Retrieve the corresponding material object from the database
    matiere selected_matiere = database->findMatiereByName(selected_material.toStdString());

    // Find the closest diameter
    for (const auto &pression_obj : selected_matiere.pressions) {
        if (pression_obj.bar == selected_pressure.toInt()) {
            for (const auto &diametre_obj : pression_obj.diametre) {
                float current_diametre = diametre_obj.second;
                if (current_diametre > diametre && current_diametre < minValue) {
                    minValue = current_diametre;
                    closestDuo = {diametre_obj.first, current_diametre};
                    break; // On coupe la boucle quand on a trouvé
                }
            }
        }
    }

    // Set the result text
    QString str;
    if (closestDuo.first == 0 && closestDuo.second == 0) {
        str = "Calcul Invalide";
    } else {
        str = "Il faut un tuyau de " + selected_material + " en " +
              selected_pressure + " bar, de [ " + QString::number(closestDuo.first) + " ; " +
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


