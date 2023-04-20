//
// Created by qraim on 11/04/23.
//

#include <QAbstractButton>
#include "bdd.h"

float QStringToFloat(const QString &str) {
    QString modifiedStr = str;
    modifiedStr.replace(',', '.');
    return modifiedStr.toFloat();
}


std::vector<tableau> read_materials_from_csv(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<tableau> materials;

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << filename << std::endl;
        return materials;
    }

    // On passe la premiere ligne
    std::string line;
    std::getline(file, line);

    std::locale::global(std::locale::classic());

    float a;
    float b;
    double k;
    int bar;
    int diameter_key;
    float diameter_value;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;

        std::getline(ss, token, ';');
        std::string nom = token;

        std::getline(ss, token, ';');
        a = std::stof(token);

        std::getline(ss, token, ';');
        b = std::stof(token);

        std::getline(ss, token, ';');
        std::replace(token.begin(), token.end(), ',', '.');
        k = std::stod(token);

        std::getline(ss, token, ';');
        bar = std::stoi(token);

        std::getline(ss, token, ';');
        diameter_key = std::stoi(token);

        std::getline(ss, token, ';');
        diameter_value = static_cast<float>(std::stod(token));

        // Si la matiere existe deja dans le vecteur
        auto it = std::find_if(materials.begin(), materials.end(), [&](const tableau& t) {
            return t.nom == nom;
        });

        // On ajoute le diametre à la matiere correspondante dans la bonne matiere
        if (it != materials.end()) {
            auto& matieres = it->matieres;
            auto mat_it = std::find_if(matieres.begin(), matieres.end(), [&](const matiere& m) {
                return m.a == a && m.b == b && m.k == k;
            });

            if (mat_it != matieres.end()) {
                auto& pressions = mat_it->pressions;
                auto press_it = std::find_if(pressions.begin(), pressions.end(), [&](const pression& p) {
                    return p.bar == bar;
                });

                if (press_it != pressions.end()) {
                    press_it->diametre.emplace_back(diameter_key, diameter_value);
                } else {
                    pressions.emplace_back(bar, std::vector<std::pair<int, float>>{ {diameter_key, diameter_value} });
                }
            } else {
                matieres.emplace_back(nom, a, b, k, std::vector<pression>{ {bar, {{diameter_key, diameter_value}}} });
            }
        } else {
            //Si il existe pas, on le créé
            materials.emplace_back(nom, std::vector<matiere>{ {nom, a, b, k, std::vector<pression>{ {bar, {{diameter_key, diameter_value}}} }} });
        }
    }

    file.close();
    return materials;
}



void write_materials_to_csv(const std::vector<tableau>& materials, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Impossible d'ouvrir le fichier : " << filename << std::endl;
        return;
    }
    file << "Materiau;A;B;K;Bar;Diametre exterieur ;Diametre intérieur\n";
    for (const auto& t : materials) {
        for (const auto& m : t.matieres) {
            for (const auto& p : m.pressions) {
                for (const auto& d : p.diametre) {
                    file << m.nom << ";" << std::fixed << std::setprecision(2) << m.a << ";" << m.b << ";"
                         << m.k << ";"
                         << p.bar << ";" << d.first << ";" << d.second << "\n";
                }
            }
        }
    }
    file.close();
}



bdd::bdd(QWidget *parent)
        : QWidget(parent) {

    materials = read_materials_from_csv("BDD.csv");

    // Création des boutons
    QPushButton *button1 = new QPushButton("Afficher", this);
    QPushButton *button2 = new QPushButton("Modifier", this);
    QPushButton *button3 = new QPushButton("Ajouter", this);
    QPushButton *button4 = new QPushButton("Suprimmer", this);
    QPushButton *button5 = new QPushButton("Liste tuyaux", this);
    QPushButton *button6 = new QPushButton("Coefficient", this);
    QPushButton *button7 = new QPushButton("Trouver", this);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    connect(button1, &QPushButton::clicked, this, &::bdd::afficher_tableaux);
    connect(button2, &QPushButton::clicked, this, &::bdd::modifier_tableaux);
    connect(button3, &QPushButton::clicked, this, &::bdd::ajouter_tableaux);
    connect(button4, &QPushButton::clicked, this, &::bdd::suprimmer_tableaux);
    connect(button5, &QPushButton::clicked, this, &::bdd::tuyau_dispo);
    connect(button6, &QPushButton::clicked, this, &::bdd::montre_coef);
    connect(button7, &QPushButton::clicked, this, &::bdd::trouver_tuyau);




    mainLayout->addWidget(button1);
    mainLayout->addWidget(button2);
    mainLayout->addWidget(button3);
    mainLayout->addWidget(button4);
    mainLayout->addWidget(button5);
    mainLayout->addWidget(button6);
    mainLayout->addWidget(button7);

    setLayout(mainLayout);
}




