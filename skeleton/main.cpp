#include <ctype.h>

#include <PxPhysicsAPI.h>

#include <vector>

#include "core.hpp"
#include "RenderUtils.hpp"
#include "callbacks.hpp"
#include "Particle.h"
#include <iostream>
#include "ParticleSystem.h"
std::string display_text = "This is a test";


using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;
PxFoundation*			gFoundation = NULL;
PxPhysics*				gPhysics	= NULL;

ParticleSystem* gParticleSystem = nullptr;
PxMaterial*				gMaterial	= NULL;

PxPvd*                  gPvd        = NULL;

PxDefaultCpuDispatcher*	gDispatcher = NULL;
PxScene*				gScene      = NULL;
ContactReportCallback gContactReportCallback;
std::vector<Particle*> projectiles;

// Initialize physics engine
void initPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	// Crear el sistema de partículas
	gParticleSystem = new ParticleSystem();


	EmitterData fountain;
	fountain.position = PxVec3(0.0f, 1.0f, 0.0f);
	fountain.positionVar = PxVec3(0.2f, 0.1f, 0.2f);
	fountain.emitRate = 100.0f;
	fountain.particleLife = 3.0f;
	fountain.particleRadius = 0.15f;
	fountain.color = Vector4(0.2f, 0.6f, 1.0f, 1.0f);
	fountain.velDist = VelocityDistribution::GAUSSIAN;
	fountain.velMean = PxVec3(0.0f, 6.0f, 0.0f);
	fountain.velStdDev = PxVec3(1.0f, 1.0f, 1.0f);

	gParticleSystem->addEmitter(fountain);

	// For Solid Rigids +++++++++++++++++++++++++++++++++++++
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = contactReportFilterShader;
	sceneDesc.simulationEventCallback = &gContactReportCallback;
	gScene = gPhysics->createScene(sceneDesc);

	/*p = new Particle(
		Vector3(0.0f, 40.0f, 0.0f),
		Vector3(35.0f, 0.0f, 0.0f),
		Vector3(0.0f, -9.8f, 0.0f),
		0.99f,
		2.0f,
		Vector4(0.0f, 0.0f, 0.0f, 1.0f)
	);*/



	}


// Function to configure what happens in each step of physics
// interactive: true if the game is rendering, false if it offline
// t: time passed since last call in milliseconds
void stepPhysics(bool interactive, double t)
{
	PX_UNUSED(interactive);
	for (auto proj : projectiles) {
		proj->intergrateEulerSemiExplicit(t);
	}
	if (gParticleSystem) gParticleSystem->update(t);
	gScene->simulate(t);
	gScene->fetchResults(true);
}
void Shoot() 
{
	Vector3 shootPos = GetCamera()->getEye();
	Vector3 shootDir = GetCamera()->getDir();
	shootDir.normalize();
	float speed = 82.0f;
	float desiredSpeed = 50.0f;
	Particle* proj = new Particle(
		shootPos,
		PxVec3(speed*shootDir.x, speed *shootDir.y, speed * shootDir.z),
		PxVec3(desiredSpeed* shootDir.x, desiredSpeed * shootDir.y, desiredSpeed * shootDir.z),
		PxVec3(0.0f, -9.8f, 0.0f),
		50.0f,
		0.99f,
		2.0f,
		Vector4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	projectiles.push_back(proj);
}
// Function to clean data
// Add custom code to the begining of the function
void cleanupPhysics(bool interactive)
{
	PX_UNUSED(interactive);

	// Rigid Body ++++++++++++++++++++++++++++++++++++++++++
	gScene->release();
	gDispatcher->release();
	// -----------------------------------------------------
	gPhysics->release();	
	PxPvdTransport* transport = gPvd->getTransport();
	gPvd->release();
	transport->release();
	if (gParticleSystem) {
		delete gParticleSystem;
		gParticleSystem = nullptr;
	}

	gFoundation->release();
	}

// Function called when a key is pressed
void keyPress(unsigned char key, const PxTransform& camera)
{
	PX_UNUSED(camera);

	switch(toupper(key))
	{
	//case 'B': break;
	//case ' ':	break;
	case 'P':
	{
		Shoot();
		break;
	}
	default:
		break;
	}
}

void onCollision(physx::PxActor* actor1, physx::PxActor* actor2)
{
	PX_UNUSED(actor1);
	PX_UNUSED(actor2);
}


int main(int, const char*const*)
{
#ifndef OFFLINE_EXECUTION 
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif

	return 0;
}