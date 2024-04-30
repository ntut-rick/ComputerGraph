
#ifdef _MSC_VER
#include "freeglut_std.h"
#else
#include <freeglut.h>
#endif

#include <GL/gl.h>
#include <math.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "pryamid.h"
#include "stb_image.h"

#include "global.h"
#include "loader.hpp"

void ChangeSize(int, int);
void RenderScene();
void DisplayTypeMenuCallback(int);
void ColorModeMenuCallback(int);
void ModelSelectMenuCallback(int);

enum DisplayType {
  Point,
  Line,
  Face,
};

DisplayType dt = DisplayType::Face;

enum ColorMode {
  Single,
  Random,
};

ColorMode cm = ColorMode::Random;

enum ActiveModel {
  Gourd,
  Octahedron,
  Teapot,
  Teddy,
};

GLenum glShadeType = GL_SMOOTH;
float xtrans = 0;
float ytrans = 0;
float ztrans = 0;

float xangle = 0;
float yangle = 0;
float zangle = 0;

float aangle = 0;

float xscale = 1;
float yscale = 1;
float zscale = 1;

float ydelta = .2f;
float xdelta = .2f;

float clickx = 1;
float clicky = 1;

Vertex camera_loc = {0.01, 0.02, 0.1};
Vertex camera_lookat = {0, 0, 0};

void Loop() {
  // yangle += ydelta;
  // xangle += xdelta;
  glutPostRedisplay();
}

#define PI 3.14159265358979323846

double deg2deg(double deg) { return deg; }

int size = 10;
const auto border = 0.05;

bool active = false;
float x = 0;
float y = 0;

void MouseHandler(int button, int state, int _x, int _y) {
  const auto half_width = 400 * (1.0 - border);
  if (button != GLUT_LEFT_BUTTON) {
    return;
  }

  active = true;
  const auto unit = (half_width * 2) / size;
  x = ((_x - half_width) / unit) - 0.25;
  y = -((_y - half_width) / unit) + 0.25;

  x = std::floor(x) * 2 / size;
  y = std::floor(y) * 2 / size;

  x += 1.0 / size;
  y += 1.0 / size;
}

void SpecialKeyHandler(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_RIGHT:
    yangle += 10.0f;
    break;
  case GLUT_KEY_LEFT:
    yangle -= 10.0f;
    break;
  case GLUT_KEY_UP:
    xangle += 10.0f;
    break;
  case GLUT_KEY_DOWN:
    xangle -= 10.0f;
    break;
  }
}

void NormalKeyHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'x':
    exit(0);
  }
}

// clang-format off
void rot_x(float theta) {
  double rot_x[] = {
      1, 0, 0, 0, //
      0, cos(deg2deg(theta)), sin(deg2deg(theta)), 0, //
      0, -sin(deg2deg(theta)), cos(deg2deg(theta)), 0, //
      0, 0, 0, 1, //
  };
  glMultMatrixd(rot_x);
}
void rot_y(float theta) {
  double rot_y[] = {
      cos(deg2deg(theta)), 0, -sin(deg2deg(theta)), 0, //
      0, 1, 0, 0, //
      sin(deg2deg(theta)), 0, cos(deg2deg(theta)), 0, //
      0, 0, 0, 1, //
  };
  glMultMatrixd(rot_y);
}
void rot_z(float theta) {
  double rot_z[] = {
      cos(deg2deg(theta)), -sin(deg2deg(theta)), 0, 0, //
      sin(deg2deg(theta)), cos(deg2deg(theta)), 0, 0, //
      0, 0, 1, 0, //
      0, 0, 0, 1, //
  };
  glMultMatrixd(rot_z);
}
// clang-format on

struct Color {
  float r, g, b;
};

Model m;

Color RandomColor() {
  const auto r = [] {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  };

  return Color{
      r(),
      r(),
      r(),
  };
}

Color GetColor() {
  if (cm == ColorMode::Single) {
    return Color{
        1,
        1,
        1,
    };
  } else {
    return RandomColor();
  }
}

void Size(int value) { size = value; }

int main(int argc, char **argv) {
  int err;
  if (argc == 2) {
    err = load_model(argv[1], m);
  } else {
    printf("loading default model\n");
    err = load_model("../assets/octahedron.obj", m);
  }

  if (err != 0) {
    return 1;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutInitWindowPosition(600, 80);
  glutCreateWindow("Simple Triangle");

  // glutCreateMenu(DisplayTypeMenuCallback);
  // glutAddMenuEntry("Point", DisplayType::Point);
  // glutAddMenuEntry("Line", DisplayType::Line);
  // glutAddMenuEntry("Face", DisplayType::Face);

  glutCreateMenu(Size);
  glutAddMenuEntry("10", 10);
  glutAddMenuEntry("15", 15);
  glutAddMenuEntry("20", 20);

  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutSpecialFunc(SpecialKeyHandler);
  glutKeyboardFunc(NormalKeyHandler);
  glutMouseFunc(MouseHandler);

  glutReshapeFunc(ChangeSize);
  glutDisplayFunc(RenderScene);
  glutIdleFunc(Loop);
  glutMainLoop(); // http://www.programmer-club.com.tw/ShowSameTitleN/opengl/2288.html
  return 0;
}
void ChangeSize(int w, int h) {
  // printf("Window Size= %d X %d\n", w, h);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-10, 10, -10, 10, -10, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void RenderScene() {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1 - border, 1 + border, -1 - border, 1 + border, -10, 10);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);

  glLoadIdentity();
  gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
  glShadeModel(glShadeType);

  glBegin(GL_LINES);
  for (float i = -1.0; i <= 1.0; i += 2.0 / size) {
    glVertex3f(i, -1, 0);
    glVertex3f(i, 1, 0);
  }
  for (float i = -1.0; i <= 1.0; i += 2.0 / size) {
    glVertex3f(-1, i, 0);
    glVertex3f(1, i, 0);
  }
  glEnd();

  if (active) {
    glPointSize(800.0 / (2 * border + size));
    glBegin(GL_POINTS);
    glVertex3f(x, y, 0);
    glEnd();
  }

  glutSwapBuffers();
}

void DisplayTypeMenuCallback(int value) {
  dt = static_cast<DisplayType>(value);
  glutPostRedisplay();
}

void ColorModeMenuCallback(int value) {
  cm = static_cast<ColorMode>(value);
  glutPostRedisplay();
}

void ModelSelectMenuCallback(int value) {
  switch (static_cast<ActiveModel>(value)) {
  case Gourd:
    load_model("../assets/gourd.obj", m);
    break;
  case Octahedron:
    load_model("../assets/octahedron.obj", m);
    break;
  case Teapot:
    load_model("../assets/teapot.obj", m);
    break;
  case Teddy:
    load_model("../assets/teddy.obj", m);
    break;
  }
}
