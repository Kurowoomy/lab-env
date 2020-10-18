#pragma once

#define _USE_MATH_DEFINES //for PI
#include <math.h>

class Vec2 {
public:
    //initialization
    float x, y;
    Vec2() {
        x = 0;
        y = 0;
    }
    Vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    //operator overloading
    void operator=(const Vec2& vec2) {
        x = vec2.x;
        y = vec2.y;
    }
    Vec2 operator+(Vec2& vec2) const { //adds this and vec4, returns a new Vec4
        return Vec2(x + vec2.x, y + vec2.y);
    }
    Vec2 operator-(Vec2& vec2) const { //subtracts vec4 from this
        return Vec2(x - vec2.x, y - vec2.y);
    }
    ///Scale the vector by multiplying it with a float.
    Vec2 operator*(float scalar) const { //scale the vector
        return Vec2(x * scalar, y * scalar);
    }
    ///Multiplying two vectors results in a dot product.
    float operator*(Vec2& vec2) const { //dot product
        return x * vec2.x + y * vec2.y;
    }

    //operations
    ///Get length of vector.
    float length() const {
        return sqrtf(x * x + y * y);
    }
    ///Normalize this vector, return new vector as result. Depends on length().
    Vec2 normalize() const { //normalizing
        Vec2 vec = Vec2(x / length(), y / length());
        return vec;
    }
    void set(float x, float y) {
        this->x = x;
        this->y = y;
    }

    ~Vec2() {}
};
class Vec3 {
public:
    //initialization
    float x, y, z;
    Vec3() {
        x = 0;
        y = 0;
        z = 0;
    }
    Vec3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    //operator overloading
    void operator=(const Vec3& vec3) {
        x = vec3.x;
        y = vec3.y;
        z = vec3.z;
    }
    Vec3 operator+(Vec3& vec3) const { //adds this and vec4, returns a new Vec4
        return Vec3(x + vec3.x, y + vec3.y, z + vec3.z);
    }
    Vec3 operator-(Vec3& vec3) const { //subtracts vec4 from this
        return Vec3(x - vec3.x, y - vec3.y, z - vec3.z);
    }
    ///Scale the vector by multiplying it with a float.
    Vec3 operator*(float scalar) const { //scale the vector
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    ///Multiplying two vectors results in a dot product.
    float operator*(Vec3& vec3) const { //dot product
        return x * vec3.x + y * vec3.y + z * vec3.z;
    }

    //operations
    ///Returns the cross product as a new vector.
    Vec3 crossProduct(Vec3& vec3) const { //cross product
        return Vec3(y * vec3.z - z * vec3.y, z * vec3.x - x * vec3.z, x * vec3.y - y * vec3.x);
    }
    ///Get length of vector.
    float length() const {
        return sqrtf(x * x + y * y + z * z);
    }
    ///Normalize this vector, return new vector as result. Depends on length().
    Vec3 normalize() const { //normalizing
        Vec3 vec = Vec3(x / length(), y / length(), z / length());
        return vec;
    }
    void set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    ~Vec3() {}
};
///All elements are of type float. The value of the members x, y, z, and w need to be changed directly, no functions used. Default values are (0, 0, 0, 1).
class Vec4
{
public:
    //initialization
    float x, y, z, w;
    Vec4() {
        x = 0;
        y = 0;
        z = 0;
        w = 1;
    }
    Vec4(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
        w = 1;
    }

    //operator overloading
    void operator=(const Vec4& vec4) {
        x = vec4.x;
        y = vec4.y;
        z = vec4.z;
        w = vec4.w;
    }
    Vec4 operator+(Vec4& vec4) const { //adds this and vec4, returns a new Vec4
        return Vec4(x + vec4.x, y + vec4.y, z + vec4.z);
    }
    Vec4 operator-(Vec4& vec4) const { //subtracts vec4 from this
        return Vec4(x - vec4.x, y - vec4.y, z - vec4.z);
    }
    ///Scale the vector by multiplying it with a float.
    Vec4 operator*(float scalar) const { //scale the vector
        return Vec4(x*scalar, y*scalar, z*scalar);
    }
    ///Multiplying two vectors results in a dot product.
    float operator*(Vec4& vec4) const { //dot product
        return x*vec4.x + y*vec4.y + z*vec4.z;
    }

