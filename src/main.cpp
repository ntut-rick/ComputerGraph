#include "NiuBiObject.hpp"
#include "Transform.hpp"
#include <array>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <stdio.h>

/*** freeglut***/
#include <freeglut.h>
#include <freeglut_std.h>
#include <vector>

#include "NiuBiObject.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define IMPL_TRANSFORM_YOURSELF
#include "myglTransform.h"

enum class ControlMode {
  OBJECT_ROTATE,
  CAMERA_UP,
  CAMERA_POS,
};
ControlMode current_control_mode = ControlMode::OBJECT_ROTATE;

// enum class CurrentControlAxis {
//   X, Y, Z,
// };

static void RenderScene(void);
static void ChangeSize(int, int);

static void OnKeyBoardPress(unsigned char, int, int);

static void MousePress(int, int, int, int);
static void MouseDrag(int, int);
static void mouseWheel(int, int, int, int);

static int BuildManu();
static void MenuCallback(int);
static void ColorModeMenuCallback(int);
static void RenderModeMenuCallback(int);
static void RotationModeCallback(int);

static void LoadJykuoTexture();

// bool mousepoint01Status = false;
// struct { GLfloat x,y,z; } mouseViewport1WorldPos1 = {10, 10, 0};
// struct { GLfloat x,y,z; } mouseViewport1WorldPos2 = {10, 10, 0};

// fuck windows >>> https://cplusplus.com/forum/general/12435/
struct { GLfloat left, right, bottom, top, zNear, zFar; }
ortho_settings = {-10, 10, -10, 10, -100, 100};
struct { GLfloat x,y,z; } camera_pos = {0, 0, 10};
struct { GLfloat x,y,z; } camera_look_at = {0, 0, 0};

std::vector<NiuBiObject> loaded_objs;
int selected_obj_index = 0;

