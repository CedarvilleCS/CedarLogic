// This file includes opengl whether on windows, mac, or linux.
// 10/1/2016 - Tyler J. Drake

#ifndef GLWRAPPER_H
#define GLWRAPPER_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif