#include "pch.h"
#include "rigid_body_collider3.h"

using namespace std;

RigidBodyCollider3::RigidBodyCollider3(const shared_ptr<Surface3>& surface)
{
	setSurface(surface);
}

RigidBodyCollider3::RigidBodyCollider3(const shared_ptr<Surface3>& surface, const Vector3D& _linearVelocity, const Vector3D& _angularVelocity) : linearVelocity(_linearVelocity), angularVelocity(_angularVelocity)
{
	setSurface(surface);
}

Vector3D RigidBodyCollider3::velocityAt(const Vector3D& point) const
{
	Vector3D r = point - surface()->transform.translation();
	return linearVelocity + angularVelocity.cross(r);
}
