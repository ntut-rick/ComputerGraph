
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

#include "loader.hpp"

void ChangeSize(int, int);
void RenderScene();
void DisplayTypeMenuCallback(int);
void ColorModeMenuCallback(int);
void ModelSelectMenuCallback(int);

struct point {
  float x, y;
};

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

#define POINT_COUNT 2

point points[POINT_COUNT];
int click_index = 0;

void ScreenToView(int ix, int iy, double *ox, double *oy) {
  const auto half_width = 400 * (1.0 - border);
  const auto unit = (half_width * 2) / size;
  double tx = ((ix - half_width) / unit) - 0.25;
  double ty = -((iy - half_width) / unit) + 0.25;

  tx = (std::floor(tx) * 2 + 1.0) / size;
  ty = (std::floor(ty) * 2 + 1.0) / size;

  *ox = tx;
  *oy = ty;
}

void MouseHandler(int button, int state, int _x, int _y) {
  if (button != GLUT_LEFT_BUTTON || state == GLUT_DOWN) {
    return;
  }
  printf("%d\n", click_index);

  if (click_index >= 4) {
    return;
  }

  double tx, ty;
  ScreenToView(_x, _y, &tx, &ty);

  if (abs(tx) < 1.0 && abs(ty) < 1.0) {
    points[click_index].x = tx;
    points[click_index].y = ty;
    click_index++;
  }
}

void SpecialKeyHandler(int key, int x, int y) {}

void NormalKeyHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'r':
    click_index = 0;
    break;
  case 'x':
    exit(0);
  }
}

struct Color {
  float r, g, b;
};

Model m;

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

double lerp(double a, double b, double t) { return a + (b - a) * t; }

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

  glPointSize(800.0 / (2 * border + size));
  glBegin(GL_POINTS);
  for (int i = 0; i < click_index; ++i) {
    glVertex3f(points[i].x, points[i].y, 0);
  }
  glEnd();

  if (click_index == POINT_COUNT) {
    for (int i = 0; i < click_index; ++i) {
      glColor3f(1.0, 0.0, 0.0);
      glBegin(GL_LINES);
      const auto next = (i + 1) % click_index;
      glVertex3f(points[i].x, points[i].y, 0);
      glVertex3f(points[next].x, points[next].y, 0);
      glEnd();
      glColor3f(1.0, 1.0, 1.0);

      printf("%d\n", i);
      printf("%f %f\n", points[i].x, points[i].y);
      printf("%f %f\n", points[next].x, points[next].y);
      const point begin = {points[i].x, points[i].y};
      const point end = {points[next].x, points[next].y};

      const auto unit = 2.0 / size;
      const auto x_step = (end.x - begin.x) / size;
      const auto y_step = (end.y - begin.y) / size;
      printf("steps: %f %f\n", unit, y_step);

      printf("begin y: %f end y: %f\n", begin.y, end.y);
      glBegin(GL_POINTS);
      point p = begin;
      while (p.x < end.x) {
        const auto t = (p.x - begin.x) / (end.x - begin.x);
        const auto y = lerp(begin.y, end.y, t);
        glVertex3f(p.x, p.y, 0.0);

        p.x += unit;
      }
      glEnd();
      printf("\n");
    }
  }

  glutSwapBuffers();
}