std::tuple<float, float, double> bdd::get_material_coefficients( const std::string& material_name) {
    for (const auto& t : materials) {
        for (const auto& m : t.matieres) {
            if (m.nom == material_name) {
                return std::make_tuple(m.a, m.b, m.k);
            }
        }
    }
    // Si on n'a pas trouvé la matière dans la liste des matériaux, on renvoie un triple avec des valeurs nulles
    return std::make_tuple(0.0f, 0.0f, 0.0f);
}


void bdd::afficher_tableaux() {
    QWidget *window = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(window);
    window->setLayout(layout);
    window->setWindowTitle("Matiere Tables");

    QTabWidget *tabWidget = new QTabWidget(window);
    layout->addWidget(tabWidget);

    for (const auto &table : materials) {
        for (const auto &mat : table.matieres) {
            QWidget *tab = new QWidget;
            QVBoxLayout *tabLayout = new QVBoxLayout(tab);
            tab->setLayout(tabLayout);

            QTableWidget *matiereTable = new QTableWidget(tab);
            matiereTable->verticalHeader()->setVisible(false);
            tabLayout->addWidget(matiereTable);

            // Get the set of unique inner diameters
            std::set<int> unique_inner_diameters;
            for (const auto &p : mat.pressions) {
                for (const auto &d : p.diametre) {
                    unique_inner_diameters.insert(d.first);
                }
            }

            // Create the table
            int rows = unique_inner_diameters.size();
            int cols = mat.pressions.size() + 1;
            matiereTable->setRowCount(rows);
            matiereTable->setColumnCount(cols);

            // Set header labels
            QStringList headerLabels;
            headerLabels << "Diametre exterieur";
            for (const auto &p : mat.pressions) {
                headerLabels << QString::number(p.bar) + " bar";
            }
            matiereTable->setHorizontalHeaderLabels(headerLabels);

            // Fill the table
            int row = 0;
            for (const int inner_diameter : unique_inner_diameters) {
                QTableWidgetItem *item = new QTableWidgetItem(QString::number(inner_diameter));
                item->setTextAlignment(Qt::AlignCenter);
                matiereTable->setItem(row, 0, item);
                int col = 1;
                for (const auto &p : mat.pressions) {
                    auto it = std::find_if(p.diametre.begin(), p.diametre.end(), [inner_diameter](const std::pair<int, float> &d) {
                        return d.first == inner_diameter;
                    });
                    if (it != p.diametre.end()) {
                        QTableWidgetItem *item = new QTableWidgetItem(QString::number(it->second, 'f', 2));
                        item->setTextAlignment(Qt::AlignCenter);
                        matiereTable->setItem(row, col, item);
                    }
                    col++;
                }
                row++;
            }

            matiereTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            tabWidget->addTab(tab, QString::fromStdString(mat.nom));
        }
    }

    window->showMaximized();
}


std::vector<std::pair<int, float>> bdd::get_possible_tuyaux(const matiere& m, int bar) {
    std::vector<std::pair<int, float>> possible_tuyaux;

    // Recherche de la pression correspondante à la barre spécifiée
    auto it = std::find_if(m.pressions.begin(), m.pressions.end(), [&](const pression& p) {
        return p.bar == bar;
    });

    if (it != m.pressions.end()) {
        possible_tuyaux = it->diametre;
    }

    return possible_tuyaux;
}

