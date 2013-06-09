// Vita OpenGL Facade
#ifndef VITAGLFACADE_H
#define VITAGLFACADE_H
#include "VitaMath.h" // because I want my stacks to directly reference the Matrix
#include <vector>
// Recreate some OpenGL functionality for the Vita
// #define GL_MATRIX_MODE  0x0BA0
// #define GL_MODELVIEW    0x1700
// #define GL_PROJECTION   0x1701
// #define GL_TEXTURE  0x1702
class VitaGLFacade
{
private:
    std::vector<Matrix4> matrixStack;
    std::vector<Matrix4> projectStack;
    std::vector<Matrix4>& GetSelectedStack();
    Matrix4& _Top();
    int mMatrixMode;
public:
     VitaGLFacade();
     void glMatrixMode(int mode);
     void glPushMatrix();
     void glPopMatrix();
     void glTranslatef(float x, float y, float z);
     void glRotatef(float angle, float x, float y, float z);
     void glScalef(float x, float y, float z);
     void glLoadMatrixf(float *m);
     void glLoadIdentity();
     void glOrtho(float left, float right, float bottom, float top, float near, float far);
     void PrintMatrix(const Matrix4& mat) const;
     Matrix4 CalcModelViewMatrix();
     Matrix4 CalcProjMatrix();
     Matrix4 CalcModelViewProjMatrix();
     const Matrix4& Top();
};
#endif
