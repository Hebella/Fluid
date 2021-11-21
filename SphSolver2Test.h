#pragma once

#include "box2.h"
#include "implicit_surface_set2.h"
#include "plane2.h"
#include "rigid_body_collider2.h"
#include "sph_solver2.h"
#include "sphere2.h"
#include "surface_to_implicit2.h"
#include "volume_particle_emitter2.h"
#include "constant_vector_field2.h"
#include "point_particle_emitter2.h"
#include <iostream>

using namespace std;

inline void saveParticleDataXy(const shared_ptr<ParticleSystemData2>& particles, vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords)
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

inline void TestSphSolver2(vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords)
{
    SphSolver2 solver;
    solver.setViscosityCoefficient(0.1);
    solver.setPseudoViscosityCoefficient(10.0);

    SphSystemData2Ptr particles = solver.sphSystemData();
    const double targetSpacing = particles->targetSpacing();

    cout << targetSpacing << endl;
    BoundingBox2D initialBound(Vector2D(), Vector2D(1, 0.5));
    initialBound.expand(-targetSpacing);

    auto emitter = std::make_shared<VolumeParticleEmitter2>(
        std::make_shared<SurfaceToImplicit2>(
            std::make_shared<Sphere2>(Vector2D(), 10.0)),
        initialBound,
        particles->targetSpacing(),
        Vector2D());
    emitter->setJitter(0.0);
    solver.setEmitter(emitter);

    Box2Ptr box = std::make_shared<Box2>(Vector2D(), Vector2D(1, 1));
    box->isNormalFlipped = true;
    RigidBodyCollider2Ptr collider = std::make_shared<RigidBodyCollider2>(box);

    solver.setCollider(collider);

    for (Frame frame(0, 1.0 / 60.0); frame.index < 100; frame.advance())
    {
        cout << frame.index << endl;

        solver.update(frame);

        saveParticleDataXy(particles, x_coords, y_coords);
    }

}


inline void TestSphSolver2WaterDrop(vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords)
{
    const double targetSpacing = 0.05;

    BoundingBox2D domain(Vector2D(), Vector2D(1, 2));

    SphSolver2 solver;
    solver.setViscosityCoefficient(0.0);

    SphSystemData2Ptr particles = solver.sphSystemData();
    particles->setTargetDensity(1000.0);
    particles->setTargetSpacing(targetSpacing);

    ImplicitSurfaceSet2Ptr surfaceSet = make_shared<ImplicitSurfaceSet2>();
    surfaceSet->addExplicitSurface(make_shared<Plane2>(Vector2D(0, 1), Vector2D(0, 0.25 * domain.height())));
    surfaceSet->addExplicitSurface(make_shared<Sphere2>(domain.midPoint(), 0.15 * domain.width()));

    BoundingBox2D sourceBound(domain);
    sourceBound.expand(-targetSpacing);

    auto emitter = make_shared<VolumeParticleEmitter2>(surfaceSet, sourceBound, targetSpacing, Vector2D());
    solver.setEmitter(emitter);

    Box2Ptr box = make_shared<Box2>(domain);
    box->isNormalFlipped = true;
    RigidBodyCollider2Ptr collider = make_shared<RigidBodyCollider2>(box);

    solver.setCollider(collider);

    saveParticleDataXy(particles, x_coords, y_coords);

    for (Frame frame(0, 1.0 / 60.0); frame.index < 50; frame.advance())
    {
        cout << frame.index << endl;

        solver.update(frame);

        saveParticleDataXy(particles, x_coords, y_coords);
    }
}

inline void TestParticleSystemSolver2(vector<Array1<double>>& x_coords, vector<Array1<double>>& y_coords)
{
    Plane2Ptr plane = std::make_shared<Plane2>(Vector2D(0, 1), Vector2D());
    RigidBodyCollider2Ptr collider = std::make_shared<RigidBodyCollider2>(plane);
    ConstantVectorField2Ptr wind = std::make_shared<ConstantVectorField2>(Vector2D(1, 0));

    ParticleSystemSolver2 solver;
    solver.setCollider(collider);
    solver.setRestitutionCoefficient(1.0);
    solver.setWind(wind);

    ParticleSystemData2Ptr particles = solver.particleSystemData();
    PointParticleEmitter2Ptr emitter
    = std::make_shared<PointParticleEmitter2>(
        Vector2D(0, 3),
        Vector2D(0, 1), 5.0, 45.0);
    emitter->setMaxNumberOfNewParticlesPerSecond(100);
    solver.setEmitter(emitter);

    saveParticleDataXy(particles, x_coords, y_coords);

    for (Frame frame(0, 1.0 / 60.0); frame.index < 360; frame.advance()) {
        cout << frame.index << endl;

        solver.update(frame);

        saveParticleDataXy(particles, x_coords, y_coords);
    }
}