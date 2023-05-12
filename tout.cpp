//
// Created by qraim on 12/04/23.
//

#include "tout.h"
#include <QIcon>

tout::tout(QWidget *parent) : QWidget(parent) {

    setStyleSheet("background-color: #404c4d; color: white; font-size: 24px;");

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

    goutte2 = std::make_unique<gag2>(database,nullptr);
    goutte2->setWindowFlags(Qt::Window);

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
                subOptionsComboBox->addItem("Goutte à goutte 2");

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
        QString openDatabaseButtonStyle = "QPushButton { font-weight: bold; font-size: 24px; padding: 6px 12px; }";
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
                on_show_gag2_button_clicked();

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

    // Créez un QGridLayout avec 3 colonnes et 4 lignes
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(20);

    // Ajoutez un QLabel sur la deuxième colonne de la première ligne
    QLabel *label1 = new QLabel("Type de calcul");
    label1->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(label1, 0, 0);

    // Ajoutez un QLabel sur la deuxième colonne de la deuxième ligne
    QLabel *label2 = new QLabel("Type de tuyau");
    label2->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(label2, 1, 0);

    // Ajoutez openDatabaseButton sur la première colonne de la dernière ligne
    gridLayout->addWidget(openDatabaseButton, 3, 0);

    // Ajoutez startButton sur la deuxième colonne de la troisième ligne
    gridLayout->addWidget(startButton, 2, 1);

    // Ajoutez mainOptionsComboBox sur les trois colonnes de la première ligne
    gridLayout->addWidget(mainOptionsComboBox, 0, 1);

    // Ajoutez subOptionsComboBox sur la deuxième colonne de la deuxième ligne
    gridLayout->addWidget(subOptionsComboBox, 1, 1);

    // Définir le layout pour le widget
    setLayout(gridLayout);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // Définir des styles pour les widgets QComboBox et le bouton de démarrage
    QString comboBoxStyle = "QComboBox { font-weight: bold; font-size: 24px; padding: 6px 12px; min-width: 150px; }";
    QString buttonStyle = "QPushButton { font-weight: bold; font-size: 24px; padding: 6px 12px; min-width: 100px; }";

    // Créez un style pour les labels
    QString labelStyle = "QLabel { font-weight: bold; font-size: 24px; padding: 6px 12px; }";

    // Appliquez le style aux labels
    label1->setStyleSheet(labelStyle);
    label2->setStyleSheet(labelStyle);

    mainOptionsComboBox->setStyleSheet(comboBoxStyle);
    subOptionsComboBox->setStyleSheet(comboBoxStyle);
    startButton->setStyleSheet(buttonStyle);
    openDatabaseButton->setStyleSheet(openDatabaseButtonStyle);

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

void tout::on_show_gag2_button_clicked() {
    goutte2->refresh();
    goutte2->show();
}

void tout::closeEvent(QCloseEvent *event) {
    QApplication::quit();
}



