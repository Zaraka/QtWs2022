/***************************************************************************
** MIT License
**
** Copyright (c) 2022 Nikita Vaňků
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
******************************************************************************/

#include "SceneProviderRenderer.h"

#include "ServiceManager.h"

#include <QDebug>

struct VertexData3D {
    QVector3D vertex;
    QVector2D texCoord;
    QVector3D normal;
};
Q_DECLARE_TYPEINFO(VertexData3D, Q_PRIMITIVE_TYPE);

namespace {
    constexpr float FOV       = 45.f;
    constexpr float NearPlane = 0.1f;
    constexpr float FarPlane  = 100.f;
} // namespace

SceneProviderRenderer::SceneProviderRenderer(QObject* parent) : QObject{parent} {
    initializeOpenGLFunctions();
}

SceneProviderRenderer::~SceneProviderRenderer() {
    cleanup();
}

QOpenGLFramebufferObject* SceneProviderRenderer::scene() const {
    return m_scene;
}

void SceneProviderRenderer::init(QQuickWindow* window, const QSize& resolution) {
    m_window = window;

    setupRendering();

    if (m_scene) {
        if (m_scene->isBound()) {
            m_scene->release();
        }
        delete m_scene;
        m_scene = nullptr;
    }

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::Attachment::Depth);
    format.setMipmap(true);
    m_scene = new QOpenGLFramebufferObject(resolution, format);

    m_program->bind();
    m_matrixProjection.setToIdentity();
    m_matrixProjection.perspective(FOV,
                                   static_cast<float>(resolution.width()) / static_cast<float>(resolution.height()),
                                   NearPlane,
                                   FarPlane);
    m_program->setUniformValue("projection", m_matrixProjection);
    m_program->release();

    ServiceManager::getInstance().addRenderer(this);
}

void SceneProviderRenderer::setupRendering() {
    m_resourceService = ServiceManager::getInstance().resourceService();

    m_texture = new QOpenGLTexture(QImage(QStringLiteral(":/watermelon.jpg")));

    m_vao = new QOpenGLVertexArrayObject();

    m_program = new QOpenGLShaderProgram(this);

    m_vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    if (!m_vertexBuffer->create()) {
        qFatal("Couldn't create Vertex Buffer");
    }

    const QVector<VertexData3D> cubeVertices{
        {{-1, -1, -1}, {0, 0}, {0, 0, -1}}, // VertexCoord(3), TextureCoord(2), Normal(3)
        {{1, -1, -1}, {1, 0}, {0, 0, -1}},
        {{1, 1, -1}, {1, 1}, {0, 0, -1}},
        {{-1, 1, -1}, {0, 1}, {0, 0, -1}},
        //
        {{-1, -1, 1}, {0, 0}, {0, 0, 1}},
        {{1, -1, 1}, {1, 0}, {0, 0, 1}},
        {{1, 1, 1}, {1, 1}, {0, 0, 1}},
        {{-1, 1, 1}, {0, 1}, {0, 0, 1}},
        //
        {{-1, 1, 1}, {1, 0}, {-1, 0, 0}},
        {{-1, 1, -1}, {1, 1}, {-1, 0, 0}},
        {{-1, -1, -1}, {0, 1}, {-1, 0, 0}},
        {{-1, -1, 1}, {0, 0}, {-1, 0, 0}},
        //
        {{1, 1, 1}, {1, 0}, {1, 0, 0}},
        {{1, 1, -1}, {1, 1}, {1, 0, 0}},
        {{1, -1, -1}, {0, 1}, {1, 0, 0}},
        {{1, -1, 1}, {0, 0}, {1, 0, 0}},
        //
        {{-1, -1, -1}, {0, 1}, {0, -1, 0}},
        {{1, -1, -1}, {1, 1}, {0, -1, 0}},
        {{1, -1, 1}, {1, 0}, {0, -1, 0}},
        {{-1, -1, 1}, {0, 0}, {0, -1, 0}},
        //
        {{-1, 1, -1}, {0, 1}, {0, 1, 0}},
        {{1, 1, -1}, {1, 1}, {0, 1, 0}},
        {{1, 1, 1}, {1, 0}, {0, 1, 0}},
        {{-1, 1, 1}, {0, 0}, {0, 1, 0}},
    };

    m_vertexBuffer->bind();
    m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer->allocate(cubeVertices.constData(), cubeVertices.size() * static_cast<int>(sizeof(VertexData3D)));

    m_elementBuffer = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
    if (!m_elementBuffer->create()) {
        qFatal("Couldn't create Element Buffer");
    }
    const QVector<unsigned int> quadIndices = QVector<unsigned int>{
        0,  1,  2,  2,  3,  0,  //
        4,  5,  6,  6,  7,  4,  //
        8,  9,  10, 10, 11, 8,  //
        12, 13, 14, 14, 15, 12, //
        16, 17, 18, 18, 19, 16, //
        20, 21, 22, 22, 23, 20, //
    };
    m_elementBuffer->bind();
    m_elementBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_elementBuffer->allocate(quadIndices.constData(), quadIndices.size() * static_cast<int>(sizeof(unsigned int)));

    if (!m_program->create()) {
        qFatal("Couldn't create Shader Program");
    }
    if (!m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, QStringLiteral(":/cube.vsh"))) {
        qFatal("Vertex shader compilation failed");
    }
    if (!m_program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, QStringLiteral(":/cube.fsh"))) {
        qFatal("Fragment shader compilation failed");
    }

    if (!m_program->link()) {
        qFatal("Couldn't link shader program");
    }

    m_program->bind();

    m_program->setUniformValue("texture", 0);
    m_matrixProjection.perspective(FOV, 1.0f, NearPlane, FarPlane);

    constexpr float viewZ = -4.5;
    m_matrixView.lookAt({0, 0, viewZ}, {0, 0, 0}, {0, 1, 0});

    m_program->setUniformValue("objectColor", 1, 1, 1);
    m_program->setUniformValue("lightColor", 1, 1, 1);
    m_program->setUniformValue("lightPos", 0, 0, viewZ * 4);
    m_program->setUniformValue("viewPos", 0, 0, viewZ);

    m_program->setUniformValue("projection", m_matrixProjection);
    m_program->setUniformValue("model", m_matrixModel);
    m_program->setUniformValue("view", m_matrixView);
    m_program->setUniformValue("trans_inv_model", m_matrixModel.normalMatrix());

    QOpenGLVertexArrayObject::Binder binder(m_vao);
    m_vertexBuffer->bind();
    m_program->enableAttributeArray("verCoord");
    m_program->setAttributeBuffer("verCoord", GL_FLOAT, 0, 3, sizeof(VertexData3D));

    m_program->enableAttributeArray("texCoord");
    m_program->setAttributeBuffer("texCoord", GL_FLOAT, sizeof(QVector3D), 2, sizeof(VertexData3D));

    m_program->enableAttributeArray("normal");
    m_program->setAttributeBuffer("normal", GL_FLOAT, sizeof(QVector2D) + sizeof(QVector3D), 3, sizeof(VertexData3D));

    m_elementBuffer->bind();

    m_program->release();
}

