// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "window.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>
#include <inttypes.h>

#include "galois.h"

int get_cong_for_fac(mat32gf2* mat, int i);
void get_mat_for_num(u32 num, mat32gf2* out);

extern int facs[];
extern int invs[];
extern uint32_t period; 

//! [0]
Window::Window(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *formGroup = new QGroupBox("");

    QLabel *seedLabel = new QLabel(tr("Seed (hex):"));
    seedLabel->setAlignment(Qt::AlignRight);
    QLabel *positionLabel = new QLabel(tr("Position:"));
    positionLabel->setAlignment(Qt::AlignRight);

    seedLineEdit = new QLineEdit;
    QRegularExpression rx("^[0-9a-fA-F]{1,8}$");
    QValidator *hexValidator = new QRegularExpressionValidator(rx, this);
    seedLineEdit->setValidator(hexValidator);

    connect(seedLineEdit, &QLineEdit::returnPressed, this, &Window::seedEntered);

    positionLineEdit = new QLineEdit;
    positionLineEdit->setReadOnly(true);

    QGridLayout *formLayout = new QGridLayout;
    formLayout->addWidget(seedLabel, 0, 0);
    formLayout->addWidget(seedLineEdit, 0, 1, 1, 2);
    formLayout->addWidget(positionLabel, 1, 0);
    formLayout->addWidget(positionLineEdit, 1, 1, 1, 2);
    formGroup->setLayout(formLayout);

    formLayout->setColumnStretch(0, 0);
    formLayout->setColumnStretch(1, 1);
//! [8]
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(formGroup, 0, 0);
    setLayout(layout);

    setWindowTitle(tr("Yu-Gi-Oh! The Sacred Cards - RNG Position finder"));
}
//! [8]

//! [10]
void Window::seedEntered()
{
   bool ok = false;
   uint32_t seed = seedLineEdit->text().toUInt(&ok, 16);
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
        positionLineEdit->setText(QString::number(pos));
    } else {
        positionLineEdit->setText("invalid");
    }
   }
}
//! [10]
