//
// Created by qraim on 12/04/23.
//

#include "tout.h"
#include <QIcon>

tout::tout(QWidget *parent) : QWidget(parent) {


    setWindowIcon(QIcon("./logo.png"));

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

    // Créez le premier QComboBox avec des options principales
    QComboBox *mainOptionsComboBox = new QComboBox(this);
    mainOptionsComboBox->addItem("Perte de charge");
    mainOptionsComboBox->addItem("Diametre");
    mainOptionsComboBox->addItem("Débit");

    // Créez le deuxième QComboBox pour les sous-options
    QComboBox *subOptionsComboBox = new QComboBox(this);

    // Connectez le signal currentIndexChanged du premier QComboBox pour mettre à jour le deuxième QComboBox
    connect(mainOptionsComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        subOptionsComboBox->clear();
        switch (index) {
            case 0: // Perte
                subOptionsComboBox->addItem("Herse d'alimentation");
                subOptionsComboBox->addItem("Goutte à goutte");
                subOptionsComboBox->addItem("Tube simple");
                break;
            case 1: // Diametre
                subOptionsComboBox->addItem("Un peu tout");
                break;
            case 2 :
                subOptionsComboBox->addItem("Tube simple");
                break;
        }
    });

    // Créez le QPushButton pour ouvrir la base de données
    QPushButton *openDatabaseButton = new QPushButton("Base de données", this);

    // Définir le style pour openDatabaseButton
    QString openDatabaseButtonStyle = "QPushButton { font-weight: bold; font-size: 12px; padding: 6px 12px; }";
    openDatabaseButton->setStyleSheet(openDatabaseButtonStyle);

    // Positionner l'openDatabaseButton dans le coin supérieur droit de la fenêtre
    openDatabaseButton->setGeometry(width() - openDatabaseButton->width() - 20, 20, openDatabaseButton->width(), openDatabaseButton->height());
    openDatabaseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Connectez le signal cliqué de openDatabaseButton au slot on_show_database_button_clicked()
    connect(openDatabaseButton, &QPushButton::clicked, this, &tout::on_show_database_button_clicked);

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
        }
    });

    subOptionsComboBox->addItem("Herse d'alimentation");
    subOptionsComboBox->addItem("Goutte à goutte");
    subOptionsComboBox->addItem("Tube simple");

    // Configurez le layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setSpacing(20);

    // Créez un layout horizontal pour les deux widgets QComboBox
    QHBoxLayout *comboBoxLayout = new QHBoxLayout();
    comboBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    comboBoxLayout->setSpacing(10);

    // Ajoutez mainOptionsComboBox et subOptionsComboBox au comboBoxLayout
    comboBoxLayout->addWidget(mainOptionsComboBox);
    comboBoxLayout->addWidget(subOptionsComboBox);

    // Ajoutez comboBoxLayout et startButton au mainLayout
    mainLayout->addLayout(comboBoxLayout);
    mainLayout->addWidget(startButton, 0, Qt::AlignHCenter);

    // Créez un layout horizontal pour openDatabaseButton
    QHBoxLayout *bottomLeftLayout = new QHBoxLayout();
    bottomLeftLayout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    bottomLeftLayout->addWidget(openDatabaseButton);

    // Ajoutez bottomLeftLayout au mainLayout
    mainLayout->addLayout(bottomLeftLayout);

    // Définir le layout pour le widget
    setLayout(mainLayout);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Définir des styles pour les widgets QComboBox et le bouton de démarrage
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
    perteherse->showMaximized();
}

void tout::on_show_pcdimm_button_clicked() {
    pcdimm->refresh();
    pcdimm->show();
}

void tout::on_show_MW_button_clicked() {
    MW->refresh();
    MW->show();
}

void tout::on_show_gag_button_clicked() {
    goutte->refresh();
    goutte->showMaximized();
}


void tout::closeEvent(QCloseEvent *event) {
    QApplication::quit();
}




