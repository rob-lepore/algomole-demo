#include "stdafx.h"
#include "renderthread.h"

#include <algomole/utils/Logger.h>

void RenderThread::run() {

    am::utils::Logger log("Core");

    log.startTimer();
    am::gfx::Mesh* mesh = se->generateSurfaceMesh(str);
    log.logElapsedTime("Surface mesh generated");

    //log.log("Surface area = " + std::to_string(mesh->surfaceArea()));
    
	emit resultReady(mesh, str);
}