#include <math.h>
#include <stdio.h>

#include <GLTools.h>// OpenGL toolkit
#include <freeglut_std.h>
#include <math3d.h>

#include "obj.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.1415926535f

// Define a macro to print a 4x4 array
#define PRINT_4X4_ARRAY(arr) \
    do { \
        for (int i = 0; i < 16; ++i) { \
            std::cout << arr[i] << "\t"; \
            if ((i + 1) % 4 == 0) \
                std::cout << std::endl; \
        } \
    } while (0)

#define GLUT_SCROLL_UP 4
#define GLUT_SCROLL_DOWN 3


static void load_image(GLuint *textures, const char *path) {
  int width, height, channels;
  unsigned char *imageData = stbi_load(path, &width, &height, &channels, 0);

  glGenTextures(1, textures);
  glBindTexture(GL_TEXTURE_2D, *textures);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (imageData) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);
    printf("%s success\n", path);
  } else {
    printf("%s fail\n", path);
  }

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, *textures);
}

template<typename T>
struct xyz { T x, y, z; };
template<typename T>
struct xy { T x, y; };

int kUpdate_time = 10;
struct { int w, h; } kWindow = { 800, 600 };

float kMoonAngle = 0;

bool kIsStop = false;

xy<GLfloat> kViewRot = {0, PI / 2};

GLfloat kAmbientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat kDiffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat kSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat kLightPos[4] =  {-75.0f, 200.0f, 50.0f, 0.0f};
GLfloat kSpecref[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLuint kTextures[4];

struct NiubeObjPlusX {
  Obj m_obj;
  GLuint m_tex;
  NiubeObjPlusX(const char *objPath, const char *texPath) {
    m_obj = readObj(objPath);
    load_image(&m_tex, texPath);
  }
  NiubeObjPlusX() {}
};

NiubeObjPlusX sphere;
NiubeObjPlusX moon;
NiubeObjPlusX moon2;
NiubeObjPlusX sun;

M3DMatrix44f shadowMat;

xy<int> kMP;

struct {
  xyz<GLfloat> rot = {PI / 4, PI / 2, 0};
  xy<GLfloat> rotDelta = {0.02, 0.2f};
  float radius = 100;
} kLightData;

void renderObj(const Obj &obj) {
  M3DVector3f vNormal;
  glBegin(GL_TRIANGLES);
  for (auto i = 0; i < obj.faces.size(); i++) {
    FaceIndices face = obj.faces[i];
    float v1[3] = {obj.vertices[face.v[0]].x, obj.vertices[face.v[0]].y, obj.vertices[face.v[0]].z};
    float v2[3] = {obj.vertices[face.v[1]].x, obj.vertices[face.v[1]].y, obj.vertices[face.v[1]].z};
    float v3[3] = {obj.vertices[face.v[2]].x, obj.vertices[face.v[2]].y, obj.vertices[face.v[2]].z};

    float uv1[2] = {obj.uvs[face.uv[0]].x, 1 - obj.uvs[face.uv[0]].y};
    float uv2[2] = {obj.uvs[face.uv[1]].x, 1 - obj.uvs[face.uv[1]].y};
    float uv3[2] = {obj.uvs[face.uv[2]].x, 1 - obj.uvs[face.uv[2]].y};

    // Front face
    m3dFindNormal(vNormal, v1, v2, v3);
    glNormal3fv(vNormal);

    glTexCoord2fv(uv1);
    glVertex3fv(v1);
    glTexCoord2fv(uv2);
    glVertex3fv(v2);
    glTexCoord2fv(uv3);
    glVertex3fv(v3);
  }
  glEnd();
}

void drawPlusX(const NiubeObjPlusX &obj, bool DrawShadow = false) {
  const GLfloat white[4] = {1, 1, 1, 1.0};
  const GLfloat shadowColor[4] = {10.0 / 255.0, 10.0 / 255.0, 40.0 / 255.0, 0.5};

  if (!DrawShadow) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, obj.m_tex);
    glColor3fv(white);
  } else {
    glDisable(GL_TEXTURE_2D);
    glColor3fv(shadowColor);
  }
  renderObj(obj.m_obj);
}

// This function does any needed initialization on the rendering
// context.
void SetupRC() {
  kLightPos[0] = cos(kLightData.rot.y) * kLightData.radius * cos(kLightData.rot.x);
  kLightPos[1] = sin(kLightData.rot.x) * kLightData.radius;
  kLightPos[2] = sin(kLightData.rot.y) * kLightData.radius * cos(kLightData.rot.x);

  GLfloat fAspect = (GLfloat)kWindow.w / (GLfloat)kWindow.h;

  glViewport(0, 0, kWindow.w, kWindow.h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(120.0f, fAspect, 0.01, 200.0);

  float length = 100.0f;
  gluLookAt(0.0f, 0.0f, 0.0f,
            length * cos(kViewRot.y), length * sin(kViewRot.x), length * sin(kViewRot.y),
            0.0f, 1.0f, 0.0f);

  glLightfv(GL_LIGHT0, GL_POSITION, kLightPos);

  // Any three points on the ground (counterclockwise order)
  M3DVector3f points[3] = {{-30.0f, -1.0f, -20.0f},
                           {-30.0f, -1.0f, 20.0f},
                           {30.0f, -1.0f, 20.0f}};

  // glEnable(GL_DEPTH_TEST); // Hidden surface removal
  glFrontFace(GL_CCW);    // Counter clock-wise polygons face out
  glEnable(GL_CULL_FACE); // Do not calculate inside of jet

  // Setup and enable light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, kAmbientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, kDiffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, kSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, kLightPos);
  glEnable(GL_LIGHT0);

  // Enable color tracking
  glEnable(GL_COLOR_MATERIAL);

  // Set Material properties to follow glColor values
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  // All materials hereafter have full specular reflectivity
  // with a high shine
  glMaterialfv(GL_FRONT, GL_SPECULAR, kSpecref);
  glMateriali(GL_FRONT, GL_SHININESS, 128);

  // Light blue background
  glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

  // Get the plane equation from three points on the ground
  M3DVector4f vPlaneEquation;
  m3dGetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]);

  // Calculate projection matrix to draw shadow on the ground
  m3dMakePlanarShadowMatrix(shadowMat, vPlaneEquation, kLightPos);

  glEnable(GL_NORMALIZE);
}