    //operations
    ///Returns the cross product as a new vector.
    Vec4 crossProduct(Vec4& vec4) const { //cross product
        return Vec4(y*vec4.z - z*vec4.y, z*vec4.x - x*vec4.z, x*vec4.y - y*vec4.x);
    }
    ///Get length of vector.
    float length() const {
        return sqrtf(x*x + y*y + z*z);
    }
    ///Normalize this vector, return new vector as result. Depends on length().
    Vec4 normalize() const { //normalizing
        Vec4 vec = Vec4(x / length(), y / length(), z / length());
        return vec;
    }
    void set(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    ~Vec4(){}

};

///All elements are of type float. Specify index in matrix to change value of element, no functions used. Default matrix is an identity matrix.
class Matrix4 {
public:
    //initialization
    float m[16];
    Matrix4(){ //identity matrix
        m[0] = 1; m[1] = 0; m[2] = 0; m[3] = 0;
        m[4] = 0; m[5] = 1; m[6] = 0; m[7] = 0;
        m[8] = 0; m[9] = 0; m[10] = 1; m[11] = 0;
        m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
    }
    Matrix4(float* viewMatrixValues)
    {
        for (int i = 0; i < 16; i++) {
            m[i] = viewMatrixValues[i];
        }
    }
    
    //operator overloading
    void operator=(Matrix4 matrix4) {
        for (int i = 0; i < 16; i++) {
            m[i] = matrix4[i];
        }
    }
    ///Let's us use [] instead of member m to get an element. Out of range returns element of index 0.
    float& operator[](int num) {
        if(num < 16 && num >=0)
            return m[num];
        return m[0]; //returns first element if out of range
    }
    ///Matrix multiplication returns a new matrix.
    Matrix4 operator*(Matrix4& matrix4) const { //matrix multiplication
        Matrix4 newMatrix4;
        int mIndex = 0;
        for (int i = 0; i < 4; i++) { //rows
            for (int j = 0; j < 4; j++) { //columns
                newMatrix4[mIndex] = m[4 * i] * matrix4[j] + m[1 + 4 * i] * matrix4[4 + j] +
                                        m[2 + 4*i] * matrix4[8 + j] + m[3 + 4*i] * matrix4[12 + j];
                mIndex++;
            }
        }
        return newMatrix4;
    }
    ///Multiplying matrix with vector returns result as a new vector.
    Vec4 operator*(const Vec4& vec4) const { //vector multiplied with matrix, returns new vector
        Vec4 newVec4;
        newVec4.x = m[0] * vec4.x + m[1] * vec4.y + m[2] * vec4.z + m[3] * vec4.w;
        newVec4.y = m[4] * vec4.x + m[5] * vec4.y + m[6] * vec4.z + m[7] * vec4.w;
        newVec4.z = m[8] * vec4.x + m[9] * vec4.y + m[10] * vec4.z + m[11] * vec4.w;
        newVec4.w = m[12] * vec4.x + m[13] * vec4.y + m[14] * vec4.z + m[15] * vec4.w;
        return newVec4;
    }

