#pragma once
#include "physics_animation.h"
#include "array1.h"
#include <vector>
#include "vector3.h"
#include "vector_field3.h"

using namespace std;
class SimpleMassSpringAnimation : public PhysicsAnimation
{
public:
	struct Edge
	{
		size_t first;
		size_t second;
	};

	struct Constraint
	{
		size_t pointIndex;
		Vector3D fixedPosition;
		Vector3D fixedVelocity;
	};

	// positions of the points
	vector<Vector3D> positions;
	vector<Vector3D> velocities;
	vector<Vector3D> forces;
	vector<Edge> edges;

	double mass = 1.0;
	Vector3D gravity = Vector3D(0.0, -9.8, 0.0);
	double stiffness = 500.0;
	double restLength = 1.0;
	double dampingCoefficient = 1.0;
	double dragCoefficient = 0.1;
	shared_ptr<VectorField3> wind;
	vector<Constraint> constraints;
	double floorPositionY = -300.0;
	double restitutionCoefficient = 0.3;

	SimpleMassSpringAnimation(size_t numberOfPoints = 10): PhysicsAnimation()
	{
		size_t numberOfEdges = numberOfPoints - 1;

		positions.resize(numberOfPoints);
		velocities.resize(numberOfPoints);
		forces.resize(numberOfPoints);
		edges.resize(numberOfEdges);

		for (size_t i = 0; i < numberOfPoints; ++i)
		{
			positions[i].x = static_cast<double>(i);
		}

		for (size_t i = 0; i < numberOfEdges; ++i)
			edges[i] = Edge{ i, i + 1 };

	}

	void exportStates(Array1<double>& x, Array1<double>& y) const
	{
		x.resize(positions.size());
		y.resize(positions.size());
		
		for (size_t i = 0; i < positions.size(); ++i)
		{
			x[i] = positions[i].x;
			y[i] = positions[i].y;
		}
	}

protected:
	void onAdvanceTimeStep(double timeIntervalInSeconds) override
	{
		size_t numberOfPoints = positions.size();
		size_t numberOfEdges = edges.size();

		for (size_t i = 0; i < numberOfPoints; ++i)
		{
			forces[i] = mass * gravity;

			Vector3D  relativeVel = velocities[i];
			if (wind != NULL)
				relativeVel -= wind->sample(positions[i]);
			forces[i] += -dragCoefficient * relativeVel;
		}

		for (size_t i = 0; i < numberOfEdges; ++i)
		{
			size_t pointIndex0 = edges[i].first;
			size_t pointIndex1 = edges[i].second;

			Vector3D pos0 = positions[pointIndex0];
			Vector3D pos1 = positions[pointIndex1];
			Vector3D r = pos0 - pos1;
			double distance = r.length();
			if (distance > 0.0)
			{
				Vector3D force = -stiffness * (distance - restLength) * r.normalized();
				forces[pointIndex0] += force;
				forces[pointIndex1] -= force;
			}

			//  damping force tries to reduce the ¡°relative¡± velocity between two points
			Vector3D vel0 = velocities[pointIndex0];
			Vector3D vel1 = velocities[pointIndex1];
			Vector3D damping = -dampingCoefficient * (vel0 - vel1);
			forces[pointIndex0] += damping;
			forces[pointIndex1] -= damping;
			//cout << damping.x << " " << damping.y << " " << damping.z << endl;
		}


		for (size_t i = 0; i < numberOfPoints; ++i)
		{
			Vector3D newAcceleration = forces[i] / mass;
			Vector3D newVelocity = velocities[i] + timeIntervalInSeconds * newAcceleration;
			Vector3D newPosition = positions[i] + timeIntervalInSeconds * newVelocity;

			// collision
			if (newPosition.y < floorPositionY)
			{
				newPosition.y = floorPositionY;

				if (newVelocity.y < 0.0)
				{
					newVelocity.y *= -restitutionCoefficient;
					newVelocity.y += timeIntervalInSeconds * newVelocity.y;
				}
			}

			velocities[i] = newVelocity;
			positions[i] = newPosition;			
		}

		for (size_t i = 0; i < constraints.size(); ++i)
		{
			size_t pointIndex = constraints[i].pointIndex;
			positions[pointIndex] = constraints[i].fixedPosition;
			positions[pointIndex] = constraints[i].fixedVelocity;
		}
	}
};