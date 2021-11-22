#pragma once

#include "../Utils/Surface/surface3.h"
#include <functional>

using namespace std;

class Collider3
{
private:
	shared_ptr<Surface3> _surface;
	double _frictionCoefficient = 0.0;
	function<void(Collider3*, double, double)> _onUpdateCallback;
protected:
	struct ColliderQueryResult final
	{
		double distance;
		Vector3D point;
		Vector3D normal;
		Vector3D velocity;
	};

	void setSurface(const shared_ptr<Surface3>& newSurface);

	void getClosestPoint(const shared_ptr<Surface3>& surface, const Vector3D& queryPoint, ColliderQueryResult* result) const;

	bool isPenetrating(const ColliderQueryResult& colliderPoint, const Vector3D& position, double radius);

public:
	Collider3();

	virtual ~Collider3();

	virtual Vector3D velocityAt(const Vector3D& point) const = 0;

	void resolveCollision(double radius, double restitutionCoefficient, Vector3D* newPosition, Vector3D* newVelocity);

	double frictionCoeffient() const;

	void setFrictionCoefficient(double newFrictionCoefficent);

	const shared_ptr<Surface3>& surface() const;

	void update(double currentTimeInSeconds, double timeIntervalInSeconds);

	void setOnBeginUpdateCallback(const function<void(Collider3*, double, double)>& callback);
};