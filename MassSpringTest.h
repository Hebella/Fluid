#include <iostream>
#include "animation.h"
#include "array1.h"

#include <fstream>
#include <string>
#include <vector>

#include "SimpleMassSpringAnimation.h"

using namespace std;


inline void TestMassSpring(vector<Array1<double>> & x_coords, vector<Array1<double>> &y_coords)
{
    Array1<double> x;
    Array1<double> y;

    SimpleMassSpringAnimation anim(8);
    anim.constraints.push_back(SimpleMassSpringAnimation::Constraint{ 0, Vector3D(0.0, 0.0, 0.0), Vector3D() });
    anim.exportStates(x, y);

    char filename[256];
    snprintf(filename, sizeof(filename), "data.#line2,0000,x.npy");
    snprintf(filename, sizeof(filename), "data.#line2,0000,y.npy");

    for (Frame frame(0, 1.0 / 60.0); frame.index < 720; frame.advance())
    {
        anim.update(frame);
        anim.exportStates(x, y);

        x_coords.push_back(x);
        y_coords.push_back(y);

        snprintf(filename, sizeof(filename), "data.#line2,%04d,x.npy", frame.index);
        snprintf(filename, sizeof(filename), "data.#line2,%04d,y.npy", frame.index);
    }



}
