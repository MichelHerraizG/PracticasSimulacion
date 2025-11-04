#include "AimingReticle.h"
#include <cmath>

extern physx::PxPhysics* gPhysics;
extern physx::PxMaterial* gMaterial;

AimingReticle::AimingReticle(float distance)
    : horizontalAngle(0.0f)
    , verticalAngle(0.0f)
    , aimDistance(distance)
    , visible(true)
{
    PxBoxGeometry lineGeom(0.03f, 0.03f, aimDistance * 0.5f);
    directionLine = new RenderItem(CreateShape(lineGeom, gMaterial),
        Vector4(1.0f, 0.0f, 0.0f, 1.0f));

    lineTransform = PxTransform(PxVec3(0, 0, 0));
    directionLine->transform = &lineTransform;
    directionLine->transform->rotate(Vector3(180, 0, 0));
}

AimingReticle::~AimingReticle()
{
    if (directionLine) {
        DeregisterRenderItem(directionLine);
        delete directionLine;
    }
}

void AimingReticle::rotateLeft(float angle)
{
    horizontalAngle += angle;
}

void AimingReticle::rotateRight(float angle)
{
    horizontalAngle -= angle;
}

void AimingReticle::rotateUp(float angle)
{
    verticalAngle += angle;
    if (verticalAngle > PxPi / 3.0f) verticalAngle = PxPi / 3.0f;
}

void AimingReticle::rotateDown(float angle)
{
    verticalAngle -= angle;
    if (verticalAngle < 0.0f) verticalAngle = 0.0f;
}

PxVec3 AimingReticle::getAimDirection() const
{
    float x = cos(verticalAngle) * sin(horizontalAngle);
    float y = sin(verticalAngle);
    float z = cos(verticalAngle) * cos(horizontalAngle);

    PxVec3 dir(x, y, z);
    dir.normalize();
    return dir;
}

void AimingReticle::update(const PxVec3& ballPosition)
{
    if (!visible) return;
    updateTransforms(ballPosition);
}

void AimingReticle::updateTransforms(const PxVec3& ballPosition)
{
    PxVec3 aimDir = getAimDirection();
    PxVec3 lineCenter = ballPosition + aimDir * (aimDistance * 0.5f);

    PxVec3 defaultDir(0, 0, 1);
    PxVec3 axis = defaultDir.cross(aimDir);
    float axisLength = axis.magnitude();

    PxQuat rotation;
    if (axisLength > 0.001f) {
        axis.normalize();
        float angle = acos(defaultDir.dot(aimDir));
        rotation = PxQuat(angle, axis);
    }
    else {
        rotation = PxQuat(PxIdentity);
    }

    lineTransform = PxTransform(lineCenter, rotation);
}

void AimingReticle::setColor(const Vector4& color)
{
    if (directionLine) {
        directionLine->color = color;
    }
}
