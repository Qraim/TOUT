
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

class etude : public QWidget
{
    Q_OBJECT
public:
    etude(std::shared_ptr<bdd> db,QWidget *parent = nullptr);

signals:
private :
    std::shared_ptr<bdd> database;
    std::vector<std::vector<float>> _Donnees;

    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;


    void setupUI();
    void init();
    void rafraichirTableau();
    void clearchild();
    void initCalcul();

    void calcul();



};

#endif // ETUDE_H
