
#include "freeglut_std.h"
#include <GL/gl.h>
#include <stdio.h>
/*** freeglut***/
#include <freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "pryamid.h"

void ChangeSize(int, int);
void RenderScene(void);
void MenuCallback(int);

GLenum glShadeType = GL_SMOOTH;
float yangle = 45;
float xangle = 0;
float zangle = 0;

float xtrans = 0;
float ydelta = .2f;
float xdelta = .2f;

void Loop() {
  // yangle += ydelta;
  // xangle += xdelta;
  glutPostRedisplay();
}

void SpecialKeyHandler(int key, int x, int y) 
{
    switch (key) 
    {    
       case GLUT_KEY_RIGHT: yangle += 10.0f; break;
       case GLUT_KEY_LEFT: yangle -= 10.0f; break;
       case GLUT_KEY_UP: xangle += 10.0f; break;
       case GLUT_KEY_DOWN: xangle -= 10.0f; break;
    }
}

void NormalKeyHandler (unsigned char key, int x, int y)
{
    switch (key) 
    {    
       case 'd': yangle += 10.0f; break;
       case 'a': yangle -= 10.0f; break;
       case 'w': xangle += 10.0f; break;
       case 's': xangle -= 10.0f; break;
       case 'q': zangle += 10.0f; break;
       case 'e': zangle -= 10.0f; break;
       
       case 'f': xtrans += 10.0f; break;
       case 'g': xtrans -= 10.0f; break;
       case 'r':
       zangle = 0;
       yangle = 45;
       xangle = 0;
       xtrans = 0;
       break;
    }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(800, 800);
  glutInitWindowPosition(600, 80);
  glutCreateWindow("Simple Triangle");

  glutCreateMenu(MenuCallback);
  glutAddMenuEntry("GL_SMOOTH", 1);
  glutAddMenuEntry("GL_FLAT", 2);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutSpecialFunc (SpecialKeyHandler);
  glutKeyboardFunc (NormalKeyHandler);

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
  glOrtho(-10, 10, -10, 10,
          -10, 100);
  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity();
}
void RenderScene(void) {
  GLuint texture;
  int width, height, channels;
  unsigned char* imageData = stbi_load("./kjy01601.png", &width, &height, &channels, 0);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if(imageData) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
      width, height,
      0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
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
  gluLookAt(
    1, 2, 10,
    0, 0, 0,
    1, 1, 0);
  glShadeModel(glShadeType);

  glBegin(GL_LINES);
    glVertex3f(100,0,0);
    glVertex3f(-100,0,0);
  glEnd();
    glBegin(GL_LINES);
    glVertex3f(0,-100,0);
    glVertex3f(0,100,0);
  glEnd();
      glBegin(GL_LINES);
    glVertex3f(0,0,-100);
    glVertex3f(0,0,100);
  glEnd();

  glRotatef(yangle,0,1,0);
  glRotatef(xangle,1,0,0);
  glRotatef(zangle,0,0,1);
  glTranslatef(xtrans,0,0);
  glBegin(GL_TRIANGLES);
    // glColor3f(1,1,0);
    glTexCoord2f(0.5f, 0.0f); glVertex3fv(PRYAMID_POINTS[0]);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[1]);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[2]);
  glEnd();
  glBegin(GL_TRIANGLES);
    // glColor3f(0,1,1);
    glTexCoord2f(0.5f, 0.0f); glVertex3fv(PRYAMID_POINTS[0]);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[2]);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[3]);
  glEnd();
  glBegin(GL_TRIANGLES);
    // glColor3f(1,0,1);
    glTexCoord2f(0.5f, 0.0f); glVertex3fv(PRYAMID_POINTS[0]);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[3]);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[4]);
  glEnd();
    glBegin(GL_TRIANGLES);
    // glColor3f(1,0,0);
    glTexCoord2f(0.5f, 0.0f); glVertex3fv(PRYAMID_POINTS[0]);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[4]);
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(PRYAMID_POINTS[1]);
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
