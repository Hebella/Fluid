#pragma once
#include "physics_animation.h"
#include "particle_system_data3.h"
#include "vector_field3.h"
#include "array_accessor1.h"
#include "array_utils.h"
#include "collider3.h"
#include "particle_emitter3.h"

using namespace std;

class ParticleSystemSolver3 : public PhysicsAnimation
{
private:
	double _dragCoefficient = 1e-4;
	double _restitutionCoefficient = 0.0;
	Vector3D _gravity = Vector3D(0.0, kGravity, 0.0);

	shared_ptr<ParticleSystemData3> _particleSystemData;
	Array1<Vector3D> _newPositions;
	Array1<Vector3D> _newVelocities;
	shared_ptr<VectorField3> _wind;
	shared_ptr<Collider3> _collider;
	shared_ptr<ParticleEmitter3> _emitter;

	void beginAdvanceTimeStep(double timeStepInSeconds);

	void endAdvanceTimeStep(double timeStepInSeconds);

	void accumulateExternalForces();

	void timeIntegration(double timeIntervalInSeconds);

	void updateCollider(double timeIntervalInSeconds);

	void updateEmitter(double timeIntervalInSeconds);

protected:
	void onInitialize() override;

	void onAdvanceTimeStep(double timeIntervalInSeconds) override;

	virtual void accumulateForces(double timeStepInSeconds);

	virtual void onBeginAdvanceTimeStep(double timeStepInSeconds);

	virtual void onEndAdvanceTimeStep(double timeStepInSeconds);

	void resolveCollision();

	void resolveCollision(ArrayAccessor1<Vector3D> newPositions, ArrayAccessor1<Vector3D> newVelocities);

	void setParticleSystemData(const shared_ptr<ParticleSystemData3>& newParticles);

public:
	ParticleSystemSolver3();

	ParticleSystemSolver3(double radius, double mass);

	virtual ~ParticleSystemSolver3();

	double dragCoefficient() const;

	void setDragCoefficient(double newDragCoefficient);

	double restitutionCoefficient() const;

	void setRestitutionCoefficient(double newRestitutionCoefficient);

	const Vector3D& gravity() const;

	void setGravity(const Vector3D& newGravity);

	const shared_ptr<ParticleSystemData3>& particleSystemData() const;

	const shared_ptr<VectorField3> wind() const;

	void setWind(const shared_ptr<VectorField3>& newWind);

	const shared_ptr<Collider3>& collider() const;

	void setCollider(const shared_ptr<Collider3>& newCollider);

	const shared_ptr<ParticleEmitter3>& emitter() const;

	void setEmitter(const shared_ptr<ParticleEmitter3>& newEmitter);
};