// GLfloat xangle = 0;
// GLfloat yangle = 0;
GLfloat objRotMatrix[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

GLenum render_mode = GL_TRIANGLES;

struct { int width, height; } window_size = {400, 400};

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(window_size.width, window_size.height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Niu-Bi de CG Midterm Work");

  // glutCreateMenu(MenuCallback);
  BuildManu();
  loaded_objs.clear();
  for (int i=1; i<argc; i++) {
    if (!std::filesystem::exists(argv[i])) {
      // skip non-file arguments
      continue;
    }
    auto obj = NiuBiObject::fromFile(argv[i]);
    loaded_objs.push_back(obj);
    glutAddMenuEntry(argv[i], i);
  }
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  LoadJykuoTexture();

  glutReshapeFunc(ChangeSize);
  glutKeyboardFunc(OnKeyBoardPress);

  glutMouseFunc(MousePress);
  glutMotionFunc(MouseDrag);
  glutMouseWheelFunc(mouseWheel);

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
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
  glEnd();
  glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
  glEnd();
  glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
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

void RenderScene(void) {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // init the viewport
  glViewport(0, 0, window_size.width, window_size.height);

  glMatrixMode(GL_PROJECTION);
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

  drawXYZaxes();

  glMultMatrixf(objRotMatrix);
  // TODO: objTransMatrix
  // glMultMatrixf(objTransMatrix);

  drawSelectedOjbect();

  glutSwapBuffers();
}

void my_magic_rotate(
  GLfloat *m,
  GLfloat angle,
  GLfloat x,
  GLfloat y,
  GLfloat z
) {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(angle, x, y, z);
  glMultMatrixf(m);
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
  glPopMatrix();
}

int last_x, last_y;
void MousePress(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    last_x = x;
    last_y = y;
    // printf("reset xy\n");
    // if (mousepoint01Status == false) {
    //   mouseViewport1WorldPos1[0] = (2 * ((float)x / 800) - 1) * (10);
    //   mouseViewport1WorldPos1[1] = (-2 * ((float)y / 800) + 1) * (10);
    //   mouseViewport1WorldPos1[2] = 0;
    //   mousepoint01Status = true;
    // } else {
    //   mouseViewport1WorldPos2[0] = (2 * ((float)x / 800) - 1) * (10);
    //   mouseViewport1WorldPos2[1] = (-2 * ((float)y / 800) + 1) * (10);
    //   mouseViewport1WorldPos2[2] = 0;
    //   mousepoint01Status = false;
    // }
  }
  // gluInvertMatrix();
  glutPostRedisplay();
}
void MouseDrag(int x, int y) {
  const int dx = x - last_x;
  const int dy = y - last_y;
  // xangle += dy;
  // yangle += dx;
  // printf("xa:%f, ya:%f\\", xangle, yangle);
  // printf("moude delta = %d %d\n", dx, dy);
  my_magic_rotate(objRotMatrix, dy, 1, 0, 0);
  my_magic_rotate(objRotMatrix, dx, 0, 1, 0);
  // for (int i = 0; i < 16; ++i) {
  //   std::cout << objRotMatrix[i] << " ";
  //   if ((i + 1) % 4 == 0) {
  //       std::cout << std::endl;
  //   }
  // }
  last_x = x;
  last_y = y;
  glutPostRedisplay();
}
void mouseWheel(int button, int dir, int x, int y)
{
  static float scale = 10;
  if (dir > 0) {
    // camera_pos.z /= 1.2;
    scale /= 1.2;
  } else {
    // camera_pos.z *= 1.2;
    scale *= 1.2;
  }
  ortho_settings.left = -1*scale;
  ortho_settings.right = 1*scale;
  ortho_settings.bottom = -1*scale;
  ortho_settings.top = 1*scale;
  // printf("z=%lf\n", scale);
  glutPostRedisplay();
}

void MenuCallback(int value) {
  selected_obj_index = value-1;
  printf("selected_obj_index = %d\n", selected_obj_index);
  glutPostRedisplay();
}
void ColorModeMenuCallback(int value) {
  switch(value) {
    case 1:
      setcolorfuncion = singlecolor;
      break;
    case 2:
      setcolorfuncion = randomcolor;
      break;
  }
  glutPostRedisplay();
}
void RenderModeMenuCallback(int value) {
  switch(value) {
    case 1:
      render_mode = GL_POINTS;
      break;
    case 2:
      render_mode = GL_LINE_LOOP;
      break;
    case 3:
      render_mode = GL_TRIANGLES;
      break;
  }
  glutPostRedisplay();
}
void RotationModeCallback(int value) {
  //TODO: RotationModeCallback
  printf("TODO: RotationModeCallback called\n");
}

void OnKeyBoardPress(unsigned char key, int x, int y) {
  switch (key) {
  case ' ':
    // xangle = 0;
    // yangle = 0;
    // current_display_obj->set_transform_to_target({0, 0, 0}, ortho_settings);
    // transform->set_rotation_by_euler({0, 0, 0});
    // transform->set_rotation_by_axis(0, {0, 0, 1});
    camera_pos = {0, 0, 10};
    camera_look_at = {0, 0, 0};

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, objRotMatrix);
    break;
  }
  glutPostRedisplay();
}

int BuildManu() {
  int submenu_color = glutCreateMenu(ColorModeMenuCallback);
  glutAddMenuEntry("Single Color", 1);
  glutAddMenuEntry("Random Colors", 2);

  int submenu_renderMode = glutCreateMenu(RenderModeMenuCallback);
  glutAddMenuEntry("Point", 1);
  glutAddMenuEntry("Line", 2);
  glutAddMenuEntry("Face", 3);

  // GLUT Doesn't Support remove menu item, so we need to recreate one. by HEKEPOIU
  int menu_id = glutCreateMenu(MenuCallback);

  glutAddSubMenu("Color Mode", submenu_color);
  glutAddSubMenu("Render Mode", submenu_renderMode);

  return menu_id;

//  glutAddMenuEntry("Reload Path", count);
}