void bdd::modifier_tableaux() {
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Modifier diametre intérieur");

    QFormLayout *formLayout = new QFormLayout(dialog);
    dialog->setLayout(formLayout);

    // Widget pour selectionner les parametres
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QComboBox *pressureComboBox = new QComboBox(dialog);
    QComboBox *outerDiameterComboBox = new QComboBox(dialog);
    QLineEdit *currentInnerDiameterLineEdit = new QLineEdit(dialog);
    QLineEdit *newInnerDiameterLineEdit = new QLineEdit(dialog);

    // rempli la combobox
    auto populateMatiereComboBox = [&]() {
        matiereComboBox->clear();
        for (const auto &table : materials) {
            for (const auto &mat : table.matieres) {
                matiereComboBox->addItem(QString::fromStdString(mat.nom));
            }
        }
    };

    populateMatiereComboBox();

    // Connectez le signal de matiereComboBox pour mettre à jour pressureComboBox et outerDiameterComboBox
    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        pressureComboBox->clear();
        outerDiameterComboBox->clear();
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        std::set<int> unique_pressures;
        for (const auto &table : materials) {
            for (const auto &mat : table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (const auto &p : mat.pressions) {
                        unique_pressures.insert(p.bar);
                    }
                }
            }
        }
        for (const int pressure : unique_pressures) {
            pressureComboBox->addItem(QString::number(pressure));
        }
    });

    // Connectez le signal de pressureComboBox pour mettre à jour outerDiameterComboBox

    QObject::connect(pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        outerDiameterComboBox->clear();
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        std::set<int> unique_outer_diameters;
        for (const auto &table : materials) {
            for (const auto &mat : table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (const auto &p : mat.pressions) {
                        if (p.bar == selectedPressure) {
                            for (const auto &d : p.diametre) {
                                unique_outer_diameters.insert(d.first);
                            }
                        }
                    }
                }
            }
        }
        for (const int outer_diameter : unique_outer_diameters) {
            outerDiameterComboBox->addItem(QString::number(outer_diameter));
        }
    });

    // Connectez le signal de outerDiameterComboBox pour afficher le diamètre intérieur actuel
    QObject::connect(outerDiameterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) {
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        int selectedOuterDiameter = outerDiameterComboBox->currentText().toInt();
        for (const auto &table : materials) {
            for (const auto &mat : table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (const auto &p : mat.pressions) {
                        if (p.bar == selectedPressure) {
                            for (const auto &d : p.diametre) {
                                if (d.first == selectedOuterDiameter) {
                                    currentInnerDiameterLineEdit->setText(QString::number(d.second));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    });

    // Déclenchez le signal de matiereComboBox pour remplir pressureComboBox et outerDiameterComboBox avec les valeurs initiales
    matiereComboBox->currentIndexChanged(matiereComboBox->currentIndex());

    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Pression:", pressureComboBox);
    formLayout->addRow("Diametre exterieur:", outerDiameterComboBox);
    formLayout->addRow("Diametre interieur actuel:", currentInnerDiameterLineEdit);
    formLayout->addRow("Nouveau diametre interieur:", newInnerDiameterLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    formLayout->addWidget(buttonBox);

    // Connecte les signaux aux boutons
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, [&]() {
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        int selectedOuterDiameter = outerDiameterComboBox->currentText().toInt();
        float currentInnerDiameter = QStringToFloat(currentInnerDiameterLineEdit->text());
        float newInnerDiameter = QStringToFloat(newInnerDiameterLineEdit->text());

        // mise o jours des données
        for (auto &table : materials) {
            for (auto &mat : table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (auto &p : mat.pressions) {
                        if (p.bar == selectedPressure) {
                            for (auto &d : p.diametre) {
                                if (d.first == selectedOuterDiameter && d.second == currentInnerDiameter) {
                                    d.second = newInnerDiameter;
                                }
                            }
                        }
                    }
                }
            }
        }

        dialog->accept();
    });

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    dialog->exec();
}





bdd::~bdd() {
    write_materials_to_csv(materials, "BDD.csv");
}


void bdd::ajouter_tableaux() {
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Ajouter");

    QFormLayout *formLayout = new QFormLayout(dialog);
    dialog->setLayout(formLayout);

    // Widgets pour le formulaire
    QCheckBox *newMatiereCheckBox = new QCheckBox("Nouvelle matiere", dialog);
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QLineEdit *matiereNameLineEdit = new QLineEdit(dialog);
    QLineEdit *coeffALineEdit = new QLineEdit(dialog);
    QLineEdit *coeffBLineEdit = new QLineEdit(dialog);
    QLineEdit *coeffKLineEdit = new QLineEdit(dialog);
    QLineEdit *pipeBarLineEdit = new QLineEdit(dialog);
    QLineEdit *innerDiameterLineEdit = new QLineEdit(dialog);
    QLineEdit *outerDiameterLineEdit = new QLineEdit(dialog);

    // Remplir le ComboBox des matières
    for (const auto &table: materials) {
        for (const auto &mat: table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }

    // Connecter le signal de newMatiereCheckBox pour activer/désactiver matiereNameLineEdit et les LineEdits de coefficient
    QObject::connect(newMatiereCheckBox, &QCheckBox::toggled, [&](bool checked) {
        matiereNameLineEdit->setEnabled(checked);
        coeffALineEdit->setEnabled(checked);
        coeffBLineEdit->setEnabled(checked);
        coeffKLineEdit->setEnabled(checked);
    });

    formLayout->addRow(newMatiereCheckBox);
    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Nouvelle Matiere:", matiereNameLineEdit);
    formLayout->addRow("Coefficient A:", coeffALineEdit);
    formLayout->addRow("Coefficient B:", coeffBLineEdit);
    formLayout->addRow("Coefficient K:", coeffKLineEdit);
    formLayout->addRow("Pression:", pipeBarLineEdit);
    formLayout->addRow("Diametre extérieur:", innerDiameterLineEdit);
    formLayout->addRow("Diametre intérieur:", outerDiameterLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    formLayout->addWidget(buttonBox);

    // Connecter le bouton Accepter pour ajouter une nouvelle matière ou tuyau
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, [&]() {

        int pipeBar;
        float outerDiameter;
        int innerDiameter;
        if (newMatiereCheckBox->isChecked()) {
            std::string matiereName = matiereNameLineEdit->text().toStdString();
            float coeffA = QStringToFloat(coeffALineEdit->text());
            float coeffB = QStringToFloat(coeffBLineEdit->text());
            float coeffK = QStringToFloat(coeffKLineEdit->text());
            pipeBar = pipeBarLineEdit->text().toInt();
            innerDiameter = innerDiameterLineEdit->text().toInt();
            outerDiameter = QStringToFloat(outerDiameterLineEdit->text());

            matiere newMatiere(matiereName, coeffA, coeffB, coeffK, {{pipeBar, {{innerDiameter, outerDiameter}}}});

            materials[0].matieres.push_back(newMatiere);
        } else {
            std::string selectedMatiere = matiereComboBox->currentText().toStdString();
            pipeBar = pipeBarLineEdit->text().toInt();
            pipeBar = pipeBarLineEdit->text().toInt();
            innerDiameter = innerDiameterLineEdit->text().toInt();
            outerDiameter = QStringToFloat(outerDiameterLineEdit->text());

            for (auto &table: materials) {
                for (auto &mat: table.matieres) {
                    if (mat.nom == selectedMatiere) {
                        bool barFound = false;
                        for (auto &p: mat.pressions) {
                            if (p.bar == pipeBar) {
                                // Ajouter le nouveau diamètre à la pression existante
                                p.diametre.push_back(std::make_pair(innerDiameter, outerDiameter));
                                barFound = true;
                                break;
                            }
                        }
                        if (!barFound) {
                            // Ajouter la nouvelle pression avec le nouveau diamètre
                            mat.pressions.push_back(pression(pipeBar, {{innerDiameter, outerDiameter}}));
                        }
                        break;
                    }
                }
            }
        }

        dialog->accept();
    });

    // Connecter le bouton Rejeter pour fermer la boîte de dialogue
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    matiereNameLineEdit->setEnabled(false);
    coeffALineEdit->setEnabled(false);
    coeffBLineEdit->setEnabled(false);
    coeffKLineEdit->setEnabled(false);

    dialog->exec();
}




void bdd::suprimmer_tableaux() {
    // Création d'une boîte de dialogue
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Supprimer un tuyau");

    // Création d'un layout de formulaire
    QFormLayout *formLayout = new QFormLayout(dialog);
    dialog->setLayout(formLayout);

    // Création des ComboBox pour la sélection de matière, pression et diamètre intérieur
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QComboBox *pressureComboBox = new QComboBox(dialog);
    QComboBox *innerDiameterComboBox = new QComboBox(dialog);

    // Remplissage de matiereComboBox avec les noms des matières
    for (const auto &table: materials) {
        for (const auto &mat: table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }

    // Fonction pour mettre à jour pressureComboBox avec les pressions uniques pour la matière sélectionnée
    auto updatePressureComboBox = [&]() {
        pressureComboBox->clear();
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();

        for (const auto &table: materials) {
            for (const auto &mat: table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (const auto &p: mat.pressions) {
                        pressureComboBox->addItem(QString::number(p.bar));
                    }
                }
            }
        }
    };

    updatePressureComboBox();

    // Fonction pour mettre à jour innerDiameterComboBox avec les diamètres intérieurs uniques pour la matière et la pression sélectionnées
    auto updateInnerDiameterComboBox = [&]() {
        innerDiameterComboBox->clear();
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        for (const auto &table: materials) {
            for (const auto &mat: table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (const auto &p: mat.pressions) {
                        if (p.bar == selectedPressure) {
                            for (const auto &d: p.diametre) {
                                innerDiameterComboBox->addItem(QString::number(d.first));
                            }
                        }
                    }
                }
            }
        }
    };

    // Connexion des signaux pour mettre à jour les ComboBox
    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        updatePressureComboBox();
        updateInnerDiameterComboBox();
    });

    QObject::connect(pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateInnerDiameterComboBox);

    pressureComboBox->currentIndexChanged(pressureComboBox->currentIndex());

    // Ajout des ComboBox au layout de formulaire
    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Pression:", pressureComboBox);
    formLayout->addRow("Diametre interieur:", innerDiameterComboBox);

    // Création d'une boîte de boutons pour valider ou annuler
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    formLayout->addWidget(buttonBox);

    // Connecte les signaux
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, [&]() {
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        int selectedInnerDiameter = innerDiameterComboBox->currentText().toInt();

        // Supression du tuyau
        for (auto &table: materials) {
            for (auto &mat: table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (auto &p: mat.pressions) {
                        if (p.bar == selectedPressure) {
                            p.diametre.erase(std::remove_if(p.diametre.begin(), p.diametre.end(),
                                                            [selectedInnerDiameter](const std::pair<int, float> &d) {
                                                                return d.first == selectedInnerDiameter;
                                                            }), p.diametre.end());
                        }
                    }
                    mat.pressions.erase(
                            std::remove_if(mat.pressions.begin(), mat.pressions.end(), [](const pression &p) {
                                return p.diametre.empty();
                            }), mat.pressions.end());
                }
            }
            table.matieres.erase(std::remove_if(table.matieres.begin(), table.matieres.end(), [](const matiere &mat) {
                return mat.pressions.empty();
            }), table.matieres.end());
        }
        dialog->accept();
    });

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    dialog->exec();
}


