#include "../Utils/pch.h"
#include "collider3.h"
#include <algorithm>

using namespace std;

void Collider3::setSurface(const shared_ptr<Surface3>& newSurface)
{
	_surface = newSurface;
}

void Collider3::getClosestPoint(const shared_ptr<Surface3>& surface, const Vector3D& queryPoint, ColliderQueryResult* result) const
{
	result->distance = surface->closestDistance(queryPoint);
	result->point = surface->closestPoint(queryPoint);
	result->normal = surface->closestNormal(queryPoint);
	result->velocity = velocityAt(queryPoint);
}

bool Collider3::isPenetrating(const ColliderQueryResult& colliderPoint, const Vector3D& position, double radius)
{
	return _surface->isInside(position) || colliderPoint.distance < radius; 
	// the distance from the particle to the colliderPoint is less than the particle's radius
}

Collider3::Collider3() {}

Collider3::~Collider3() {}

void Collider3::resolveCollision(double radius, double restitutionCoefficient, Vector3D* newPosition, Vector3D* newVelocity)
{
	if (!_surface->isValidGeometry())
		return;

	ColliderQueryResult colliderPoint;

	getClosestPoint(_surface, *newPosition, &colliderPoint);

	if (isPenetrating(colliderPoint, *newPosition, radius))
	{
		Vector3D targetNormal = colliderPoint.normal;
		Vector3D targetPoint = colliderPoint.point + radius * targetNormal;
		Vector3D colliderVelAtTargetPoint = colliderPoint.velocity; // the collider may be moving

		Vector3D relativeVel = *newVelocity - colliderVelAtTargetPoint;
		double normalDotRelativeVel = targetNormal.dot(relativeVel);
		Vector3D relativeVelN = normalDotRelativeVel * targetNormal;
		Vector3D relativeVelT = relativeVel - relativeVelN; // V = Vn + Vt

		if (normalDotRelativeVel < 0.0)
		{
			Vector3D deltaRelativeVelN = (-restitutionCoefficient - 1.0) * relativeVelN;
			relativeVelN *= -restitutionCoefficient;

			if (relativeVelT.lengthSquared() > 0.0)
			{
				double frictionScale = max(1.0 - _frictionCoefficient * deltaRelativeVelN.lengthSquared() / relativeVelT.length(), 0.0);
				relativeVelT *= frictionScale;
			}

			*newVelocity = relativeVelN + relativeVelT + colliderVelAtTargetPoint;
		}
		*newPosition = targetPoint;
	}
}

double Collider3::frictionCoeffient() const
{
	return _frictionCoefficient;
}

void Collider3::setFrictionCoefficient(double newFrictionCoefficent)
{
	_frictionCoefficient = max(newFrictionCoefficent, 0.0);
}

const shared_ptr<Surface3>& Collider3::surface() const
{
	return _surface;
}

void Collider3::update(double currentTimeInSeconds, double timeIntervalInSeconds)
{
	if (!_surface->isValidGeometry())
		return;
	_surface->updateQueryEngine();

	if (_onUpdateCallback)
		_onUpdateCallback(this, currentTimeInSeconds, timeIntervalInSeconds);
}

void Collider3::setOnBeginUpdateCallback(const function<void(Collider3*, double, double)>& callback)
{
	_onUpdateCallback = callback;
}

