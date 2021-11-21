#pragma once
#include "vector3.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class PointNeighborSearcher3
{
public:
	PointNeighborSearcher3() {};

	virtual ~PointNeighborSearcher3() {};

	virtual string typeName() const = 0;

	virtual void build(const ConstArrayAccessor1<Vector3D>& points) = 0;

	virtual void forEachNearbyPoint(const Vector3D& origin, double radius, const function<void(size_t, const Vector3D&)>& callback) const = 0;

	virtual bool hasNearbyPoint(const Vector3D& origin, double radius) const = 0;

	virtual shared_ptr<PointNeighborSearcher3> clone() const = 0;
};