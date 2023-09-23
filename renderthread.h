#pragma once
#include <qthread.h>
#include <algomole/gfx/mesh.h> 
#include <algomole/controller/surfaceextractor.h>

class RenderThread :
    public QThread
{
    Q_OBJECT
public:
    RenderThread(QObject* parent = nullptr) : QThread(parent) {}
    void run() override;
    std::string str;
    am::pipeline::controller::SurfaceExtractor* se;


signals:
    void resultReady(am::gfx::Mesh* mesh, std::string s);
};