void bdd::tuyau_dispo() {
    // Création d'une boîte de dialogue
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Tuyaux disponibles");

    // Création d'un layout de formulaire
    QFormLayout *formLayout = new QFormLayout(dialog);
    dialog->setLayout(formLayout);

    // Ajout de ComboBox pour les matières et les pressions
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QComboBox *pressureComboBox = new QComboBox(dialog);

    // Remplir le ComboBox des matières
    for (const auto &table: materials) {
        for (const auto &mat: table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }

    // Fonction pour mettre à jour le ComboBox des pressions en fonction de la matière sélectionnée
    auto updatePressureComboBox = [&]() {
        pressureComboBox->clear();
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        for (const auto &table: materials) {
            for (const auto &mat: table.matieres) {
                if (mat.nom == selectedMatiere) {
                    for (const auto &p: mat.pressions) {
                        pressureComboBox->addItem(QString::number(p.bar));
                    }
                }
            }
        }
    };

    // Connexion du signal pour mettre à jour le ComboBox des pressions
    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updatePressureComboBox);
    matiereComboBox->currentIndexChanged(matiereComboBox->currentIndex());

    // Ajout des ComboBox au layout de formulaire
    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Pression:", pressureComboBox);

    // Création d'un tableau pour afficher les tuyaux disponibles
    QTableWidget *tableWidget = new QTableWidget(dialog);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Diametre extérieur" << "Diametre intérieur");
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    formLayout->addRow(tableWidget);

    // Fonction pour mettre à jour le tableau avec les tuyaux disponibles en fonction de la matière et de la pression sélectionnées
    auto updateTableWidget = [&]() {
        tableWidget->clearContents();
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        for (const auto &table: materials) {
            for (const auto &mat: table.matieres) {
                if (mat.nom == selectedMatiere) {
                    std::vector<std::pair<int, float>> tuyaux = get_possible_tuyaux(mat, selectedPressure);
                    tableWidget->setRowCount(tuyaux.size());
                    int row = 0;
                    for (const auto &tuyau : tuyaux) {
                        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(tuyau.first)));
                        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(tuyau.second)));
                        row++;
                    }
                }
            }
        }
    };

    // Connexion des signaux pour mettre à jour le tableau
    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateTableWidget);
    QObject::connect(pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateTableWidget);
    pressureComboBox->currentIndexChanged(pressureComboBox->currentIndex());

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dialog);
    formLayout->addWidget(buttonBox);
    // Connecter le bouton Ok pour fermer la boîte de dialogue
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    dialog->resize(400,400);
    dialog->exec();
}



