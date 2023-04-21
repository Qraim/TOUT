//
// Created by qraim on 06/04/23.
//

#include "pcdim.h"
#include <cmath>
#include <QTableWidget>

pcdim::pcdim(std::shared_ptr<bdd> db,QWidget *parent)
        : QWidget(parent), database(db),
          mainLayout(new QGridLayout(this))
{


    materialComboBox = new QComboBox(this);
    pressureComboBox = new QComboBox(this);
    innerDiameterComboBox = new QComboBox(this);

    updateComboBoxes();

    connect(materialComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onMaterialComboBoxIndexChanged(int)));
    connect(pressureComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPressureComboBoxIndexChanged(int)));

    // Ajoutez les noms souhaités pour les labels dans ce tableau
    QString labelNames[] = {"Débit", "Diametre", "Vitesse", "Longueur", "Perte"};
    QString units[] = {"(m3/h)", "(mm)", "(m/s)", "(m)", "(m)"};

    for (int i = 0; i < 5; ++i)
    {
        // Utilisez le tableau labelNames pour définir les noms des labels
        labels[i] = new QLabel(labelNames[i], this);
        mainLayout->addWidget(labels[i], i, 0);

        unitsLabels[i] = new QLabel(units[i], this);
        mainLayout->addWidget(unitsLabels[i], i, 2);

        inputs[i] = new QLineEdit(this);
        mainLayout->addWidget(inputs[i], i, 1);
        inputs[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        buttons[i] = new QPushButton(QString("Effacer"));
        mainLayout->addWidget(buttons[i], i, 3);
        connect(buttons[i], &QPushButton::clicked, this, &pcdim::clearInput);
    }

    mainLayout->addWidget(materialComboBox, 5, 0);
    mainLayout->addWidget(pressureComboBox, 5, 1);
    mainLayout->addWidget(innerDiameterComboBox, 5, 2);

    // Renommer les deux boutons
    bottomButtons[0] = new QPushButton("Calculer", this);
    mainLayout->addWidget(bottomButtons[0], 6, 0, 1, 4, Qt::AlignCenter);
    connect(bottomButtons[0], &QPushButton::clicked, this, &pcdim::calculate);

    bottomButtons[1] = new QPushButton("Effacer", this);
    mainLayout->addWidget(bottomButtons[1], 7, 0, 1, 4, Qt::AlignCenter);
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
    float vitesse = (vitessetext.toFloat()*1000)/3600;

    QString diametretext = innerDiameterComboBox->currentText();
    diametretext.replace(',', '.');
    float diametre = diametretext.toFloat();

    float debit = (vitesse * M_PI * std::pow((diametre / 2), 2)) / 1000; // Debit m³/h
    return debit;
}

float pcdim::calculvitesse()
{
    QString debittext = inputs[0]->text();
    debittext.replace(',', '.');
    float debit = (debittext.toFloat()*1000)/3600;

    QString diametretext = innerDiameterComboBox->currentText();;
    diametretext.replace(',', '.');
    float diametre = diametretext.toFloat();

    float vitesse = (1000 * debit) / (M_PI * std::pow((diametre / 2), 2)); // Vitesse m/s
    return vitesse;
}

float pcdim::calculdiametre()
{

    QString debittext = inputs[0]->text();
    debittext.replace(',', '.');
    float debit = (debittext.toFloat()*1000)/3600;

    QString vitessetext = inputs[2]->text();
    vitessetext.replace(',', '.');
    float vitesse = vitessetext.toFloat();

    float debits_m3s = debit / 3600; // Converti m³/h en m³/s
    float diametre = std::sqrt((4 * debits_m3s) / (M_PI * vitesse))*1000; // Calcul du diametre

    float diametre_mm; // Converti le diameter en millimetres

    return diametre;

}


std::tuple<float, float, float> pcdim::getMaterialProperties(const std::string & pipe_material) {
    return database->get_material_coefficients(pipe_material);
}