// here is how whole solar system work
void drawObjAndItsShadow(bool drawShadow) {
  const float scale = 0.2;

  glScaled(scale, scale, scale);
  glTranslatef(0.0f, 0, 2.0f);
  glRotatef(kMoonAngle, 0, 1, 0);

  glPushMatrix();

  glTranslatef(0.0f, -5.0f * scale, 0);
  drawPlusX(sun, drawShadow);

  glPopMatrix();
  glPushMatrix();
  glRotatef(kMoonAngle, 0, 1, 0);
  glTranslatef(0, cos(kMoonAngle / 30.0)*2, 1.5f);
  drawPlusX(moon, drawShadow);

  glPopMatrix();
  glRotatef(kMoonAngle, 0, 1, 0);
  glTranslatef(0, sin(kMoonAngle / 30.0)*2, -1.5f);
  drawPlusX(moon2, drawShadow);
}

/* START GLUT CALLBACK FUNCTIONS */

void RenderScene(void) {
  SetupRC();
  // Clear the window with current clearing color
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // glEnable(GL_CULL_FACE);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);

  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glEnable(GL_BLEND);

  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  {
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, kLightPos);

    // sphere
    {
      const float scale = 1;
      glLoadIdentity();
      glColor3f(1, 1, 1);
      glScaled(scale, scale, scale);
      drawPlusX(sphere);
    }

    glLoadIdentity();
    drawObjAndItsShadow(false);

    // Get ready to draw the shadow and the ground
    // First disable lighting and save the projection state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glLoadIdentity();
    glMultMatrixf((GLfloat *)shadowMat);
    drawObjAndItsShadow(true);

    // Draw the light source
    glLoadIdentity();
    glTranslatef(kLightPos[0], kLightPos[1], kLightPos[2]);
    glColor3f(1, 0, 1);
    glutSolidSphere(10.0f, 10, 10);
  }
  glPopMatrix();

  glutSwapBuffers();
}

void MouseMotionHandler(int mx, int my) {
  kViewRot.y += 0.025f * (mx - kMP.x);
  kViewRot.x += -0.025f * (my - kMP.y);
  if (kViewRot.x > PI / 2) { kViewRot.x = PI / 2; }
  if (kViewRot.x < -PI / 2) { kViewRot.x = -PI / 2; }
  // update last mouse position
  kMP.x = mx;
  kMP.y = my;
  glutPostRedisplay();
}
void Timer(int value) {
  if (!kIsStop) {
    kMoonAngle += 3;
  }

  glutPostRedisplay();
  glutTimerFunc(kUpdate_time, Timer, 0);
}
void MouseHandler(int button, int state, int x, int y) {
  if (button == GLUT_SCROLL_DOWN) { kLightData.rot.y -= kLightData.rotDelta.y; }
  if (button == GLUT_SCROLL_UP) { kLightData.rot.y += kLightData.rotDelta.y; }

  // renew the last position
  kMP.x = x;
  kMP.y = y;

  glutPostRedisplay();
}
void NormalKeyHandler(unsigned char key, int x, int y) {
  switch (key) {
    case 'w': kLightData.rot.x += kLightData.rotDelta.x; break;
    case 's': kLightData.rot.x -= kLightData.rotDelta.x; break;
    case 'a': kMoonAngle += 5; break;
    case 'd': kMoonAngle -= 5; break;
    case ' ': kIsStop = !kIsStop; break;
  }
  if ( kLightData.rot.x > PI/2) { kLightData.rot.x = PI/2; }
  if ( kLightData.rot.x < 0) { kLightData.rot.x = 0; }

  glutPostRedisplay();
}
void ChangeSize(int w, int h) {
  kWindow = {w, h};
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(kWindow.w, kWindow.h);
  glutCreateWindow("Niu-be de CG final work");
  sphere = NiubeObjPlusX("../obj/sphere.obj", "../texture/theworldif.jpg");
  sun = NiubeObjPlusX("../obj/trump.obj", "../texture/tumpLPcolors.png");
  moon = NiubeObjPlusX("../obj/sphere.obj", "../texture/kjy01601.png");
  moon2 = NiubeObjPlusX("../obj/sphere.obj", "../texture/wkc.bmp");
  // Front Face (before rotation)
  glutReshapeFunc(ChangeSize);
  glutMouseFunc(MouseHandler);
  glutMotionFunc(MouseMotionHandler);
  glutKeyboardFunc(NormalKeyHandler);
  glutDisplayFunc(RenderScene);

  // glutIdleFunc(Loop);
  glutTimerFunc(0, Timer, 0);
  glutMainLoop();

  return 0;
}
