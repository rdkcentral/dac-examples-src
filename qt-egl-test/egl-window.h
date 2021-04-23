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

#pragma once

#include <QOpenGLExtraFunctions>
#include <QOpenGLPaintDevice>
#include <QOpenGLWindow>

class EGLWindow : public QWindow, protected QOpenGLFunctions {
  Q_OBJECT
 public:
  explicit EGLWindow(QWindow *parent = nullptr);
  ~EGLWindow();

  virtual void render(QPainter *painter);
  virtual void render();

  virtual void initialize();

  void setRendering(const bool rendering);

 public slots:
  void renderLater();
  void renderNow();

 protected:
  bool event(QEvent *event) override;

  void exposeEvent(QExposeEvent *event) override;

 private:
  bool rendering;
  int color;

  QOpenGLContext *context;
  QOpenGLPaintDevice *device;
};
