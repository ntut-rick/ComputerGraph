#ifndef DRAWABLEOBJECT_HPP
#define DRAWABLEOBJECT_HPP

#include <array>

#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

using Vertex = std::array<GLfloat, 3>;
using Face = std::array<int, 3>;

class NiuBiObject {
public:
  std::string name;
  float scale;
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
  static NiuBiObject fromFile(const std::string &filepath) {
    std::ifstream obj_file(filepath);
    if (!obj_file) {
      throw std::runtime_error("Can't open file: " + filepath);
    }

    NiuBiObject obj;

    std::string line{};
    GLfloat max_b = std::numeric_limits<GLfloat>::min();
    GLfloat min_b = std::numeric_limits<GLfloat>::max();
    while (getline(obj_file, line)) {
      if (line.empty()) {
        continue;
      }
      switch (line.front()) {
        case 'v': {
          float x, y, z;
          sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
          min_b = std::min(x, min_b);
          min_b = std::min(y, min_b);
          min_b = std::min(z, min_b);
          max_b = std::max(x, max_b);
          max_b = std::max(y, max_b);
          max_b = std::max(z, max_b);
          obj.vertices.push_back({x, y, z});
        } break;
        case 'f': {
          int a, b, c;
          sscanf(line.c_str(), "f %i %i %i", &a, &b, &c);
          obj.faces.push_back({a-1, b-1, c-1});
        } break;
      } // switch
    } // while
    obj.name = filepath;
    obj.scale = 20.0f / (max_b-min_b);
    std::cout << filepath << " scaler: " << obj.scale << std::endl;
    return obj;
  }
};

#endif