#pragma once

#include <QOpenGLWidget>
#include <vector>
#include <QKeyEvent>
#include <QOpenGLExtraFunctions>
#include <QOpenGLFramebufferObject>

#include <glm/glm.hpp>
#include <iostream>

#include <algomole/am.h>
#include <algomole/gfx/mesh.h>
#include "camera.h"
#include "trackball.h"



class Renderer : public QOpenGLWidget {

public:

    typedef struct {
        unsigned int light_color_pointer;
        unsigned int light_power_pointer;
        unsigned int light_vector_pointer;
        unsigned int material_diffuse;
        unsigned int material_ambient;
        unsigned int material_specular;
        unsigned int material_shininess;
    } LightShaderUniform;

    typedef struct {
        glm::vec4 vector;
        glm::vec3 color;
        float power;
    } Light;

    typedef struct {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    } Material;

    typedef struct RenderMesh {
        am::gfx::Mesh* m;
        unsigned int VAO;
        unsigned int VBO;
        unsigned int IBO;
        RenderMesh(am::gfx::Mesh* mesh) {
            m = mesh;
        };
    } RenderMesh;


    Renderer(QWidget* parent = nullptr);
    ~Renderer();

    void addMesh(am::gfx::Mesh* m);

    void saveToImage(const QString& filepath) {
        QSize widgetSize = size();

        // Create an FBO
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        QOpenGLFramebufferObject fbo(widgetSize, format);

        // Bind the FBO for rendering
        fbo.bind();

        // Your OpenGL rendering code here
        // Make sure to use the same OpenGL context of the QOpenGLWidget for rendering.

        // After rendering, ensure that all OpenGL commands are finished
        QOpenGLContext* currentContext = QOpenGLContext::currentContext();
        Q_ASSERT(currentContext);
        QOpenGLFunctions* f = currentContext->functions();
        f->glFlush();
        f->glFinish();

        // Read the contents of the FBO
        QImage image = fbo.toImage();
        // The image may be upside down, so you might need to flip it if necessary
        image = image.mirrored(false, true);

        // Save the image to a file
        image.save(filepath);

        // Unbind the FBO and release its resources
        fbo.release();
    }

protected:
    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

    void keyPressEvent(QKeyEvent* keyEvent);

    void mousePressEvent(QMouseEvent* event);

    void mouseReleaseEvent(QMouseEvent* event);

    void mouseMoveEvent(QMouseEvent* event);

    void wheelEvent(QWheelEvent* event);

public slots:
    void setLightY(int newValue) {
        light.vector.y = newValue;
        repaint();
    }

    void setLightX(int newValue) {
        light.vector.x = newValue;
        repaint();
    }

    void setLightZ(int newValue) {
        light.vector.z = newValue;
        repaint();
    }

    void setLightPower(int newValue) {
        light.power = newValue;
        repaint();
    }

    void rotateScene();

    void update() {
        repaint();
    }

    void setMaterialColor(float r, float g, float b) {
        material.diffuse = { r,g,b };
    }

    void clear() {
        meshes.clear();
        repaint();
    }


private:

    void createShaders(void);
    void destroyShaders(void);


    void createVAO(RenderMesh* m);
    void destroyVAO(RenderMesh* m);
    bool hasVAO(RenderMesh* m);
    void render(RenderMesh* m);

    unsigned int loadShader(const char* filename, GLenum shader_type);

    std::vector<RenderMesh*> meshes;
    GLuint matView, matProj, matModel;
    glm::mat4 view, projection, model;

    GLuint loc_view_pos;

    GLuint programId;
    GLuint vertexShaderId;
    GLuint fragmentShaderId;

    Camera camera;
    Trackball tb;

    struct Perspective {
        float fovY, aspect, near_plane, far_plane;
    } perspectiveSetup;

    Light light;
    LightShaderUniform light_unif;
    Material material;
};