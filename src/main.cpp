
#include "freeglut_std.h"
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
/*** freeglut***/
#include <freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "pryamid.h"
#include "stb_image.h"

#include "loader.hpp"

void ChangeSize(int, int);
void RenderScene(void);
void MenuCallback(int);

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

float clickx = 0;
float clicky = 0;

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
  case 'w':
    xangle += 0.05f;
    break;
  case 'q':
    xangle -= 0.05f;
    break;
  case 's':
    yangle += 0.05f;
    break;
  case 'a':
    yangle -= 0.05f;
    break;
  case 'x':
    zangle += 0.05f;
    break;
  case 'z':
    zangle -= 0.05f;
    break;

  case 'r':
    xtrans += 0.1f;
    break;
  case 'e':
    xtrans -= 0.1f;
    break;
  case 'f':
    ytrans += 0.1f;
    break;
  case 'd':
    ytrans -= 0.1f;
    break;
  case 'v':
    ztrans += 0.1f;
    break;
  case 'c':
    ztrans -= 0.1f;
    break;

  case 'y':
    xscale += 0.1f;
    break;
  case 't':
    xscale -= 0.1f;
    break;
  case 'h':
    yscale += 0.1f;
    break;
  case 'g':
    yscale -= 0.1f;
    break;
  case 'n':
    zscale += 0.1f;
    break;
  case 'b':
    zscale -= 0.1f;
    break;
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

void rot_x(float theta) {
  double rot_x[] = {
      1,
      0,
      0,
      0, //
      0,
      cos(deg2deg(theta)),
      sin(deg2deg(theta)),
      0, //
      0,
      -sin(deg2deg(theta)),
      cos(deg2deg(theta)),
      0, //
      0,
      0,
      0,
      1, //
  };

  glMultMatrixd(rot_x);
}
void rot_y(float theta) {
  double rot_y[] = {
      cos(deg2deg(theta)),
      0,
      -sin(deg2deg(theta)),
      0, //
      0,
      1,
      0,
      0, //
      sin(deg2deg(theta)),
      0,
      cos(deg2deg(theta)),
      0, //
      0,
      0,
      0,
      1, //
  };
  glMultMatrixd(rot_y);
}
void rot_z(float theta) {
  double rot_z[] = {
      cos(deg2deg(theta)),
      -sin(deg2deg(theta)),
      0,
      0, //
      sin(deg2deg(theta)),
      cos(deg2deg(theta)),
      0,
      0, //
      0,
      0,
      1,
      0, //
      0,
      0,
      0,
      1, //
  };
  glMultMatrixd(rot_z);
}

int main(int argc, char **argv) {
  Model m;
  int err = load_model("../assets/octahedron.obj", m);

  if (err != 0) {
    return 1;
  }

  for (const auto &v : m.vertices) {
    printf("%f %f %f\n", v.x, v.y, v.z);
  }

  return 0;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutInitWindowPosition(600, 80);
  glutCreateWindow("Simple Triangle");

  glutCreateMenu(MenuCallback);
  glutAddMenuEntry("GL_SMOOTH", 1);
  glutAddMenuEntry("GL_FLAT", 2);
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
void RenderScene(void) {
  GLuint texture;
  int width, height, channels;
  unsigned char *imageData =
      stbi_load("./kjy01601.png", &width, &height, &channels, 0);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (imageData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);
  } else {
    printf("nmsl");
  }

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  // glBegin(GL_QUADS); // Example: Drawing a textured quad
  //   glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.0f);
  //   glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.0f);
  //   glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  0.0f);
  //   glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  0.0f);
  // glEnd();
  // glBindTexture(GL_TEXTURE_2D, 0);

  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);

  glLoadIdentity();
  gluLookAt(1, 2, 10, 0, 0, 0, 0, 1, 0);
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
  glBegin(GL_TRIANGLES);
  // glColor3f(1,1,0);
  glTexCoord2f(0.5f, 0.0f);
  glVertex3fv(PRYAMID_POINTS[0]);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[1]);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[2]);
  glEnd();
  glBegin(GL_TRIANGLES);
  // glColor3f(0,1,1);
  glTexCoord2f(0.5f, 0.0f);
  glVertex3fv(PRYAMID_POINTS[0]);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[2]);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[3]);
  glEnd();
  glBegin(GL_TRIANGLES);
  // glColor3f(1,0,1);
  glTexCoord2f(0.5f, 0.0f);
  glVertex3fv(PRYAMID_POINTS[0]);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[3]);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[4]);
  glEnd();
  glBegin(GL_TRIANGLES);
  // glColor3f(1,0,0);
  glTexCoord2f(0.5f, 0.0f);
  glVertex3fv(PRYAMID_POINTS[0]);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[4]);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3fv(PRYAMID_POINTS[1]);
  glEnd();

  glutSwapBuffers();
}

void MenuCallback(int value) {
  switch (value) {
  case 1:
    glShadeType = GL_SMOOTH;
    break;
  case 2:
    glShadeType = GL_FLAT;
    break;
  }
  glutPostRedisplay();
}
