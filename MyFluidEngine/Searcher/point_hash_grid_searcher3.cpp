#include "point_hash_grid_searcher3.h"
#include "../Utils/Array/array1.h"

PointHashGridSearcher3::PointHashGridSearcher3(const Size3& resolution, double gridSpacing): PointHashGridSearcher3(resolution.x, resolution.y, resolution.z, gridSpacing)
{

}

PointHashGridSearcher3::PointHashGridSearcher3(size_t resolutionX, size_t resolutionY, size_t resolutionZ, double gridSpacing): _gridSpacing(gridSpacing)
{
	_resolution.x = max(static_cast<ssize_t>(resolutionX), kOneSSize);
	_resolution.y = max(static_cast<ssize_t>(resolutionY), kOneSSize);
	_resolution.z = max(static_cast<ssize_t>(resolutionZ), kOneSSize);
}

PointHashGridSearcher3::PointHashGridSearcher3(const PointHashGridSearcher3& other)
{
	set(other);
}

void PointHashGridSearcher3::build(const ConstArrayAccessor1<Vector3D>& points)
{
	_buckets.clear();
	_points.clear();

	_buckets.resize(_resolution.x * _resolution.y * _resolution.z);
	_points.resize(points.size());

	if (points.size() == 0)
		return;

	for (size_t i = 0; i < points.size(); ++i)
	{
		_points[i] = points[i];
		size_t key = getHashKeyFromPosition(points[i]);
		_buckets[key].push_back(i);
	}

}

void PointHashGridSearcher3::forEachNearbyPoint(const Vector3D& origin, double radius, const function<void(size_t, const Vector3D&)>& callback) const
{
	if (_buckets.empty())
		return;
	size_t nearbyKeys[8];
	getNearbyKeys(origin, nearbyKeys); // find buckets where the points are nearby

	const double queryRadiusSquared = radius * radius;

	for (int i = 0; i < 8; ++i)
	{
		const auto& bucket = _buckets[nearbyKeys[i]];
		size_t numberOfPointsInBucket = bucket.size();

		for (size_t j = 0; j < numberOfPointsInBucket; ++j)
		{
			size_t pointIndex = bucket[j];
			double rSquared = (_points[pointIndex] - origin).lengthSquared();
			if (rSquared <= queryRadiusSquared)
				callback(pointIndex, _points[pointIndex]);
		}
	}
}

bool PointHashGridSearcher3::hasNearbyPoint(const Vector3D& origin, double radius) const
{
	if (_buckets.empty())
		return false;
	size_t nearbyKeys[8];
	getNearbyKeys(origin, nearbyKeys); // find buckets where the points are nearby

	const double queryRadiusSquared = radius * radius;

	for (int i = 0; i < 8; ++i)
	{
		const auto& bucket = _buckets[nearbyKeys[i]];
		size_t numberOfPointsInBucket = bucket.size();

		for (size_t j = 0; j < numberOfPointsInBucket; ++j)
		{
			size_t pointIndex = bucket[j];
			double rSquared = (_points[pointIndex] - origin).lengthSquared();
			if (rSquared <= queryRadiusSquared)
				return true;
		}
	}
	return false;
}

shared_ptr<PointNeighborSearcher3> PointHashGridSearcher3::clone() const
{
	return shared_ptr<PointNeighborSearcher3>(new PointHashGridSearcher3(*this));
}

void PointHashGridSearcher3::add(const Vector3D& point)
{
	if (_buckets.empty())
	{
		Array1<Vector3D> arr = { point };
		build(arr);
	}
	else
	{
		size_t i = _points.size();
		_points.push_back(point);
		size_t key = getHashKeyFromPosition(point);
		_buckets[key].push_back(i);
	}
}

const vector<vector<size_t>>& PointHashGridSearcher3::buckets() const
{
	return _buckets;
}

