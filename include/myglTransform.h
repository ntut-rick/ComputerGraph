#ifdef MY_GL_TRANSFORM_IMPLEMENTATION
#ifdef glRotatef
#undef glRotatef
#endif
void glRotatef(
   GLfloat angle,
   GLfloat x,
   GLfloat y,
   GLfloat z
) {
    float r = sqrt(x*x + y*y + z*z);
    x /= r;
    y /= r;
    z /= r;
    float c = cos(angle);
    float s = sin(angle);
    GLfloat m[16] = {
        x*x*(1-c)+c  , x*y*(1-c)-z*s, x*z*(1-c)+y*s, 0,
        y*x*(1-c)+z*s, y*y*(1-c)+c  , y*z*(1-c)-x*s, 0,
        z*x*(1-c)-y*s, z*y*(1-c)+x*s, z*z*(1-c)+c  , 0,
        0, 0, 0, 1
    };
    glMultMatrixf(m);
}

#ifdef glTranslatef
#undef glTranslatef
#endif
void glTranslatef(
   GLfloat x,
   GLfloat y,
   GLfloat z
) {
    GLfloat m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1
    };
    glMultMatrixf(m);
}

#ifdef glScalef 
#undef glScalef 
#endif
void glScalef(GLfloat x, GLfloat y, GLfloat z) {
    GLfloat m[16] = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
    glMultMatrixf(m);
}

#endif /* MY_GL_TRANSFORM_IMPLEMENTATION */
