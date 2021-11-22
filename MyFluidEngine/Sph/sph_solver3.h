#pragma once

#include "../Utils/constants.h"
#include "../ParticleSystem/particle_system_solver3.h"
#include "sph_system_data3.h"

using namespace std;

class SphSolver3 : public ParticleSystemSolver3
{
private:
	double _eosExponent = 7.0; // Tait's equation
	double _negativePressureScale = 0.0;
	double _viscosityCoefficient = 0.01;
	double _pseudoViscosityCoefficient = 10.0;
	double _speedOfSound = 100.0;
	double _timeStepLimitScale = 1.0;
protected:
	unsigned int numberOfSubTimeSteps(double timeIntervalInSeconds) const override;

	void accumulateForces(double timeStepInSeconds) override;

	void onBeginAdvanceTimeStep(double timeStepInSeconds) override;

	void onEndAdvanceTimeStep(double timeStepInSeconds) override;

	virtual void accumulateNonPressureForces(double timeStepInSeconds);

	virtual void accumulatePressureForce(double timeStepInSeconds);

	void computePressure();

	void accumulatePressureForce(
		const ConstArrayAccessor1<Vector3D>& positions,
		const ConstArrayAccessor1<double>& densities,
		const ConstArrayAccessor1<double>& pressures,
		ArrayAccessor1<Vector3D> pressureForces
	);

	void accumulateViscosityForce();

	void computePseudoViscosity(double timeStepInSeconds);
public:
	SphSolver3();
	SphSolver3(double targetDensity, double targetSpacing, double relativeKernelRadius);
	virtual ~SphSolver3();

	double eosExponent() const;
	void setEosExponent(double newEosExponent);

	double negativePressureScale() const;
	void setNegativePressureScale(double newNegativePressureScale);

	double viscosityCoefficient() const;
	void setViscosityCoefficient(double newViscosityCoefficient);

	double pseudoViscosityCoefficient() const;
	void setPseudoViscosityCoefficient(double newPseudoViscosityCoefficient);

	double speedOfSound() const;
	void setSpeedOfSound(double newSpeedOfSound);

	double timeStepLimitScale() const;
	void setTimeStepLimitScale(double newScale);

	shared_ptr<SphSystemData3> sphSystemData() const;
};