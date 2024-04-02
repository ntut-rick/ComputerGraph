
#include "freeglut_std.h"
#include <GL/gl.h>
#include <stdio.h>
/*** freeglut***/
#include <freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "pryamid.h"

#define IMPL_TRANSFORM_YOURSELF
#include "myglTransfrom.h"

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

float xscale = 1;
float yscale = 1;
float zscale = 1;

struct cood_t {
  float x,y;
};

// mouse click pos in view
struct cood_t mc1 = {0,0};
struct cood_t mc2 = {0,0};
// struct cood_t win = {400,400};

void Loop() {
  // yangle += ydelta;
  // xangle += xdelta;
  glutPostRedisplay();
}

//window width/height
int ww=400, wh=400;

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
       case 'q': zangle += .1f; break;
       case 'e': zangle -= .1f; break;
       
       case 'f': xtrans += 10.0f; break;
       case 'g': xtrans -= 10.0f; break;

       case 'o': xscale += 10.0f; break;
       case 'i': xscale -= 10.0f; break;
       case 'r':
       zangle = 0;
       yangle = 45;
       xangle = 0;
       xtrans = 0;
       break;
    }
}

void mouseClick(int button, int state, int mx, int my) {
  if (button != GLUT_LEFT_BUTTON) {
    return;
  }
  printf("mouse: %d %d\n", mx, my);


  if(x<ww/2) { // left (view 1)
    printf("left");
    glViewport(0,0,ww/2,wh);
  } else { // right (view 2)
    printf("right");
    glViewport(ww/2,0,ww/2,wh);
  }

  GLint viewport[4]; // x y w h
  glGetIntegerv(GL_VIEWPORT, viewport);
  GLint offx = viewport[0];
  GLint offy = viewport[1];
  GLint w = viewport[2];
  GLint h = viewport[3];

  struct cood_t *mc = x<ww/2 ? &mc1 : &mc2;
  mc->x = ((float)(mx-offx) / w)*2 - 1;
  mc->y = ((float)(my-offy) / h)*-2 + 1;
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(600, 80);
  glutCreateWindow("Simple Jykuo");

  glutCreateMenu(MenuCallback);
  glutAddMenuEntry("GL_SMOOTH", 1);
  glutAddMenuEntry("GL_FLAT", 2);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutSpecialFunc (SpecialKeyHandler);
  glutKeyboardFunc (NormalKeyHandler);
  glutMouseFunc(mouseClick);

  glutReshapeFunc(ChangeSize);
  glutDisplayFunc(RenderScene);
  glutIdleFunc(Loop);

  glutMainLoop(); // http://www.programmer-club.com.tw/ShowSameTitleN/opengl/2288.html
  return 0;
}
void ChangeSize(int w, int h) {
  ww = w;
  wh = h;
  // printf("Window Size= %d X %d\n", w, h);
  // glViewport(0, 0, w, h);
}

void drawXYZaxes(void) {
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
}

void drawJykuo(void) {
  // load jykuo texture BEGIN
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
    printf("where's jkuoy?");
  }
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);

  // load jykuo texture BEGIN

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
}

void drawView1(void) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-10, 10, -10, 10,
          -10, 100);
  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity();
  
  glLoadIdentity();
  // gluLookAt(
  //   1, 2, 10,
  //   0, 0, 0,
  //   1, 1, 0);
  gluLookAt(
    0, 0, 10,
    0, 0, 0,
    0, 1, 0);
  glShadeModel(glShadeType);

  drawXYZaxes();
  
  // rotation vector
  glBegin(GL_LINES);
    glColor3f(1,.3,.3);
    glVertex3f(0,0,0);
    glVertex3f(10*mc1.x, 10*mc1.y, 0.0f);
  glEnd();

  glRotatef(zangle,mc1.x,mc1.y,0);

  glColor3f(1,1,1);
  drawJykuo();
}
void drawView2(void) {

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-10, 10, -10, 10,
          -10, 100);
  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity();
  
  glLoadIdentity();
  gluLookAt(
    0, 0, 10,
    0, 0, 0,
    0, 1, 0);
  glShadeModel(glShadeType);

  drawXYZaxes();
  
  // rotation vector
  glBegin(GL_LINES);
    glColor3f(1,.3,.3);
    glVertex3f(0,0,0);
    glVertex3f(10*mc2.x, 10*mc2.y, 0.0f);
  glEnd();

  glRotatef(zangle,mc2.x,mc2.y,0);

  glColor3f(1,1,1);
  drawJykuo();
}

void RenderScene(void) {

  glEnable(GL_DEPTH_TEST);

  // glBegin(GL_QUADS); // Example: Drawing a textured quad
  //   glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.0f);
  //   glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.0f);
  //   glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  0.0f);
  //   glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  0.0f);
  // glEnd();
  // glBindTexture(GL_TEXTURE_2D, 0);

  // glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glViewport(0,0,ww/2,wh);
  glColor3f(0,1,1);
  drawView1();

  glLoadIdentity();
  glViewport(ww/2,0,ww/2,wh);
  glColor3f(1,1,0);
  drawView2();

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
