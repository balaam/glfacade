// facade cpp

#include "VitaGLFacade.h"
#include <vector>
#include <math.h>
#include <assert.h>
#include <stdio.h>

// TEMP
#include <gl/gl.h>
#include <gl/glext.h>

// Not as efficent as having an array of stacks but clearer.
std::vector<Matrix4>& VitaGLFacade::GetSelectedStack()
{
    if(mMatrixMode == GL_PROJECTION)
    {
        return projectStack;
    }
    else
    {
        assert(mMatrixMode == GL_MODELVIEW);
        return matrixStack;
    }
}

VitaGLFacade::VitaGLFacade()
{
    mMatrixMode = GL_PROJECTION; // this seems to be the default mode.
}

void VitaGLFacade::PrintMatrix(const Matrix4& mat) const
{
    // Better to implement this than add a function to matrix
    Vector4 col0 = mat.getCol0();
    Vector4 col1 = mat.getCol1();
    Vector4 col2 = mat.getCol2();
    Vector4 col3 = mat.getCol3();

    const char * format = "%.5f\t%.5f\t%.5f\t%.5f\n";
    printf(format, col0.getX(), col1.getX(), col2.getX(), col3.getX());
    printf(format, col0.getY(), col1.getY(), col2.getY(), col3.getY());
    printf(format, col0.getZ(), col1.getZ(), col2.getZ(), col3.getZ());
    printf(format, col0.getW(), col1.getW(), col2.getW(), col3.getW());
}

void VitaGLFacade::glMatrixMode(int mode)
{
    assert(mode == GL_PROJECTION || mode == GL_MODELVIEW);
    mMatrixMode = mode;
}

void VitaGLFacade::glPushMatrix()
{
    std::vector<Matrix4>& matStack = GetSelectedStack();
    // This isn't super efficent
    Matrix4 mat;
    mat = mat.identity();
    matStack.push_back(mat);
}

void VitaGLFacade::glPopMatrix()
{
    std::vector<Matrix4>& matStack = GetSelectedStack();
    if(matStack.empty())
    {
        return;
    }

    matStack.pop_back();
}

Matrix4& VitaGLFacade::_Top()
{
    std::vector<Matrix4>& matStack = GetSelectedStack();
    //
    // Should always have an identity matrix at bottom of the stack
    //
    if(matStack.size() == 0)
    {
        Matrix4 mat;
        mat = mat.identity();
        matStack.push_back(mat);
    }
    return matStack.back();
}

const Matrix4& VitaGLFacade::Top()
{
    return _Top();
}

// There's a lot of matrix creation that can be trimmed down here.
void VitaGLFacade::glTranslatef(float x, float y, float z)
{
    // glTranslate produces a translation by (x, y, z) .
    // The current matrix (see glMatrixMode) is multiplied by this translation
    // matrix, with the product replacing the current matrix, as if glMultMatrix
    // were called with the following matrix for its argument:

    // (
    //     1   0   0   x
    //     0   1   0   y
    //     0   0   1   z
    //     0   0   0   1
    // )


    Matrix4 mat;
    mat = mat.identity();
    mat.setTranslation(Vector4(x, y, z, 1));
    Matrix4& top = _Top();
    top = top * mat;
}

//#define M_PI 3.14159265
float DegreeToRadian(float angle)
{
   return M_PI * angle / 180.0;
}

float RadianToDegree(float angle)
{
   return angle * (180.0 / M_PI);
}

// Look to replace this with Vita matrix rotations
void VitaGLFacade::glRotatef(float angle, float x, float y, float z)
{
    // glRotate produces a rotation of angle degrees around the vector (x, y, z) .
    // The current matrix (see glMatrixMode) is multiplied by a rotation matrix
    // with the product replacing the current matrix


    // If it's not normal - it nees to be normalised!

    float c = cos(DegreeToRadian(angle));
    float s = sin(DegreeToRadian(angle));
    // (
    //     x^2 (1 - c) + c       xy (1 - c) - zs     xz (1 - c) + ys     0
    //     xy (1 - c) + zs      y^2 (1 - c) + c      yz (1 - c) - xs     0
    //     xz (1 - c) - ys      yz (1 - c) + xs     z^2 (1 - c) + c      0
    //     0                    0                   0                   1
    // )

    // Your assigning columns to rows

    // Matrix4 mat = Matrix4(
    //     Vector4((x * x) * (1 - c) + c,       (x * y) * (1 - c) - (z * s), (x * z) * (1 - c) + (y * s),  0),
    //     Vector4((x * y) * (1 - c) + (z * s), ((y * y) * (1 - c)) + c,       (y * z) * (1 - c) - (x * s),  0),
    //     Vector4((x * z) * (1 - c) - (y * s), ((y * z) * (1 - c)) + (x * s), (z * z) * (1 - c) + c,        0),
    //     Vector4(0,                           0,                           0,                            1));

    Matrix4 mat = Matrix4(
            Vector4((x * x) * (1 - c) + c, (x * y) * (1 - c) + (z * s), (x * z) * (1 - c) - (y * s), 0),
            Vector4((x * y) * (1 - c) - (z * s), ((y * y) * (1 - c)) + c, ((y * z) * (1 - c)) + (x * s), 0),
            Vector4((x * z) * (1 - c) + (y * s), (y * z) * (1 - c) - (x * s), (z * z) * (1 - c) + c, 0),
            Vector4(0, 0, 0, 1)
        );
    // do the rotation
    Matrix4& top = _Top();
    top = top * mat;

    // OpenGL
    // 0.00    8.00    -4.00   212.00
    // 1.00    9.00    -5.00   313.00
    // 2.00    10.00   -6.00   414.00
    // 3.00    11.00   -7.00   515.00

    // Facade
    // 0.00    4.00    8.00    212.00
    // 1.34    3.12    4.91    229.87
    // -1.79   -7.16   -12.53  -465.32
    // 3.00    7.00    11.00   515.00

}

