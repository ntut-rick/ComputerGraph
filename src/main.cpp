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

enum class ControlMode {
  CAMERAMOVE,
  LOOKATPOINTMOVE,
  TRANSLATE,
  ROTATE,
  SCALE,
};

enum class CurrentControlAxis {
  X,
  Y,
  Z,
};
ControlMode current_control_mode = ControlMode::CAMERAMOVE;

void ChangeSize(int, int);
void RenderScene(void);
void MenuCallback(int);
void ColorModeMenuCallback(int);
void OnKeyBoardPress(unsigned char, int, int);
void MousePress(int button, int state, int x, int y);
void RenderModeMenuCallback(int);
void RotationModeCallback(int);

void MouseDrag(int, int);

std::array<float, 3> mouseViewport1WorldPos1{10, 10, 0};
std::array<float, 3> mouseViewport1WorldPos2{10, 10, 0};
std::array<int, 6> ortho_settings = {-10, 10, -10, 10, -50, 50};
std::array<double, 3> camera_pos = {0, 0, 10};
std::array<double, 3> camera_look_at = {0, 0, 0};

bool mousepoint01Status = false;

///

int BuildManu();

std::vector<NiuBiObject> loaded_objs;
int selected_obj_index = 0;

GLfloat xangle = 0;
GLfloat yangle = 0;

GLenum render_mode = GL_LINES;

std::array<int, 2> windowSize{800, 800};
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(800, 800);
  glutInitWindowPosition(600, 80);
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

  glutReshapeFunc(ChangeSize);
  glutKeyboardFunc(OnKeyBoardPress);
  glutMouseFunc(MousePress);
  glutMotionFunc(MouseDrag);
  glutDisplayFunc(RenderScene);
  glutMainLoop(); // http://www.programmer-club.com.tw/ShowSameTitleN/opengl/2288.html
  return 0;
}
void ChangeSize(int w, int h) {
  printf("Window Size= %d X %d\n", w, h);
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(ortho_settings[0], ortho_settings[1], ortho_settings[2],
          ortho_settings[3], ortho_settings[4], ortho_settings[5]);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], camera_look_at[0],
            camera_look_at[1], camera_look_at[2], 0, 1, 0);
  glLoadIdentity();
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

void RenderScene(void) {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // TODO: we should calculate the up vector, but now I am lazy to do it.
  gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], camera_look_at[0],
            camera_look_at[1], camera_look_at[2], 0, 1, 0);
  glEnable(GL_DEPTH_TEST);

  drawXYZaxes();

  glRotatef(xangle, 1,0,0);
  glRotatef(yangle, 0,1,0);

  if (selected_obj_index < loaded_objs.size()) {
    auto curret_obj = loaded_objs[selected_obj_index];
    for(auto face : curret_obj.faces) {
      glColor3f(1.0f, 1.0f, 1.0f);
      setcolorfuncion();
      glBegin(render_mode);
        glVertex3fv(curret_obj.vertices[face[0]].data());
        glVertex3fv(curret_obj.vertices[face[1]].data());
        glVertex3fv(curret_obj.vertices[face[2]].data());
      glEnd();
    }
  }

  glutSwapBuffers();
}


int last_x, last_y;
void MousePress(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    last_x = x;
    last_y = y;
    printf("reset xy\n");
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
  // static int last_x = x;
  // static int last_y = y;
  const int dx = x - last_x;
  const int dy = y - last_y;
  xangle += dy;
  yangle += dx;
  // printf("xa:%f, ya:%f\\", xangle, yangle);
  // printf("moude delta = %d %d\n", dx, dy);
  last_x = x;
  last_y = y;
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
      render_mode = GL_LINES;
      break;
    case 3:
      render_mode = GL_TRIANGLES;
      break;
  }
  glutPostRedisplay();
}

void RotationModeCallback(int value) {
//  auto transform = current_display_obj->get_transform();
//  switch (value) {
//  case 1: {
//    transform->set_rotation_type(Transform::RotationType::EULER);
//    break;
//  }
//  case 2: {
//    transform->set_rotation_type(Transform::RotationType::ANYAXIS);
//    break;
//  }
//  }
//  glutPostRedisplay();
//}
//
//void print4mat(GLfloat array[16]) {
//  for (int i = 0; i < 4; i++) {
//    for (int j = 0; j < 4; j++) {
//      printf("%-3f ", array[4 * j + i]);
//    }
//    printf("\n");
//  }
}

// fuck the switch hell, it's so ugly, i know how to fix it, but I'm too lazy,
// some one help me to separate to self Input class.
void OnKeyBoardPress(unsigned char key, int x, int y) {
//  if (current_display_obj == nullptr) {
//    return;
//  }
//
//  auto transform = current_display_obj->get_transform();
//
//  if (key == ' ') {
//    current_display_obj->set_transform_to_target({0, 0, 0}, ortho_settings);
//    transform->set_rotation_by_euler({0, 0, 0});
//    transform->set_rotation_by_axis(0, {0, 0, 1});
//    camera_pos = {0, 0, 10};
//    camera_look_at = {0, 0, 0};
//    glutPostRedisplay();
//    return;
//  }
//
//  glutPostRedisplay();
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