Point3I PointHashGridSearcher3::getBucketIndex(const Vector3D& position) const
{
	Point3I bucketIndex;
	bucketIndex.x = static_cast<ssize_t>(floor(position.x / _gridSpacing));
	bucketIndex.y = static_cast<ssize_t>(floor(position.y / _gridSpacing));
	bucketIndex.z = static_cast<ssize_t>(floor(position.z / _gridSpacing));
	return bucketIndex;
}

size_t PointHashGridSearcher3::getHashKeyFromPosition(const Vector3D& position) const
{
	Point3I bucketIndex = getBucketIndex(position);
	return getHashKeyFromBucketIndex(bucketIndex);
}

void PointHashGridSearcher3::getNearbyKeys(const Vector3D& position, size_t* nearbyKeys) const
{
	Point3I originIndex = getBucketIndex(position), nearbyBucketIndex[8];
	for (int i = 0; i < 8; ++i)
		nearbyBucketIndex[i] = originIndex;

	if ((originIndex.x + 0.5) * _gridSpacing <= position.x) // (originIndex.x + 0.5) <= position.x / _gridSpacing < (originIndex.x + 1)
	{
		nearbyBucketIndex[4].x += 1; // front 0 4 
		nearbyBucketIndex[5].x += 1; //       2 6
		nearbyBucketIndex[6].x += 1; // back  1 5
		nearbyBucketIndex[7].x += 1; //       3 7
	}
	else
	{
		nearbyBucketIndex[4].x -= 1; 
		nearbyBucketIndex[5].x -= 1; 
		nearbyBucketIndex[6].x -= 1; 
		nearbyBucketIndex[7].x -= 1; 
	}

	if ((originIndex.y + 0.5) * _gridSpacing <= position.y)
	{
		nearbyBucketIndex[2].y += 1; 
		nearbyBucketIndex[3].y += 1;
		nearbyBucketIndex[6].y += 1; 
		nearbyBucketIndex[7].y += 1;
	}
	else
	{
		nearbyBucketIndex[2].y -= 1;
		nearbyBucketIndex[3].y -= 1;
		nearbyBucketIndex[6].y -= 1;
		nearbyBucketIndex[7].y -= 1;
	}

	if ((originIndex.z + 0.5) * _gridSpacing <= position.z)
	{
		nearbyBucketIndex[1].z += 1;
		nearbyBucketIndex[3].z += 1;
		nearbyBucketIndex[5].z += 1;
		nearbyBucketIndex[7].z += 1;
	}
	else
	{
		nearbyBucketIndex[1].z -= 1;
		nearbyBucketIndex[3].z -= 1;
		nearbyBucketIndex[5].z -= 1;
		nearbyBucketIndex[7].z -= 1;
	}

	for (int i = 0; i < 8; ++i)
		nearbyKeys[i] = getHashKeyFromBucketIndex(nearbyBucketIndex[i]);
}

size_t PointHashGridSearcher3::getHashKeyFromBucketIndex(const Point3I& bucketIndex) const
{
	Point3I wrappedIndex = bucketIndex;
	wrappedIndex.x = bucketIndex.x % _resolution.x;
	wrappedIndex.y = bucketIndex.y % _resolution.y;
	wrappedIndex.z = bucketIndex.z % _resolution.z;
	if (wrappedIndex.x < 0)
		wrappedIndex.x += _resolution.x;
	if (wrappedIndex.y < 0)
		wrappedIndex.y += _resolution.y;
	if (wrappedIndex.z < 0)
		wrappedIndex.z += _resolution.z;
	return static_cast<size_t>((wrappedIndex.z * _resolution.y + wrappedIndex.y) * _resolution.x + wrappedIndex.x);
}



PointNeighborSearcher3& PointHashGridSearcher3::operator=(const PointHashGridSearcher3& other)
{
	set(other);
	return *this;
}

void PointHashGridSearcher3::set(const PointHashGridSearcher3& other)
{
	_gridSpacing = other._gridSpacing;
	_resolution = other._resolution;
	_points = other._points;
	_buckets = other._buckets;
}
