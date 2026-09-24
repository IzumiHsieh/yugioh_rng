// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "window.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <QValidator>
#include <inttypes.h>

#include "galois.h"

int get_cong_for_fac(mat32gf2* mat, int i);
void get_mat_for_num(u32 num, mat32gf2* out);
u32 get_seed_for_pos(u32 pos);

extern int facs[];
extern int invs[];
extern uint32_t period;

//! [0]
Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *seedToPosFormGroup = new QGroupBox("");
    seedToPosFormGroup->setTitle("Seed to position");

    QLabel *seedLabel1 = new QLabel(tr("Seed (hex):"));
    seedLabel1->setAlignment(Qt::AlignRight);
    QLabel *positionLabel1 = new QLabel(tr("Position:"));
    positionLabel1->setAlignment(Qt::AlignRight);

    seedInputLineEdit = new QLineEdit;
    QRegularExpression rx("^[0-9a-fA-F]{1,8}$");
    QValidator *hexValidator = new QRegularExpressionValidator(rx, this);
    seedInputLineEdit->setValidator(hexValidator);

    connect(seedInputLineEdit, &QLineEdit::returnPressed, this, &Window::seedEntered);

    positionOutputLineEdit = new QLineEdit;
    positionOutputLineEdit->setReadOnly(true);

    QGridLayout *formLayout1 = new QGridLayout;
    formLayout1->addWidget(seedLabel1, 0, 0);
    formLayout1->addWidget(seedInputLineEdit, 0, 1, 1, 2);
    formLayout1->addWidget(positionLabel1, 1, 0);
    formLayout1->addWidget(positionOutputLineEdit, 1, 1, 1, 2);
    seedToPosFormGroup->setLayout(formLayout1);

    formLayout1->setColumnStretch(0, 0);
    formLayout1->setColumnStretch(1, 1);
   
    QGroupBox *posToSeedFormGroup = new QGroupBox("");
    posToSeedFormGroup->setTitle("Position to seed");

    QLabel *seedLabel2 = new QLabel(tr("Seed (hex):"));
    seedLabel2->setAlignment(Qt::AlignRight);
    QLabel *positionLabel2 = new QLabel(tr("Position:"));
    positionLabel2->setAlignment(Qt::AlignRight);

    positionInputLineEdit = new QLineEdit
    ;QRegularExpression rx2("^[0-9]+$");
    QValidator *posValidator = new QRegularExpressionValidator(rx2, this);
    positionInputLineEdit->setValidator(posValidator);
    
    connect(positionInputLineEdit, &QLineEdit::returnPressed, this, &Window::positionEntered);
    
    seedOutputLineEdit = new QLineEdit;
    seedOutputLineEdit->setReadOnly(true);

    QGridLayout *formLayout2 = new QGridLayout;
    formLayout2->addWidget(positionLabel2, 0, 0);
    formLayout2->addWidget(positionInputLineEdit, 0, 1, 1, 2);
    formLayout2->addWidget(seedLabel2, 1, 0);
    formLayout2->addWidget(seedOutputLineEdit, 1, 1, 1, 2);
    posToSeedFormGroup->setLayout(formLayout2);

    formLayout2->setColumnStretch(0, 0);
    formLayout2->setColumnStretch(1, 1);
//! [8]
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(seedToPosFormGroup, 0, 0);
    layout->addWidget(posToSeedFormGroup, 1, 0);
    setLayout(layout);

    setWindowTitle(tr("Yu-Gi-Oh! The Sacred Cards - RNG Position finder"));
}
//! [8]

//! [10]
void Window::seedEntered()
{
   bool ok = false;
   uint32_t seed = seedInputLineEdit->text().toUInt(&ok, 16);
   if (ok) {
    mat32gf2 el_mat;
    get_mat_for_num(seed, &el_mat);
    long long pos = 0;
    bool validSeed = true;
    for (int i=0; i<5; i++) {
        long long a = get_cong_for_fac(&el_mat, i);
        if (a == -1) {
            validSeed = false;
            break;
        }
        long long m = period / facs[i];
        pos += a*m*invs[i];
        pos = (pos % period);
    }
    if (validSeed) {
        positionOutputLineEdit->setText(QString::number(pos));
    } else {
        positionOutputLineEdit->setText("invalid");
    }
   }
}
//! [10]

void Window::positionEntered() {
    bool ok = false;
    uint32_t pos = positionInputLineEdit->text().toUInt(&ok);
    if (ok) {
        u32 seed = get_seed_for_pos(pos);
        seedOutputLineEdit->setText(QString("%1").arg(seed, 8, 16, '0'));
    }
}
