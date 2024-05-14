
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

struct point {
  float x, y;
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

int refreshMills = 100;

void Timer(int value) {
  glutPostRedisplay();
  glutTimerFunc(refreshMills, Timer, 0);
  if (draw_index < to_draw.size()) {
    draw_index++;
  }
}

#define PI 3.14159265358979323846

double deg2deg(double deg) { return deg; }

int size = 10;
const auto border = 0.05;

#define MAX_POINT_COUNT 100

triangle triangles[MAX_POINT_COUNT];
int triangle_count = 0;
int click_index = 0;

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

void MouseHandler(int button, int state, int _x, int _y) {
  if (button != GLUT_LEFT_BUTTON || state == GLUT_DOWN) {
    return;
  }
  // printf("%d\n", click_index);

  double tx, ty;
  ScreenToView(_x, _y, &tx, &ty);

  if (tx <= 2.0 && ty <= 2.0) {
    triangles[triangle_count].points[click_index].x = tx;
    triangles[triangle_count].points[click_index].y = ty;
    click_index++;
  }

  if (click_index == 3) {
    triangle_count++;
    click_index = 0;
  }
}

void SpecialKeyHandler(int key, int x, int y) {}

void NormalKeyHandler(unsigned char key, int x, int y) {
  switch (key) {
  case 'r':
    triangle_count = 0;
    click_index = 0;
    checked = false;
    to_draw.clear();
    draw_index = 0;
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
  triangle_count = 0;
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

void DrawLine(point p1, point p2) {
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

  glColor3f(1.0, 0.0, 0.0);
  glPointSize((800.0 * (1.0 - border)) / size);
  glBegin(GL_POINTS);
  for (int i = 0; i < triangle_count * 3 + click_index; ++i) {
    glVertex3f(triangles[i / 3].points[i % 3].x,
               triangles[i / 3].points[i % 3].y, 0);
  }
  glEnd();
  glColor3f(1.0, 1.0, 1.0);

  // for (int i = 0; i < click_index; ++i) {
  //   printf("P%d: %f %f\n", i + 1, points[i].x, points[i].y);
  // }

  for (int i = 0; i < triangle_count; ++i) {
    const auto p0 = triangles[i].points[0];
    const auto p1 = triangles[i].points[1];
    const auto p2 = triangles[i].points[2];
    DrawLine(p0, p1);
    DrawLine(p1, p2);
    DrawLine(p2, p0);

    const auto max_x = std::max({p0.x, p1.x, p2.x});
    const auto max_y = std::max({p0.y, p1.y, p2.y});
    const auto min_x = std::min({p0.x, p1.x, p2.x});
    const auto min_y = std::min({p0.y, p1.y, p2.y});

    printf("%f %f %f %f\n", max_x, max_y, min_x, min_y);

    if (!checked) {
      for (float x = min_x; x < max_x; x += 2.0 / size) {
        for (float y = min_y; y < max_y; y += 2.0 / size) {
          if (isLeft(p0, p1, point{x, y}) && isLeft(p1, p2, point{x, y}) &&
              isLeft(p2, p0, point{x, y})) {
            to_draw.push_back(point{x, y});
          }
        }
      }
      checked = true;
    }
    glColor3f(0.0, 0.0, 1.0);
    glPointSize((800.0 * (1.0 - border)) / size);
    glBegin(GL_POINTS);
    for (std::size_t i = 0; i < draw_index; ++i) {
      glVertex3f(to_draw[i].x, to_draw[i].y, 0);
    }
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
  }

  glutSwapBuffers();
}
