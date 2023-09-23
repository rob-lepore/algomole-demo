#include "stdafx.h"
#include "algomoleqt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    /*
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "r", stdin);
    */

    QApplication a(argc, argv);
    AlgomoleQt w;
    w.show();
    return a.exec();
}
