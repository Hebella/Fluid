#include "sph_solver3.h"
#include "../Utils/physics_helpers.h"
#include "sph_kernels3.h"

using namespace std;

static double kTimeStepLimitBySpeedFactor = 0.4;
static double kTimeStepLimitByForceFactor = 0.25;

SphSolver3::SphSolver3()
{
	setParticleSystemData(make_shared<SphSystemData3>());
	setIsUsingFixedSubTimeSteps(false);
}

SphSolver3::SphSolver3(double targetDensity, double targetSpacing, double relativeKernelRadius)
{
	auto sphParticles = make_shared<SphSystemData3>();
	sphParticles->setTargetDensity(targetDensity);
	sphParticles->setTargetSpacing(targetSpacing);
	sphParticles->setRelativeKernelRadius(relativeKernelRadius);
	setIsUsingFixedSubTimeSteps(false);
}

SphSolver3::~SphSolver3()
{
}

double SphSolver3::eosExponent() const
{
	return _eosExponent;
}

void SphSolver3::setEosExponent(double newEosExponent)
{
	_eosExponent = max(newEosExponent, 1.0);
}

double SphSolver3::negativePressureScale() const
{
	return _negativePressureScale;
}

void SphSolver3::setNegativePressureScale(double newNegativePressureScale)
{
	_negativePressureScale = clamp(newNegativePressureScale, 0.0, 1.0);
}

double SphSolver3::viscosityCoefficient() const
{
	return _viscosityCoefficient;
}

void SphSolver3::setViscosityCoefficient(double newViscosityCoefficient)
{
	_viscosityCoefficient = max(newViscosityCoefficient, 0.0);
}

double SphSolver3::pseudoViscosityCoefficient() const
{
	return _pseudoViscosityCoefficient;
}

void SphSolver3::setPseudoViscosityCoefficient(double newPseudoViscosityCoefficient)
{
	_pseudoViscosityCoefficient = max(newPseudoViscosityCoefficient, 0.0);
}

double SphSolver3::speedOfSound() const
{
	return _speedOfSound;
}

void SphSolver3::setSpeedOfSound(double newSpeedOfSound)
{
	_speedOfSound = max(newSpeedOfSound, kEpsilonD);
}

double SphSolver3::timeStepLimitScale() const
{
	return _timeStepLimitScale;
}

void SphSolver3::setTimeStepLimitScale(double newScale)
{
	_timeStepLimitScale = max(newScale, 0.0);
}

shared_ptr<SphSystemData3> SphSolver3::sphSystemData() const
{
	return dynamic_pointer_cast<SphSystemData3>(particleSystemData());
}

unsigned int SphSolver3::numberOfSubTimeSteps(double timeIntervalInSeconds) const
{
	auto particles = sphSystemData();
	size_t numberOfPartcles = particles->numberOfParticles();
	auto f = particles->forces();

	const double kernelRadius = particles->kernelRadius();
	const double mass = particles->mass();

	double maxForceMagnitude = 0.0;

	for (size_t i = 0; i < numberOfPartcles; ++i)
		maxForceMagnitude = max(maxForceMagnitude, f[i].length());

	double timeStepLimitBySpeed = kTimeStepLimitBySpeedFactor * kernelRadius / _speedOfSound;
	double timeStepLimitByForce = kTimeStepLimitByForceFactor * sqrt(kernelRadius * mass / maxForceMagnitude);

	double desiredTimeStep = _timeStepLimitScale * min(timeStepLimitBySpeed, timeStepLimitByForce);

	return static_cast<unsigned int>(ceil(timeIntervalInSeconds / desiredTimeStep));
}

void SphSolver3::accumulateForces(double timeStepInSeconds)
{
	accumulateNonPressureForces(timeStepInSeconds);
	accumulatePressureForce(timeStepInSeconds);
}

void SphSolver3::onBeginAdvanceTimeStep(double timeStepInSeconds)
{
	auto particles = sphSystemData();

	particles->buildNeighborSearcher();
	particles->buildNeighborLists();
	particles->updateDensities();
}

