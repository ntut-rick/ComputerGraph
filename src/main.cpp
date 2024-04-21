
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

ColorMode cm = ColorMode::Single;

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

float xscale = 1;
float yscale = 1;
float zscale = 1;

float ydelta = .2f;
float xdelta = .2f;

float clickx = 1;
float clicky = 1;

void Loop() {
  // yangle += ydelta;
  // xangle += xdelta;
  glutPostRedisplay();
}

#define PI 3.14159265358979323846

double deg2deg(double deg) { return deg; }

void MouseHandler(int button, int state, int x, int y) {
  if (button != GLUT_LEFT_BUTTON) {
    return;
  }

  clickx = (float)x / 400 - 1;
  clicky = -(float)y / 400 + 1;
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
    // clang-format off
  case 'w': xangle += 0.05f; break;
  case 'q': xangle -= 0.05f; break;
  case 's': yangle += 0.05f; break;
  case 'a': yangle -= 0.05f; break;
  case 'x': zangle += 0.05f; break;
  case 'z': zangle -= 0.05f; break;

  case 'r': xtrans += 0.1f; break;
  case 'e': xtrans -= 0.1f; break;
  case 'f': ytrans += 0.1f; break;
  case 'd': ytrans -= 0.1f; break;
  case 'v': ztrans += 0.1f; break;
  case 'c': ztrans -= 0.1f; break;

  case 'y': xscale += 0.1f; break;
  case 't': xscale -= 0.1f; break;
  case 'h': yscale += 0.1f; break;
  case 'g': yscale -= 0.1f; break;
  case 'n': zscale += 0.1f; break;
  case 'b': zscale -= 0.1f; break;
  // clang-format on
  case '1':
    xangle = 0;
    yangle = 45;
    zangle = 0;
    xtrans = 0;
    ytrans = 0;
    ztrans = 0;
    xscale = 1;
    yscale = 1;
    zscale = 1;
    break;
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

  auto dtMenu = glutCreateMenu(DisplayTypeMenuCallback);
  glutAddMenuEntry("Point", DisplayType::Point);
  glutAddMenuEntry("Line", DisplayType::Line);
  glutAddMenuEntry("Face", DisplayType::Face);

  auto colorMenu = glutCreateMenu(ColorModeMenuCallback);
  glutAddMenuEntry("Single", ColorMode::Single);
  glutAddMenuEntry("Random", ColorMode::Random);

  auto modelMenu = glutCreateMenu(ModelSelectMenuCallback);
  glutAddMenuEntry("Gourd", ActiveModel::Gourd);
  glutAddMenuEntry("Octahedron", ActiveModel::Octahedron);
  glutAddMenuEntry("Teapot", ActiveModel::Teapot);
  glutAddMenuEntry("Teddy", ActiveModel::Teddy);

  glutCreateMenu(nullptr);
  glutAddSubMenu("Display Type", dtMenu);
  glutAddSubMenu("Color Mode", colorMenu);
  glutAddSubMenu("Active Model", modelMenu);

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

void DrawPoint() {
  glPointSize(5);
  glBegin(GL_POINTS);
  for (const auto &v : m.vertices) {
    Color c = GetColor();
    glColor3f(c.r, c.g, c.b);

    glVertex3f(v.x, v.y, v.z);
  }
  glEnd();
}

void DrawLine() {
  glBegin(GL_LINES);

  for (const auto &f : m.faces) {
    Color c = GetColor();
    glColor3f(c.r, c.g, c.b);

    const auto p0 = m.vertices[f.p0 - 1];
    const auto p1 = m.vertices[f.p1 - 1];
    const auto p2 = m.vertices[f.p2 - 1];

    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);

    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p2.x, p2.y, p2.z);

    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
  }
  glEnd();
}

void DrawFace() {
  glBegin(GL_TRIANGLES);

  for (const auto &f : m.faces) {
    Color c = GetColor();
    glColor3f(c.r, c.g, c.b);

    const auto p0 = m.vertices[f.p0 - 1];
    glVertex3f(p0.x, p0.y, p0.z);

    const auto p1 = m.vertices[f.p1 - 1];
    glVertex3f(p1.x, p1.y, p1.z);

    const auto p2 = m.vertices[f.p2 - 1];
    glVertex3f(p2.x, p2.y, p2.z);
  }
  glEnd();
}

void RenderScene() {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-max_vertex, max_vertex, -max_vertex, max_vertex, -max_vertex,
          max_vertex);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);

  glLoadIdentity();
  gluLookAt(0.01, 0.02, 0.1, 0, 0, 0, 0, 1, 0);
  glShadeModel(glShadeType);

  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex3f(100, 0, 0);
  glVertex3f(-100, 0, 0);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0, -100, 0);
  glVertex3f(0, 100, 0);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex3f(0, 0, -100);
  glVertex3f(0, 0, 100);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(1.0f, 0.0f, 1.0f);
  glVertex3f(-10 * clickx, -10 * clicky, 0.0f);
  glVertex3f(10 * clickx, 10 * clicky, 0.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnd();

  double trans[] = {
      1,      0,      0,      0, //
      0,      1,      0,      0, //
      0,      0,      1,      0, //
      xtrans, ytrans, ztrans, 1, //
  };

  double l = sqrt(clickx * clickx + clicky * clicky);
  double ux = clickx / l;
  double uy = clicky / l;

  double scale[] = {
      xscale, 0, 0, 0, 0, yscale, 0, 0, 0, 0, zscale, 0, 0, 0, 0, 1,
  };
  /* glRotatef(yangle, 0, 1, 0); */
  glMultMatrixd(trans);
  rot_z(asin(ux));
  rot_y(xangle);
  rot_z(-asin(ux));
  glMultMatrixd(scale);

  srand(69420);
  switch (dt) {
  case DisplayType::Point:
    DrawPoint();
    break;
  case DisplayType::Line:
    DrawLine();
    break;
  case DisplayType::Face:
    DrawFace();
    break;
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
