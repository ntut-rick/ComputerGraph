#include "main.hpp"

int update_time = 10;
int lightId = 0;
int windowx = 1920 / 2;
int windowy = 1080 - 40;

float mousex = 0;
float mousey = 0;
float r = 0;

bool stop = false;

GLfloat xRot = 0.0f;
GLfloat yRot = 1.57f;

GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
GLfloat diffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f};
GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPos[4][4] = {
    {-75.0f, 50.0f, 50.0f, 0.0f},
    {-75.0f, 100.0f, 50.0f, 0.0f},
    {-75.0f, 150.0f, 50.0f, 0.0f},
    {-75.0f, 200.0f, 50.0f, 0.0f},
};
GLfloat specref[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLuint textures[4];

Obj sphere;
Obj girl;
Obj master;

M3DMatrix44f shadowMat;

void Timer(int value)
{
  // RenderScene();
  glutPostRedisplay(); // Post re-paint request to activate display()

  if (!stop)
    r += 2;
  glutTimerFunc(update_time, Timer, 0); // next Timer call milliseconds later
}
void MouseHandler(int button, int state, int x, int y)
{

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    mousex = ((float)x / (float)windowx) * 2 - 1;
    mousey = -2 * ((float)y / (float)windowy) + 1;
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
  {
    mousex = ((float)x / (float)windowx) * 2 - 1;
    mousey = -2 * ((float)y / (float)windowy) + 1;
  }
  printf("mouse x=%.2f,y=%.2f\n", mousex, mousey);
  // xRot = mousex * 3.14;
  // yRot = mousey * 3.14;
}
void renderObj(Obj &obj)
{
  M3DVector3f vNormal;
  glBegin(GL_TRIANGLES);
  for (auto i = 0; i < obj.faces.size(); i++)
  {

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
void DrawJet(int nShadow)
{
  M3DVector3f vNormal; // Storeage for calculated surface normal

  float scale = 0.2;
  GLfloat white[4] = {1, 1, 1, 1.0};
  GLfloat shadowColor[4] = {10.0 / 255.0, 10.0 / 255.0, 40.0 / 255.0, 0.5};
  // printf("r=%.2f\n", r);

  // glTranslatef(0.0f, 10.0f, 0.0f);
  glPushMatrix();
  {

    // glPushMatrix();
    glScaled(scale, scale, scale);
    glTranslatef(0.0f, -5.0 * scale, 2.0f);
    glRotatef(r, 0, 1, 0);
    if (nShadow == 0)
    {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, textures[2]);
      glColor4fv(white);
    }
    else
    {
      glDisable(GL_TEXTURE_2D);
      glColor4fv(shadowColor);
    }
    renderObj(master);

    glPushMatrix();
    {
      // glScaled(scale, scale, scale);
      glTranslatef(0.0f, cos(r / 30.0), 1.5f);
      glRotatef(r, 0, 1, 0);

      if (nShadow == 0)
      {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glColor4fv(white);
      }
      else
      {
        glDisable(GL_TEXTURE_2D);
        glColor4fv(shadowColor);
      }

      renderObj(girl);
    }
    glPopMatrix();
  }
  glPopMatrix();
}

// This function does any needed initialization on the rendering
// context.
void SetupRC()
{

  GLfloat fAspect = (GLfloat)windowx / (GLfloat)windowy;

  glViewport(0, 0, windowx, windowy);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(120.0f, fAspect, 0.01, 100.0);

  float length = 100.0f;
  gluLookAt(0.0f, 0.0f, 0.0f,
            length * cos(yRot), length * sin(xRot), length * sin(yRot),
            0.0f, 1.0f, 0.0f);

  glLightfv(GL_LIGHT0, GL_POSITION, lightPos[lightId]);

  // Any three points on the ground (counter clockwise order)
  M3DVector3f points[3] = {{-30.0f, -1.0f, -20.0f},
                           {-30.0f, -1.0f, 20.0f},
                           {30.0f, -1.0f, 20.0f}};

  // glEnable(GL_DEPTH_TEST); // Hidden surface removal
  glFrontFace(GL_CCW);    // Counter clock-wise polygons face out
  glEnable(GL_CULL_FACE); // Do not calculate inside of jet

  // Setup and enable light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos[lightId]);
  glEnable(GL_LIGHT0);

  // Enable color tracking
  glEnable(GL_COLOR_MATERIAL);

  // Set Material properties to follow glColor values
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  // All materials hereafter have full specular reflectivity
  // with a high shine
  glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
  glMateriali(GL_FRONT, GL_SHININESS, 128);

  // Light blue background
  glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

  // Get the plane equation from three points on the ground
  M3DVector4f vPlaneEquation;
  m3dGetPlaneEquation(vPlaneEquation, points[0], points[1], points[2]);

  // Calculate projection matrix to draw shadow on the ground
  m3dMakePlanarShadowMatrix(shadowMat, vPlaneEquation, lightPos[lightId]);

  glEnable(GL_NORMALIZE);
}
// Called to draw scene
void RenderScene(void)
{
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
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos[lightId]);

  glPushMatrix();
  {

    // sphere
    float scale = 100;
    glPushMatrix();
    {
      glColor4f(1, 1, 1, 1);
      glScaled(scale, scale, scale);
      glBindTexture(GL_TEXTURE_2D, textures[0]);
      renderObj(sphere);
    }
    glPopMatrix();

    glPushMatrix();
    {
      DrawJet(0);
    }
    glPopMatrix();

    // Get ready to draw the shadow and the ground
    // First disable lighting and save the projection state
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    {
      glMultMatrixf((GLfloat *)shadowMat);
      DrawJet(1);
    }
    glPopMatrix();

    // Draw the light source
    glPushMatrix();
    {
      glTranslatef(lightPos[lightId][0], lightPos[lightId][1], lightPos[lightId][2]);
      glColor4f(1, 0, 1, 0.5);
      glutSolidSphere(10.0f, 10, 10);
    }
    glPopMatrix();
  }
  glPopMatrix();

  glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y)
{
  if (key == GLUT_KEY_UP)
    xRot += 0.05f;

  if (key == GLUT_KEY_DOWN)
    xRot -= 0.05f;

  if (key == GLUT_KEY_LEFT)
    yRot -= 0.05f;

  if (key == GLUT_KEY_RIGHT)
    yRot += 0.05f;

  glutPostRedisplay();
}
void NormalKeyHandler(unsigned char key, int x, int y)
{
  if (key == '1')
    lightId = 0;
  if (key == '2')
    lightId = 1;
  if (key == '3')
    lightId = 2;
  if (key == '4')
    lightId = 3;
  if (key == ' ')
    stop = !stop;

  printf("lightId: %d\n", lightId);

  glutPostRedisplay();
}
void ChangeSize(int w, int h)
{
  windowx = w;
  windowy = h;
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(windowx, windowy);
  glutCreateWindow("110590049 final");
  sphere = readObj("../obj/sphere.obj");
  girl = readObj("../obj/girl.obj");
  master = readObj("../obj/monster.obj");
  load_image(&textures[0], "../texture/balcony.png");
  load_image(&textures[1], "../texture/girl.jpeg");
  load_image(&textures[2], "../texture/monster.jpg");
  // Front Face (before rotation)
  glutReshapeFunc(ChangeSize);
  glutMouseFunc(MouseHandler);
  glutSpecialFunc(SpecialKeys);
  glutKeyboardFunc(NormalKeyHandler);
  glutDisplayFunc(RenderScene);

  // glutIdleFunc(Loop);
  glutTimerFunc(0, Timer, 0);
  glutMainLoop();

  return 0;
}