void bdd::montre_coef() {
    // Création d'une boîte de dialogue
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Coefficients pour une matière");

    // Création d'un layout vertical
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    dialog->setLayout(layout);

    // Ajout d'un label pour la sélection de la matière
    QLabel *inputLabel = new QLabel("Sélectionnez une matière:", dialog);
    layout->addWidget(inputLabel);

    // Ajout d'une ComboBox pour lister les matières
    QComboBox *matiereComboBox = new QComboBox(dialog);
    // Remplir le ComboBox avec les noms des matières
    for (const auto &table : materials) {
        for (const auto &mat : table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }
    layout->addWidget(matiereComboBox);

    // Ajout d'un label pour afficher les résultats
    QLabel *resultLabel = new QLabel(dialog);
    layout->addWidget(resultLabel);

    // Connecter le changement d'index pour afficher les coefficients
    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        // Récupérer le nom de la matière sélectionnée
        std::string matiere_name = matiereComboBox->currentText().toStdString();
        // Récupérer les coefficients de la matière
        auto [a, b, k] = get_material_coefficients(matiere_name);

        // Si les coefficients sont trouvés
        if (a != 0.0f || b != 0.0f || k != 0.0f) {
            // Afficher les coefficients dans le label de résultat
            resultLabel->setText("Coefficients pour " + QString::fromStdString(matiere_name) + ":\na: " + QString::number(a, 'f', 2) + "\nb: " + QString::number(b, 'f', 2) + "\nk: " + QString::number(k, 'f', 2));
        } else {
            // Afficher un message d'erreur si la matière n'est pas trouvée
            resultLabel->setText("Matière non trouvée. Veuillez entrer un nom de matière valide.");
        }
    });

    // Ajout d'une boîte de boutons pour fermer la boîte de dialogue
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dialog);
    layout->addWidget(buttonBox);
    // Connecter le bouton Ok pour fermer la boîte de dialogue
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    // Exécuter la boîte de dialogue
    dialog->exec();

}