void VitaGLFacade::glLoadMatrixf(float *m)
{
    // glLoadMatrix replaces the current matrix with the one specified in m.
    // The current matrix is the projection matrix, modelview matrix, or
    // texture matrix, determined by the current matrix mode (see glMatrixMode).

    // m points to a 4x4 matrix of single-
    // or double-precision floating-point values stored in column-major order
    Matrix4& top = _Top();

    // On Vita can do this without recreating the matrix.
    // Though this function is only used for debug so it does't matter.
    top = Matrix4(
        Vector4(m[0], m[1], m[2], m[3]),
        Vector4(m[4], m[5], m[6], m[7]),
        Vector4(m[8], m[9], m[10], m[11]),
        Vector4(m[12], m[13], m[14], m[15])
    );
}

void VitaGLFacade::glOrtho(float left, float right, float bottom, float top, float near, float far)
{
    // glOrtho describes a transformation that produces a parallel projection.
    // The current matrix (see glMatrixMode) is multiplied by this matrix and
    // the result replaces the current matrix, as if glMultMatrix were called
    // with the following matrix as its argument:

    // (
    //      2/(right - left)    0   0   tx
    //      0   2/(top - bottom)    0   ty
    //      0   0   ( -2)/(far - near)  tz
    //      0   0   0   1
    // )

    // where

    // tx = - (right + left)/(right - left)
    // ty = - (top + bottom)/(top - bottom)
    // tz = - (far + near)/(far - near)

    float tx = - (right + left) / (right - left);
    float ty = - (top + bottom) / (top - bottom);
    float tz = - (far + near) / (far - near);

       Matrix4 mat = Matrix4(
            Vector4(2/(right - left) , 0, 0, 0),
            Vector4(0, 2/(top - bottom), 0, 0),
            Vector4(0, 0, ( -2)/(far - near), 0),
            Vector4(tx, ty, tz, 1)
        );
    Matrix4& topM = _Top();
    topM = topM * mat;
}


void VitaGLFacade::glScalef(float x, float y, float z)
{
    // glScale produces a nonuniform scaling along the x, y, and z axes.
    // The three parameters indicate the desired scale factor along each of the three axes.

    // The current matrix (see glMatrixMode) is multiplied by this scale matrix,
    // and the product replaces the current matrix as if glScale were called
    // with the following matrix as its argument:

    // (
    //      x   0   0   0
    //      0   y   0   0
    //      0   0   z   0
    //      0   0   0   1
    // )

    Matrix4 mat = Matrix4(
            Vector4(x, 0, 0, 0),
            Vector4(0, y, 0, 0),
            Vector4(0, 0, z, 0),
            Vector4(0, 0, 0, 1)
        );
    Matrix4& top = _Top();
    top = top * mat;
}

void VitaGLFacade::glLoadIdentity()
{
	// glLoadIdentity replaces the current matrix with the identity matrix.
	Matrix4& top = _Top();
    top = top.identity();
}

Matrix4 VitaGLFacade::CalcModelViewMatrix()
{
	Matrix4 mat;
	mat = mat.identity();
	for(int i = 0; i < (int)matrixStack.size(); i++)
	{
		mat = mat * matrixStack[i];
	}
	return mat;
}

Matrix4 VitaGLFacade::CalcProjMatrix()
{
    Matrix4 mat;
    mat = mat.identity();
    for(int i = 0; i < (int)projectStack.size(); i++)
    {
        mat = mat * projectStack[i];
    }
    return mat;
}

Matrix4 VitaGLFacade::CalcModelViewProjMatrix()
{
    return CalcProjMatrix() * CalcModelViewMatrix();
}
