

#include "PxPhysicsAPI.h"
#include "RenderUtils.hpp"

using namespace physx;

class AimingReticle
{
public:
	AimingReticle(float distance);
	~AimingReticle();

	void update(const PxVec3& ballPosition);

	void rotateLeft(float angle);
	void rotateRight(float angle);
	void rotateUp(float angle);
	void rotateDown(float angle);

	PxVec3 getAimDirection() const;

	void setColor(const Vector4& color);

private:
	void updateTransforms(const PxVec3& ballPosition);

	float horizontalAngle;
	float verticalAngle;
	float rotationSpeed;

	float aimDistance;
	bool visible;

	RenderItem* directionLine;

	PxTransform lineTransform;
};
