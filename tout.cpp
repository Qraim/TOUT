//
// Created by qraim on 12/04/23.
//

#include "tout.h"

tout::tout(QWidget *parent) : QWidget(parent) {

    setWindowTitle(QString::fromStdString("PACHA"));

    database = std::make_shared<bdd>(nullptr);
    database->setWindowFlags(Qt::Window);

    tube = std::make_unique<tubesimple>(database, nullptr);
    tube->setWindowFlags(Qt::Window);

    pcdimm = std::make_unique<pcdim>(database, nullptr);
    pcdimm->setWindowFlags(Qt::Window);

    MW = std::make_unique<MainWindow>(database,nullptr);
    MW->setWindowFlags(Qt::Window);

    perteherse = std::make_unique<pertechargeherse>(database,nullptr);
    perteherse->setWindowFlags(Qt::Window);

    goutte = std::make_unique<gag>(database,nullptr);
    goutte->setWindowFlags(Qt::Window);

    // Create the first QComboBox with main options
    QComboBox *mainOptionsComboBox = new QComboBox(this);
    mainOptionsComboBox->addItem("Perte");
    mainOptionsComboBox->addItem("Diametre");
    mainOptionsComboBox->addItem("Débit");
    mainOptionsComboBox->addItem("Base de données");

    // Create the second QComboBox for sub-options
    QComboBox *subOptionsComboBox = new QComboBox(this);

    // Connect the first QComboBox's currentIndexChanged signal to update the second QComboBox
    connect(mainOptionsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        subOptionsComboBox->clear();
        switch (index) {
            case 0: // Perte
                subOptionsComboBox->addItem("Perte Herse");
                subOptionsComboBox->addItem("Goutte à goutte");
                subOptionsComboBox->addItem("Tube simple");
                break;
            case 1: // Diametre
                subOptionsComboBox->addItem("PCDIM");
                break;
            case 2 :
                subOptionsComboBox->addItem("Débit tube simple");
                break;
            case 3: // BDD
                subOptionsComboBox->addItem("Base de données");
                break;
        }
    });

    QPushButton *startButton = new QPushButton("Lancer", this);

    connect(startButton, &QPushButton::clicked, [=]() {
        switch (mainOptionsComboBox->currentIndex()) {
            case 0: // Perte
                if (subOptionsComboBox->currentIndex() == 0) {
                    on_show_pertechargeherse_button_clicked();
                } else if (subOptionsComboBox->currentIndex() == 1) {
                    on_show_gag_button_clicked();
                } else if (subOptionsComboBox->currentIndex() == 2) {
                    on_show_tubesimple_button_clicked();
                }
                break;
            case 1: // Diametre
                if (subOptionsComboBox->currentIndex() == 0) {
                    on_show_pcdimm_button_clicked();
                } else if (subOptionsComboBox->currentIndex() == 1) {
                    on_show_MW_button_clicked();
                }
                break;
            case 2: // BDD
                if (subOptionsComboBox->currentIndex() == 0) {
                    on_show_MW_button_clicked();
                }
                break;
            case 3: // Débit
                if (subOptionsComboBox->currentIndex() == 0) {
                    on_show_database_button_clicked();
                }
                break;
        }
    });



    // Set up the main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setSpacing(20);

    // Create a horizontal layout for the two QComboBox widgets
    QHBoxLayout *comboBoxLayout = new QHBoxLayout();
    comboBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    comboBoxLayout->setSpacing(10);

    // Add mainOptionsComboBox and subOptionsComboBox to the comboBoxLayout
    comboBoxLayout->addWidget(mainOptionsComboBox);
    comboBoxLayout->addWidget(subOptionsComboBox);

    // Add comboBoxLayout and startButton to the mainLayout
    mainLayout->addLayout(comboBoxLayout);
    mainLayout->addWidget(startButton, 0, Qt::AlignHCenter);

    // Set the layout for the widget
    setLayout(mainLayout);

    // Set some styles for the QComboBox widgets and the start button
    QString comboBoxStyle = "QComboBox { font-weight: bold; font-size: 14px; padding: 6px 12px; min-width: 150px; }";
    QString buttonStyle = "QPushButton { font-weight: bold; font-size: 14px; padding: 6px 12px; min-width: 100px; }";

    mainOptionsComboBox->setStyleSheet(comboBoxStyle);
    subOptionsComboBox->setStyleSheet(comboBoxStyle);
    startButton->setStyleSheet(buttonStyle);
}

void tout::on_show_tubesimple_button_clicked() {
    tube->refresh();
    tube->show();
}

void tout::on_show_database_button_clicked() {
    database->show();
}

void tout::on_show_pertechargeherse_button_clicked() {
    perteherse->refresh();
    perteherse->show();
}

void tout::on_show_pcdimm_button_clicked() {
    pcdimm->show();
}

void tout::on_show_MW_button_clicked() {
    MW->refresh();
    MW->show();
}

void tout::on_show_gag_button_clicked() {
    goutte->refresh();
    goutte->showFullScreen();
}



