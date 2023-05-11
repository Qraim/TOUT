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

    MW = std::make_unique<MainWindow>(database, nullptr);
    MW->setWindowFlags(Qt::Window);

    perteherse = std::make_unique<pertechargeherse>(database, nullptr);
    perteherse->setWindowFlags(Qt::Window);

    goutte = std::make_unique<gag>(database, nullptr);
    goutte->setWindowFlags(Qt::Window);

    goutte2 = std::make_unique<gag2>(database, nullptr);
    goutte2->setWindowFlags(Qt::Window);

    QPushButton *mainOptionButton = new QPushButton("Perte de charge", this);
    QPushButton *subOptionButton = new QPushButton("Herse d'alimentation", this);

    QString buttonStyle = "QPushButton { font-weight: bold; font-size: 24px; padding: 6px 12px; min-width: 100px; }";
    mainOptionButton->setStyleSheet(buttonStyle);
    subOptionButton->setStyleSheet(buttonStyle);

    QPushButton *startButton = new QPushButton("Lancer", this);
    startButton->setStyleSheet(buttonStyle);

    connect(mainOptionButton, &QPushButton::clicked, [=]() {
        static int mainOptionIndex = 0;
        mainOptionIndex = (mainOptionIndex + 1) % 3;

        switch (mainOptionIndex) {
            case 0: // Perte
                mainOptionButton->setText("Perte de charge");
                subOptionButton->setText("Herse d'alimentation");
                break;
            case 1: // Diametre
                mainOptionButton->setText("Diamètre");
                subOptionButton->setText("Un peu tout");
                break;
            case 2: // Débit
                mainOptionButton->setText("Débit");
                subOptionButton->setText("Tube simple");
                break;
        }
    });

    connect(subOptionButton, &QPushButton::clicked, [=]() {
        QString currentText = subOptionButton->text();
        QString mainOptionText = mainOptionButton->text();

        if (mainOptionText == "Perte de charge") {
            if (currentText == "Herse d'alimentation") {
                subOptionButton->setText("Goutte à goutte");
            } else if (currentText == "Goutte à goutte") {
                subOptionButton->setText("Tube simple");

            } else if (currentText == "Tube simple") {
                subOptionButton->setText("Herse d'alimentation");
            }
        } else if (mainOptionText == "Diamètre") {
            // Add additional cases for Diamètre if necessary
        } else if (mainOptionText == "Débit") {
            // Add additional cases for Débit if necessary
        }
    });

    connect(startButton, &QPushButton::clicked, [=]() {
        QString mainOptionText = mainOptionButton->text();
        QString subOptionText = subOptionButton->text();

        if (mainOptionText == "Perte de charge") {
            if (subOptionText == "Herse d'alimentation") {
                on_show_pertechargeherse_button_clicked();
            } else if (subOptionText == "Goutte à goutte") {
                on_show_gag2_button_clicked();
            } else if (subOptionText == "Goutte à goutte 2") {
                on_show_gag_button_clicked();
            } else if (subOptionText == "Tube simple") {
                on_show_tubesimple_button_clicked();
            }
        } else if (mainOptionText == "Diamètre") {
            on_show_pcdimm_button_clicked();
        } else if (mainOptionText == "Débit") {
            // Option à ajouter dans le futur
            on_show_MW_button_clicked();
        }
    });


    QPushButton *openDatabaseButton = new QPushButton("Base de données", this);
    QString openDatabaseButtonStyle = "QPushButton { font-weight: bold; font-size: 24px; padding: 6px 12px; }";
    openDatabaseButton->setStyleSheet(openDatabaseButtonStyle);
    // Connect the clicked signal of openDatabaseButton to the slot on_show_database_button_clicked()
    connect(openDatabaseButton, &QPushButton::clicked, this, &tout::on_show_database_button_clicked);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setSpacing(20);

    QHBoxLayout *mainOptionLayout = new QHBoxLayout();
    QHBoxLayout *subOptionLayout = new QHBoxLayout();
    QHBoxLayout *startButtonLayout = new QHBoxLayout();
    QHBoxLayout *openDatabaseLayout = new QHBoxLayout();

    mainOptionLayout->addWidget(mainOptionButton, 1);
    subOptionLayout->addWidget(subOptionButton, 1);
    startButtonLayout->addWidget(startButton, 1);
    openDatabaseLayout->addWidget(openDatabaseButton, 1);

    mainOptionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    subOptionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    startButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    openDatabaseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mainLayout->addLayout(mainOptionLayout);
    mainLayout->addLayout(subOptionLayout);
    mainLayout->addLayout(startButtonLayout);
    mainLayout->addLayout(openDatabaseLayout);

    setLayout(mainLayout);

    this->setFixedSize(this->width()-250, this->height()-200);
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




