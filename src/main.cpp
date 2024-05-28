#include <array>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <functional>

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

static void my_glutcb_mousedrag(int, int);

static void mouseWheel(int, int, int, int);

static void MenuCallback(int);

static void ColorModeMenuCallback(int);

static void RenderModeMenuCallback(int);

static void TransformModeCallback(int);

static void LoadJykuoTexture();


struct {
    int width, height;
} window_size = {400, 400};

// fuck windows >>> https://cplusplus.com/forum/general/12435/
struct {
    GLfloat left, right, bottom, top, zNear, zFar;
}
        ortho_settings = {-10, 10, -10, 10, -100, 100};
struct {
    GLfloat x, y, z;
} camera_pos = {0, 0, 10};
struct {
    GLfloat x, y, z;
} camera_look_at = {0, 0, 0};

std::vector<NiuBiObject> loaded_objs;
int selected_obj_index = 0;

GLfloat objRotMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
GLfloat objTranMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
GLfloat cameraRotMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 10, 1};

GLenum render_mode = GL_TRIANGLES;
float zoomingScaler = 0.9;

int kDimenstion = 10;
struct {
    int x, y;
} selectedPosition = {0, 0};
struct vec2i_t {
    int x, y;
};
struct color_t {
    float r, g, b;

    static color_t random() {
      return {float(rand()) / RAND_MAX,
              float(rand()) / RAND_MAX,
              float(rand()) / RAND_MAX};
    }
};
struct colored_vec2i_t : public vec2i_t {
    color_t c;
};

#define POLYGON_N 12
#if POLYGON_N < 3
#error "Digon== nice try"
#endif

std::vector<colored_vec2i_t> kPoints;

std::queue<colored_vec2i_t> kQueue;
std::vector<colored_vec2i_t> kFilled;

