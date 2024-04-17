#include <filesystem>
#include <vector>

struct Vertex {
  float x, y, z;
};

struct Face {
  unsigned int p0, p1, p2;
};

struct Model {
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
};

int load_model(const std::filesystem::path &path, Model &model);
