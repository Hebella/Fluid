#pragma once

#include "constants.h"
#include "particle_system_data3.h"
#include <vector>

using namespace std;

class SphSystemData3 : public ParticleSystemData3
{
private:
	double _targetDensity = kWaterDensity;

	double _targetSpacing = 0.1;

	double _kernelRadiusOverTargetSpacing = 1.8;

	double _kernelRadius;

	size_t _pressureIdx;

	size_t _densityIdex;

	void computeMass();

public:
	SphSystemData3();

	explicit SphSystemData3(size_t numberOfParticles);

	SphSystemData3(const SphSystemData3& other);

	virtual ~SphSystemData3();

	void setRadius(double newRadius) override;

	void setMass(double newMass) override;

	ConstArrayAccessor1<double> densities() const;

	ArrayAccessor1<double> densities();

	ConstArrayAccessor1<double> pressures() const;

	ArrayAccessor1<double> pressures();

	void setTargetSpacing(double spacing);

	double targetSpacing() const;

	void updateDensities();

	void setTargetDensity(double targetDensity);

	double targetDensity() const;

	void setRelativeKernelRadius(double relativeRadius);

	double relatvieKernelRadius() const;

	void setKernelRadius(double kernelRadius);

	double kernelRadius() const;

	double sumofKernelNearby(const Vector3D& position) const; // sum of kernel function evaluation of each nearby particle

	double interpolate(const Vector3D& origin, const ConstArrayAccessor1<double>& values) const;

	Vector3D interpolate(const Vector3D& origin, const ConstArrayAccessor1<Vector3D>& values) const;

	Vector3D gradientAt(size_t i, const ConstArrayAccessor1<Vector3D>& values) const;

	double laplacianAt(size_t i, const ConstArrayAccessor1<double>& values) const;

	Vector3D laplacianAt(size_t i, const ConstArrayAccessor1<Vector3D>& values) const;

	void buildNeighborSearcher();

	void buildNeighborLists(); // builds neighbor lists with kernel radius

	void set(const SphSystemData3& other);

	SphSystemData3& operator=(const SphSystemData3& other);
};