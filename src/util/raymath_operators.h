#pragma once
#include "raylib.h"

// Vector3 to Vector2 conversions
inline Vector2 xy(Vector3 v) { return {v.x, v.y}; }
inline Vector2 xz(Vector3 v) { return {v.x, v.z}; }
inline Vector2 yz(Vector3 v) { return {v.y, v.z}; }

//Addition overloads: componentwise addition
inline Vector2 operator+(const Vector2& a, const Vector2& b) {
    return Vector2Add(a,b);
}

inline Vector3 operator+(const Vector3& a, const Vector3& b) {
    return Vector3Add(a,b);
}

inline Vector4 operator+(const Vector4& a, const Vector4& b) {
    return (Vector4){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w};
}

inline Vector2& operator+=(Vector2& a, const Vector2& b) {
    a=a+b;
    return a;
}

inline Vector3& operator+=(Vector3& a, const Vector3& b) {
    a=a+b;
    return a;
}

inline Vector4& operator+=(Vector4& a, const Vector4& b) {
    a=a+b;
    return a;
}

inline Matrix operator+(const Matrix& left, const Matrix& right) {
    return MatrixAdd(left,right);
}

inline Matrix& operator+=(Matrix& left, const Matrix& right) {
    left=MatrixAdd(left,right);
    return left;
}

inline Color operator+(const Color& a, const Color& b) {
    int red, green, blue, alpha;  //To ensure that addition of a+b does not result in components greater than unsigned char's max of 255, we upconvert to int, then check the sum, and cast back to unsigned char
    Color c;
    red = int(a.r) + int(b.r);
    green = int(a.g) + int(b.g);
    blue = int(a.b) + int(b.b);
    alpha = int(a.a) + int(b.a);

    c.r = u_char((red > 255) ? 255 : red);
    c.g = u_char((green > 255) ? 255 : green);
    c.b = u_char((blue > 255) ? 255 : blue);
    c.r = u_char((alpha > 255) ? 255 : alpha);
    return c;
}

inline Color& operator+=(Color&a, const Color& b) {
    a=a+b;
    return a;
}

//Negation: Unary Minus operator
inline Vector2& operator-(Vector2& a) {
    a=Vector2Negate(a);
    return a;
}

inline Vector3& operator-(Vector3& a) {
    a=Vector3Negate(a);
    return a;
}

//Since Quaternion is a Vector4 typedef, I did not provide a negation for Vector4 to avoid confusion with QuaternionInvert();

//Subtraction overloads: componentwise subtraction
inline Vector2 operator-(const Vector2& a, const Vector2& b) {
    return Vector2Subtract(a,b);
}

inline Vector3 operator-(const Vector3& a, const Vector3& b) {
    return Vector3Subtract(a,b);
}

inline Vector4 operator-(const Vector4& a, const Vector4& b) {
    return (Vector4){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w};
}

inline Vector2& operator-=(Vector2& a, const Vector2& b) {
    a=a-b;
    return a;
}

inline Vector3& operator-=(Vector3& a, const Vector3& b) {
    a=a-b;
    return a;
}

inline Vector4& operator-=(Vector4& a, const Vector4& b) {
    a=a-b;
    return a;
}

inline Matrix operator-(const Matrix& left, const Matrix& right) {
    return MatrixSubtract(left,right);
}

inline Matrix& operator-=(Matrix& left, const Matrix& right) {
    left=MatrixSubtract(left,right);
    return left;
}

inline Color operator-(const Color& a, const Color& b) {
    int red,green,blue,alpha;  //Convert to int, clamp to min zero, convert back to unsigned char
    Color c;
    red = int(a.r) - int(b.r);
    green = int(a.g) - int(b.g);
    blue = int(a.b) - int(b.b);
    alpha = int(a.a) - int(b.a);

    c.r = u_char((red < 0) ? 0 : red);
    c.g = u_char((green < 0) ? 0 : green);
    c.b = u_char((blue < 0) ? 0 : blue);
    c.r = u_char((alpha < 0) ? 0 : alpha);
    return c;
}

inline Color& operator-=(Color&a, const Color& b) {
    a=a-b;
    return a;
}

//Multiplication overload only provides for multiplying a vector by a scalar.   Vector * Vector is not overloaded to avoid confusion whether one intends a dot product, cross product, etc.
inline Vector2 operator*(const Vector2& a, float b) {
    return Vector2Scale(a,b);
}

inline Vector3 operator*(const Vector3& a, float b) {
    return Vector3Scale(a,b);
}

inline Rectangle operator*(const Rectangle& a, float b) {
    return Rectangle{a.x * b, a.y * b, a.width * b, a.height * b};
}

inline Matrix operator*(const Matrix& left, const Matrix& right) {
    return MatrixMultiply(left,right);
}

