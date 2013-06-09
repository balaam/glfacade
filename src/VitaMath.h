#ifndef VITAMATH_H
#define VITAMATH_H

//
// Mocked up quickly to resemble the Vita API
//
class Vector4
{
    float mX;
    float mY;
    float mZ;
    float mW;
    static Vector4 mAxisX;
    static Vector4 mAxisY;
    static Vector4 mAxisZ;
    static Vector4 mAxisW;
public:
    static Vector4 xAxis();
    static Vector4 yAxis();
    static Vector4 zAxis();
    static Vector4 wAxis();
    Vector4();
    Vector4(float x, float y, float z, float w);
    const float getX() const;
    const float getY() const;
    const float getZ() const;
    const float getW() const;
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setW(float w);
    void setXYZ(Vector4 v);
    void print();
};


class Matrix4
{
    Vector4 mCol0;
    Vector4 mCol1;
    Vector4 mCol2;
    Vector4 mCol3;

    float mOpenGLMat[16];
public:
    const Matrix4 identity();

    Matrix4();
    Matrix4(Vector4 col0, Vector4 col1, Vector4 col2, Vector4 col3);
    const Vector4 operator *(Vector4 vec) const;
    const Matrix4 operator *(Matrix4 mat) const;
    const Matrix4 translation(Vector4 translateVec);
    Matrix4& setTranslation(Vector4 translateVec);
    const Vector4 getCol0() const;
    const Vector4 getCol1() const;
    const Vector4 getCol2() const;
    const Vector4 getCol3() const;
    void print();
    float* GetOpenGLMat();
};
#endif