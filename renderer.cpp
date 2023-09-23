#include "stdafx.h"
#include "renderer.h"


#include "glm/ext.hpp"

using namespace am::gfx;

Renderer::Renderer(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    camera.setPosition({ 0,0,-250,0 });

    view = glm::mat4(1);
    projection = glm::mat4(1);
    model = glm::mat4(1);


}

Renderer::~Renderer()
{
}

void Renderer::addMesh(Mesh* m) {
    this->meshes.push_back(new RenderMesh(m));
    this->repaint();
}

void Renderer::rotateScene() {
    model = glm::rotate(model, 0.008f, glm::vec3(0, 1, 0));
    repaint();
}


void Renderer::initializeGL() {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

    std::cout << "INFO: OpenGL Version: " << f->glGetString(GL_VERSION) << "\n";


    this->createShaders();

    f->glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    f->glEnable(GL_DEPTH_TEST);
    f->glDepthFunc(GL_LESS);
    f->glEnable(GL_CULL_FACE);
    f->glCullFace(GL_BACK);
    f->glFrontFace(GL_CCW);

    matProj = f->glGetUniformLocation(programId, "Projection");
    matModel = f->glGetUniformLocation(programId, "Model");
    matView = f->glGetUniformLocation(programId, "View");

    loc_view_pos = f->glGetUniformLocation(programId, "ViewPos");

    //location delle variabili uniformi per la gestione della luce
    light_unif.light_color_pointer = f->glGetUniformLocation(programId, "light.color");
    light_unif.light_power_pointer = f->glGetUniformLocation(programId, "light.power");
    light_unif.light_vector_pointer = f->glGetUniformLocation(programId, "light.vector");
    //location delle variabili uniformi per la gestione dei materiali
    light_unif.material_ambient = f->glGetUniformLocation(programId, "material.ambient");
    light_unif.material_diffuse = f->glGetUniformLocation(programId, "material.diffuse");
    light_unif.material_specular = f->glGetUniformLocation(programId, "material.specular");
    light_unif.material_shininess = f->glGetUniformLocation(programId, "material.shininess");

    perspectiveSetup = {};
    perspectiveSetup.aspect = 1;
    perspectiveSetup.fovY = 45.0f;
    perspectiveSetup.far_plane = 2000.0f;
    perspectiveSetup.near_plane = 0.1f;

    light = {
        {0,1,2,0},
        {0.9,0.9,0.9},
        50.0f
    };
    //0.24725	0.1995	0.0745	0.75164	0.60648	0.22648	0.628281	0.555802	0.366065	0.4
    material = {
        glm::vec3(0.9),
        glm::vec3(0.4),
        glm::vec3(0.2),
        16
    };

}

void Renderer::paintGL()
{
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    f->glUseProgram(programId);

    view = glm::lookAt(glm::vec3(camera.getPosition()), glm::vec3(camera.getTarget()), glm::vec3(camera.getUpVector()));

    f->glUniformMatrix4fv(matProj, 1, GL_FALSE, value_ptr(projection));
    f->glUniformMatrix4fv(matView, 1, GL_FALSE, value_ptr(view));
    f->glUniformMatrix4fv(matModel, 1, GL_FALSE, value_ptr(model));

    //Passo allo shader il puntatore a  colore luce, posizione ed intensit
    f->glUniform4f(light_unif.light_vector_pointer, light.vector.x, light.vector.y, light.vector.z, light.vector.w);
    f->glUniform3f(light_unif.light_color_pointer, light.color.x, light.color.y, light.color.z);
    f->glUniform1f(light_unif.light_power_pointer, light.power);

    //Passo allo shader il puntatore ai valori del materiale
    f->glUniform3f(light_unif.material_ambient, material.ambient.x, material.ambient.y, material.ambient.z);
    f->glUniform3f(light_unif.material_diffuse, material.diffuse.x, material.diffuse.y, material.diffuse.z);
    f->glUniform3f(light_unif.material_specular, material.specular.x, material.specular.y, material.specular.z);
    f->glUniform1f(light_unif.material_shininess, material.shininess);


    //Passo allo shader il puntatore alla posizione della camera
    f->glUniform3f(loc_view_pos, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

    f->glClear(GL_COLOR_BUFFER_BIT);
    for (RenderMesh* m : this->meshes) {
        if (!hasVAO(m)) {
            createVAO(m);
        }
        render(m);
    }


}

void Renderer::resizeGL(int w, int h)
{
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

    
    perspectiveSetup.aspect = (float)w / h;
    projection = glm::perspective(glm::radians(perspectiveSetup.fovY), perspectiveSetup.aspect, perspectiveSetup.near_plane, perspectiveSetup.far_plane);
    f->glViewport(0, 0, w, h);
    
    /*
    projection = glm::perspective(glm::radians(perspectiveSetup.fovY), perspectiveSetup.aspect, perspectiveSetup.near_plane, perspectiveSetup.far_plane);

    float AspectRatio_mondo = 1;
    if (AspectRatio_mondo > w / h) 
    {
        f->glViewport(0, 0, w, w / AspectRatio_mondo);
    }
    else {
        f->glViewport(0, 0, h * AspectRatio_mondo, h);
    }
    */

}

void Renderer::createShaders(void)
{
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

    vertexShaderId = loadShader("./shaders/vertexShader.glsl", GL_VERTEX_SHADER);
    fragmentShaderId = loadShader("./shaders/fragmentShader.glsl", GL_FRAGMENT_SHADER);

    programId = f->glCreateProgram();
    f->glAttachShader(programId, vertexShaderId);
    f->glAttachShader(programId, fragmentShaderId);
    f->glLinkProgram(programId);
    f->glUseProgram(programId);

}

void Renderer::destroyShaders(void)
{
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

}

void Renderer::keyPressEvent(QKeyEvent* keyEvent) {
    switch (keyEvent->key()) {
    case Qt::Key_Up:
    {
        camera.moveUp();
        break;
    }
    case Qt::Key_Down:
    {
        camera.moveDown();
        break;
    }
    case Qt::Key_A:
    {
        camera.moveLeft();
        break;
    }
    case Qt::Key_D:
    {
        camera.moveRight();
        break;
    }
    case Qt::Key_S:
    {
        camera.moveBack();
        break;
    }
    case Qt::Key_W:
    {
        camera.moveForward();
        break;
    }
    case Qt::Key_R:
    {
        rotateScene();
        break;
    }
    case Qt::Key_P:
    {
        QPoint globalPos = mapToGlobal(QPoint(0, 0));
        QScreen* screen = QGuiApplication::screenAt(globalPos);
        QPixmap screenshot = screen->grabWindow(winId(), 0, 0, width(),height());
        screenshot.save("images/screenshot.png");
        std::cout << "Image saved\n";
        break;
    }

    }
    repaint();
}

void Renderer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->tb.setEnabled(true, event->pos().x(), event->pos().y());
        //qDebug() << "start";
    }
}

