#ifndef MY_UTILS_H
#define MY_UTILS_H

// Function to project window coordinates onto a fixed y position in world space
void projectToFixedYPlane(
    double winX, double winY,
    double yFixed,
    double *worldX, double *worldY, double *worldZ);

#endif /* MY_UTILS_H */
