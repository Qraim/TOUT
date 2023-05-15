//
// Created by qraim on 06/04/23.
//

#ifndef PCDIM_PCDIM_H
#define PCDIM_PCDIM_H


#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <tuple>
#include <iostream>
#include <QString>
#include <QKeyEvent>
#include <QDialog>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHeaderView>
#include "bdd.h"

class pcdim : public QWidget {
Q_OBJECT
public:

     pcdim(std::shared_ptr<bdd> db,QWidget *parent = nullptr);
    ~pcdim();

    void refresh(){
      for(int i=0;i<5;++i){
        inputs[i]->clear();
      }
      materialComboBox->setCurrentText("PVC");
      pressureComboBox->setCurrentText("10");

    }

private:

    void clearInput();
    void calculate();
    void clearAll();

    std::shared_ptr<bdd> database;

    QGridLayout *mainLayout;
    QLabel *unitsLabels[5];
    QLabel *labels[5];
    QLineEdit *inputs[5];
    QPushButton *buttons[5];
    QPushButton *bottomButtons[2];
    QPushButton *diametersButton;

    QComboBox *materialComboBox;
    QComboBox *pressureComboBox;
    QComboBox *innerDiameterComboBox;
    QComboBox *Unite;

    ///
    /// @return
    /// Calcule le débit en fonction des champs de l'interface utilisateur et renvoie le résultat
    ///
    float calculdebit();

    /// @return
    /// Calcule la vitesse de l'eau en fonction des champs de l'interface utilisateur et renvoie le résultat
    float calculvitesse();

    ///
    /// @return
    /// Calcule le diamètre interne de la canalisation en fonction des champs de l'interface utilisateur et renvoie le résultat
    float calculdiametre();

    ///
    /// @return
    /// Calcule les pertes de charge en fonction des champs de l'interface utilisateur et renvoie le résultat
    float calculperte();

    ///
    /// @param obj : Objet pour lequel l'événement doit être filtré
    /// @param event : Événement qui doit être filtré
    /// @return
    /// Filtre les événements pour l'objet donné et retourne true si l'événement doit être traité normalement, sinon false.
    bool eventFilter(QObject *obj, QEvent *event) override;

    ///
    /// @param pipe_material : Matériau de la canalisation
    /// @return
    /// Renvoie un tuple contenant les propriétés matérielles de la canalisation en fonction du matériau de la canalisation
    std::tuple<float, float, float> getMaterialProperties(const std::string &pipe_material);

    ///
    /// Affiche le tableau des diamètres de la canalisation dans une nouvelle fenêtre
    void showDiametersTable();

    void updateComboBoxes();

private slots:

    void onMaterialComboBoxIndexChanged(int index);

    void onPressureComboBoxIndexChanged(int index);

};

#endif //PCDIM_PCDIM_H
