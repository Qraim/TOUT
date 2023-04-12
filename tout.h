//
// Created by qraim on 12/04/23.
//

#ifndef TOUT_TOUT_H
#define TOUT_TOUT_H

#include <memory>
#include "bdd.h"
#include "tubesimple.h"
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
    ~tout();
private:
    std::shared_ptr<bdd> database;
    std::unique_ptr<tubesimple> tube;

    QPushButton *show_tubesimple_button;
    QPushButton *show_database_button;


    void on_show_tubesimple_button_clicked();

    void on_show_database_button_clicked();
};


#endif //TOUT_TOUT_H