    //functions
    ///Rotation matrix around X-axis. Counter-clockwise.
    static Matrix4 rotationX(const float rad) {
        Matrix4 xMatrix;
        xMatrix[5] = cosf(rad); xMatrix[6] = -sinf(rad);
        xMatrix[9] = sinf(rad); xMatrix[10] = cosf(rad);
        return xMatrix;
    }
    ///Rotation matrix around Y-axis. Counter-clockwise.
    static Matrix4 rotationY(const float rad) {
        Matrix4 yMatrix;
        yMatrix[0] = cosf(rad); yMatrix[2] = sinf(rad);
        yMatrix[8] = -sinf(rad); yMatrix[10] = cosf(rad);
        return yMatrix;
    }
    ///Rotation matrix around Z-axis. Counter-clockwise.
    static Matrix4 rotationZ(const float rad) {
        Matrix4 zMatrix;
        zMatrix[0] = cosf(rad); zMatrix[1] = -sinf(rad);
        zMatrix[4] = sinf(rad); zMatrix[5] = cosf(rad);
        return zMatrix;
    }
    ///Rotation matrix around arbitrary vector vec4. Counter-clockwise.
    static Matrix4 rotationVector(const Vec4& vec, const float rad) {
        Matrix4 vectorMatrix;
        Vec4 vec4 = vec.normalize();
        vectorMatrix[0] = cosf(rad) + vec4.x * vec4.x * (1 - cosf(rad));
        vectorMatrix[1] = vec4.x * vec4.y * (1 - cosf(rad)) - vec4.z * sinf(rad);
        vectorMatrix[2] = vec4.x * vec4.z * (1 - cosf(rad)) + vec4.y * sinf(rad);
        vectorMatrix[4] = vec4.y * vec4.x * (1 - cosf(rad)) + vec4.z * sinf(rad);
        vectorMatrix[5] = cosf(rad) + vec4.y * vec4.y * (1 - cosf(rad));
        vectorMatrix[6] = vec4.y * vec4.z * (1 - cosf(rad)) - vec4.x * sinf(rad);
        vectorMatrix[8] = vec4.z * vec4.x * (1 - cosf(rad)) - vec4.y * sinf(rad);
        vectorMatrix[9] = vec4.z * vec4.y * (1 - cosf(rad)) + vec4.x * sinf(rad);
        vectorMatrix[10] = cosf(rad) + vec4.z * vec4.z * (1 - cosf(rad));
        return vectorMatrix;
    }
    /// Set scalar = 1 for original size.
    static Matrix4 scaleMatrix(float scalar) {
        Matrix4 scaledMatrix;
        scaledMatrix[0] = scalar;
        scaledMatrix[5] = scalar;
        scaledMatrix[10] = scalar;
        return scaledMatrix;
    }
    /// Set float parameters = 0 for no translation.
    static Matrix4 translationMatrix(float x, float y, float z) {
        Matrix4 translationMatrix;
        translationMatrix[3] = x;
        translationMatrix[7] = y;
        translationMatrix[11] = z;
        return translationMatrix;
    }
    /// 
    static Matrix4 perspectiveMatrix(const float angle, const float aspect, const float near, const float far) {
        Matrix4 perspectiveMatrix;
        if (far - near == 0) {
            return perspectiveMatrix;
        }
        float scale = -1/tanf(2 * M_PI - angle * (M_PI / 180) / 2);
        float zp = near + far;
        float zm = far - near;

        perspectiveMatrix[0] = scale;
        perspectiveMatrix[5] = scale * aspect;
        perspectiveMatrix[10] = -zp / zm;
        perspectiveMatrix[11] = -2 * far * near / zm;
        perspectiveMatrix[14] = -1;
        perspectiveMatrix[15] = 0;

        return perspectiveMatrix;
    }
    static Matrix4 viewMatrix(Vec4 eye, Vec4 target, Vec4 up) {
        Vec4 zAxis = (eye - target).normalize();
        Vec4 xAxis = up.crossProduct(zAxis).normalize();
        Vec4 yAxis = zAxis.crossProduct(xAxis).normalize();

        float viewMatrix[16] =
        {
            xAxis.x, yAxis.x, zAxis.x, -(xAxis * eye),
            xAxis.y, yAxis.y, zAxis.y, -(yAxis * eye),
            xAxis.z, yAxis.z, zAxis.z, -(zAxis * eye),
            0.0f,    0.0f,    0.0f,    1.0f
        };
        return Matrix4(viewMatrix);
    }
    ///Returns transposed matrix, where rows and columns have switched places.
    Matrix4 transpose() const { //switch places between rows and columns
        Matrix4 transposedMatrix;

        int mIndex = 0;
        for (int i = 0; i < 4; i++) { //rows
            for (int j = 0; j < 4; j++) { //columns
                transposedMatrix[mIndex] = m[i + j * 4];
                mIndex++;
            }
        }
        return transposedMatrix;
    }
    ///Changes resultMatrix to inverse of originMatrix if determinator != 0. Else resultMatrix remains unchanged.
    static bool inverse(Matrix4& originMatrix, Matrix4& resultMatrix) { //resultMatrix is the result of inversing originMatrix
        Matrix4 tempMatrix;

        tempMatrix[0] = originMatrix[5] * originMatrix[10] * originMatrix[15] -
                            originMatrix[5] * originMatrix[11] * originMatrix[14] -
                            originMatrix[9] * originMatrix[6] * originMatrix[15] +
                            originMatrix[9] * originMatrix[7] * originMatrix[14] +
                            originMatrix[13] * originMatrix[6] * originMatrix[11] -
                            originMatrix[13] * originMatrix[7] * originMatrix[10];
        tempMatrix[4] = -originMatrix[4] * originMatrix[10] * originMatrix[15] +
                            originMatrix[4] * originMatrix[11] * originMatrix[14] +
                            originMatrix[8] * originMatrix[6] * originMatrix[15] -
                            originMatrix[8] * originMatrix[7] * originMatrix[14] -
                            originMatrix[12] * originMatrix[6] * originMatrix[11] +
                            originMatrix[12] * originMatrix[7] * originMatrix[10];
        tempMatrix[8] = originMatrix[4] * originMatrix[9] * originMatrix[15] -
                            originMatrix[4] * originMatrix[11] * originMatrix[13] -
                            originMatrix[8] * originMatrix[5] * originMatrix[15] +
                            originMatrix[8] * originMatrix[7] * originMatrix[13] +
                            originMatrix[12] * originMatrix[5] * originMatrix[11] -
                            originMatrix[12] * originMatrix[7] * originMatrix[9];
        tempMatrix[12] = -originMatrix[4] * originMatrix[9] * originMatrix[14] +
                            originMatrix[4] * originMatrix[10] * originMatrix[13] +
                            originMatrix[8] * originMatrix[7] * originMatrix[14] -
                            originMatrix[8] * originMatrix[6] * originMatrix[13] -
                            originMatrix[12] * originMatrix[7] * originMatrix[10] +
                            originMatrix[12] * originMatrix[6] * originMatrix[9];
        tempMatrix[1] = -originMatrix[1] * originMatrix[10] * originMatrix[15] +
                            originMatrix[1] * originMatrix[11] * originMatrix[14] +
                            originMatrix[9] * originMatrix[2] * originMatrix[15] -
                            originMatrix[9] * originMatrix[3] * originMatrix[14] -
                            originMatrix[13] * originMatrix[2] * originMatrix[11] +
                            originMatrix[13] * originMatrix[3] * originMatrix[10];
        tempMatrix[5] = originMatrix[0] * originMatrix[10] * originMatrix[15] -
                            originMatrix[0] * originMatrix[11] * originMatrix[14] -
                            originMatrix[8] * originMatrix[2] * originMatrix[15] +
                            originMatrix[8] * originMatrix[3] * originMatrix[14] +
                            originMatrix[12] * originMatrix[2] * originMatrix[11] -
                            originMatrix[12] * originMatrix[3] * originMatrix[10];
        tempMatrix[9] = -originMatrix[0] * originMatrix[9] * originMatrix[15] +
                            originMatrix[0] * originMatrix[11] * originMatrix[13] +
                            originMatrix[8] * originMatrix[1] * originMatrix[15] -
                            originMatrix[8] * originMatrix[3] * originMatrix[13] -
                            originMatrix[12] * originMatrix[1] * originMatrix[11] +
                            originMatrix[12] * originMatrix[3] * originMatrix[9];
        tempMatrix[13] = originMatrix[0] * originMatrix[9] * originMatrix[14] -
                            originMatrix[0] * originMatrix[10] * originMatrix[13] -
                            originMatrix[8] * originMatrix[1] * originMatrix[14] +
                            originMatrix[8] * originMatrix[2] * originMatrix[13] +
                            originMatrix[12] * originMatrix[1] * originMatrix[10] -
                            originMatrix[12] * originMatrix[2] * originMatrix[9];
        tempMatrix[2] = originMatrix[1] * originMatrix[6] * originMatrix[15] -
                            originMatrix[1] * originMatrix[7] * originMatrix[14] -
                            originMatrix[5] * originMatrix[2] * originMatrix[15] +
                            originMatrix[5] * originMatrix[3] * originMatrix[14] +
                            originMatrix[13] * originMatrix[2] * originMatrix[7] -
                            originMatrix[13] * originMatrix[3] * originMatrix[6];
        tempMatrix[6] = -originMatrix[0] * originMatrix[6] * originMatrix[15] +
                            originMatrix[0] * originMatrix[7] * originMatrix[14] +
                            originMatrix[4] * originMatrix[2] * originMatrix[15] -
                            originMatrix[4] * originMatrix[3] * originMatrix[14] -
                            originMatrix[12] * originMatrix[2] * originMatrix[7] +
                            originMatrix[12] * originMatrix[3] * originMatrix[6];
        tempMatrix[10] = originMatrix[0] * originMatrix[5] * originMatrix[15] -
                            originMatrix[0] * originMatrix[7] * originMatrix[13] -
                            originMatrix[4] * originMatrix[1] * originMatrix[15] +
                            originMatrix[4] * originMatrix[3] * originMatrix[13] +
                            originMatrix[12] * originMatrix[1] * originMatrix[7] -
                            originMatrix[12] * originMatrix[3] * originMatrix[5];
        tempMatrix[14] = -originMatrix[0] * originMatrix[5] * originMatrix[14] +
                            originMatrix[0] * originMatrix[6] * originMatrix[13] +
                            originMatrix[4] * originMatrix[1] * originMatrix[14] -
                            originMatrix[4] * originMatrix[2] * originMatrix[13] -
                            originMatrix[12] * originMatrix[1] * originMatrix[6] +
                            originMatrix[12] * originMatrix[2] * originMatrix[5];
        tempMatrix[3] = -originMatrix[1] * originMatrix[6] * originMatrix[11] +
                            originMatrix[1] * originMatrix[7] * originMatrix[10] +
                            originMatrix[5] * originMatrix[2] * originMatrix[11] -
                            originMatrix[5] * originMatrix[3] * originMatrix[10] -
                            originMatrix[9] * originMatrix[2] * originMatrix[7] +
                            originMatrix[9] * originMatrix[3] * originMatrix[6];
        tempMatrix[7] = originMatrix[0] * originMatrix[6] * originMatrix[11] -
                            originMatrix[0] * originMatrix[7] * originMatrix[10] -
                            originMatrix[4] * originMatrix[2] * originMatrix[11] +
                            originMatrix[4] * originMatrix[3] * originMatrix[10] +
                            originMatrix[8] * originMatrix[2] * originMatrix[7] -
                            originMatrix[8] * originMatrix[3] * originMatrix[6];
        tempMatrix[11] = -originMatrix[0] * originMatrix[5] * originMatrix[11] +
                            originMatrix[0] * originMatrix[7] * originMatrix[9] +
                            originMatrix[4] * originMatrix[1] * originMatrix[11] -
                            originMatrix[4] * originMatrix[3] * originMatrix[9] -
                            originMatrix[8] * originMatrix[1] * originMatrix[7] +
                            originMatrix[8] * originMatrix[3] * originMatrix[5];
        tempMatrix[15] = originMatrix[0] * originMatrix[5] * originMatrix[10] -
                            originMatrix[0] * originMatrix[6] * originMatrix[9] -
                            originMatrix[4] * originMatrix[1] * originMatrix[10] +
                            originMatrix[4] * originMatrix[2] * originMatrix[9] +
                            originMatrix[8] * originMatrix[1] * originMatrix[6] -
                            originMatrix[8] * originMatrix[2] * originMatrix[5];

        float determinator = originMatrix[0] * tempMatrix[0] + originMatrix[1] * tempMatrix[4] +
                                originMatrix[2] * tempMatrix[8] + originMatrix[3] * tempMatrix[12];
        if (determinator == 0) return false; //no inverse for this matrix exists

        for (int i = 0; i < 16; i++) {
            resultMatrix[i] = tempMatrix[i] / determinator;
        }
        return true;
    }

    ~Matrix4(){}

};


