//
// Created by qraim on 12/04/23.
//

#include "tout.h"

tout::tout(QWidget *parent) : QWidget(parent) {

    database = std::make_shared<bdd>(nullptr);
    database->setWindowFlags(Qt::Window);

    tube = std::make_unique<tubesimple>(database, nullptr);
    tube->setWindowFlags(Qt::Window);

    // Configure the show_tubesimple_button
    show_tubesimple_button = new QPushButton("TubeSimple", this);
    show_tubesimple_button->setText("Show tubesimple");

    // Configure the show_database_button
    show_database_button = new QPushButton("BDD", this);
    show_database_button->setText("Show BDD");

    // Set up the grid layout
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(show_tubesimple_button, 0, 0); // Place button at row 0, column 0
    gridLayout->addWidget(show_database_button, 0, 1);  // Place button at row 0, column 1

    setLayout(gridLayout);

    connect(show_tubesimple_button, &QPushButton::clicked, this, &tout::on_show_tubesimple_button_clicked);
    connect(show_database_button, &QPushButton::clicked, this, &tout::on_show_database_button_clicked);
}



void tout::on_show_tubesimple_button_clicked() {
    tube->show();
}

void tout::on_show_database_button_clicked() {
    database->show();
}

tout::~tout(){
    delete show_tubesimple_button;
}