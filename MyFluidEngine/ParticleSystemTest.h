#include "Collider/rigid_body_collider3.h"
#include "Utils/Field/constant_vector_field3.h"
#include "ParticleSystem/particle_system_solver3.h"
#include "ParticleEmitter/point_particle_emitter3.h"
#include "Utils/Plane/plane3.h"
#include <iostream>

using namespace std;

inline void saveParticleDataXy(const shared_ptr<ParticleSystemData3>& particles, vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords)
{
	size_t n = particles->numberOfParticles();
	Array1<double> x(n);
	Array1<double> y(n);
	auto positions = particles->positions();
	for (size_t i = 0; i < n; ++i)
	{
		x[i] = positions[i].x;
		y[i] = positions[i].y;

		//cout << x[i] << " " << y[i] << endl;
	}

	x_coords.push_back(x);
	y_coords.push_back(y);
}

inline void TestParticleSystem(vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords)
{
	/*Plane3Ptr plane = make_shared<Plane3>(Vector3D(0, 1, 0), Vector3D());
	shared_ptr<RigidBodyCollider3> collider = make_shared<RigidBodyCollider3>(plane);

	ParticleSystemSolver3 solver;
	solver.setCollider(collider);
	solver.setDragCoefficient(0.0);
	solver.setRestitutionCoefficient(1.0);

	shared_ptr<ParticleSystemData3> particles = solver.particleSystemData();
	particles->addParticle({ 0.0, 3.0, 0.0 }, { 20.0, 0.0, 0.0 }); // position, speed

	Array1<double> x(1);
	Array1<double> y(1);
	Frame frame;
	frame.timeIntervalInSeconds = 1.0 / 300.0;
	for (; frame.index < 1000; frame.advance())
	{
		solver.update(frame);
		x[0] = particles->positions()[0].x;
		y[0] = particles->positions()[0].y;
		if (frame.index > 0)
		{
			x_coords.push_back(x);
			y_coords.push_back(y);
		}
		
	}*/
	Plane3Ptr plane = std::make_shared<Plane3>(Vector3D(0, 1, 0), Vector3D());
	shared_ptr<RigidBodyCollider3> collider = make_shared<RigidBodyCollider3>(plane);

	ParticleSystemSolver3 solver;
	solver.setCollider(collider);
	solver.setRestitutionCoefficient(1.0);

	shared_ptr<ParticleSystemData3> particles = solver.particleSystemData();
	PointParticleEmitter3Ptr emitter
		= std::make_shared<PointParticleEmitter3>(
			Vector3D(0, 3, 0),
			Vector3D(0, 1, 0), 5.0, 45.0);
	emitter->setMaxNumberOfNewParticlesPerSecond(100);
	solver.setEmitter(emitter);

	saveParticleDataXy(particles, x_coords, y_coords);

	for (Frame frame(0, 1.0 / 60.0); frame.index < 360; frame.advance()) {
		cout << frame.index << endl;

		solver.update(frame);

		saveParticleDataXy(particles, x_coords, y_coords);
	}
}