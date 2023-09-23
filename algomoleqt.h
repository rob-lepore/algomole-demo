#pragma once

#include <QtWidgets/QWidget>
#include "ui_algomoleqt.h"
#include <algomole/gfx/mesh.h>

class AlgomoleQt : public QWidget
{
    Q_OBJECT

public:
    AlgomoleQt(QWidget *parent = nullptr);
    ~AlgomoleQt();

public slots:
    void makeModel();
    void clear();
    void handleRenderResult(am::gfx::Mesh* mesh, std::string s);

private:
    Ui::AlgomoleQtClass ui;
    Renderer* renderer;
    void getPDBFile(QString url);


};