void SceneProviderRenderer::synchronize() {
    m_program->bind();

    m_matrixModel.setToIdentity();
    QQuaternion quaternion = QQuaternion::fromEulerAngles(static_cast<float>(m_resourceService->pitch()),
                                                          static_cast<float>(m_resourceService->yaw()),
                                                          static_cast<float>(m_resourceService->roll()));
    quaternion.normalize();
    m_matrixModel.rotate(quaternion);
    m_program->setUniformValue("model", m_matrixModel);
    m_program->setUniformValue("trans_inv_model", m_matrixModel.normalMatrix());

    m_program->release();
}

void SceneProviderRenderer::render() {
    m_window->beginExternalCommands();
    // Here will go all of our custom rendering code

    if (!m_scene || !m_program) {
        return;
    }

    m_scene->bind();
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        QOpenGLVertexArrayObject::Binder binder(m_vao);

        glViewport(0, 0, m_scene->size().width(), m_scene->size().height());

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_program->bind();
        m_texture->bind();

        glDrawElements(GL_TRIANGLES, m_elementBuffer->size(), GL_UNSIGNED_INT, nullptr);

        m_program->release();
    }
    m_scene->release();

    // it is important that we left the OpenGL state unchanged
    m_window->endExternalCommands();
}

void SceneProviderRenderer::cleanup() {
    if (m_program) {
        m_program->release();
        m_program->deleteLater();
        m_program = nullptr;
    }

    if (m_texture) {
        if (m_texture->isBound()) {
            m_texture->release();
        }
        if (m_texture->isCreated()) {
            m_texture->destroy();
        }
        delete m_texture;
        m_texture = nullptr;
    }

    if (m_vao) {
        if (m_vao->isCreated()) {
            m_vao->release();
            m_vao->destroy();
        }
        delete m_vao;
        m_vao = nullptr;
    }

    if (m_elementBuffer) {
        if (m_elementBuffer->isCreated()) {
            m_elementBuffer->release();
            m_elementBuffer->destroy();
        }
        delete m_elementBuffer;
        m_elementBuffer = nullptr;
    }

    if (m_vertexBuffer) {
        if (m_vertexBuffer->isCreated()) {
            m_vertexBuffer->release();
            m_vertexBuffer->destroy();
        }
        delete m_vertexBuffer;
        m_vertexBuffer = nullptr;
    }
}
