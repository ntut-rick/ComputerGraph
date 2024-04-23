#include <array>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>

/*** freeglut***/
#include <freeglut.h>
#include <freeglut_std.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "NiuBiObject.hpp"
// #define MY_GL_TRANSFORM_IMPLEMENTATION
#include "myglTransform.h"

// Define a macro to print a 4x4 array
#define PRINT_4X4_ARRAY(arr) \
    do { \
        for (int i = 0; i < 16; ++i) { \
            std::cout << arr[i] << "\t"; \
            if ((i + 1) % 4 == 0) \
                std::cout << std::endl; \
        } \
    } while (0)

static void RenderScene(void);
static void ChangeSize(int, int);

static void OnKeyBoardPress(unsigned char, int, int);

static void MousePress(int, int, int, int);
static void MouseDrag(int, int);
static void mouseWheel(int, int, int, int);

static void MenuCallback(int);
static void ColorModeMenuCallback(int);
static void RenderModeMenuCallback(int);
static void TransformModeCallback(int);

static void LoadJykuoTexture();


struct { int width, height; } window_size = {400, 400};

// fuck windows >>> https://cplusplus.com/forum/general/12435/
struct { GLfloat left, right, bottom, top, zNear, zFar; }
ortho_settings = {-10, 10, -10, 10, -100, 100};
struct { GLfloat x,y,z; } camera_pos = {0, 0, 10};
struct { GLfloat x,y,z; } camera_look_at = {0, 0, 0};

std::vector<NiuBiObject> loaded_objs;
int selected_obj_index = 0;

GLfloat objRotMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
GLfloat objTranMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
GLfloat cameraRotMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,10,1};

GLenum render_mode = GL_TRIANGLES;
float zoomingScaler = 0.9;

int dimenstion = 10;
struct { int x,y; } selectedPosition = {0,0};

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(window_size.width, window_size.height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Niu-Bi de CG Midterm Work");

  glutCreateMenu(MenuCallback);
  for(int i=10; i<1000; i+=5) {
    auto a = std::to_string(i);
    glutAddMenuEntry(a.c_str(), i);
  }
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutReshapeFunc(ChangeSize);
  glutKeyboardFunc(OnKeyBoardPress);

  glutMouseWheelFunc(mouseWheel);
  glutMouseFunc(MousePress);

  glutDisplayFunc(RenderScene);
  glutMainLoop(); // http://www.programmer-club.com.tw/ShowSameTitleN/opengl/2288.html
  return 0;
}
void ChangeSize(int w, int h) {
  window_size = {w, h};
  // reference: https://gist.github.com/insaneyilin/9320e8263b29e3c172c4f5963b8db693
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
  glutPostRedisplay();
}

void LoadJykuoTexture() {
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
    printf("Can't found jykuo :(");
  }
  
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
}

