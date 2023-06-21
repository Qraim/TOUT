//
// Created by qraim on 05/04/23.
//
#ifndef PERTEDECHARGEHERSE_TABLEAU_H
#define PERTEDECHARGEHERSE_TABLEAU_H

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
#include <QDebug>
#include <QTimer>
#include <QPlainTextEdit>


#include <vector>
#include <math.h>
#include "bdd.h"

class pertechargeherse : public QWidget {
Q_OBJECT

public:
    pertechargeherse(std::shared_ptr<bdd> db,QWidget *parent = nullptr);
    void refresh(){
        _Donnees.clear();
        clearchild();
        sigmadebitcase->clear();
        sigmalongueurcase->clear();
        sigmapertecase->clear();
        sigmapiezocase->clear();
        Materiau->clear();
        std::vector<std::string> matiere_names = database->getAllMatiereNames();
        for (const auto& matiere_name : matiere_names) {
            Materiau->addItem(QString::fromStdString(matiere_name));
        }
        Materiau->setCurrentText("PVC");
        inputQ->setFocus();

    }

    void importdonees(float debitLH, float diametre, float interval,
                      std::vector<float> &data, float hauteurligne);

private:

    std::shared_ptr<bdd> database;

    int ligne;

    QLineEdit *inputD;
    QLineEdit *inputQ;
    QLineEdit *inputH;
    QLineEdit *inputL;
    QComboBox *Materiau;
    QComboBox *unite;
    QGridLayout *gridLayout;
    QScrollArea *scrollArea;
    QWidget* scrollWidget;

    QLineEdit *sigmadebitcase;
    QLineEdit *sigmalongueurcase;
    QLineEdit *sigmapertecase;
    QLineEdit *sigmapiezocase;

    std::vector<std::vector<float>> _Donnees;

    /// @brief Ajoute une nouvelle donnée à la base de données.
    void AjoutDonne();

    /// @brief Ajoute une nouvelle ligne dans le tableau de données.
    void AjoutLigne();

    /// @brief Déplace le focus vers l'élément d'entrée suivant.
    void focusNextInput();

    /// @brief Déplace le focus vers l'élément d'entrée précédent.
    void focusPreviousInput();

    /// @brief Gère les événements de pression des touches pour les éléments de l'interface utilisateur.
    /// @param event : Événement de pression de touche
    void keyPressEvent(QKeyEvent *event) override;

    /// @brief Vérifie si toutes les entrées sont remplies.
    bool Allinputfill();

    /// @brief Calcule et affiche les résultats en fonction des entrées.
    void calcul();

    /// @brief Rafraîchit le tableau de données.
    void RafraichirTableau();

    /// @brief Efface les données des éléments enfants (par exemple, les entrées).
    void clearchild();

    /// @brief Copie les données de la dernière ligne du tableau.
    void recopiederniereligne();

    /// @brief Affiche une boîte de dialogue permettant de mettre à jour les données d'une ligne.
    void showUpdateDialog();

    /// @brief Met à jour les données d'une ligne spécifiée dans le tableau.
    /// @param rowNumber : Numéro de la ligne à mettre à jour
    /// @param debit : Valeur de débit à mettre à jour
    /// @param diameter : Valeur de diamètre à mettre à jour
    /// @param length : Valeur de longueur à mettre à jour
    /// @param height : Valeur de hauteur à mettre à jour
    void updateData(int rowNumber, float debit, float diameter, float length, float height);

    /// @brief Supprime une ligne du tableau.
    void enleverLigne();

    /// @brief Charge les données à partir du fichier spécifié.
    /// @param fileName : Chemin du fichier à partir duquel charger les données
    void loadData(const QString &fileName);

    /// @brief Enregistre les données dans le fichier spécifié.
    /// @param fileName : Chemin du fichier dans lequel enregistrer les données
    void saveData(const QString &fileName);

    /// @brief Crée un rapport PDF à partir des données.
    /// @param fileName : Chemin du fichier dans lequel enregistrer le rapport PDF
    void createPdfReport(const QString &fileName);

    ///
    /// @brief Modifi la valeur d'une case
    ///
    void on_lineEdit_editingFinished(const QString &text, int row, int column);

    ///
    /// @brief Permet de mettre le focus sur la prochaine case
    ///
    bool focusNextPrevChild(bool next) override;

    ///
    /// @brief Permet de trouver les coordonnées d'une case
    ///
    void findWidgetIndex(QLineEdit *widget, int &row, int &col);

    ///
    /// @brief Ajoute une ligne dans le vecteur
    ///
    void addRow();

    ///
    /// @brief Ajoute une ligne entre deux ligne dans le vecteur
    ///
    void insererLigne(int position, const std::vector<float>& newRow);

    ///
    /// @brief Permet de changer plusieur diametres d'un seul coup
    ///
    void editDiameter();

    ///
    /// @brief Non utilisable car on n'en voulait pas
    ///
    void importData();


protected :

    bool eventFilter(QObject *obj, QEvent *event) override;

    /// @brief Enregistre les données sous forme de PDF.
    void saveAsPdf();

    /// @brief Fonction enveloppe pour charger les données.
    void loadDataWrapper();

    /// @brief Fonction enveloppe pour enregistrer les données.
    void saveDataWrapper();
   };

#endif //PERTEDECHARGEHERSE_TABLEAU_H
