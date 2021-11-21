#pragma once
#include "point_neighbor_searcher3.h"
#include "point3.h"
#include "size3.h"
#include "array_accessor1.h"

class PointHashGridSearcher3 final : public PointNeighborSearcher3
{
private:
	double  _gridSpacing = 1.0;
	Point3I _resolution = Point3I(1, 1, 1);
	vector<Vector3D> _points;
	vector<vector<size_t>> _buckets;

	size_t getHashKeyFromPosition(const Vector3D& position) const;

	void getNearbyKeys(const Vector3D& position, size_t* nearbyKeys) const;

public:
	// The grid spacing must be 2x or greater than search radius.
	PointHashGridSearcher3(const Size3& resolution, double gridSpacing);

	PointHashGridSearcher3(size_t resolutionX, size_t resolutionY, size_t resolutionZ, double gridSpacing);

	PointHashGridSearcher3(const PointHashGridSearcher3& other);

	string typeName() const override
	{
		return "PointHashGridSearcher3";
	}

	void build(const ConstArrayAccessor1<Vector3D>& points) override;

	void forEachNearbyPoint(const Vector3D& origin, double radius, const function<void(size_t, const Vector3D&)>& callback) const override;
	
	bool hasNearbyPoint(const Vector3D& origin, double radius) const override;

	shared_ptr<PointNeighborSearcher3> clone() const override;

	void add(const Vector3D& point);

	const vector<vector<size_t>>& buckets() const;

	size_t getHashKeyFromBucketIndex(const Point3I& bucketIndex) const;

	Point3I getBucketIndex(const Vector3D& position) const;

	PointNeighborSearcher3& operator=(const PointHashGridSearcher3& other);

	void set(const PointHashGridSearcher3& other);
};