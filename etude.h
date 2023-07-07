
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

const QString WHITE_TEXT = "QLineEdit { color: white; font-size: 20px;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString RED_TEXT = "QLineEdit { color: black; font-size: 20px;background-color : #e6ffff ;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString ORANGE_TEXT = "QLineEdit { color: white; font-size: 20px;background-color : green; border: 1px solid gray; border-radius: 4px;padding: 2px 4px;}";
const QString BLUE_TEXT = "QLineEdit { color: white; font-size: 20px;background-color : blue;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString PINK_TEXT = "QLineEdit { color: white; font-size: 20px;background-color : pink;border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";
const QString YELLOW_TEXT = "QLineEdit { color: yellow;font-size: 20px;border: 1px solid white; border-radius: 4px;padding: 2px 4px; }";
const QString SPEED_TEXT = "QLineEdit { color: white; font-size: 20px;background-color : red; border: 1px solid gray; border-radius: 4px;padding: 2px 4px; }";


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
    QGridLayout *bottomlayout;
    QGridLayout *headerLayout ;


    void updateAllLineEditsFromDonnees();

    ///
    /// \param ligne ligne ou changer le nombre d'arroseurs
    /// \param nombre nombre d'arroseurs à placer
    void modifierarro(int ligne, float nombre);

    ///
    /// @brief Initialise les données ou les paramètres nécessaires. Permet de choisir entre 2.2/0.6 ou 1.6/0.5
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
    /// @brief Efface les éléments visible.
    ///
    void clearchild();

    ///
    /// @brief Initialise les calculs à effectuer (zamont, zaval, debit).
    ///
    void initCalcul();

    ///
    /// @brief Ouvre une fenetre permettant de diviser les données en plusieurs parcelles
    ///
    void divideData();

    ///
    /// @brief Effectue un calcul en fonction des valeurs entrées.
    ///
    void calcul();

    ///
    /// @brief Met à jour les données en récupérant les donnes contenu dans les parcelles.
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


    /// @brief fonction générant le pdf
    /// @param fileName : nom du fichier
    void exportPdf(const QString &fileName);

    /// @brief Permet de choisir l'emplacement et le nom du fichier
    void savePdf();

    /// @brief Permet de sauvegarder dans un fichier les donnees
    /// @param filename : nom du fichier
    void saveToFile(const std::string &filename) const;

    /// @brief permet de modifier le débit d'une ligne
    /// @param row : ligne ciblé
    /// @param newDiameter : nouveau débit
    void updateDebit(int row, const QString &newDiameter);

    /// @brief change le débit entre deux bornes
    /// @param debut
    /// @param fin
    /// @param dia nouveau débit
    void modifierdebit(int debut, int fin, float dia);

    /// @brief permet d'afficher un menu permettant de rentrer index début/fin et un nouveau debit
    void changerDebitDialog();

    /// @brief permet de choisir l'emplacement et le nom de la sauvegarde
    void saveDataWrapper();

    /// @brief permet de choisir le fichier à ouvrir
    void loadDataWrapper();

    /// @brief permet de charger une sauvegarde
    /// @param filename : nom du fichier
    void readFromFile(const std::string &filename);

    void keyPressEvent(QKeyEvent *event) override;

    /// permet de changer le débit
    /// \param multi
    void doubledebit(float multi);

    void optimizeparcelle();
};

#endif // ETUDE_H
