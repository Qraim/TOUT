//
// Created by qraim on 06/04/23.
//

#include "pcdim.h"
#include <cmath>

#include <math.h>
#include <QTableWidget>
float PI3 = 3.14159265359;

pcdim::pcdim(std::shared_ptr<bdd> db,QWidget *parent)
        : QWidget(parent), database(db),
          mainLayout(new QGridLayout(this))
{

  setWindowTitle(QString::fromStdString("Un peu tout"));
  setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

    materialComboBox = new QComboBox(this);
    pressureComboBox = new QComboBox(this);
    innerDiameterComboBox = new QComboBox(this);

    updateComboBoxes();

    connect(materialComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMaterialComboBoxIndexChanged(int)));
    connect(pressureComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPressureComboBoxIndexChanged(int)));

    // Débit
    labels[0] = new QLabel("Débit", this);
    mainLayout->addWidget(labels[0], 0, 0);

    unitsLabels[0] = new QLabel("(m3/h)", this);
    mainLayout->addWidget(unitsLabels[0], 0, 2);

    inputs[0] = new QLineEdit(this);
    mainLayout->addWidget(inputs[0], 0, 1);
    inputs[0]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    buttons[0] = new QPushButton(QString("Effacer"));
    mainLayout->addWidget(buttons[0], 0, 3);
    connect(buttons[0], &QPushButton::clicked, this, &pcdim::clearInput);

    // Diamètre
    labels[1] = new QLabel("Diametre", this);
    mainLayout->addWidget(labels[1], 1, 0);

    unitsLabels[1] = new QLabel("(mm)", this);
    mainLayout->addWidget(unitsLabels[1], 1, 2);

    inputs[1] = new QLineEdit(this);
    mainLayout->addWidget(inputs[1], 1, 1);
    inputs[1]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    buttons[1] = new QPushButton(QString("Effacer"));
    mainLayout->addWidget(buttons[1], 1, 3);
    connect(buttons[1], &QPushButton::clicked, this, &pcdim::clearInput);

    // ComboBoxes for diameter
    mainLayout->addWidget(materialComboBox, 2, 0);
    mainLayout->addWidget(pressureComboBox, 2, 1);
    mainLayout->addWidget(innerDiameterComboBox, 2, 3);

    // Vitesse
    labels[2] = new QLabel("Vitesse", this);
    mainLayout->addWidget(labels[2], 3, 0);

    unitsLabels[2] = new QLabel("(m/s)", this);
    mainLayout->addWidget(unitsLabels[2], 3, 2);

    inputs[2] = new QLineEdit(this);
    mainLayout->addWidget(inputs[2], 3, 1);
    inputs[2]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    buttons[2] = new QPushButton(QString("Effacer"));
    mainLayout->addWidget(buttons[2], 3, 3);
    connect(buttons[2], &QPushButton::clicked, this, &pcdim::clearInput);

    // Longueur
    labels[3] = new QLabel("Longueur", this);
    mainLayout->addWidget(labels[3], 4, 0);

    unitsLabels[3] = new QLabel("(m)", this);
    mainLayout->addWidget(unitsLabels[3], 4, 2);

    inputs[3] = new QLineEdit(this);
    mainLayout->addWidget(inputs[3], 4, 1);
    inputs[3]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    buttons[3] = new QPushButton(QString("Effacer"));
    mainLayout->addWidget(buttons[3], 4, 3);
    connect(buttons[3], &QPushButton::clicked, this, &pcdim::clearInput);

    // Perte
    labels[4] = new QLabel("Perte", this);
    mainLayout->addWidget(labels[4], 5, 0);

    unitsLabels[4] = new QLabel("(m)", this);
    mainLayout->addWidget(unitsLabels[4], 5, 2);

    inputs[4] = new QLineEdit(this);
    mainLayout->addWidget(inputs[4], 5, 1);
    inputs[4]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    buttons[4] = new QPushButton(QString("Effacer"));
    mainLayout->addWidget(buttons[4], 5, 3);
    connect(buttons[4], &QPushButton::clicked, this, &pcdim::clearInput);

    // Renommer les deux boutons
    bottomButtons[0] = new QPushButton("Calculer", this);
    mainLayout->addWidget(bottomButtons[0], 7, 0, 1, 4, Qt::AlignCenter);
    connect(bottomButtons[0], &QPushButton::clicked, this, &pcdim::calculate);

    bottomButtons[1] = new QPushButton("Effacer", this);
    mainLayout->addWidget(bottomButtons[1], 8, 0, 1, 4, Qt::AlignCenter);
    connect(bottomButtons[1], &QPushButton::clicked, this, &pcdim::clearAll);

    for (int i = 0; i < 5; ++i)
    {
        inputs[i]->installEventFilter(this);
    }

    diametersButton = new QPushButton("Diamètre", this);
    mainLayout->addWidget(diametersButton, 0, 4);
    connect(diametersButton, &QPushButton::clicked, this, &pcdim::showDiametersTable);

}

pcdim::~pcdim() = default;

void pcdim::showDiametersTable() {
    database->afficher_tableaux();
}

void pcdim::clearInput()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    for (int i = 0; i < 5; ++i)
    {
        if (buttons[i] == button)
        {
            inputs[i]->clear();
            break;
        }
    }
}

void pcdim::clearAll()
{
    for (int i = 0; i < 5; ++i){
        inputs[i]->clear();
    }
}

float pcdim::calculdebit()
{

    QString vitessetext = inputs[2]->text();
    vitessetext.replace(',', '.');
    float vitesse = vitessetext.toFloat();

    QString diametretext = innerDiameterComboBox->currentText();
    diametretext.replace(',', '.');
    float diametre = diametretext.toFloat();

    float debit = (vitesse * PI3 * std::pow((diametre / 2), 2)) / 1000; // Debit m³/h
    return debit;
}

