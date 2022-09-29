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

#pragma once

#include "ResourceService.h"

#include <QMatrix4x4>
#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QQuickWindow>

//!
//! \brief The BackgroundRenderRenderer class
//! This class does the actual rendering directed by SceneProvider on the render thread
//!
class SceneProviderRenderer : public QObject, public QOpenGLFunctions {
    Q_OBJECT
public:
    explicit SceneProviderRenderer(QObject* parent = nullptr);
    ~SceneProviderRenderer();

    QOpenGLFramebufferObject* scene() const;

public slots:
    void init(QQuickWindow* window, const QSize& resolution);
    void setupRendering();
    void synchronize();
    void render();
    void cleanup();

private:
    // Here we will store all the rendered framebuffers, for this demo, we will just store one
    // But BackgroundRenderRender can be easily extended to render many scenes
    QOpenGLFramebufferObject* m_scene = nullptr;

    // Necessary OpenGL objects for rendering scene
    QOpenGLVertexArrayObject* m_vao = nullptr;
    QOpenGLBuffer* m_vertexBuffer   = nullptr;
    QOpenGLBuffer* m_elementBuffer  = nullptr;
    QOpenGLShaderProgram* m_program = nullptr;
    QOpenGLTexture* m_texture       = nullptr;
    QMatrix4x4 m_matrixModel;
    QMatrix4x4 m_matrixView;
    QMatrix4x4 m_matrixProjection;
    bool m_init = false;

    QQuickWindow* m_window = nullptr;

    ResourceService* m_resourceService = nullptr;
};
