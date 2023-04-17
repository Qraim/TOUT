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

    // Skip the first line (column headers)
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
        k = std::stod(token); ;

        std::getline(ss, token, ';');
        bar = std::stoi(token);

        std::getline(ss, token, ';');
        diameter_key = std::stoi(token);

        std::getline(ss, token, ';');
        diameter_value = static_cast<float>(std::stod(token));

        // Check if the material already exists in the materials vector
        auto it = std::find_if(materials.begin(), materials.end(), [&](const tableau& t) {
            return t.nom == nom;
        });

        // If the material exists, add the diameter to the corresponding pressure
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
            // If the material doesn't exist, add it to the materials vector
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
    std::cout<<"here"<<std::endl;
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
    QPushButton *button5 = new QPushButton("Tuyaux", this);
    QPushButton *button6 = new QPushButton("Coef", this);
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

std::string bdd::find_matiere_for_diametre(float diametre_interieur) {
    for (const auto& tab : materials) {
        for (const auto& mat : tab.matieres) {
            for (const auto& press : mat.pressions) {
                for (const auto& diam : press.diametre) {
                    if (diam.first == diametre_interieur) {
                        return mat.nom;
                    }
                }
            }
        }
    }
    return "";
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

    // Create widgets for selecting matiere, pressure, outer diameter, and inner diameter
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QComboBox *pressureComboBox = new QComboBox(dialog);
    QComboBox *outerDiameterComboBox = new QComboBox(dialog);
    QLineEdit *currentInnerDiameterLineEdit = new QLineEdit(dialog);
    QLineEdit *newInnerDiameterLineEdit = new QLineEdit(dialog);

    // Function to populate matiereComboBox with matiere names
    auto populateMatiereComboBox = [&]() {
        matiereComboBox->clear();
        for (const auto &table : materials) {
            for (const auto &mat : table.matieres) {
                matiereComboBox->addItem(QString::fromStdString(mat.nom));
            }
        }
    };

    populateMatiereComboBox();

    // Connect matiereComboBox's signal to update pressureComboBox and outerDiameterComboBox
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

    // Connect pressureComboBox's signal to update outerDiameterComboBox
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

    // Connect outerDiameterComboBox's signal to display the current inner diameter
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

    // Trigger matiereComboBox signal to fill pressureComboBox and outerDiameterComboBox with initial values
    matiereComboBox->currentIndexChanged(matiereComboBox->currentIndex());

    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Pression:", pressureComboBox);
    formLayout->addRow("Diametre exterieur:", outerDiameterComboBox);
    formLayout->addRow("Diametre interieur actuel:", currentInnerDiameterLineEdit);
    formLayout->addRow("Nouveau diametre interieur:", newInnerDiameterLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    formLayout->addWidget(buttonBox);

    // Connect buttonBox signals
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, [&]() {
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        int selectedOuterDiameter = outerDiameterComboBox->currentText().toInt();
        float currentInnerDiameter = QStringToFloat(currentInnerDiameterLineEdit->text());
        float newInnerDiameter = QStringToFloat(newInnerDiameterLineEdit->text());

        // Update the inner diameter for the selected pipe
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

    // Show the modification window
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

    // Create widgets for the form
    QCheckBox *newMatiereCheckBox = new QCheckBox("Nouvelle matiere", dialog);
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QLineEdit *matiereNameLineEdit = new QLineEdit(dialog);
    QLineEdit *coeffALineEdit = new QLineEdit(dialog);
    QLineEdit *coeffBLineEdit = new QLineEdit(dialog);
    QLineEdit *coeffKLineEdit = new QLineEdit(dialog);
    QLineEdit *pipeBarLineEdit = new QLineEdit(dialog);
    QLineEdit *innerDiameterLineEdit = new QLineEdit(dialog);
    QLineEdit *outerDiameterLineEdit = new QLineEdit(dialog);

    // Populate matiereComboBox with matiere names
    for (const auto &table : materials) {
        for (const auto &mat : table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }

    // Connect newMatiereCheckBox's signal to enable/disable matiereNameLineEdit and coefficient LineEdits
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

    // Connect buttonBox signals
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

            // Create new matiere with the input values
            matiere newMatiere(matiereName, coeffA, coeffB, coeffK, {{pipeBar, {{innerDiameter, outerDiameter}}}});

            // Add the new matiere to materials (assuming it's added to the first tableau)
            materials[0].matieres.push_back(newMatiere);
        } else {
            std::string selectedMatiere = matiereComboBox->currentText().toStdString();
            pipeBar = pipeBarLineEdit->text().toInt();
            pipeBar = pipeBarLineEdit->text().toInt();
            innerDiameter = innerDiameterLineEdit->text().toInt();
            outerDiameter = QStringToFloat(outerDiameterLineEdit->text());

            // Add the new pipe to the existing matiere
            for (auto &table : materials) {
                for (auto &mat : table.matieres) {
                    if (mat.nom == selectedMatiere) {
                        bool barFound = false;
                        for (auto &p : mat.pressions) {
                            if (p.bar == pipeBar) {
                                // Add the new diameter to the existing bar
                                p.diametre.push_back(std::make_pair(innerDiameter, outerDiameter));
                                barFound = true;
                                break;
                            }
                        }
                        if (!barFound) {
                            // Add the new bar with the new diameter
                            mat.pressions.push_back(pression(pipeBar, {{innerDiameter, outerDiameter}}));
                        }
                        break;
                    }
                }
            }
        }

        dialog->accept();
    });

    QObject::connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    matiereNameLineEdit->setEnabled(false);
    coeffALineEdit->setEnabled(false);
    coeffBLineEdit->setEnabled(false);
    coeffKLineEdit->setEnabled(false);

    // Show the addition window
    dialog->exec();
}



