#pragma once
#include "sph_kernels3.h"

inline SphStdKernel3::SphStdKernel3() : h(0), h2(0), h3(0), h5(0) {}

inline SphStdKernel3::SphStdKernel3(double kernelRadius) : h(kernelRadius), h2(h * h), h3(h2 * h), h5(h2 * h3) {}

inline SphStdKernel3::SphStdKernel3(const SphStdKernel3& other) : h(other.h), h2(other.h2), h3(other.h3), h5(other.h5) {}

inline double SphStdKernel3::operator()(double distance) const
{
	if (distance * distance >= h2)
		return 0.0;
	else
	{
		double x = 1.0 - distance * distance / h2;
		return 315.0 / (64.0 * kPiD * h3) * x * x * x;
	}
}

inline double SphStdKernel3::firstDerivative(double distance) const
{
	if (distance >= h)
		return 0.0;
	else
	{
		double x = 1.0 - distance * distance / h2;
		return -945.0 / (32.0 * kPiD * h5) * distance * x * x;
	}
}

inline Vector3D SphStdKernel3::gradient(const Vector3D& point) const
{
	double dist = point.length();
	if (dist > 0.0)
		return gradient(dist, point / dist);
	else
		return Vector3D(0, 0, 0);
}

inline Vector3D SphStdKernel3::gradient(double distance, const Vector3D& direction) const
{
	return -firstDerivative(distance) * direction;
}

inline double SphStdKernel3::secondDerivative(double distance) const
{
	if (distance * distance >= h2)
		return 0.0;
	else
	{
		double x = distance * distance / h2;
		return 945.0 / (32.0 * kPiD * h5) * (1 - x) * (5 * x - 1);
	}
}