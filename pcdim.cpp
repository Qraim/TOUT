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

    QLabel *Warning = new QLabel("<p align='center'>Attention, le diametre doit correspondre à un diametre intérieur présent<br>dans la base de données</p>");
    Warning->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(Warning, 8, 0, 1, 4);

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
    float vitesse = inputs[2]->text().toFloat(); // Vitesse de QLineEdit
    float diametre = inputs[1]->text().toFloat(); // Diametre intérieur de QLineEdit

    float debit = (vitesse * M_PI * std::pow((diametre / 2), 2)) / 1000; // Debit m³/h
    return debit;
}

float pcdim::calculvitesse()
{
    float debit = inputs[0]->text().toFloat(); // Debit de QLineEdit
    float diametre = inputs[1]->text().toFloat(); // Diametre intérieur de QLineEdit

    float vitesse = (1000 * debit) / (M_PI * std::pow((diametre / 2), 2)); // Vitesse m/s
    return vitesse;
}

float pcdim::calculdiametre()
{
    float debit = inputs[0]->text().toFloat(); // Debit de QLineEdit
    float vitesse = inputs[2]->text().toFloat(); // Vitesse de QLineEdit

    float debits_m3s = debit / 3600; // Converti m³/h en m³/s
    float diametre = std::sqrt((4 * debits_m3s) / (M_PI * vitesse))*1000; // Calcul du diametre

    float diametre_mm = diametre * 1000; // Converti le diameter en millimetres

    return diametre;

}
std::string pcdim::gettableau(float inner_diameter) {
    auto result = database->find_matiere_and_pressure_for_diametre(inner_diameter);

    return result.first;
}


std::tuple<float, float, float> pcdim::getMaterialProperties(const std::string & pipe_material) {
    return database->get_material_coefficients(pipe_material);
}


float pcdim::calculperte() {

    float debit_ls = inputs[0]->text().toFloat() / 3600; // Convertit le débit de m3/h en l/s
    float diametre = inputs[1]->text().toFloat(); // Convertit le diamètre de mm en m
    float longueur = inputs[3]->text().toFloat(); // Longueur en mètres

    std::string pipe_material = gettableau(diametre);

    std::tuple<float, float, double> material_properties = getMaterialProperties(pipe_material);

    float a = std::get<0>(material_properties);
    float b = std::get<1>(material_properties);
    double k = std::get<2>(material_properties);

    float perte = k * std::pow(debit_ls, a) * std::pow(diametre, b) * std::pow(longueur, 1.0);

    return perte;

}

void pcdim::calculate()
{
    // Vérifie si les valeurs ont été entrées pour les différents champs
    bool debitEntered = !inputs[0]->text().isEmpty();
    bool diametreEntered = !inputs[1]->text().isEmpty();
    bool vitesseEntered = !inputs[2]->text().isEmpty();
    bool longueurEntered = !inputs[3]->text().isEmpty();

    // Si toutes les valeurs sont entrées, calculez la perte et affichez-la
    if (debitEntered && diametreEntered && vitesseEntered && longueurEntered) {
        float perte = calculperte();
        inputs[4]->setText(QString::number(perte));
        return;
    }

    // Si le débit et le diamètre sont entrés, calculez la vitesse et affichez-la
    if (debitEntered && diametreEntered) {
        float vitesse = calculvitesse();
        inputs[2]->setText(QString::number(vitesse));
        return;
    }
    // Sinon, si le débit et la vitesse sont entrés, calculez le diamètre et affichez-le
    else if (debitEntered && vitesseEntered) {
        float diametre = calculdiametre();
        inputs[1]->setText(QString::number(diametre));
        return;
    }
    // Sinon, si le diamètre et la vitesse sont entrés, calculez le débit et affichez-le
    else if (diametreEntered && vitesseEntered) {
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




