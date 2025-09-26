#pragma once
#include "PxPhysicsAPI.h"
#include "Vector3D.h"
#include "RenderUtils.hpp"

using namespace physx;
class Particle 
{
public:
	Particle(const Vector3& pos,
		const Vector3& vel = Vector3(0.0f),
		const Vector3& acc = Vector3(0.0f, -9.8f, 0.0f),
		float damping_ = 0.99f,
		float radius = 1.0f,
		const Vector4& color = Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	~Particle();
	void changeAcceleration(Vector3 newAcceleration);
	void intergrateEulerExplicit(double dt);
	void intergrateEulerSemiExplicit(double dt);
	void intergrateVerlet(double dt);
private:
	Vector3 position;
	Vector3 velocity;
	Vector3 prePosition;
	Vector3 acceleration;
	float stepNumber;
	float damping;          
	PxTransform transform;  
	RenderItem* renderItem; 

};