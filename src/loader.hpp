#include <array>
#include <filesystem>
#include <vector>

struct Vertex {
  float x, y, z;
};

struct Face {
  unsigned int x, y, z;
};

struct Model {
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
};

int load_model(const std::filesystem::path &path, Model &model);