void drawXYZaxes(void) {
  glBegin(GL_LINES);
    glColor3f(1.0f, 0.5f, 0.5f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
  glEnd();
  glBegin(GL_LINES);
    glColor3f(0.5f, 1.0f, 0.5f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
  glEnd();
  glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 1.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
  glEnd();
}

void singlecolor(void) {
  glColor3f(1,1,1);
}
void randomcolor(void) {
  float r,g,b;
  r = (float)rand() / RAND_MAX;
  g = (float)rand() / RAND_MAX;
  b = (float)rand() / RAND_MAX;
  glColor3f(r,g,b);
}
void (*setcolorfuncion)(void) = singlecolor;

void drawSelectedOjbect(void) {
  if (selected_obj_index >= loaded_objs.size()) {
    return;
  }
  auto curret_obj = loaded_objs[selected_obj_index];
  // srand(7414);
  for(auto face : curret_obj.faces) {
    setcolorfuncion();
    glBegin(render_mode);
      // Deprecated uv setup
      // glTexCoord2f(.5f, .0f); glVertex3fv(curret_obj.vertices[face[0]].data());
      // glTexCoord2f(.35f, .6f); glVertex3fv(curret_obj.vertices[face[1]].data());
      // glTexCoord2f(.65f, .6f); glVertex3fv(curret_obj.vertices[face[2]].data());
      glTexCoord2f(.5f, .55f); glVertex3fv(curret_obj.vertices[face[0]].data());
      glTexCoord2f(.3f, .3f); glVertex3fv(curret_obj.vertices[face[1]].data());
      glTexCoord2f(.7f, .3f); glVertex3fv(curret_obj.vertices[face[2]].data());
    glEnd();
    // Debugging black lines
    // glColor3f(0,0,0);
    // glBegin(GL_LINE_LOOP);
    //   glVertex3fv(curret_obj.vertices[face[0]].data());
    //   glVertex3fv(curret_obj.vertices[face[1]].data());
    //   glVertex3fv(curret_obj.vertices[face[2]].data());
    // glEnd();
  }
}

void drawPoint(int x, int y, GLenum mode=GL_QUADS) {
  const auto realsize = dimenstion*2+1;
  const float size = 20.0f/realsize;
  const float halfsize = size/2;
  glBegin(mode);
    glVertex3f(x*size-halfsize, y*size-halfsize, 0);
    glVertex3f(x*size-halfsize, y*size+halfsize, 0);
    glVertex3f(x*size+halfsize, y*size+halfsize, 0);
    glVertex3f(x*size+halfsize, y*size-halfsize, 0);
  glEnd();
}


void drawGrid() {
  const auto realsize = dimenstion*2+1;
  const float size = 20.0f/realsize;
  const float halfsize = size/2;
  for(int i=-dimenstion; i<=dimenstion; i++) {
    for(int j=-dimenstion; j<=dimenstion; j++) {
      drawPoint(i,j,GL_LINE_LOOP);
    }
  }
}

void RenderScene(void) {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // init the viewport
  glViewport(0, 0, window_size.width, window_size.height);

  glMatrixMode(GL_PROJECTION);
  ortho_settings.left = -10/zoomingScaler;
  ortho_settings.right = 10/zoomingScaler;
  ortho_settings.bottom = -10/zoomingScaler;
  ortho_settings.top = 10/zoomingScaler;
  glLoadIdentity();
  glOrtho(ortho_settings.left, ortho_settings.right, ortho_settings.bottom, ortho_settings.top,
          ortho_settings.zNear, ortho_settings.zFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // TODO: we should calculate the up vector, but now I am lazy to do it.
  gluLookAt(
    camera_pos.x, camera_pos.y, camera_pos.z,
    camera_look_at.x, camera_look_at.y, camera_look_at.z,
    0, 1, 0);
  glEnable(GL_DEPTH_TEST);

  drawGrid();
  drawPoint(selectedPosition.x, selectedPosition.y);

  glutSwapBuffers();
}

void myUnproject(
  int x, int y, int z,
  GLdouble *wx, GLdouble *wy, GLdouble *wz) {
		GLdouble modelview[16], projection[16];
		GLint viewport[4];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);
		gluUnProject(x, y, 0, modelview, projection, viewport,
                 wx, wy, wz);
}

int last_x, last_y;
void MousePress(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    last_x = x;
    last_y = y;
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
    GLdouble wx,wy,wz;
		myUnproject(x,y,0, &wx, &wy, &wz);
		
    const float len = 10.0f/dimenstion;
    printf("%f\n", len);
    selectedPosition.x = (int)(wx>0 ? wx/len+0.5 : wx/len-0.5);
    selectedPosition.y = (int)(-wy>0 ? -wy/len+0.5 : -wy/len-0.5);
    // printf("raw xy (%lf,%lf)\n", wx, wy);
    printf("    xy (%d,%d)\n", selectedPosition.x, selectedPosition.y);
	}
  glutPostRedisplay();
}
void mouseWheel(int button, int dir, int x, int y)
{
  // static float zoomingScaler = 10;
  if (dir > 0) {
    // camera_pos.z /= 1.2;
    zoomingScaler *= 1.2;
  } else {
    // camera_pos.z *= 1.2;
    zoomingScaler /= 1.2;
  }
  // printf("zoomingScaler=%lf\n", zoomingScaler);
  glutPostRedisplay();
}

void MenuCallback(int value) {
  std::cout << "dim: " << value << std::endl;
  dimenstion = value;
  glutPostRedisplay();
}
void OnKeyBoardPress(unsigned char key, int x, int y) {
  switch (key) {
  case ' ':
    // current_display_obj->set_transform_to_target({0, 0, 0}, ortho_settings);
    // transform->set_rotation_by_euler({0, 0, 0});
    // transform->set_rotation_by_axi(0, {0, 0, 1});
    camera_pos = {0, 0, 10};
    camera_look_at = {0, 0, 0};

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, objRotMatrix);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, objTranMatrix);
    glLoadIdentity();
    glTranslatef(0,0,10);
    glGetFloatv(GL_MODELVIEW_MATRIX, cameraRotMatrix);
    glPopMatrix();
    break;
  }
  glutPostRedisplay();
}