#pragma once

#include "collider3.h"

using namespace std;

class RigidBodyCollider3 final : public Collider3
{
public:
	Vector3D linearVelocity;

	Vector3D angularVelocity;

	explicit RigidBodyCollider3(const shared_ptr<Surface3>& surface);

	RigidBodyCollider3(const shared_ptr<Surface3>& surface, const Vector3D& _linearVelocity, const Vector3D& _angularVelocity);

	// the velocity of the collider at given point
	Vector3D velocityAt(const Vector3D& point) const override;
};