void bdd::suprimmer_tableaux() {
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Supprimer un tuyau");

    QFormLayout *formLayout = new QFormLayout(dialog);
    dialog->setLayout(formLayout);

    // Create widgets for selecting matiere, pressure, and inner diameter
    QComboBox *matiereComboBox = new QComboBox(dialog);
    QComboBox *pressureComboBox = new QComboBox(dialog);
    QComboBox *innerDiameterComboBox = new QComboBox(dialog);

    // Populate matiereComboBox with matiere names
    for (const auto &table: materials) {
        for (const auto &mat: table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }

    // Function to update pressureComboBox with unique pressures for the selected matiere
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

    // Connect matiereComboBox's signal to update pressureComboBox and innerDiameterComboBox
    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
        updatePressureComboBox();
        updateInnerDiameterComboBox();
    });

    // Connect pressureComboBox's signal to update innerDiameterComboBox
    QObject::connect(pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateInnerDiameterComboBox);

    // Trigger pressureComboBox signal to fill innerDiameterComboBox with initial values
    pressureComboBox->currentIndexChanged(pressureComboBox->currentIndex());

    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Pression:", pressureComboBox);
    formLayout->addRow("Diametre interieur:", innerDiameterComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
    formLayout->addWidget(buttonBox);

    // Connect buttonBox signals
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, [&]() {
        std::string selectedMatiere = matiereComboBox->currentText().toStdString();
        int selectedPressure = pressureComboBox->currentText().toInt();
        int selectedInnerDiameter = innerDiameterComboBox->currentText().toInt();

        // Remove the selected pipe
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

    // Show the delete window
    dialog->exec();
}


void bdd::tuyau_dispo() {

    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Tuyaux disponibles");

    QFormLayout *formLayout = new QFormLayout(dialog);
    dialog->setLayout(formLayout);

    QComboBox *matiereComboBox = new QComboBox(dialog);
    QComboBox *pressureComboBox = new QComboBox(dialog);

    for (const auto &table: materials) {
        for (const auto &mat: table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }

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

    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updatePressureComboBox);
    matiereComboBox->currentIndexChanged(matiereComboBox->currentIndex());

    formLayout->addRow("Matiere:", matiereComboBox);
    formLayout->addRow("Pression:", pressureComboBox);

    QTableWidget *tableWidget = new QTableWidget(dialog);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Diametre extérieur"<< "Diametre intérieur");
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    formLayout->addRow(tableWidget);

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

    QObject::connect(matiereComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateTableWidget);
    QObject::connect(pressureComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), updateTableWidget);
    pressureComboBox->currentIndexChanged(pressureComboBox->currentIndex());

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dialog);
    formLayout->addWidget(buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    dialog->resize(400,400);
    dialog->exec();
}



