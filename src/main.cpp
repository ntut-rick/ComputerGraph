
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

#define POINT_COUNT 4

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
  // printf("%d\n", click_index);

  if (click_index >= POINT_COUNT) {
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

void Size(int value) {
  size = value;
  click_index = 0;
}

void print_info(point p1, point p2) {
  const auto dx = p2.x - p1.x;
  const auto dy = p2.y - p1.y;

  const auto s = abs(dy / dx);

  if (dx > 0 && dy > 0 && s <= 1.0) {
    printf("region 1\n");
    return;
  }
  if (dx > 0 && dy > 0 && s > 1.0) {
    printf("region 2\n");
    return;
  }
  if (dx <= 0 && dy > 0 && s > 1.0) {
    printf("region 3\n");
    return;
  }
  if (dx <= 0 && dy > 0 && s <= 1.0) {
    printf("region 4\n");
    return;
  }
  if (dx <= 0 && dy <= 0 && s <= 1.0) {
    printf("region 5\n");
    return;
  }
  if (dx <= 0 && dy <= 0 && s > 1.0) {
    printf("region 6\n");
    return;
  }
  if (dx > 0 && dy <= 0 && s > 1.0) {
    printf("region 7\n");
    return;
  }
  if (dx > 0 && dy <= 0 && s <= 1.0) {
    printf("region 8\n");
    return;
  }
}

int main(int argc, char **argv) {
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

  glColor3f(1.0, 0.0, 0.0);
  glPointSize((800.0 * (1.0 - border)) / size);
  glBegin(GL_POINTS);
  for (int i = 0; i < click_index; ++i) {
    glVertex3f(points[i].x, points[i].y, 0);
  }
  glEnd();
  glColor3f(1.0, 1.0, 1.0);

  if (click_index == POINT_COUNT) {
    for (int i = 0; i < click_index; ++i) {
      printf("P%d: %f %f\n", i + 1, points[i].x, points[i].y);
    }

    for (int i = 0; i < click_index; ++i) {
      const auto next = (i + 1) % click_index;

      printf("P%d-P%d: ", i + 1, next + 1);
      print_info(points[i], points[next]);

      const point begin = {points[i].x, points[i].y};
      const point end = {points[next].x, points[next].y};

      const auto x1 = begin.x;
      const auto y1 = begin.y;
      const auto x2 = end.x;
      const auto y2 = end.y;

      const auto dx = abs(x2 - x1);
      const auto dy = abs(y2 - y1);

      const auto step = 2.0 / size;

      const auto sign_x = (x2 - x1) > 0 ? step : -step;
      const auto sign_y = (y2 - y1) > 0 ? step : -step;

      auto x = x1;
      auto y = y1;

      float p;

      glBegin(GL_POINTS);
      if (dx > dy) {
        p = 2 * dy - dx;
        while (std::abs(x - x2) > 0.01) {
          glVertex3f(x, y, 0);
          x += sign_x;
          if (p < 0)
            p += 2 * dy;
          else {
            p += 2 * (dy - dx);
            y += sign_y;
          }
        }
      } else {
        p = 2 * dx - dy;
        while (std::abs(y - y2) > 0.01) {
          glVertex3f(x, y, 0);
          y += sign_y;
          if (p < 0)
            p += 2 * dx;
          else {
            p += 2 * (dx - dy);
            x += sign_x;
          }
        }
      }
      glEnd();
    }
  }

  glutSwapBuffers();
}
