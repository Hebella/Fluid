#pragma once

#include "../Utils/Vector/vector3.h"
#include <vector>
#include "../Utils/Array/array1.h"
#include "../Utils/Array/array_accessor1.h"
#include "../Searcher/point_neighbor_searcher3.h"

using namespace std;

class ParticleSystemData3
{
private:
	double _radius = 1e-3;
	double _mass = 1e-3;
	vector<Array1<double>> _scalarDataList;
	vector<Array1<Vector3D>> _vectorDataList;
	size_t _positionIdx;
	size_t _velocityIdx;
	size_t _forceIdx;
	size_t _numberOfParticles = 0;

	shared_ptr<PointNeighborSearcher3> _neighborSearcher;
	vector<vector<size_t>> _neighborLists;

public:
	ParticleSystemData3();
	ParticleSystemData3(size_t numberOfParticles);
	virtual ~ParticleSystemData3();

	void resize(size_t newNumberOfParticles);
	size_t numberOfParticles() const;

	double radius() const;
	virtual void setRadius(double newRadius);
	double mass() const;
	virtual void setMass(double newMass);

	// returns the position array
	ConstArrayAccessor1<Vector3D> positions() const;
	ConstArrayAccessor1<Vector3D> velocities() const;
	ConstArrayAccessor1<Vector3D> forces() const;

	ArrayAccessor1<Vector3D> positions();
	ArrayAccessor1<Vector3D> velocities();
	ArrayAccessor1<Vector3D> forces();

	// return the index of the data which can be used later on to access the data using scalarDataAt function
	size_t addScalarData(double initialVal = 0.0);
	size_t addVectorData(const Vector3D& initialVal = Vector3D());

	ConstArrayAccessor1<double> scalarDataAt(size_t idx) const;
	ArrayAccessor1<double> scalarDataAt(size_t idx);

	ConstArrayAccessor1<Vector3D> vectorDataAt(size_t idx) const;
	ArrayAccessor1<Vector3D> vectorDataAt(size_t idx);

	void addParticle(const Vector3D& newPosition, const Vector3D& newVelocity = Vector3D(), const Vector3D& newForce = Vector3D());
	void addParticles(const ConstArrayAccessor1<Vector3D>& newPositions, const ConstArrayAccessor1<Vector3D>& newVelocities = ConstArrayAccessor1<Vector3D>(), const ConstArrayAccessor1<Vector3D>& newForces = ConstArrayAccessor1<Vector3D>());

	const shared_ptr<PointNeighborSearcher3>& neighborSearcher() const;

	void setNeighborSearcher(const shared_ptr<PointNeighborSearcher3>& newNeighborSearcher);

	const vector<vector<size_t>>& neighborLists() const;

	void buildNeighborSearcher(double maxSearchRadius);

	void buildNeighborLists(double maxSearchRadius);

	void set(const ParticleSystemData3& other);

	ParticleSystemData3& operator=(const ParticleSystemData3& other);
};

typedef shared_ptr<ParticleSystemData3> ParticleSystemData3Ptr;