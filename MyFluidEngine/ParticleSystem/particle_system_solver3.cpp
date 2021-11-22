#include "particle_system_solver3.h"
#include "../Utils/Field/constant_vector_field3.h"

using namespace std;

void ParticleSystemSolver3::beginAdvanceTimeStep(double timeStepInSeconds)
{
	auto forces = _particleSystemData->forces();
	setRange1(forces.size(), Vector3D(), &forces);

	updateCollider(timeStepInSeconds);

	updateEmitter(timeStepInSeconds);

	size_t n = _particleSystemData->numberOfParticles();
	_newPositions.resize(n);
	_newVelocities.resize(n);

	onBeginAdvanceTimeStep(timeStepInSeconds);
}

void ParticleSystemSolver3::endAdvanceTimeStep(double timeStepInSeconds)
{
	// update data
	size_t n = _particleSystemData->numberOfParticles();
	auto positions = _particleSystemData->positions();
	auto velovities = _particleSystemData->velocities();
	parallelFor(kZeroSize, n, [&](size_t i) {
		positions[i] = _newPositions[i];
		velovities[i] = _newVelocities[i];
		});

	onEndAdvanceTimeStep(timeStepInSeconds);
}

void ParticleSystemSolver3::accumulateExternalForces()
{
	size_t n = _particleSystemData->numberOfParticles();
	auto forces = _particleSystemData->forces();
	auto velocities = _particleSystemData->velocities();
	auto positions = _particleSystemData->positions();
	const double mass = _particleSystemData->mass();

	parallelFor(kZeroSize, n, [&](size_t i) {
		Vector3D force = mass * _gravity;

		Vector3D relativeVel = velocities[i] - _wind->sample(positions[i]);
		force += -_dragCoefficient * relativeVel;

		forces[i] += force;
		});
}

void ParticleSystemSolver3::timeIntegration(double timeIntervalInSeconds)
{
	size_t n = _particleSystemData->numberOfParticles();
	auto forces = _particleSystemData->forces();
	auto velocities = _particleSystemData->velocities();
	auto positions = _particleSystemData->positions();
	const double mass = _particleSystemData->mass();

	parallelFor(kZeroSize, n, [&](size_t i) {
		Vector3D& newVelocity = _newVelocities[i];
		newVelocity = velocities[i] + timeIntervalInSeconds * forces[i] / mass; // a = F / m

		Vector3D& newPosition = _newPositions[i];
		newPosition = positions[i] + timeIntervalInSeconds * newVelocity;
		});
}

void ParticleSystemSolver3::updateCollider(double timeIntervalInSeconds)
{
	if (_collider != nullptr)
		_collider->update(currentTimeInSeconds(), timeIntervalInSeconds);
}

void ParticleSystemSolver3::updateEmitter(double timeIntervalInSeconds)
{
	if (_emitter != nullptr)
		_emitter->update(currentTimeInSeconds(), timeIntervalInSeconds);
}

void ParticleSystemSolver3::onInitialize()
{
	updateCollider(0.0);

	updateEmitter(0.0);
}

void ParticleSystemSolver3::onAdvanceTimeStep(double timeIntervalInSeconds)
{
	beginAdvanceTimeStep(timeIntervalInSeconds);

	accumulateForces(timeIntervalInSeconds);
	timeIntegration(timeIntervalInSeconds);
	resolveCollision();

	endAdvanceTimeStep(timeIntervalInSeconds);
}

void ParticleSystemSolver3::accumulateForces(double timeStepInSeconds)
{
	accumulateExternalForces();
}

void ParticleSystemSolver3::onBeginAdvanceTimeStep(double timeStepInSeconds)
{
}

void ParticleSystemSolver3::onEndAdvanceTimeStep(double timeStepInSeconds)
{
}

void ParticleSystemSolver3::resolveCollision()
{
	resolveCollision(_newPositions.accessor(), _newVelocities.accessor());
}

void ParticleSystemSolver3::resolveCollision(ArrayAccessor1<Vector3D> newPositions, ArrayAccessor1<Vector3D> newVelocities)
{
	if (_collider != nullptr)
	{
		size_t numberOfPariticles = _particleSystemData->numberOfParticles();
		const double radius = _particleSystemData->radius();

		parallelFor(kZeroSize, numberOfPariticles, [&](size_t i) {
			_collider->resolveCollision(radius, _restitutionCoefficient, &newPositions[i], &newVelocities[i]);
			});
	}
}

void ParticleSystemSolver3::setParticleSystemData(const shared_ptr<ParticleSystemData3>& newParticles)
{
	_particleSystemData = newParticles;
}

ParticleSystemSolver3::ParticleSystemSolver3(): ParticleSystemSolver3(1e-3, 1e-3)
{
}

ParticleSystemSolver3::ParticleSystemSolver3(double radius, double mass)
{
	_particleSystemData = make_shared<ParticleSystemData3>();
	_particleSystemData->setRadius(radius);
	_particleSystemData->setMass(mass);
	_wind = make_shared<ConstantVectorField3>(Vector3D());
}

ParticleSystemSolver3::~ParticleSystemSolver3()
{
}

double ParticleSystemSolver3::dragCoefficient() const
{
	return _dragCoefficient;
}

void ParticleSystemSolver3::setDragCoefficient(double newDragCoefficient)
{
	_dragCoefficient = newDragCoefficient;
}

double ParticleSystemSolver3::restitutionCoefficient() const
{
	return _restitutionCoefficient;
}

void ParticleSystemSolver3::setRestitutionCoefficient(double newRestitutionCoefficient)
{
	_restitutionCoefficient = clamp(newRestitutionCoefficient, 0.0, 1.0);
}

const Vector3D& ParticleSystemSolver3::gravity() const
{
	return _gravity;
}

void ParticleSystemSolver3::setGravity(const Vector3D& newGravity)
{
	_gravity = newGravity;
}

const shared_ptr<ParticleSystemData3>& ParticleSystemSolver3::particleSystemData() const
{
	return _particleSystemData;
}

const shared_ptr<VectorField3> ParticleSystemSolver3::wind() const
{
	return _wind;
}

void ParticleSystemSolver3::setWind(const shared_ptr<VectorField3>& newWind)
{
	_wind = newWind;
}

const shared_ptr<Collider3>& ParticleSystemSolver3::collider() const
{
	return _collider;
}

void ParticleSystemSolver3::setCollider(const shared_ptr<Collider3>& newCollider)
{
	_collider = newCollider;
}

const shared_ptr<ParticleEmitter3>& ParticleSystemSolver3::emitter() const
{
	return _emitter;
}

void ParticleSystemSolver3::setEmitter(const shared_ptr<ParticleEmitter3>& newEmitter)
{
	_emitter = newEmitter;
	newEmitter->setTarget(_particleSystemData);
}
