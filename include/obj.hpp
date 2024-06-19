#ifndef OBJ_HPP
#define OBJ_HPP

#include <sstream>
#include <fstream>
#include <vector>
#include <random>
// #include <stdio>

struct Vector3d
{
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
};
struct Vector2d
{
    float x = 0.0;
    float y = 0.0;
};

struct FaceIndices
{
    int v[3] = {-1, -1, -1};
    int uv[3] = {-1, -1, -1};
};

class BoundBox
{
public:
    Vector3d center = {0, 0, 0};
    Vector3d right_top;
    Vector3d left_bottom;
    bool pushed = false;
    void push_point(Vector3d vertex)
    {
        if (!pushed)
        {
            right_top = vertex;
            left_bottom = vertex;
            pushed = true;
        }
        if (vertex.x > right_top.x)
        {
            right_top.x = vertex.x;
        }
        else if (vertex.x < left_bottom.x)
        {
            left_bottom.x = vertex.x;
        }
        if (vertex.y > right_top.y)
        {
            right_top.y = vertex.y;
        }
        else if (vertex.y < left_bottom.y)
        {
            left_bottom.y = vertex.y;
        }
        if (vertex.z > right_top.z)
        {
            right_top.z = vertex.z;
        }
        else if (vertex.z < left_bottom.z)
        {
            left_bottom.z = vertex.z;
        }
    }
    void update_center()
    {
        center = {left_bottom.x + (right_top.x - left_bottom.x) / 2, left_bottom.y + (right_top.y - left_bottom.y) / 2, left_bottom.z + (right_top.z - left_bottom.z) / 2};
    }
    float get_radius()
    {
        this->update_center();
        return sqrt(pow(right_top.x - center.x, 2) + pow(right_top.y - center.y, 2) + pow(right_top.z - center.z, 2));
    }
};

float generateRandomFloat(int seed)
{
    std::mt19937 gen(seed);                                 // Mersenne Twister random number engine with seed
    std::uniform_real_distribution<float> dist(0.0f, 1.0f); // Uniform distribution between 0.0 and 1.0
    return dist(gen);
}

struct Obj
{
    std::vector<Vector3d> vertices;
    std::vector<FaceIndices> faces;
    std::vector<Vector2d> uvs;
    BoundBox box;
};
Obj readObj(const std::string &filename)
{
    std::ifstream in(filename);
    std::string line;

    Obj obj;

    while (std::getline(in, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            Vector3d vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            // printf("x=%.2f,y=%.2f,z=%.2f\n", vertex.x, vertex.y, vertex.z);

            obj.box.push_point(vertex);

            obj.vertices.push_back(vertex);
        }
        else if (prefix == "f")
        {
            FaceIndices face;

            for (int i = 0; i < 3; i++)
            {
                iss >> face.v[i];
                face.v[i]--;
                if (iss.peek() == '/')
                {
                    iss.ignore();
                    iss >> face.uv[i];
                    face.uv[i]--;
                }
                if (iss.peek() == '/')
                {
                    int temp;
                    iss.ignore();
                    iss >> temp;
                }
            }

            // printf("v1=%d,v2=%d,v3=%d\n", face.v1, face.v2, face.v3);

            obj.faces.push_back(face);
        }
        else if (prefix == "vt")
        {
            Vector2d uv;
            iss >> uv.x >> uv.y;
            // printf("v1=%d,v2=%d,v3=%d\n", face.v1, face.v2, face.v3);
            obj.uvs.push_back({uv.x, uv.y});
        }
        else
        {
            iss.ignore();
        }
    }
    obj.box.update_center();
    return obj;
}
#endif