void bdd::montre_coef() {
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Coefficients pour une matière");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    dialog->setLayout(layout);

    QLabel *inputLabel = new QLabel("Sélectionnez une matière:", dialog);
    layout->addWidget(inputLabel);

    QComboBox *matiereComboBox = new QComboBox(dialog);
    for (const auto &table : materials) {
        for (const auto &mat : table.matieres) {
            matiereComboBox->addItem(QString::fromStdString(mat.nom));
        }
    }
    layout->addWidget(matiereComboBox);

    QPushButton *submitButton = new QPushButton("Soumettre", dialog);
    layout->addWidget(submitButton);

    QLabel *resultLabel = new QLabel(dialog);
    layout->addWidget(resultLabel);

    QObject::connect(submitButton, &QPushButton::clicked, [&]() {
        std::string matiere_name = matiereComboBox->currentText().toStdString();
        auto [a, b, k] = get_material_coefficients(matiere_name);

        if (a != 0.0f || b != 0.0f || k != 0.0f) {
            resultLabel->setText("Coefficients pour " + QString::fromStdString(matiere_name) + ":\na: " + QString::number(a, 'f', 2) + "\nb: " + QString::number(b, 'f', 2) + "\nk: " + QString::number(k, 'f', 2));
        } else {
            resultLabel->setText("Matière non trouvée. Veuillez entrer un nom de matière valide.");
        }
    });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dialog);
    layout->addWidget(buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    dialog->exec();
}

std::pair<std::string, float> bdd::find_matiere_and_outer_diameter(float inner_diameter) {
    for (const auto& tab : materials) {
        for (const auto& mat : tab.matieres) {
            for (const auto& press : mat.pressions) {
                for (const auto& diam : press.diametre) {
                    if (diam.second == inner_diameter) {
                        return {mat.nom, diam.first};
                    }
                }
            }
        }
    }
    return {"", 0.0f};
}


void bdd::trouver_tuyau() {
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("Trouver un tuyau");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    dialog->setLayout(layout);

    QLabel *inputLabel = new QLabel("Entrez le diamètre intérieur:", dialog);
    layout->addWidget(inputLabel);

    QLineEdit *diameterInput = new QLineEdit(dialog);
    QDoubleValidator *validator = new QDoubleValidator(dialog);
    validator->setBottom(0);
    diameterInput->setValidator(validator);
    layout->addWidget(diameterInput);

    QPushButton *submitButton = new QPushButton("Soumettre", dialog);
    layout->addWidget(submitButton);

    QLabel *resultLabel = new QLabel(dialog);
    layout->addWidget(resultLabel);

    QObject::connect(submitButton, &QPushButton::clicked, [&]() {
        float inner_diameter = QStringToFloat(diameterInput->text());
        auto [matiere_name, pressure, outer_diameter] = find_matiere_pressure_and_outer_diameter(inner_diameter);

        if (!matiere_name.empty()) {
            resultLabel->setText("Matière: " + QString::fromStdString(matiere_name) + "\nPression: " + QString::number(pressure) + "\nDiamètre extérieur: " + QString::number(outer_diameter));
        } else {
            resultLabel->setText("Tuyau non trouvé. Veuillez entrer un diamètre intérieur valide.");
        }
    });

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dialog);
    layout->addWidget(buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    dialog->exec();
}


matiere bdd::findMatiereByName(const std::string& material_name){
    for(const auto& tab : materials){
        for(const auto& mat : tab.matieres){
            if(mat.nom == material_name)
                return mat;
        }
    }
}

std::vector<int> bdd::getAllPressuresForMatiere(const std::string &material_name) {
    std::vector<int> pressures;

    // Iterate through materials to find the material by name
    for (const auto &mat : materials) {
        for (const auto &matiere : mat.matieres) {
            if (matiere.nom == material_name) {
                // Iterate through the pressures vector and collect pressure values
                for (const auto &p : matiere.pressions) {
                    pressures.push_back(p.bar);
                }
                // Return the vector of pressure values
                return pressures;
            }
        }
    }

    // If the material is not found, return an empty vector
    return pressures;
}



std::pair<std::string, QString> bdd::find_matiere_and_pressure_for_diametre(float inner_diameter) {
    for(const auto& tab : materials){
        for(const auto& mat : tab.matieres){
            for(auto& press : mat.pressions){
                for(const auto& diam : press.diametre){
                    if(diam.second == inner_diameter){
                        return {mat.nom,QString::number(press.bar)};
                    }
                }
            }
        }
    }
    return {"",""};
}

std::vector<std::string> bdd::getAllMatiereNames() {
    std::vector<std::string> matieres;
    for(const auto &t : materials){
        for(const auto &m : t.matieres){
            matieres.push_back(m.nom);
        }
    }
    return matieres;
}



std::tuple<std::string, int, int> bdd::find_matiere_pressure_and_outer_diameter(float inner_diameter) {
    for (const auto &table : materials) {
        for (const auto &mat : table.matieres) {
            for (const auto &p : mat.pressions) {
                for (const auto &d : p.diametre) {
                    float current_inner_diameter = d.second;
                    if (std::abs(current_inner_diameter - inner_diameter) < 0.001) {
                        return std::make_tuple(mat.nom, p.bar, d.first);
                    }
                }
            }
        }
    }
    return std::make_tuple("", 0, 0);
}
