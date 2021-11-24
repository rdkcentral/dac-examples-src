bin_PROGRAMS = essos-sample essos-egl

essos_sample_SOURCES = essos-sample.cpp
essos_sample_CXXFLAGS = ${AM_CXXFLAGS}
essos_sample_CXXFLAGS += ${EGL_CFLAGS}
essos_sample_LDFLAGS = $(AM_FLAGS) -lessos -lGLESv2

essos_egl_SOURCES = essos-egl.cpp
essos_egl_CXXFLAGS = ${AM_CXXFLAGS}
essos_egl_CXXFLAGS += ${EGL_CFLAGS}
essos_egl_LDFLAGS = $(AM_FLAGS) -lessos -lGLESv2