void SphSolver3::onEndAdvanceTimeStep(double timeStepInSeconds)
{
	computePseudoViscosity(timeStepInSeconds);
}

void SphSolver3::accumulateNonPressureForces(double timeStepInSeconds)
{
	ParticleSystemSolver3::accumulateForces(timeStepInSeconds);
	accumulateViscosityForce();
}

void SphSolver3::computePressure()
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();
	auto d = particles->densities();
	auto p = particles->pressures();

	const double targetDensity = particles->targetDensity();
	const double eosScale = targetDensity * square(_speedOfSound);

	parallelFor(kZeroSize, numberOfParticles, [&](size_t i) {
		p[i] = computePressureFromEos(d[i], targetDensity, eosScale, eosExponent(), negativePressureScale());
		});
}

void SphSolver3::accumulatePressureForce(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	auto x = particles->positions();
	auto d = particles->densities();
	auto p = particles->pressures();
	auto f = particles->forces();

	computePressure();
	accumulatePressureForce(x, d, p, f);
}

void SphSolver3::accumulatePressureForce(const ConstArrayAccessor1<Vector3D>& positions, const ConstArrayAccessor1<double>& densities, const ConstArrayAccessor1<double>& pressures, ArrayAccessor1<Vector3D> pressureForces)
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();

	const double massSquared = square(particles->mass());
	const SphStdKernel3 kernel(particles->kernelRadius());

	parallelFor(kZeroSize, numberOfParticles, [&](size_t i) {
		const auto& neighbors = particles->neighborLists()[i];
		for (size_t j : neighbors)
		{
			double dist = positions[i].distanceTo(positions[j]);

			if (dist > 0.0)
			{
				Vector3D dir = (positions[j] - positions[i]) / dist;
				pressureForces[i] -= massSquared 
					* (pressures[i] / (densities[i] * densities[i]) 
						+ pressures[j] / (densities[j] * densities[j])) 
					* kernel.gradient(dist, dir);
			}
		}
		});
}

void SphSolver3::accumulateViscosityForce()
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();

	auto x = particles->positions();
	auto v = particles->velocities();
	auto d = particles->densities();
	auto f = particles->forces();

	const double massSquared = square(particles->mass());
	const SphStdKernel3 kernel(particles->kernelRadius());

	parallelFor(kZeroSize, numberOfParticles, [&](size_t i) {
		const auto& neighbors = particles->neighborLists()[i];
		for (size_t j : neighbors)
		{
			double dist = x[i].distanceTo(x[j]);

			f[i] += viscosityCoefficient() * massSquared
				* (v[j] - v[i]) / d[j] * kernel.secondDerivative(dist);
		}
		});
}

void SphSolver3::computePseudoViscosity(double timeStepInSeconds)
{
	auto particles = sphSystemData();
	size_t numberOfParticles = particles->numberOfParticles();

	auto x = particles->positions();
	auto v = particles->velocities();
	auto d = particles->densities();

	const double mass = particles->mass();
	const SphStdKernel3 kernel(particles->kernelRadius());

	Array1<Vector3D> smoothedVelocities(numberOfParticles);

	parallelFor(kZeroSize, numberOfParticles, [&](size_t i)
		{
			double weightSum = 0.0;
			Vector3D smoothedVelocity;

			const auto& neighbors = particles->neighborLists()[i];
			for (size_t j : neighbors)
			{
				double dist = x[i].distanceTo(x[j]);
				double wj = mass / d[j] * kernel(dist);
				weightSum += wj;
				smoothedVelocity += wj * v[j];
			}

			double wi = mass / d[i];
			weightSum += wi;
			smoothedVelocity += wi * v[i];

			if (weightSum > 0.0)
				smoothedVelocity /= weightSum;

			smoothedVelocities[i] = smoothedVelocity;
		});

	double factor = timeStepInSeconds * _pseudoViscosityCoefficient;
	factor = clamp(factor, 0.0, 1.0);

	parallelFor(kZeroSize, numberOfParticles, [&](size_t i) {
		v[i] = lerp(v[i], smoothedVelocities[i], factor);
		});
}
