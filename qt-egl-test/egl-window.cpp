//
// Copyright (C) 2020  Damian Wrobel <dwrobel@ertelnet.rybnik.pl>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#include "egl-window.h"

#include <QPainter>

EGLWindow::EGLWindow(QWindow *parent)
    : QWindow(parent),
      rendering(false),
      color(0),
      context(nullptr),
      device(nullptr) {
  setSurfaceType(QWindow::OpenGLSurface);
}

EGLWindow::~EGLWindow() {}

// This code is based on Qt OpenGL example code which is:
// Copyright (C) 2018 The Qt Company Ltd.
// Licensed under the BSD-3 license.
void EGLWindow::render(QPainter *painter) {
  Q_UNUSED(painter);
  color = (color + 1) % 256;
  const float c = color / 255.0;
  glClearColor(0.0, c, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EGLWindow::initialize() {}

void EGLWindow::render() {
  if (!device) device = new QOpenGLPaintDevice;

  device->setSize(size() * devicePixelRatio());
  device->setDevicePixelRatio(devicePixelRatio());

  QPainter painter(device);
  render(&painter);
}

void EGLWindow::renderLater() { requestUpdate(); }

bool EGLWindow::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::UpdateRequest:
      renderNow();
      return true;
    default:
      return QWindow::event(event);
  }
}

void EGLWindow::exposeEvent(QExposeEvent *event) {
  Q_UNUSED(event);

  if (isExposed()) renderNow();
}

void EGLWindow::renderNow() {
  if (!isExposed()) return;

  bool needsInitialize = false;

  if (!context) {
    context = new QOpenGLContext(this);
    context->setFormat(requestedFormat());
    context->create();

    needsInitialize = true;
  }

  context->makeCurrent(this);

  if (needsInitialize) {
    initializeOpenGLFunctions();
    initialize();
  }

  render();

  context->swapBuffers(this);

  if (rendering) renderLater();
}

void EGLWindow::setRendering(const bool rendering) {
  if ((this->rendering = rendering)) renderLater();
}