void bdd::trouver_tuyau() {
    // Créer une nouvelle boîte de dialogue
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Trouver un tuyau");

    // Créer un layout vertical
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    dialog->setLayout(layout);

    // Créer et ajouter un label pour le diamètre intérieur
    QLabel *inputLabel = new QLabel("Entrez le diamètre intérieur:", dialog);
    layout->addWidget(inputLabel);

    // Créer et ajouter un champ de saisie pour le diamètre intérieur
    QLineEdit *diameterInput = new QLineEdit(dialog);
    QDoubleValidator *validator = new QDoubleValidator(dialog);
    validator->setBottom(0);
    diameterInput->setValidator(validator);
    layout->addWidget(diameterInput);

    // Créer et ajouter un bouton pour soumettre la recherche
    QPushButton *submitButton = new QPushButton("Soumettre", dialog);
    layout->addWidget(submitButton);

    // Créer et ajouter un label pour afficher le résultat
    QLabel *resultLabel = new QLabel(dialog);
    layout->addWidget(resultLabel);

    // Connecter le bouton de soumission pour effectuer la recherche
    QObject::connect(submitButton, &QPushButton::clicked, [&]() {
        float inner_diameter = QStringToFloat(diameterInput->text());
        auto [matiere_name, pressure, outer_diameter] = find_matiere_pressure_and_outer_diameter(inner_diameter);

        // Si un tuyau est trouvé, afficher les informations
        if (!matiere_name.empty()) {
            resultLabel->setText("Matière: " + QString::fromStdString(matiere_name) + "\nPression: " + QString::number(pressure) + "\nDiamètre extérieur: " + QString::number(outer_diameter));
        } else {
            // Sinon, afficher un message d'erreur
            resultLabel->setText("Tuyau non trouvé. Veuillez entrer un diamètre intérieur valide.");
        }
    });

    // Créer et ajouter un bouton pour fermer la boîte de dialogue
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dialog);
    layout->addWidget(buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    // Afficher la boîte de dialogue
    dialog->exec();
}