inline Matrix& operator*=(Matrix& left, const Matrix& right) {
    left=MatrixMultiply(left,right);
    return left;
}

inline Rectangle& operator*=(Rectangle& a, const float b) {
    a=a*b;
    return a;
}

inline Vector2& operator*=(Vector2& a, const float b) {
    a=a*b;
    return a;
}

inline Vector3& operator*=(Vector3& a, const float b) {
    a=a*b;
    return a;
}

//I'm not sure if the following is useful, but here it is anyway for completeness
inline Color operator*(const Color& a, const Color& b) {
    int red, green, blue, alpha;
    Color c;
    red = int(a.r) * int(b.r);
    green = int(a.g) * int(b.g);
    blue = int(a.b) * int(b.b);
    alpha = int(a.a) * int(b.a);

    c.r = u_char((red > 255) ? 255 : (red < 0) ? 0 : red); //Nested ternary clamps return between 0 and 255
    c.g = u_char((green > 255) ? 255 : (green < 0) ? 0 : green);
    c.b = u_char((blue > 255) ? 255 : (blue < 0) ? 0 : blue);
    c.r = u_char((alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha);
    return c;
}

inline Color& operator*=(Color&a, const Color& b) {
    a=a*b;
    return a;
}

inline Color operator*(const Color& a, const float b) {
    float red, green, blue, alpha;  //We cast to float, clamp, then recast back to unsigned char
    Color c;
    red = float(a.r) * b;
    green = float(a.g) * b;
    blue = float(a.b) * b;
    alpha = float(a.a) * b;

    c.r = u_char((red > 255) ? 255 : (red < 0) ? 0 : red); //Nested ternary clamps return between 0 and 255
    c.g = u_char((green > 255) ? 255 : (green < 0) ? 0 : green);
    c.b = u_char((blue > 255) ? 255 : (blue < 0) ? 0 : blue);
    c.r = u_char((alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha);
    return c;
}

inline Color& operator*=(Color& a, const float b) {
    a = a * b;
    return a;
}

//Division overload: Merely scalar multiplication by the reciprocal, with a Divide-By-Zero check.
inline Vector2 operator/(const Vector2& a, const float b) {
    if (b==0.0) {
        std::cerr<<"Division by zero error."<<std::endl;
        throw std::domain_error("Division by zero error");
    }
    float recip=1.0/b;
    return a*recip;
}

inline Vector3 operator/(const Vector3& a, const float b) {
    if (b==0.0) {
        std::cerr<<"Division by zero error."<<std::endl;
        throw std::domain_error("Division by zero error");
    }
    float recip=1.0/b;
    return a*recip;
}

inline Vector2& operator/=(Vector2& a, const float b) {
    a=a/b;
    return a;
}

inline Vector3& operator/=(Vector3& a, const float b) {
    a=a/b;
    return a;
}

//I'm not sure if the following is useful either, but here it is for completeness
inline Color operator/(const Color& a, const Color& b) {
    float red,green,blue,alpha;
    Color c;
    red = float(a.r) / float(b.r);
    green = float(a.g) / float(b.g);
    blue = float(a.b) / float(b.b);
    alpha = float(a.a) / float(b.a);

    c.r = u_char((red > 255) ? 255 : (red < 0) ? 0 : red); //Nested ternary clamps return between 0 and 255
    c.g = u_char((green > 255) ? 255 : (green < 0) ? 0 : green);
    c.b = u_char((blue > 255) ? 255 : (blue < 0) ? 0 : blue);
    c.r = u_char((alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha);
    return c;
}

inline Color& operator/=(Color&a, const Color& b) {
    a=a/b;
    return a;
}

inline Color operator/(const Color& a, const float b) {
    float red, green, blue, alpha;  //We cast to float, clamp, then recast back to unsigned char
    Color c;
    red = float(a.r) / b;
    green = float(a.g) / b;
    blue = float(a.b) / b;
    alpha = float(a.a) / b;

    c.r = u_char((red > 255) ? 255 : (red < 0) ? 0 : red); //Nested ternary clamps return between 0 and 255
    c.g = u_char((green > 255) ? 255 : (green < 0) ? 0 : green);
    c.b = u_char((blue > 255) ? 255 : (blue < 0) ? 0 : blue);
    c.r = u_char((alpha > 255) ? 255 : (alpha < 0) ? 0 : alpha);
    return c;
}

inline Color& operator/=(Color&a, const float b) {
    a=a/b;
    return a;
}


// Equality operator overloads
//
// Integer equality
inline bool operator==(const Color& c1, const Color& c2) {
    if ( (c1.r==c2.r) && (c1.g==c2.g) && (c1.b==c2.b) && (c1.a==c2.a) ) return true;
    return false;
}

// Float equality for Vector2 and Vector3
//Takes a conservative approach and only affirms that two vectors are equal if all of their respective components are equal within machine precision.
inline bool operator==(const Vector2& a, const Vector2& b) {
    bool XIsEqual=false;
    bool YIsEqual=false;
    XIsEqual = std::fabs(a.x - b.x) <= ( (std::fabs(a.x) > std::fabs(b.x) ? std::fabs(b.x) : std::fabs(a.x)) * std::numeric_limits<float>::epsilon() );
    YIsEqual = std::fabs(a.y - b.y) <= ( (std::fabs(a.y) > std::fabs(b.y) ? std::fabs(b.y) : std::fabs(a.y)) * std::numeric_limits<float>::epsilon() );
    return (XIsEqual && YIsEqual);
}

inline bool operator==(const Vector3& a, const Vector3& b) {
    bool XIsEqual=false;
    bool YIsEqual=false;
    bool ZIsEqual=false;
    XIsEqual = std::fabs(a.x - b.x) <= ( (std::fabs(a.x) > std::fabs(b.x) ? std::fabs(b.x) : std::fabs(a.x)) * std::numeric_limits<float>::epsilon() );
    YIsEqual = std::fabs(a.y - b.y) <= ( (std::fabs(a.y) > std::fabs(b.y) ? std::fabs(b.y) : std::fabs(a.y)) * std::numeric_limits<float>::epsilon() );
    ZIsEqual = std::fabs(a.z - b.z) <= ( (std::fabs(a.z) > std::fabs(b.z) ? std::fabs(b.z) : std::fabs(a.z)) * std::numeric_limits<float>::epsilon() );
    return (XIsEqual && YIsEqual && ZIsEqual);
}

inline bool operator==(const Vector4& a, const Vector4& b) {
    bool XIsEqual=false;
    bool YIsEqual=false;
    bool ZIsEqual=false;
    bool WIsEqual=false;
    XIsEqual = std::fabs(a.x - b.x) <= ( (std::fabs(a.x) > std::fabs(b.x) ? std::fabs(b.x) : std::fabs(a.x)) * std::numeric_limits<float>::epsilon() );
    YIsEqual = std::fabs(a.y - b.y) <= ( (std::fabs(a.y) > std::fabs(b.y) ? std::fabs(b.y) : std::fabs(a.y)) * std::numeric_limits<float>::epsilon() );
    ZIsEqual = std::fabs(a.z - b.z) <= ( (std::fabs(a.z) > std::fabs(b.z) ? std::fabs(b.z) : std::fabs(a.z)) * std::numeric_limits<float>::epsilon() );
    WIsEqual = std::fabs(a.w - b.w) <= ( (std::fabs(a.w) > std::fabs(b.w) ? std::fabs(b.w) : std::fabs(a.w)) * std::numeric_limits<float>::epsilon() );
    return (XIsEqual && YIsEqual && ZIsEqual && WIsEqual);
}


// ********************************************
//
//           OUTPUT STREAM OVERLOADS
//
// ********************************************
// For use with cout, filestreams, etc.
// Two different formats are provided for printing vectors and colors.  Use the #define section at the top of the file to select betweem them.
// Other convenience overloads are provided below which allow for easy printing of components of structs.  Some of these make use of the vector format selected above when Vectors are part of the struct
inline std::ostream& operator<<(std::ostream& os, Vector2 a) {
    os<<"x="<<a.x<<", y="<<a.y;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, Vector3 a) {
    os<<"x="<<a.x<<", y="<<a.y<<", z="<<a.z;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, Vector4 a) {
    os<<"x="<<a.x<<", y="<<a.y<<", z="<<a.z<<", w="<<a.w;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, Color c) {
    os<<"R="<<(unsigned int)c.r<<" G="<<(unsigned int)c.g<<" B="<<(unsigned int)c.b<<" A="<<(unsigned int)c.a;
    return os;
}

//Per definition of Matrix in raylib.h as "Matrix type (OpenGL style 4x4 - right handed, column major)"
inline std::ostream& operator<<(std::ostream& os, Matrix m) {
    os<<" \t"<<m.m0<< "\t"<<m.m4<<" \t"<<m.m8<<" \t"<<m.m12<<"\n";
    os<<" \t"<<m.m1<< "\t"<<m.m5<<" \t"<<m.m9<<" \t"<<m.m13<<"\n";
    os<<" \t"<<m.m2<< "\t"<<m.m6<<" \t"<<m.m10<<" \t"<<m.m14<<"\n";
    os<<" \t"<<m.m3<< "\t"<<m.m7<<" \t"<<m.m11<<" \t"<<m.m15<<"\n";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, Rectangle r) {
    os<<"Rectangle corner: ("<<r.x<<","<<r.y<<"), Width="<<r.width<<"Height="<<r.height;
    return os;
}
