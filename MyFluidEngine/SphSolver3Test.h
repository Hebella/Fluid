#pragma once

#include "Sph/sph_solver3.h"
#include "ParticleEmitter/volume_particle_emitter3.h"
#include "Utils/Surface/surface_to_implicit3.h"
#include "Utils/Sphere/sphere3.h"
#include "Utils/Box/box3.h"

#include <iostream>

using namespace std;

inline void saveParticleDataXy(const shared_ptr<ParticleSystemData3>& particles, unsigned int frameNum, vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords, vector<Array1<double>>& z_coords)
{
    size_t n = particles->numberOfParticles();
    Array1<double> x(n);
    Array1<double> y(n);
    Array1<double> z(n);
    auto positions = particles->positions();
    for (size_t i = 0; i < n; ++i)
    {
        x[i] = positions[i].x;
        y[i] = positions[i].y;
        z[i] = positions[i].z;
            
        //cout << x[i] << " " << y[i] << " " << z[i] << endl;
    }

    x_coords.push_back(x);
    y_coords.push_back(y);
    z_coords.push_back(z);
}

inline void TestSphSolver3(vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords, vector<Array1<double>>& z_coords)
{
    const double targetSpacing = 0.1;

	SphSolver3 solver;
	//solver.setViscosityCoefficient(0.1);
	solver.setPseudoViscosityCoefficient(1.0);

	shared_ptr<SphSystemData3> particles = solver.sphSystemData();
    particles->setTargetDensity(1000.0);
    particles->setTargetSpacing(targetSpacing);
	
    BoundingBox3D initialBound(Vector3D(), Vector3D(0.5, 0.2, 0.5));
    initialBound.expand(-targetSpacing);

    auto emitter = std::make_shared<VolumeParticleEmitter3>(
        std::make_shared<SurfaceToImplicit3>(
            std::make_shared<Sphere3>(Vector3D(), 10.0)),
        initialBound,
        particles->targetSpacing(),
        Vector3D());
    emitter->setJitter(0.0);
    solver.setEmitter(emitter);

    Box3Ptr box = std::make_shared<Box3>(Vector3D(), Vector3D(1, 1, 0.5));
    box->isNormalFlipped = true;
    shared_ptr<RigidBodyCollider3> collider = make_shared<RigidBodyCollider3>(box);

    solver.setCollider(collider);

    for (Frame frame(0, 1.0 / 100.0); frame.index < 5; frame.advance())
    {
        cerr << frame.index << endl;

        solver.update(frame);

        saveParticleDataXy(particles, 1, x_coords, y_coords, z_coords);

        
    }
    
}