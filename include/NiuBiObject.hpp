#ifndef DRAWABLEOBJECT_HPP
#define DRAWABLEOBJECT_HPP

#include <array>

#include "Transform.hpp"
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
  std::vector<Vertex> vertices;
  std::vector<Face> faces;
  static NiuBiObject fromFile(const std::string &filepath) {
    std::ifstream obj_file(filepath);
    if (!obj_file) {
      throw std::runtime_error("Can't open file: " + filepath);
    }

    NiuBiObject obj;

    std::string line{};
    while (getline(obj_file, line)) {
      if (line.empty()) {
        continue;
      }
      switch (line.front()) {
        case 'v': {
          float x, y, z;
          sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);
          obj.vertices.push_back({x, y, z});
        } break;
        case 'f': {
          int a, b, c;
          sscanf(line.c_str(), "f %i %i %i", &a, &b, &c);
          obj.faces.push_back({a-1, b-1, c-1});
        } break;
      } // switch
    } // while

    return obj;
  }
};

#endif