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

#include <QGuiApplication>
#include <QSurfaceFormat>

#include "egl-window.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QSurfaceFormat format;
  format.setSamples(24);
  format.setRenderableType(QSurfaceFormat::OpenGLES);

  EGLWindow window;
  window.setFormat(format);
  window.showFullScreen();
  window.setRendering(true);

  return app.exec();
}
