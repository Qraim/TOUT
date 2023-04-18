//
// Created by qraim on 12/04/23.
//

#ifndef TOUT_TOUT_H
#define TOUT_TOUT_H

#include <memory>
#include "bdd.h"
#include "tubesimple.h"
#include "tableau.h"
#include "pcdim.h"
#include "MainWidow.h"
#include "gag.h"

#include <QApplication>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QShortcut>
#include <QIcon>
#include <QPixmap>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QDoubleValidator>
#include <iostream>
#include <QScrollArea>
#include <QScrollBar>
#include <QDialog>



class tout : public QWidget {
Q_OBJECT
public:
    tout(QWidget *parent = nullptr);

private:

    std::shared_ptr<bdd> database;
    std::unique_ptr<tubesimple> tube;
    std::unique_ptr<pertechargeherse> perteherse;
    std::unique_ptr<pcdim> pcdimm;
    std::unique_ptr<MainWindow> MW;
    std::unique_ptr<gag> goutte;

    QPushButton *show_tubesimple_button;
    QPushButton *show_database_button;
    QPushButton *show_pertechargeherse_button;
    QPushButton *show_pcdim_button;
    QPushButton *show_MW_button;
    QPushButton *show_gag_button;

    /// @brief Affiche la fenêtre de l'outil de calcul de perte de charge pour une herse d'alimentation.
    void on_show_pertechargeherse_button_clicked();

    /// @brief Affiche la fenêtre de l'outil de calcul de perte de charge pour un tube simple.
    void on_show_tubesimple_button_clicked();

    /// @brief Affiche la fenêtre de la base de données.
    void on_show_database_button_clicked();

    /// @brief Affiche la fenêtre de l'outil de calcul de perte de charge et de dimensionnement des tubes.
    void on_show_pcdimm_button_clicked();

    /// @brief Affiche la fenêtre principale de l'application.
    void on_show_MW_button_clicked();

    /// @brief Affiche la fenêtre de l'outil de calcul de perte de charge pour un goutte à goutte.
    void on_show_gag_button_clicked();


};


#endif //TOUT_TOUT_H
