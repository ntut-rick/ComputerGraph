#include "utils.h"

#include "freeglut.h"
#include "freeglut_std.h"
#include <GL/gl.h>

#include <stdio.h>

// Function to intersect a ray with the plane y = yFixed
void intersectRayWithYPlane(double rayStart[3], double rayEnd[3], double yFixed, double *intersection) {
    // Ray direction
    double dir[3] = {
        rayEnd[0] - rayStart[0],
        rayEnd[1] - rayStart[1],
        rayEnd[2] - rayStart[2]
    };

    // Ensure the ray is not parallel to the plane
    if (dir[1] == 0) {
        #ifdef DEBUG_PROJECT
        printf("[DEBUG_PROJECT]Ray is parallel to the y=%f plane.\n", yFixed);
        #endif
        return;
    }

    // Calculate the intersection point
    double t = (yFixed - rayStart[1]) / dir[1];
    intersection[0] = rayStart[0] + t * dir[0];
    intersection[1] = yFixed;  // y-coordinate is yFixed on the plane
    intersection[2] = rayStart[2] + t * dir[2];
}

// Function to project window coordinates onto a fixed y position in world space
void projectToFixedYPlane(double winX, double winY, double yFixed, double *worldX, double *worldY, double *worldZ) {
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    // Retrieve the current matrices and viewport
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Get the depth value at the window position
    GLfloat winZ;
    glReadPixels((int)winX, viewport[3] - (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    // Unproject window coordinates to world coordinates at the near and far planes
    GLdouble nearPoint[3], farPoint[3];
    gluUnProject(winX, viewport[3] - winY, 0.0, modelview, projection, viewport, &nearPoint[0], &nearPoint[1], &nearPoint[2]);
    gluUnProject(winX, viewport[3] - winY, 1.0, modelview, projection, viewport, &farPoint[0], &farPoint[1], &farPoint[2]);

    // Calculate the intersection of the ray with the plane y = yFixed
    double intersection[3];
    intersectRayWithYPlane(nearPoint, farPoint, yFixed, intersection);

    // Output the intersection coordinates
    *worldX = intersection[0];
    *worldY = intersection[1];
    *worldZ = intersection[2];
    #ifdef DEBUG_PROJECT
    printf("[DEBUG_PROJECT]World coordinates at y=%f: (%f, %f, %f)\n", yFixed, *worldX, *worldY, *worldZ);
    #endif
}