float pcdim::calculvitesse()
{
    QString debittext = inputs[0]->text();
    debittext.replace(',', '.');
    float debit = (debittext.toFloat()*1000)/3600;

    QString diametretext = innerDiameterComboBox->currentText();
    diametretext.replace(',', '.');
    float diametre = diametretext.toFloat();

    float vitesse = (1000 * debit) / (PI3 * std::pow((diametre / 2), 2)); // Vitesse m/s
    return vitesse;
}

float pcdim::calculdiametre()
{
    QString debittext = inputs[0]->text();
    debittext.replace(',', '.');
    float debit_m3h = debittext.toFloat();
    float debit_m3s = debit_m3h / 3600; // Convert m³/h to m³/s

    QString vitessetext = inputs[2]->text();
    vitessetext.replace(',', '.');
    float vitesse = vitessetext.toFloat();

    float diametre = std::sqrt((4 * debit_m3s) / (PI3 * vitesse)) * 1000; // Calculate the diameter in millimeters

    return diametre;
}



std::tuple<float, float, float> pcdim::getMaterialProperties(const std::string & pipe_material) {
    return database->get_material_coefficients(pipe_material);
}

float pcdim::calculperte() {

    QString diametretext = innerDiameterComboBox->currentText();
    diametretext.replace(',', '.');
    float diametre = diametretext.toFloat();

    QString longueurtext = inputs[3]->text();
    longueurtext.replace(',', '.');
    float longueur = longueurtext.toFloat();

    QString debittext = inputs[0]->text();
    debittext.replace(',', '.');
    float debit_ls = (debittext.toFloat()*1000) / 3600;

    std::string materiel = materialComboBox->currentText().toStdString();

    std::tuple<float, float, double> material_properties = getMaterialProperties(materiel);

    float a = std::get<0>(material_properties);
    float b = std::get<1>(material_properties);
    double k = std::get<2>(material_properties);

    float perte = roundf((k * std::pow(debit_ls, a) * std::pow(diametre, b) * longueur) * 100) / 100;

    return perte;

}

void pcdim::calculate()
{
    // Vérifie si les valeurs ont été entrées pour les différents champs
    bool debitEntered = !inputs[0]->text().isEmpty();
    bool vitesseEntered = !inputs[2]->text().isEmpty();
    bool longueurEntered = !inputs[3]->text().isEmpty();

    // Si le débit, la vitesse et la longueur sont renseignés, calcule la perte et l'affiche
    if (debitEntered && vitesseEntered && longueurEntered) {
        float perte = calculperte();
        inputs[4]->setText(QString::number(perte));
        return;
    }
    // Sinon, si le débit et la vitesse sont renseignés, calcule le diamètre et l'affiche
    else if (debitEntered && vitesseEntered) {
        float diametre = calculdiametre();
        inputs[1]->setText(QString::number(diametre));
        return;
    }

    // Si le débit et le diamètre sont renseignés, calcule la vitesse et l'affiche
    else if (debitEntered) {
        float vitesse = calculvitesse();
        inputs[2]->setText(QString::number(vitesse));
        return;
    }

    // Sinon, si le diamètre et la vitesse sont renseignés, calcule le débit et l'affiche
    else if (vitesseEntered) {
        float debit = calculdebit();
        inputs[0]->setText(QString::number(debit));
        return;
    }
}

bool pcdim::eventFilter(QObject *obj, QEvent *event)
{
    for (int i = 0; i < 5; ++i)
    {
        if (obj == inputs[i] && event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Control)
            {
                // Control : Champ précédent
                if (i != 0) inputs[i - 1]->setFocus();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                if (keyEvent->modifiers() == Qt::ShiftModifier)
                {
                    // Shift + Entrer: Calculer
                    calculate();
                    return true;
                }
                else if (i < 4)
                {
                    // Entrer : Champ suivant
                    inputs[i + 1]->setFocus();
                    return true;
                }
                else if (i == 4)
                {
                    // Si tout est rempli sauf perte, calculer
                    bool allFilled = true;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (inputs[j]->text().isEmpty())
                        {
                            allFilled = false;
                            break;
                        }
                    }
                    if (allFilled)
                    {
                        calculate();
                        return true;
                    }
                }
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}



void pcdim::onMaterialComboBoxIndexChanged(int index) {
    // Récupération des infos
    QString selectedMaterial = materialComboBox->itemText(index);
    std::vector<int> pressures = database->getAllPressuresForMatiere(selectedMaterial.toStdString());

    // Mise à jour des infos
    pressureComboBox->clear();
    for (int pressure : pressures) {
        pressureComboBox->addItem(QString::number(pressure));
    }

    if (!pressures.empty()) {
        onPressureComboBoxIndexChanged(0);
    }
}

void pcdim::onPressureComboBoxIndexChanged(int index) {
    // récupération des infos
    QString selectedMaterial = materialComboBox->currentText();
    int selectedPressure = pressureComboBox->itemText(index).toInt();
    std::vector<float> innerDiameters = database->getInnerDiametersForMatiereAndPressure(selectedMaterial.toStdString(), selectedPressure);

    // Mise à jour du diametre extérieur
    innerDiameterComboBox->clear();
    for (float innerDiameter : innerDiameters) {
        innerDiameterComboBox->addItem(QString::number(innerDiameter));
    }
}

void pcdim::updateComboBoxes() {
    // Récupération des infos
    std::vector<std::string> materials = database->getAllMatiereNames();

    // Mise à jour des infos
    materialComboBox->clear();
    for (const auto &material : materials) {
        materialComboBox->addItem(QString::fromStdString(material));
    }

    if (!materials.empty()) {
        onMaterialComboBoxIndexChanged(0);
    }
}
