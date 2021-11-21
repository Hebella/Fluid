#include "particle_system_data3.h"
#include "parallel.h"
#include "point_hash_grid_searcher3.h"

static const size_t kDefaultHashGridResolution = 64;

ParticleSystemData3::ParticleSystemData3(): ParticleSystemData3(0)
{

}

ParticleSystemData3::ParticleSystemData3(size_t numberOfParticles)
{
	_positionIdx = addVectorData();
	_velocityIdx = addVectorData();
	_forceIdx = addVectorData();

	_neighborSearcher = make_shared<PointHashGridSearcher3>(
		kDefaultHashGridResolution,
		kDefaultHashGridResolution,
		kDefaultHashGridResolution,
		2.0 * _radius);

	resize(numberOfParticles);
}

ParticleSystemData3::~ParticleSystemData3()
{

}

void ParticleSystemData3::resize(size_t newNumberOfParticles)
{
	_numberOfParticles = newNumberOfParticles;

	for (auto& attr : _scalarDataList)
		attr.resize(newNumberOfParticles, 0.0);

	for (auto& attr : _vectorDataList)
		attr.resize(newNumberOfParticles, Vector3D());
}

size_t ParticleSystemData3::numberOfParticles() const
{
	return _numberOfParticles;
}

double ParticleSystemData3::radius() const
{
	return _radius;
}

void ParticleSystemData3::setRadius(double newRadius)
{
	_radius = max(newRadius, 0.0);
}

double ParticleSystemData3::mass() const
{
	return _mass;
}

void ParticleSystemData3::setMass(double newMass)
{
	_mass = max(newMass, 0.0);
}

ConstArrayAccessor1<Vector3D> ParticleSystemData3::positions() const
{
	return vectorDataAt(_positionIdx);
}

ConstArrayAccessor1<Vector3D> ParticleSystemData3::velocities() const
{
	return vectorDataAt(_velocityIdx);
}

ConstArrayAccessor1<Vector3D> ParticleSystemData3::forces() const
{
	return vectorDataAt(_forceIdx);
}

ArrayAccessor1<Vector3D> ParticleSystemData3::positions()
{
	return vectorDataAt(_positionIdx);
}

ArrayAccessor1<Vector3D> ParticleSystemData3::velocities()
{
	return vectorDataAt(_velocityIdx);
}

ArrayAccessor1<Vector3D> ParticleSystemData3::forces()
{
	return vectorDataAt(_forceIdx);
}

size_t ParticleSystemData3::addScalarData(double initialVal)
{
	size_t attrIdx = _scalarDataList.size();
	_scalarDataList.emplace_back(numberOfParticles(), initialVal);
	return attrIdx;
}

size_t ParticleSystemData3::addVectorData(const Vector3D& initialVal)
{
	size_t attrIdx = _vectorDataList.size();
	_vectorDataList.emplace_back(numberOfParticles(), initialVal);
	return attrIdx;
}

ConstArrayAccessor1<double> ParticleSystemData3::scalarDataAt(size_t idx) const
{
	return _scalarDataList[idx].constAccessor();
}

ArrayAccessor1<double> ParticleSystemData3::scalarDataAt(size_t idx)
{
	return _scalarDataList[idx].accessor();
}

ConstArrayAccessor1<Vector3D> ParticleSystemData3::vectorDataAt(size_t idx) const
{
	return _vectorDataList[idx].constAccessor();
}

ArrayAccessor1<Vector3D> ParticleSystemData3::vectorDataAt(size_t idx)
{
	return _vectorDataList[idx].accessor();
}

void ParticleSystemData3::addParticle(const Vector3D& newPosition, const Vector3D& newVelocity, const Vector3D& newForce)
{
	Array1<Vector3D> newPositions = { newPosition };
	Array1<Vector3D> newVelocities = { newVelocity };
	Array1<Vector3D> newForces = { newForce };

	addParticles(newPositions.constAccessor(), newVelocities.constAccessor(), newForces.constAccessor());
}

void ParticleSystemData3::addParticles(const ConstArrayAccessor1<Vector3D>& newPositions, const ConstArrayAccessor1<Vector3D>& newVelocities, const ConstArrayAccessor1<Vector3D>& newForces)
{
	size_t oldNumberOfParticles = numberOfParticles();
	size_t newNumberOfParticles = oldNumberOfParticles + newPositions.size();

	resize(newNumberOfParticles);

	auto pos = positions();
	auto vel = velocities();
	auto frc = forces();

	parallelFor(kZeroSize, newPositions.size(), [&](size_t i)
		{
			pos[i + oldNumberOfParticles] = newPositions[i];
		});

	if (newVelocities.size() > 0)
	{
		parallelFor(kZeroSize, newPositions.size(), [&](size_t i)
			{
				vel[i + oldNumberOfParticles] = newVelocities[i];
			});
	}

	if (newForces.size() > 0)
	{
		parallelFor(kZeroSize, newPositions.size(), [&](size_t i)
			{
				frc[i + oldNumberOfParticles] = newForces[i];
			});
	}
}

const shared_ptr<PointNeighborSearcher3>& ParticleSystemData3::neighborSearcher() const
{
	return _neighborSearcher;
}

void ParticleSystemData3::setNeighborSearcher(const shared_ptr<PointNeighborSearcher3>& newNeighborSearcher)
{
	_neighborSearcher = newNeighborSearcher;
}

const vector<vector<size_t>>& ParticleSystemData3::neighborLists() const
{
	return _neighborLists;
}

void ParticleSystemData3::buildNeighborSearcher(double maxSearchRadius)
{
	_neighborSearcher = make_shared<PointHashGridSearcher3>(
		kDefaultHashGridResolution,
		kDefaultHashGridResolution,
		kDefaultHashGridResolution,
		2.0 * maxSearchRadius);

	_neighborSearcher->build(positions());
}

void ParticleSystemData3::buildNeighborLists(double maxSearchRadius)
{
	_neighborLists.resize(numberOfParticles());

	auto points = positions();

	for (size_t i = 0; i < numberOfParticles(); ++i)
	{
		Vector3D origin = points[i];
		_neighborLists[i].clear();

		_neighborSearcher->forEachNearbyPoint(origin, maxSearchRadius, [&](size_t j, const Vector3D&) {
			if (i != j)
				_neighborLists[i].push_back(j);
			});

	}
}

void ParticleSystemData3::set(const ParticleSystemData3& other)
{
	_radius = other._radius;
	_mass = other._mass;
	_positionIdx = other._positionIdx;
	_velocityIdx = other._velocityIdx;
	_forceIdx = other._forceIdx;
	_numberOfParticles = other._numberOfParticles;

	for (auto& attr : other._scalarDataList)
	{
		_scalarDataList.emplace_back(attr);
	}

	for (auto& attr : other._vectorDataList)
	{
		_vectorDataList.emplace_back(attr);
	}

	_neighborSearcher = other._neighborSearcher->clone();
	_neighborLists = other._neighborLists;
}

ParticleSystemData3& ParticleSystemData3::operator=(const ParticleSystemData3& other)
{
	set(other);
	return *this;
}
