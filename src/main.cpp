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

int kUpdate_time = 10;
int kLightId = 0;
struct { int w, h; } kWindow = { 800, 600 };

float kGirlsAngle = 0;

bool kIsStop = false;


struct {
  GLfloat x, y, z;
} kRot = {0, PI / 2, 0};
struct {
  GLfloat x, y;
} kLightRot = {PI / 4, PI / 2};

GLfloat kAmbientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat kDiffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat kSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat kLightPos[4] =  {-75.0f, 200.0f, 50.0f, 0.0f};
GLfloat kSpecref[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLuint kTextures[4];

Obj sphere;
Obj girl;
Obj master;

M3DMatrix44f shadowMat;

static struct {int x,y;} kMP;
const float kLightRotDelta = 0.2f;
const float kLightRotRadius = 100;


void renderObj(Obj &obj) {
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

////////////////////////////////////////////////
// This function just specifically draws the jet
void DrawJet(int nShadow) {
  M3DVector3f vNormal; // Storeage for calculated surface normal

  const float scale = 0.2;
  const GLfloat white[4] = {1, 1, 1, 1.0};
  const GLfloat shadowColor[4] = {10.0 / 255.0, 10.0 / 255.0, 40.0 / 255.0, 0.5};
  // printf("r=%.2f\n", r);

  // glTranslatef(0.0f, 10.0f, 0.0f);

  glScaled(scale, scale, scale);
  glTranslatef(0.0f, -5.0 * scale, 2.0f);
  glRotatef(kGirlsAngle, 0, 1, 0);
  if (nShadow == 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, kTextures[2]);
    glColor3fv(white);
  } else {
    glDisable(GL_TEXTURE_2D);
    glColor3fv(shadowColor);
  }
  renderObj(master);

  // glScaled(scale, scale, scale);
  glTranslatef(0.0f, cos(kGirlsAngle / 30.0), 1.5f);
  glRotatef(kGirlsAngle, 0, 1, 0);

  if (nShadow == 0) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, kTextures[1]);
    glColor3fv(white);
  } else {
    glDisable(GL_TEXTURE_2D);
    glColor3fv(shadowColor);
  }
  renderObj(girl);
}

// This function does any needed initialization on the rendering
// context.
void SetupRC() {
  kLightPos[0] = cos(kLightRot.y) * kLightRotRadius * cos(kLightRot.x);
  kLightPos[1] = sin(kLightRot.x) * kLightRotRadius;
  kLightPos[2] = sin(kLightRot.y) * kLightRotRadius * cos(kLightRot.x);

  GLfloat fAspect = (GLfloat)kWindow.w / (GLfloat)kWindow.h;

  glViewport(0, 0, kWindow.w, kWindow.h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(120.0f, fAspect, 0.01, 200.0);

  float length = 100.0f;
  gluLookAt(0.0f, 0.0f, 0.0f,
            length * cos(kRot.y), length * sin(kRot.x), length * sin(kRot.y),
            0.0f, 1.0f, 0.0f);

  glLightfv(GL_LIGHT0, GL_POSITION, kLightPos);

  // Any three points on the ground (counter clockwise order)
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

  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0, GL_POSITION, kLightPos);

  glPushMatrix();
  glMatrixMode(GL_MODELVIEW);
  {
    // sphere
    const float scale = 200;
    glPushMatrix();
    {
      glLoadIdentity();
      glColor3f(1, 1, 1);
      glScaled(scale, scale, scale);
      glBindTexture(GL_TEXTURE_2D, kTextures[0]);
      renderObj(sphere);
    }
    glPopMatrix();

    glPushMatrix();
    {
      glLoadIdentity();
      DrawJet(0);
    }
    glPopMatrix();

    // Get ready to draw the shadow and the ground
    // First disable lighting and save the projection state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    {
      glLoadIdentity();
      glMultMatrixf((GLfloat *)shadowMat);
      DrawJet(1);
    }
    glPopMatrix();

    // Draw the light source
    glPushMatrix();
    {
      glLoadIdentity();
      glTranslatef(kLightPos[0], kLightPos[1], kLightPos[2]);
      glColor3f(1, 0, 1);
      glutSolidSphere(10.0f, 10, 10);
    }
    glPopMatrix();
  }
  glPopMatrix();

  glutSwapBuffers();
}

void MouseMotionHandler(int mx, int my) {
  kRot.y += 0.025 * (mx - kMP.x);
  kRot.x += -0.025 * (my - kMP.y);
  if (kRot.x > PI/2) { kRot.x = PI/2; }
  if (kRot.x < -PI/2) { kRot.x = -PI/2; }
  // update last mouse position
  kMP.x = mx;
  kMP.y = my;
  glutPostRedisplay();
}
void Timer(int value) {
  if (!kIsStop) {
    kGirlsAngle += 3;
  }

  glutPostRedisplay();
  glutTimerFunc(kUpdate_time, Timer, 0);
}
void MouseHandler(int button, int state, int x, int y) {
  if (button == GLUT_SCROLL_DOWN) { kLightRot.y -= kLightRotDelta; }
  if (button == GLUT_SCROLL_UP) { kLightRot.y += kLightRotDelta; }

  // renew the last position
  kMP.x = x;
  kMP.y = y;

  glutPostRedisplay();
}
void NormalKeyHandler(unsigned char key, int x, int y) {
  switch (key) {
    case 'q': kLightRot.x += kLightRotDelta; break;
    case 'z': kLightRot.x -= kLightRotDelta; break;
    case ' ': kIsStop = !kIsStop; break;
  }
  if ( kLightRot.x > PI/2) { kLightRot.x = PI/2; }
  if ( kLightRot.x < 0) { kLightRot.x = 0; }

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
  sphere = readObj("../obj/sphere.obj");
  girl = readObj("../obj/sphere.obj");
  master = readObj("../obj/monster.obj");
  load_image(&kTextures[0], "../texture/theworldif.jpg");
  load_image(&kTextures[1], "../texture/kjy01601.png");
  load_image(&kTextures[2], "../texture/monster.jpg");
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
