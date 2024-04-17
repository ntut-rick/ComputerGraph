#include "loader.hpp"

#include <fstream>
#include <iostream>
#include <string>

int load_model(const std::filesystem::path &path, Model &model) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cerr << "File: " << path << " not found\n";
    return 1;
  }

  std::vector<Vertex> vertices;
  std::vector<Face> faces;

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string token;

    iss >> token;

    if (token == "v") {
      Vertex v;
      iss >> v.x >> v.y >> v.z;
      vertices.push_back(v);
      continue;
    }

    if (token == "f") {
      Face f;
      iss >> f.p0 >> f.p1 >> f.p2;
      faces.push_back(f);
      continue;
    }
  }

  file.close();

  model.vertices = std::move(vertices);
  model.faces = std::move(faces);
  return 0;
}
