
/*** freeglut***/
#ifdef _MSC_VER
#include "windows.h"
#pragma comment(lib, "opengl32.lib")
#endif

#include <GLTools.h> // OpenGL toolkit
#include <math3d.h>
#include <GL/gl.h>
#include <stdio.h>
#include <freeglut.h>
#include <freeglut_std.h>

#include "time.h"

#include "obj.hpp"
#include <vector>
#include <random>

#include <iostream>
#include <math.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void load_image(GLuint *textures, const char *path)
{
    int width, height, channels;
    unsigned char *imageData = stbi_load(path, &width, &height, &channels, 0);

    glGenTextures(1, textures);
    glBindTexture(GL_TEXTURE_2D, *textures);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     width, height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        stbi_image_free(imageData);
        printf("%s success\n", path);
    }
    else
    {
        printf("%s fail\n", path);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, *textures);
}