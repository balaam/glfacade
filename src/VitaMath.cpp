#include "VitaMath.h"
#include <stdio.h>

Vector4 Vector4::mAxisX = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
Vector4 Vector4::mAxisY = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
Vector4 Vector4::mAxisZ = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
Vector4 Vector4::mAxisW = Vector4(0.0f, 0.0f, 0.0f, 1.0f);

Vector4 Vector4::xAxis()
{
    return mAxisX;
}

Vector4 Vector4::yAxis()
{
    return mAxisY;
}

Vector4 Vector4::zAxis()
{
    return mAxisZ;
}

Vector4 Vector4::wAxis()
{
    return mAxisW;
}

Vector4::Vector4()
{

}

Vector4::Vector4(float x, float y, float z, float w)
{
    mX = x;
    mY = y;
    mZ = z;
    mW = w;
}

const float Vector4::getX() const
{
    return mX;
}

const float Vector4::getY() const
{
    return mY;
}

const float Vector4::getZ() const
{
    return mZ;
}

const float Vector4::getW() const
{
    return mW;
}

void Vector4::setX(float x)
{
    mX = x;
}

void Vector4::setY(float y)
{
    mY = y;
}

void Vector4::setZ(float z)
{
    mZ = z;
}

void Vector4::setW(float w)
{
    mW = w;
}

void Vector4::setXYZ(Vector4 v)
{
    mX = v.getX();
    mY = v.getY();
    mZ = v.getZ();
}

void Vector4::print()
{
    printf("%.2f\t%.2f\t%.2f\t%.2f\n", mX, mY, mZ, mW);
}


Matrix4::Matrix4()
{
}

Matrix4::Matrix4(Vector4 col0, Vector4 col1, Vector4 col2, Vector4 col3)
{
    mCol0 = col0;
    mCol1 = col1;
    mCol2 = col2;
    mCol3 = col3;
}


const Vector4 Matrix4::getCol0() const
{
    return mCol0;
}

const Vector4 Matrix4::getCol1() const
{
    return mCol1;
}

const Vector4 Matrix4::getCol2() const
{
    return mCol2;
}

const Vector4 Matrix4::getCol3() const
{
    return mCol3;
}

void Matrix4::print()
{
    // Column vectors
    printf("%.2f\t%.2f\t%.2f\t%.2f\n", mCol0.getX(), mCol1.getX(), mCol2.getX(), mCol3.getX());
    printf("%.2f\t%.2f\t%.2f\t%.2f\n", mCol0.getY(), mCol1.getY(), mCol2.getY(), mCol3.getY());
    printf("%.2f\t%.2f\t%.2f\t%.2f\n", mCol0.getZ(), mCol1.getZ(), mCol2.getZ(), mCol3.getZ());
    printf("%.2f\t%.2f\t%.2f\t%.2f\n", mCol0.getW(), mCol1.getW(), mCol2.getW(), mCol3.getW());

}

const Vector4 Matrix4::operator *(Vector4 vec) const
{
    return Vector4
    (
        ( ( ( ( mCol0.getX() * vec.getX() ) + ( mCol1.getX() * vec.getY() ) ) + ( mCol2.getX() * vec.getZ() ) ) + ( mCol3.getX() * vec.getW() ) ),
        ( ( ( ( mCol0.getY() * vec.getX() ) + ( mCol1.getY() * vec.getY() ) ) + ( mCol2.getY() * vec.getZ() ) ) + ( mCol3.getY() * vec.getW() ) ),
        ( ( ( ( mCol0.getZ() * vec.getX() ) + ( mCol1.getZ() * vec.getY() ) ) + ( mCol2.getZ() * vec.getZ() ) ) + ( mCol3.getZ() * vec.getW() ) ),
        ( ( ( ( mCol0.getW() * vec.getX() ) + ( mCol1.getW() * vec.getY() ) ) + ( mCol2.getW() * vec.getZ() ) ) + ( mCol3.getW() * vec.getW() ) )
    );
}

const Matrix4 Matrix4::operator *(Matrix4 mat) const
{
    return Matrix4
    (
        *this * mat.mCol0,
        *this * mat.mCol1,
        *this * mat.mCol2,
        *this * mat.mCol3
    );
}

const Matrix4 Matrix4::translation(Vector4 translateVec)
{
    return Matrix4
    (
        Vector4::xAxis(),
        Vector4::yAxis(),
        Vector4::zAxis(),
        Vector4(translateVec.getX(), translateVec.getY(), translateVec.getZ(), 0)
    );
}

Matrix4& Matrix4::setTranslation(Vector4 translateVec)
{
    mCol3.setXYZ(translateVec);
    return *this;
}


const Matrix4 Matrix4::identity()
{
    return Matrix4
    (
        Vector4::xAxis(),
        Vector4::yAxis(),
        Vector4::zAxis(),
        Vector4::wAxis()
    );
}

float* Matrix4::GetOpenGLMat()
{
    mOpenGLMat[0] = mCol0.getX();
    mOpenGLMat[1] = mCol0.getY();
    mOpenGLMat[2] = mCol0.getZ();
    mOpenGLMat[3] = mCol0.getW();

    mOpenGLMat[4] = mCol1.getX();
    mOpenGLMat[5] = mCol1.getY();
    mOpenGLMat[6] = mCol1.getZ();
    mOpenGLMat[7] = mCol1.getW();

    mOpenGLMat[8] = mCol2.getX();
    mOpenGLMat[9] = mCol2.getY();
    mOpenGLMat[10] = mCol2.getZ();
    mOpenGLMat[11] = mCol2.getW();

    mOpenGLMat[12] = mCol3.getX();
    mOpenGLMat[13] = mCol3.getY();
    mOpenGLMat[14] = mCol3.getZ();
    mOpenGLMat[15] = mCol3.getW();

    return mOpenGLMat;
}