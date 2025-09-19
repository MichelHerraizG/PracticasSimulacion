#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <cmath>

class Vector3D {
public:
	float x, y, z;
	Vector3D(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    Vector3D& operator=(const Vector3D& other) {
        if (this != &other) 
        {
            x = other.x;
            y = other.y;
            z = other.z;
        }
        return *this;
    }
    Vector3D operator+(const Vector3D& other) {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }
    Vector3D operator-(const Vector3D& other) {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
    Vector3D operator*(const int& escalar) {
        return Vector3D(x * escalar, y * escalar , z * escalar);

    }
    float dot(const Vector3D& other) {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }
    float module() const
    {
        return sqrt((x*x + y*y + z*z));
    }
    Vector3D normalize() const {
        float mag = module();
        if (mag == 0.0f) return Vector3D(0, 0, 0);
        return Vector3D(x / mag, y / mag, z / mag);
    }
};

#endif // VECTOR3D_HPP