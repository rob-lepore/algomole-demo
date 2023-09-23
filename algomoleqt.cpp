#include "stdafx.h"
#include "algomoleqt.h"
#include "renderer.h"
#include "renderthread.h"

#include <algomole/utils/validator.h>
#include <algomole/controller/surfaceextractorbuilder.h>
#include <algomole/parsing/pdbfileparser.h>
#include <algomole/preprocessing/boundingboxpreprocessing.h>
#include <algomole/spacefilling/atomspacefiller.h>
#include <algomole/spacefilling/gridspacefiller.h>
#include <algomole/meshing/marchingcubesmesher.h>
#include <algomole/postprocessing/nonepostprocessing.h>
#include <algomole/spacefilling/gaussianspacefiller.h>
#include <algomole/spacefilling/edtspacefiller.h>
#include <algomole/postprocessing/laplacianpostprocessing.h>
#include <algomole/meshing/alphawrapmesher.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

AlgomoleQt::AlgomoleQt(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    renderer = new Renderer();

    QWidget* rendererWidget = ui.rendererWidget;
    renderer->setParent(rendererWidget);

    ui.pushButton->setDisabled(true);

    QObject::connect(ui.loadButton, &QPushButton::clicked, this, &AlgomoleQt::makeModel);
    QObject::connect(ui.pushButton, &QPushButton::clicked, this, &AlgomoleQt::clear);

}

AlgomoleQt::~AlgomoleQt()
{
}

void AlgomoleQt::clear()
{
    renderer->clear();
    ui.idInput->setDisabled(false);
    ui.comboBox->setDisabled(false);
    ui.comboBox_2->setDisabled(false);
    ui.comboBox_3->setDisabled(false);
    ui.lineEdit->setDisabled(false);
    ui.loadButton->setDisabled(false);
    ui.pushButton->setEnabled(false);
    ui.textBrowser->setText("");
}

void AlgomoleQt::makeModel() {
   
    QString url = "https://files.rcsb.org/view/" + ui.idInput->text() + ".pdb";
    getPDBFile(url);
    
}

void AlgomoleQt::handleRenderResult(am::gfx::Mesh* mesh, std::string pdb) {
    mesh->recalculateNormals();
    renderer->addMesh(mesh);
    am::utils::Validator validator("C:/Program Files/Chimera 1.17.3/bin");
    ui.textBrowser->append("Surface Area = " + QString::number(mesh->surfaceArea()));
    ui.textBrowser->append("Volume = " + QString::number(mesh->volume()));
    
    if (false) {
        try {
            double err = validator.areaRelativeError(pdb, mesh) * 100.;
            ui.textBrowser->append("Area relative error: " + QString::number(err) + "%");
        }
        catch (std::exception e) {
            ui.textBrowser->append("Area relative error: N/A");
        }

        try {
            double err = validator.volumeRelativeError(pdb, mesh) * 100.;
            ui.textBrowser->append("Volume relative error: " + QString::number(err) + "%");
        }
        catch (std::exception e) {
            ui.textBrowser->append("Volume relative error: N/A");
        }
    }
    
    //mesh->toObjFile("./1eca-256.obj");
}


void AlgomoleQt::getPDBFile(QString url) {
    qDebug() << url;
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply* reply) {

            if (reply->error()) {
                std::cout << reply->errorString().toStdString() << "\n";
                ui.textBrowser->setText(reply->errorString());
                return;
            }
            std::cout << "\n";

            std::string res = reply->readAll().toStdString();
            /*
            std::ifstream file("assets/lta.pdb");
            std::string res((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
            file.close();
            */

            ui.idInput->setDisabled(true);
            ui.comboBox->setDisabled(true);
            ui.comboBox_2->setDisabled(true);
            ui.comboBox_3->setDisabled(true);
            ui.lineEdit->setDisabled(true);
            ui.loadButton->setDisabled(true);
            ui.pushButton->setEnabled(true);


            using namespace am::pipeline;

            int sizeIndex = ui.comboBox_3->currentIndex();
            int size = std::pow(2, sizeIndex + 6);

            SpaceFiller* sp = nullptr;
            int surface = options::VDWS;
            int spaceIndex = ui.comboBox->currentIndex();
            if (spaceIndex == 0) {
                sp = new EDTSpaceFiller;
                surface = options::MS;
            }
            else sp = new GaussianSpaceFiller;

            Mesher* mesher = nullptr;
            Postprocessing* post = nullptr;
            int meshIndex = ui.comboBox_2->currentIndex();
            if (meshIndex == 0) {
                mesher = new MarchingCubesMesher;
                post = new LaplacianPostprocessing;
            }
            else {
                mesher = new AlphaWrapMesher;
                post = new NonePostprocessing;
            }

            float probe = ui.lineEdit->text().toFloat();


            controller::SurfaceExtractorBuilder builder;
            controller::SurfaceExtractor se = builder
                .setFileParser(new PdbFileParser)
                .setPreprocessing(new BoundingBoxPreprocessing)
                .setSpaceFiller(sp)
                .setMeshBuilder(mesher)
                .setPostprocessing(post)
                .setOption("size", size)
                .setOption("surface", surface)
                .setOption("color_mode", options::CHAIN)
                .setOption("normals", options::SMOOTH)
                .setOption("filling_threads", 2)
                .setOption("log", options::NO)
                .setOption("probe_radius", probe)
                .build();

            RenderThread* renderThread = new RenderThread(this);
            renderThread->str = res;
            renderThread->se = new controller::SurfaceExtractor(se);

            connect(renderThread, &RenderThread::resultReady, this, &AlgomoleQt::handleRenderResult);
            renderThread->start();

        });

    manager->get(QNetworkRequest(QUrl(url)));
}