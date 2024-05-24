
#include <algorithm>
#ifdef _MSC_VER
#include "freeglut_std.h"
#else
#include <freeglut.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
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

struct Color {
  float r, g, b;
};

struct point {
  float x, y;
  Color c;
};

struct triangle {
  point points[3];
};

GLenum glShadeType = GL_SMOOTH;

float clickx = 1;
float clicky = 1;

Vertex camera_loc = {0.01, 0.02, 0.1};
Vertex camera_lookat = {0, 0, 0};

bool checked = false;
std::vector<point> to_draw;
std::size_t draw_index = 0;

#define PI 3.14159265358979323846

double deg2deg(double deg) { return deg; }

int size = 10;
const auto border = 0.05;

#define MAX_POINT_COUNT 100

triangle triangles[MAX_POINT_COUNT];
point points[MAX_POINT_COUNT];
int triangle_count = 0;
int click_index = 0;

std::vector<point> line_points;
std::vector<point> fill_points;

int fill_index = 0;

bool finished = false;

int refreshMills = 100;

#define PRECISION 0.001
bool feq(float a, float b) { return std::abs(a - b) < PRECISION; }

void Timer(int value) {
  glutPostRedisplay();
  glutTimerFunc(refreshMills, Timer, 0);
  if (draw_index < to_draw.size()) {
    draw_index++;
  }

  if (fill_index < fill_points.size()) {
    fill_index++;
  }
}

void ScreenToView(int ix, int iy, double *ox, double *oy) {
  const auto half_width = 400 * (1.0 - border);
  const auto unit = (half_width * 2) / size;
  double tx = ((ix - half_width) / unit) - 0.25;
  double ty = -((iy - half_width) / unit) + 0.25;

  tx = (std::floor(tx) * 2 + 1.0) / size;
  ty = (std::floor(ty) * 2 + 1.0) / size;

  *ox = tx + 1.0;
  *oy = ty + 1.0;
}

Color randColor() {
  return Color{
      float(rand()) / RAND_MAX,
      float(rand()) / RAND_MAX,
      float(rand()) / RAND_MAX,
  };
}

void MouseHandler(int button, int state, int _x, int _y) {
  if (button != GLUT_LEFT_BUTTON || state == GLUT_DOWN) {
    return;
  }
  // printf("%d\n", click_index);

  double tx, ty;
  ScreenToView(_x, _y, &tx, &ty);

  if (tx <= 2.0 && ty <= 2.0) {
    if (click_index > 1 && feq(points[0].x, tx) && feq(points[0].y, ty)) {
      finished = true;
      return;
    }

    points[click_index].x = tx;
    points[click_index].y = ty;
    points[click_index].c = randColor();
    click_index++;
  }
}

void SpecialKeyHandler(int key, int x, int y) {}

void NormalKeyHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'r':
    click_index = 0;
    finished = false;
    line_points.clear();
    fill_points.clear();
    fill_index = 0;
    break;
  case 'x':
    exit(0);
  }
}

Model m;

void Size(int value) {
  size = value;
  triangle_count = 0;
  click_index = 0;
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
  glutTimerFunc(0, Timer, 0);
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

float lerp(float a, float b, float t) { return a + (b - a) * t; }

float lerp2d(point a, point b, point t) {
  const auto total =
      std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));

  const auto delta =
      std::sqrt((t.x - a.x) * (t.x - a.x) + (t.y - a.y) * (t.y - a.y));

  return delta / total;
}

Color colorLerp(Color a, Color b, float t) {
  return Color{
      lerp(a.r, b.r, t),
      lerp(a.g, b.g, t),
      lerp(a.b, b.b, t),
  };
}

void DrawLine(point p1, point p2, std::vector<point> &points) {
  const auto x1 = p1.x;
  const auto y1 = p1.y;
  const auto x2 = p2.x;
  const auto y2 = p2.y;

  const auto dx = abs(x2 - x1);
  const auto dy = abs(y2 - y1);

  const auto step = 2.0 / size;

  const auto sign_x = (x2 - x1) > 0 ? step : -step;
  const auto sign_y = (y2 - y1) > 0 ? step : -step;

  auto x = x1;
  auto y = y1;

  float p;

  if (dx > dy) {
    p = 2 * dy - dx;
    while (!feq(x, x2)) {
      const auto t = lerp2d(p1, p2, point{x, y});
      const auto c = colorLerp(p1.c, p2.c, t);
      points.push_back(point{x, y, c});
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
    while (!feq(y, y2)) {
      const auto t = lerp2d(p1, p2, point{x, y});
      const auto c = colorLerp(p1.c, p2.c, t);
      points.push_back(point{x, y, c});
      y += sign_y;
      if (p < 0)
        p += 2 * dx;
      else {
        p += 2 * (dx - dy);
        x += sign_x;
      }
    }
  }
}

bool isLeft(point p1, point p2, point p) {
  return (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) > 0.0;
}

void RenderScene() {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0 - border, 2 + border, 0 - border, 2 + border, -10, 10);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);

  glLoadIdentity();
  gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
  glShadeModel(glShadeType);

  glBegin(GL_LINES);
  for (double i = 0.0; i <= 2.0 + 0.01; i += 2.0 / size) {
    glVertex3f(i, 0, 0);
    glVertex3f(i, 2, 0);
  }
  for (double i = 0.0; i <= 2.0 + 0.01; i += 2.0 / size) {
    glVertex3f(0, i, 0);
    glVertex3f(2, i, 0);
  }
  glEnd();

  glPointSize((800.0 * (1.0 - border)) / size);
  glBegin(GL_POINTS);
  for (int i = 0; i < click_index; ++i) {
    glColor3f(points[i].c.r, points[i].c.g, points[i].c.b);
    glVertex3f(points[i].x, points[i].y, 0);
  }
  glEnd();
  glColor3f(1.0, 1.0, 1.0);

  // for (int i = 0; i < click_index; ++i) {
  //   printf("P%d: %f %f\n", i + 1, points[i].x, points[i].y);
  // }

  if (finished) {
    for (auto i = 0; i < click_index; ++i) {
      const auto next = (i + 1) % click_index;
      DrawLine(points[i], points[next], line_points);
    }

    for (float i = 1. / size; i < 2.1; i += 2. / size) {
      std::vector<point> f;
      for (float j = 1. / size; j < 2.1; j += 2. / size) {
        for (const auto &p : line_points) {
          if (feq(p.x, j) && feq(p.y, i)) {
            f.push_back(p);
          }
        }
      }
      if (f.size() >= 2) {
        DrawLine(f[0], f[f.size() - 1], fill_points);
      }
    }

    finished = false;
  }

  glBegin(GL_POINTS);
  for (const auto &p : line_points) {
    glColor3f(p.c.r, p.c.g, p.c.b);
    glVertex3f(p.x, p.y, 0);
  }

  for (int i = 0; i < fill_index; ++i) {
    glColor3f(fill_points[i].c.r, fill_points[i].c.g, fill_points[i].c.b);
    glVertex3f(fill_points[i].x, fill_points[i].y, 0);
  }
  glEnd();
  glColor3f(1, 1, 1);

  glutSwapBuffers();
}