void Renderer::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        this->tb.setEnabled(false, event->pos().x(), event->pos().y());
        //qDebug() << "end";
    }
}

void Renderer::mouseMoveEvent(QMouseEvent* event) {

    camera.setPosition(glm::vec4(tb.arcBall(camera, event->pos().x(), event->pos().y()), 1));

    repaint();
}


void Renderer::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        camera.moveForward();
    }
    else {
        camera.moveBack();
    }
    repaint();
}

bool Renderer::hasVAO(RenderMesh* m) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

    return f->glIsVertexArray(m->VAO);
}

void Renderer::createVAO(RenderMesh* m)
{
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

    const size_t VertexSize = sizeof(Vertex);
    const size_t BufferSize = m->m->getVertices().size() * VertexSize;
    const size_t RgbOffset = sizeof(glm::vec4);


    f->glGenVertexArrays(1, &m->VAO);
    f->glBindVertexArray(m->VAO);

    f->glGenBuffers(1, &m->VBO);
    f->glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
    f->glBufferData(GL_ARRAY_BUFFER, BufferSize, m->m->getVertices().data(), GL_STATIC_DRAW);

    f->glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
    f->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);
    f->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(2*RgbOffset));

    f->glEnableVertexAttribArray(0);
    f->glEnableVertexAttribArray(1);
    f->glEnableVertexAttribArray(2);
    

    f->glGenBuffers(1, &m->IBO);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->IBO);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->m->getIndices().size() * sizeof(m->m->getIndices()[0]), m->m->getIndices().data(), GL_STATIC_DRAW);

}

void Renderer::destroyVAO(RenderMesh* m)
{
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

}

void Renderer::render(RenderMesh* m) {

    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();
    //qDebug() << this->vertices[0].pos;
    f->glBindVertexArray(m->VAO);
    f->glEnable(GL_PROGRAM_POINT_SIZE);
    QOpenGLContext::currentContext()->functions();

    f->glDrawElements(m->m->getRenderMode(), m->m->getIndices().size() * sizeof(m->m->getIndices()[0]), GL_UNSIGNED_INT, NULL);
    f->glBindVertexArray(0);
}


unsigned int Renderer::loadShader(const char* filename, GLenum shader_type) {
    QOpenGLExtraFunctions* f = QOpenGLContext::currentContext()->extraFunctions();

    GLuint shader_id = 0;
    std::ifstream file;
    long file_size = -1;
    GLchar* glsl_source;

    file.open(filename, std::ifstream::ate | std::ifstream::binary);
    file_size = file.tellg();

    file.clear();
    file.seekg(0);

    glsl_source = (char*)malloc(file_size + 1);

    //(long)fread(glsl_source, sizeof(char), file_size, file);
    file.read(glsl_source, file_size);

    glsl_source[file_size] = '\0';

    if (0 != (shader_id = f->glCreateShader(shader_type)))
    {
        const GLchar* src = glsl_source;
        f->glShaderSource(shader_id, 1, &src, NULL);
        f->glCompileShader(shader_id);

        GLenum ErrorCheckValue = f->glGetError();
        if (ErrorCheckValue != GL_NO_ERROR)
        {
            qDebug() << "ERROR: Could not compile the shaders: \n";
            exit(-1);
        }
    }
    else
        qDebug() << "ERROR: Could not create a shader.\n";

    file.close();
    return shader_id;

}