matiere bdd::findMatiereByName(const std::string& material_name){
    for(const auto& tab : materials){
        for(const auto& mat : tab.matieres){
            if(mat.nom == material_name)
                return mat;
        }
    }
    return matiere();
}


std::vector<int> bdd::getAllPressuresForMatiere(const std::string &material_name) {
    std::vector<int> pressures;

    // Boucle for pour trouver le bon materiel
    for (const auto &mat : materials) {
        for (const auto &matiere : mat.matieres) {
            if (matiere.nom == material_name) {
                for (const auto &p : matiere.pressions) {
                    pressures.push_back(p.bar);
                }
                // Retourne le vecteur de la pression demandé
                return pressures;
            }
        }
    }

    // si le materiau n'est pas trouvé renvoie un vecteur vide
    return pressures;
}


std::vector<std::string> bdd::getAllMatiereNames() {
    std::vector<std::string> matieres;
    // Parcourir les matériaux
    for(const auto &t : materials){
        // Parcourir les matières
        for(const auto &m : t.matieres){
            // Ajouter le nom de la matière au vecteur
            matieres.push_back(m.nom);
        }
    }
    // Retourner le vecteur contenant les noms de matières
    return matieres;

}


std::tuple<std::string, int, int> bdd::find_matiere_pressure_and_outer_diameter(float inner_diameter) {
    // Parcourir les matériaux
    for (const auto &table : materials) {
        // Parcourir les matières
        for (const auto &mat : table.matieres) {
            // Parcourir les pressions
            for (const auto &p : mat.pressions) {
                // Parcourir les diamètres
                for (const auto &d : p.diametre) {
                    float current_inner_diameter = d.second;

                    // Si la différence entre le diamètre intérieur courant et le diamètre intérieur recherché est inférieure à 0.001
                    if (std::abs(current_inner_diameter - inner_diameter) < 0.001) {
                        // Retourner le nom de la matière, la pression et le diamètre extérieur correspondants
                        return std::make_tuple(mat.nom, p.bar, d.first);
                    }
                }
            }
        }
    }
    // Retourner un tuple vide si aucune correspondance n'est trouvée
    return std::make_tuple("", 0, 0);
}


std::vector<float> bdd::getInnerDiametersForMatiereAndPressure(const std::string &material_name, int pressure) {
    std::vector<float> innerDiameters;

    // Find the matching material
    for (const auto &table : materials) {
        for (const auto &material : table.matieres) {
            if (material.nom == material_name) {
                // Find the matching pressure
                for (const auto &pressure_obj : material.pressions) {
                    if (pressure_obj.bar == pressure) {
                        // Extract the inner diameters from the matching tuyaux
                        for (const auto &inner_outer_diameters_pair : pressure_obj.diametre) {
                            innerDiameters.push_back(inner_outer_diameters_pair.first);
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return innerDiameters;
}


float bdd::getInnerDiameterForMatierePressureAndOuterDiameter(const std::string &material_name, int pressure, float outer_diameter) {
    // Find the matching material
    for (const auto &tableau : materials) {
        for (const auto &material : tableau.matieres) {
            if (material.nom == material_name) {
                // Find the matching pressure
                for (const auto &pressure_tuyaux_pair : material.pressions) {
                    if (pressure_tuyaux_pair.bar == pressure) {
                        // Find the matching outer diameter and return the corresponding inner diameter
                        for (const auto &outer_inner_diameters_pair : pressure_tuyaux_pair.diametre) {
                            if (outer_inner_diameters_pair.first == outer_diameter) {
                                return outer_inner_diameters_pair.second;
                            }
                        }
                    }
                }
            }
        }
    }
    // Return -1 if no matching inner diameter is found
    return -1;
}




