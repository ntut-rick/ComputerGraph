#ifdef IMPL_TRANSFORM_YOURSELF

static void rotateX(GLfloat x) {
    GLfloat m[16] = {
        1, 0, 0, 0,
        0, cos(x), -sin(x), 0,
        0, sin(x), cos(x), 0,
        0, 0, 0, 1
    };
    glMultMatrixf(m);
}

static void rotateY(GLfloat y) {
    GLfloat m[16] = {
        cos(y), 0, sin(y), 0,
        0, 1, 0, 0,
        -sin(y), 0, cos(y), 0,
        0, 0, 0, 1
    };
    glMultMatrixf(m);
}

static void rotateZ(GLfloat z) {
    GLfloat m[16] = {
        cos(z), -sin(z), 0, 0,
        sin(z), cos(z), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    glMultMatrixf(m);
}

#ifdef glRotatef
#undef glRotatef
#endif
void glRotatef(
   GLfloat angle,
   GLfloat x,
   GLfloat y,
   GLfloat z
) {
    if (x==1) {
        rotateX(angle);
        return;
    }
    if (y==1) {
        rotateY(angle);
        return;
    }
    if (z==1) {
        rotateZ(angle);
        return;
    }
    fprintf(stderr, "Sorry I didn't implement this :(\n");
    exit(-1);
    // GLfloat m[16] = {
    //     x*x-, 0, 0, 0,
    //     0, 1, 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1
    // };
    // glMultMatrixf(m);
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

#endif /* IMPL_TRANSFORM_YOURSELF */