/* Called back when timer expired */
void Timer(int value) {
  const unsigned int refreshMills = 10;

  if (!kQueue.empty()) {
    kFilled.push_back(kQueue.front());
//    printf("pop!\n");
    kQueue.pop();
  }

  glutPostRedisplay();      // Post re-paint request to activate display()
  glutTimerFunc(refreshMills, Timer, 0); // next Timer call milliseconds later
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(window_size.width, window_size.height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Niu-Bi de CG Midterm Work");

  glutCreateMenu(MenuCallback);
  for (int i = 10; i < 1000; i += 5) {
    auto a = std::to_string(i);
    glutAddMenuEntry(a.c_str(), i);
  }
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutReshapeFunc(ChangeSize);
  glutKeyboardFunc(OnKeyBoardPress);

  glutMouseWheelFunc(mouseWheel);
  glutMouseFunc(MousePress);
  glutMotionFunc(my_glutcb_mousedrag);
  glutTimerFunc(0, Timer, 0);

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
  unsigned char *imageData = stbi_load("./kjy01601.png", &width, &height, &channels, 0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (imageData) {
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
  glColor3f(1, 1, 1);
}

void randomcolor(void) {
  float r, g, b;
  r = (float) rand() / RAND_MAX;
  g = (float) rand() / RAND_MAX;
  b = (float) rand() / RAND_MAX;
  glColor3f(r, g, b);
}

void (*setcolorfuncion)(void) = singlecolor;

void drawSelectedOjbect(void) {
  if (selected_obj_index >= loaded_objs.size()) {
    return;
  }
  auto curret_obj = loaded_objs[selected_obj_index];
  // srand(7414);
  for (auto face: curret_obj.faces) {
    setcolorfuncion();
    glBegin(render_mode);
    // Deprecated uv setup
    // glTexCoord2f(.5f, .0f); glVertex3fv(curret_obj.vertices[face[0]].data());
    // glTexCoord2f(.35f, .6f); glVertex3fv(curret_obj.vertices[face[1]].data());
    // glTexCoord2f(.65f, .6f); glVertex3fv(curret_obj.vertices[face[2]].data());
    glTexCoord2f(.5f, .55f);
    glVertex3fv(curret_obj.vertices[face[0]].data());
    glTexCoord2f(.3f, .3f);
    glVertex3fv(curret_obj.vertices[face[1]].data());
    glTexCoord2f(.7f, .3f);
    glVertex3fv(curret_obj.vertices[face[2]].data());
    glEnd();
    // Debugging black kLines
    // glColor3f(0,0,0);
    // glBegin(GL_LINE_LOOP);
    //   glVertex3fv(curret_obj.vertices[face[0]].data());
    //   glVertex3fv(curret_obj.vertices[face[1]].data());
    //   glVertex3fv(curret_obj.vertices[face[2]].data());
    // glEnd();
  }
}

void drawPoint(int x, int y, color_t c = {1, 1, 1}, GLenum mode = GL_QUADS) {
  const auto realsize = kDimenstion * 2 + 1;
  const float size = 20.0f / realsize;
  const float halfsize = size / 2;
  glColor3f(c.r, c.g, c.b);
  glBegin(mode);
  glVertex3f(x * size - halfsize, y * size - halfsize, 0);
  glVertex3f(x * size - halfsize, y * size + halfsize, 0);
  glVertex3f(x * size + halfsize, y * size + halfsize, 0);
  glVertex3f(x * size + halfsize, y * size - halfsize, 0);
  glEnd();
}

void drawPoint2(int x, int y, color_t c) {
  drawPoint(x, y, c);
}

color_t colored_vec2i_lerp(colored_vec2i_t a, colored_vec2i_t b, vec2i_t mid) {
  // Calculate distances between the points a, b and mid
  float delta = sqrt((mid.x - a.x) * (mid.x - a.x) + (mid.y - a.y) * (mid.y - a.y));
  float total = sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));

  // Normalize delta if total is not zero to avoid division by zero
  float t = (total != 0.0f) ? (delta / total) : 0.0f;

  // Interpolating color based on normalized distance t
  return {
    a.c.r + t * (b.c.r - a.c.r),
    a.c.g + t * (b.c.g - a.c.g),
    a.c.b + t * (b.c.b - a.c.b)
  };
}

void drawColoredLine(
        colored_vec2i_t start, colored_vec2i_t end,
        void (*drawPointFunc)(int, int, color_t)
){
  const auto X1 = start.x;
  const auto Y1 = start.y;
  const auto X2 = end.x;
  const auto Y2 = end.y;

  // transform to quadrant I
  int magic_number_x = X2 - X1 > 0 ? 1 : -1;
  int magic_number_y = Y2 - Y1 > 0 ? 1 : -1;

  int dx = (X2 - X1) * magic_number_x;
  int dy = (Y2 - Y1) * magic_number_y;

  // start point / init point
  drawPointFunc(start.x, start.y, start.c);
  drawPointFunc(end.x, end.y, end.c);

  if (dy <= dx) {
    int d = dy - (dx / 2);
    int y = 0;
    for (int x = 1; x < dx; x++) {
      if (d < 0) {
        // E or East is chosen
        d = d + dy;
      } else {
        // NE or North East is chosen
        d += (dy - dx);
        y++;
      }
      const auto curr = vec2i_t(
              X1 + x * magic_number_x,
              Y1 + y * magic_number_y);
      drawPointFunc(curr.x, curr.y, colored_vec2i_lerp(start, end, curr));
    }
  } else {
    int d = dx - (dy / 2);
    int x = 0;
    for (int y = 1; y < dy; y++) {
      if (d < 0) {
        // E or East is chosen
        d = d + dx;
      } else {
        // NE or North East is chosen
        d += (dx - dy);
        x++;
      }
      const auto curr = vec2i_t(
              X1 + x * magic_number_x,
              Y1 + y * magic_number_y);
      drawPointFunc(curr.x, curr.y, colored_vec2i_lerp(start, end, curr));
    }
  }
}

// ref: https://www.geeksforgeeks.org/mid-point-line-generation-algorithm/
void drawLine(int X1, int Y1, int X2, int Y2) {
  // transform to quadrant I
  int magic_number_x = X2 - X1 > 0 ? 1 : -1;
  int magic_number_y = Y2 - Y1 > 0 ? 1 : -1;

  int dx = (X2 - X1) * magic_number_x;
  int dy = (Y2 - Y1) * magic_number_y;

  // start point / init point
  drawPoint(X1, Y1);

  if (dy <= dx) {
    int d = dy - (dx / 2);
    int y = 0;
    for (int x = 1; x < dx; x++) {
      if (d < 0) {
        // E or East is chosen 
        d = d + dy;
//        glColor3f(0, 1, 0);
      } else {
        // NE or North East is chosen 
        d += (dy - dx);
        y++;
//        glColor3f(0, 0, 1);
      }
      drawPoint(
              X1 + x * magic_number_x,
              Y1 + y * magic_number_y);
    }
  } else {
    int d = dx - (dy / 2);
    int x = 0;
    for (int y = 1; y < dy; y++) {
      if (d < 0) {
        // E or East is chosen 
        d = d + dx;
        glColor3f(0, 1, 0);
      } else {
        // NE or North East is chosen 
        d += (dx - dy);
        x++;
        glColor3f(0, 0, 1);
      }
      drawPoint(
              X1 + x * magic_number_x,
              Y1 + y * magic_number_y);
    }
  }
}

int determinant(vec2i_t p1, vec2i_t p2, vec2i_t p) {
  // | p1.x p1.y 1 |
  // | p2.x p2.y 1 |
  // | p.x  p.y  1 |
  return p1.x * (p2.y - p.y) +
         p2.x * (p.y - p1.y) +
         p.x * (p1.y - p2.y);
}

void pushDrawPoint(int x, int y, color_t c) {
//  printf("push!\n");
  kQueue.push({x, y, c});
}

[[deprecated("I broken the API==")]]
void fillTriangle(
        vec2i_t p1, vec2i_t p2, vec2i_t p3,
        void (*drawPointFunc)(int, int)
) {
  auto top = std::max({p1.y, p2.y, p3.y});
  auto bottom = std::min({p1.y, p2.y, p3.y});
  auto right = std::max({p1.x, p2.x, p3.x});
  auto left = std::min({p1.x, p2.x, p3.x});
  // https://en.wikipedia.org/wiki/Triple_product
  // https://en.wikipedia.org/wiki/Area_of_a_triangle
  for (auto x = left; x <= right; ++x)
    for (auto y = bottom; y <= top; ++y) {
      if (
              determinant(p1, p2, {x, y}) > 0
              && determinant(p2, p3, {x, y}) > 0
              && determinant(p3, p1, {x, y}) > 0
              ) { // CCW
        glColor3f(1, 1, 0);
        drawPointFunc(x, y);
      }
      if (
              determinant(p1, p2, {x, y}) < 0
              && determinant(p2, p3, {x, y}) < 0
              && determinant(p3, p1, {x, y}) < 0
              ) { // CW
        glColor3f(1, 0, 1);
        drawPointFunc(x, y);
      }
    }
}

struct linevec2i_t {
    vec2i_t start, end;
};

template<std::size_t N>
bool all_in_left(
        std::array<vec2i_t, N> pts,
        vec2i_t tested,
        std::function<bool(linevec2i_t, vec2i_t)> predicate
) {
  // sliding window, size = 2
  for (size_t i = 0; i <= pts.size() - 2; ++i) {
    if (!predicate({pts[i], pts[i + 1]}, tested)) {
      return false;
    }
  }
  if (!predicate({pts[pts.size()], pts[0]}, tested)) {
    return false;
  }
  return true;
}
auto Compare = [](colored_vec2i_t l, colored_vec2i_t r) { return l.y < r.y; };
std::priority_queue<
        colored_vec2i_t, std::vector<colored_vec2i_t>,
        std::function<bool(colored_vec2i_t, colored_vec2i_t)>
> kpqueue(Compare);

template<std::size_t N>
void fillPolygon(
        std::array<colored_vec2i_t, N> pts,
        void (*drawPointFunc)(int, int, color_t) = [](int x, int y, color_t c){drawPoint(x,y);}
) {
//  std::array<int, N> y_coords;
//  std::array<int, N> x_coords;
//  std::transform(pts.cbegin(), pts.cend(), y_coords.begin(), [](const colored_vec2i_t &p) { return p.y; });
//  std::transform(pts.cbegin(), pts.cend(), x_coords.begin(), [](const colored_vec2i_t &p) { return p.x; });
//  auto top = *std::max_element(y_coords.cbegin(), y_coords.cend());
//  auto bottom = *std::min_element(y_coords.cbegin(), y_coords.cend());
//  auto right = *std::max_element(x_coords.cbegin(), x_coords.cend());
//  auto left = *std::min_element(x_coords.cbegin(), x_coords.cend());

  auto isPointInPolygon = [&](int x, int y) {
      bool inside = false;
      for (std::size_t i = 0, j = N - 1; i < N; j = i++) {
        if (((pts[i].y > y) != (pts[j].y > y)) &&
            (x < (pts[j].x - pts[i].x) * (y - pts[i].y) / (pts[j].y - pts[i].y) + pts[i].x)) {
          inside = !inside;
        }
      }
      return inside;
  };
  // clear queue
  while(!kpqueue.empty()) {kpqueue.pop();}
  auto enqueuer = [](int x, int y, color_t c){kpqueue.push({x, y, c});};
  for (std::size_t i = 0; i < N-1; ++i) {
    drawColoredLine(pts[i], pts[i+1], enqueuer);
  }
  drawColoredLine(pts[N-1], pts[0], enqueuer);

  auto lasttop = kpqueue.top();
  kpqueue.pop();
  while(!kpqueue.empty()) {
    if (kpqueue.top().y == lasttop.y) {
      drawColoredLine(kpqueue.top(), lasttop, drawPointFunc);
    }
    lasttop = kpqueue.top();
    kpqueue.pop();
  }

//  for (auto y = bottom; y <= top; ++y) {
//    for (auto x = left; x <= right; ++x) {
//      if (isPointInPolygon(x, y)) {
//        drawPointFunc(x, y, {1,1,1});
//      }
//    }
//  }
}

void drawGrid() {
  const auto realsize = kDimenstion * 2 + 1;
  const float size = 20.0f / realsize;
  const float halfsize = size / 2;
  for (int i = -kDimenstion; i <= kDimenstion; i++)
    for (int j = -kDimenstion; j <= kDimenstion; j++) {
      drawPoint(i, j, {1,1,1}, GL_LINE_LOOP);
    }
}

void RenderScene(void) {
  glClearColor(0, 0, 0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // init the viewport
  glViewport(0, 0, window_size.width, window_size.height);

  glMatrixMode(GL_PROJECTION);
  ortho_settings.left = -10 / zoomingScaler;
  ortho_settings.right = 10 / zoomingScaler;
  ortho_settings.bottom = -10 / zoomingScaler;
  ortho_settings.top = 10 / zoomingScaler;
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

  glColor3f(1, 1, 1);
  drawGrid();

  glColor3f(1, 0, 0);

  // draw points
  for (const auto &p: kPoints) {
    drawPoint(p.x, p.y, p.c);
  };
//  for (int i = 0; i < kPoints.size() / POLYGON_N; ++i) {
//    glColor3f(0, 1, 0);
//    const auto r = kPoints.cbegin() + i * POLYGON_N;
//    for (int j = 0; j < POLYGON_N - 1; ++j) {
//      drawLine(r[j].x, r[j].y, r[j + 1].x, r[j + 1].y);
//    }
//    drawLine(r[POLYGON_N - 1].x, r[POLYGON_N - 1].y, r[0].x, r[0].y);
//    // fillTriangle(r[0], r[1], r[2]);
//  };
  glColor3f(1, 1, 0);
  for (int i = 0; i < kFilled.size(); ++i) {
    const auto r = kFilled[i];
    drawPoint(r.x, r.y, r.c);
  }
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

void magicTransform(int wx, int wy, int *x, int *y) {
  GLdouble ox, oy, oz;
  myUnproject(wx, wy, 0, &ox, &oy, &oz);

  const float len = 10.0f / kDimenstion;
  // printf("%f\n", len);
  *x = (int) (ox > 0 ? ox / len + 0.5 : ox / len - 0.5);
  *y = (int) (-oy > 0 ? -oy / len + 0.5 : -oy / len - 0.5);
  // printf("raw xy (%lf,%lf)\n", wx, wy);
  // printf("    xy (%d,%d)\n", selectedPosition.x, selectedPosition.y);
}

void MousePress(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    vec2i_t a;
    magicTransform(x, y, &a.x, &a.y);
    const auto c = color_t::random();
    kPoints.push_back({a.x, a.y,c });
    static size_t f = 0;

    if (kPoints.size() >= f + POLYGON_N) {
      printf("compltate 1 polygon\n");
      // fillTriangleFans(&(kPoints[f]), POLYGON_N, pushDrawPoint);

      std::array<colored_vec2i_t, POLYGON_N> pts;
      const auto &first = kPoints.cbegin() + f;
      std::copy(first, first + POLYGON_N, pts.begin());
      fillPolygon(pts, pushDrawPoint);

      f += POLYGON_N;
    }
  }
  glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y) {
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

void my_glutcb_mousedrag(int x, int y) {
}

void MenuCallback(int value) {
  std::cout << "dim: " << value << std::endl;
  kDimenstion = value;
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
      glTranslatef(0, 0, 10);
      glGetFloatv(GL_MODELVIEW_MATRIX, cameraRotMatrix);
      glPopMatrix();
      break;
  }
  glutPostRedisplay();
}