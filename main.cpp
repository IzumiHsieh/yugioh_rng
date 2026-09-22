// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QApplication>

#include "window.h"

void setup_pol_mat();
void compute_indices();

int main(int argc, char *argv[])
{
    setup_pol_mat();
    compute_indices();
    QApplication app(argc, argv);
    Window window;
    window.resize(500, 200);
    window.show();
    return app.exec();
}