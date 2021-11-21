#pragma once

#include "constants.h"
#include "vector3.h"

using namespace std;

struct SphStdKernel3
{
	double h; // kernel radius

	double h2; // square

	double h3; // cubic

	double h5; // fifth-power

	SphStdKernel3();

	explicit SphStdKernel3(double kernelRadius);

	SphStdKernel3(const SphStdKernel3& other);

	double operator()(double distance) const;

	double firstDerivative(double distance) const;

	Vector3D gradient(const Vector3D& point) const;

	Vector3D gradient(double distance, const Vector3D& direction) const;

	double secondDerivative(double distance) const;
};

#include "sph_kernels3-inl.h"