float pcdim::calculperte() {

    QString diametretext = innerDiameterComboBox->currentText();
    diametretext.replace(',', '.');
    float diametre = diametretext.toFloat();

    QString pressiontext = pressureComboBox->currentText();
    pressiontext.replace(',', '.');
    float pressure = pressiontext.toFloat();

    QString longueurtext = inputs[3]->text();
    longueurtext.replace(',', '.');
    float longueur = longueurtext.toFloat();

    QString debittext = inputs[0]->text();
    debittext.replace(',', '.');
    float debit_ls = debittext.toFloat() / 3600;

    std::string materiel = materialComboBox->currentText().toStdString();

    float diametre_interieur = database->getInnerDiameterForMatierePressureAndOuterDiameter(materiel, pressure, diametre);

    std::tuple<float, float, double> material_properties = getMaterialProperties(materiel);

    float a = std::get<0>(material_properties);
    float b = std::get<1>(material_properties);
    double k = std::get<2>(material_properties);

    float perte = k * std::pow(debit_ls, a) * std::pow(diametre_interieur, b) * std::pow(longueur, 1.0);

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

    // Si le débit et le diamètre sont renseignés, calcule la vitesse et l'affiche
    if (debitEntered) {
        float vitesse = calculvitesse();
        inputs[2]->setText(QString::number(vitesse));
        return;
    }
    // Sinon, si le débit et la vitesse sont renseignés, calcule le diamètre et l'affiche
    else if (debitEntered && vitesseEntered) {
        float diametre = calculdiametre();
        inputs[1]->setText(QString::number(diametre));
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
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                if (keyEvent->modifiers() == Qt::ShiftModifier)
                {
                    // Shift + Entrer: Calculer
                    calculate();
                    return true;
                }
                else if (keyEvent->modifiers() == Qt::ControlModifier && i > 0)
                {
                    // Control : Champ précédent
                    inputs[i - 1]->setFocus();
                    return true;
                }
                else if (i < 4)
                {
                    // Enter: Champ suivant
                    inputs[i + 1]->setFocus();
                    return true;
                }
                else if (i == 4)
                {
                    // Si tout est remplis sauf perte calculer
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
    // Get the selected material and retrieve the list of pressures for the material
    QString selectedMaterial = materialComboBox->itemText(index);
    std::vector<int> pressures = database->getAllPressuresForMatiere(selectedMaterial.toStdString());

    // Update the pressure QComboBox with the retrieved data
    pressureComboBox->clear();
    for (int pressure : pressures) {
        pressureComboBox->addItem(QString::number(pressure));
    }

    // Update the inner diameter QComboBox based on the first pressure in the list
    if (!pressures.empty()) {
        onPressureComboBoxIndexChanged(0);
    }
}

void pcdim::onPressureComboBoxIndexChanged(int index) {
    // Get the selected material and pressure, and retrieve the list of inner diameters for the material and pressure
    QString selectedMaterial = materialComboBox->currentText();
    int selectedPressure = pressureComboBox->itemText(index).toInt();
    std::vector<float> innerDiameters = database->getInnerDiametersForMatiereAndPressure(selectedMaterial.toStdString(), selectedPressure);

    // Update the inner diameter QComboBox with the retrieved data
    innerDiameterComboBox->clear();
    for (float innerDiameter : innerDiameters) {
        innerDiameterComboBox->addItem(QString::number(innerDiameter));
    }
}

void pcdim::updateComboBoxes() {
    // Retrieve the list of materials from the database
    std::vector<std::string> materials = database->getAllMatiereNames();

    // Update the material QComboBox with the retrieved data
    materialComboBox->clear();
    for (const auto &material : materials) {
        materialComboBox->addItem(QString::fromStdString(material));
    }

    // Update the pressure and inner diameter QComboBoxes based on the first material in the list
    if (!materials.empty()) {
        onMaterialComboBoxIndexChanged(0);
    }
}
