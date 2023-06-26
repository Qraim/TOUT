
#ifndef ETUDE_H
#define ETUDE_H


#include <QMainWindow>
#include <QWidget>
#include <vector>
#include <memory>
#include "bdd.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QClipboard>
#include <QRadioButton>

#include "parcelle.h"

const QString WHITE_TEXT = "QLineEdit { color: white; border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString RED_TEXT = "QLineEdit { color: black; background-color : #e6ffff ;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString ORANGE_TEXT = "QLineEdit { color: white; background-color : green; border: 1px solid gray; border-radius: 4px;padding: 2px 4px;}";
const QString BLUE_TEXT = "QLineEdit { color: white; background-color : blue;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString PINK_TEXT = "QLineEdit { color: white; background-color : pink;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";


struct ParcelInfo {
  int milieuHydro;
  int limiteParcelle;
  int commandPost;
  QString nom;
  float longueur;
  float debit;
};


class etude : public QWidget
{
    Q_OBJECT

  public:
    etude(std::shared_ptr<bdd> db,QWidget *parent = nullptr);
    void refresh();


private :
    std::shared_ptr<bdd> database;
    std::vector<std::vector<float>> _Donnees;
    std::vector<parcelle> _parcelles;

    std::string _matiere;

    bool milieu;
    bool limitations;
    bool poste;
    bool premier;
    bool asp;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;

    ///
    /// @brief Initialise les données ou les paramètres nécessaires.
    ///
    void init();

    ///
    /// @param data : Texte que l'on copie/colle depuis géogag pour extraire les données
    /// @brief Traite les données en entrée.
    ///
    void traitements(QString data);

    ///
    /// @brief Rafraîchit les données affichées dans le tableau.
    ///
    void rafraichirTableau();

    ///
    /// @brief Efface les données ou les éléments enfants.
    ///
    void clearchild();

    ///
    /// @brief Initialise les calculs à effectuer.
    ///
    void initCalcul();

    ///
    /// @brief Divise les données pour le traitement.
    ///
    void divideData();

    ///
    /// @brief Effectue un calcul en fonction des valeurs entrées.
    ///
    void calcul();

    ///
    /// @brief Met à jour les données en fonction des modifications apportées.
    ///
    void updateDonnees();

    ///
    /// @brief Affiche la boîte de dialogue des options.
    ///
    void showOptionsDialog();

    ///
    /// @brief Permet de choisir l'emplacement du poste de commande.
    ///
    void chooseCommandPost();

    ///
    /// @brief Appelle la boîte de dialogue de réglage du diamètre.
    ///
    void appelSetDiametreDialog();

    ///
    /// @param row : index de la colonne ou l'on veut modifier le diametre
    /// @param newDiameter : Diametre modifié de la ligne
    /// @brief Met à jour le diamètre à la ligne spécifiée avec la nouvelle valeur de diamètre.
    ///
    void updateDiameter(int row, const QString& newDiameter);


    bool eventFilter(QObject *obj, QEvent *event) override;

    /// @brief permet de changer le diametre entre les indexes spécifié
    /// @param debut : index de debut
    /// @param fin : index de fin
    /// @param dia : valeur à appliquer
    void modifierdiametre(int debut, int fin, float dia);

    /// @brief Qdialog pour changer les diametres
    void changerDiametreDialog();

    QLineEdit *createLineEdit(const QString &text, const QString &style,
                              QWidget *parent, bool readOnly = true);

    /// @brief permet de pouvoir utiliser Tab pour se mouvoir entre les diametres
    void setTabOrderForLineEdits();

    /// @brief permet d'atualiser l'intervalle  ciblé
    /// @param row ligne
    /// @param ligne colonne
    /// @param newDiameter nouveelle intervalle
    void updateinterval(int row, int ligne, const QString &newDiameter);


    void exportPdf(const QString &fileName);

    void savePdf();

    void saveToFile(const std::string &filename) const;

    void updateDebit(int row, const QString &newDiameter);
    void modifierdebit(int debut, int fin, float dia);
    void changerDebitDialog();

    void saveDataWrapper();

    void loadDataWrapper();

    void readFromFile(const std::string &filename);

    void keyPressEvent(QKeyEvent *event) override;
};

#endif // ETUDE_H
