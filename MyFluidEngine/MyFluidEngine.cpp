// FluidEngine.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "Utils/cnpy.h"
#include "Utils/pystring.h"
#include "Utils/constants.h"

#include <memory>
#include <direct.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include "Animation/SimpleMassSpringAnimation.h"
#include "MassSpringTest.h"
#include "ParticleSystemTest.h"
#include "SphSolver3Test.h"

using namespace std;

#define TESTS_OUTPUT_DIR "manual_tests_output"
string testCollectionDir;
string currentTestDir;

class SineAnimation : public Animation {
public:
    double x = 0.0;

protected:
    void onUpdate(const Frame& frame) override {
        x = std::sin(10.0 * frame.timeInSeconds());
    }
};

inline void createDirectory(const std::string& dirname) {
    std::vector<std::string> tokens;
    pystring::split(dirname, tokens, "/");
    std::string partialDir;
    for (const auto& token : tokens) {
        partialDir = pystring::os::path::join(partialDir, token);
#ifdef JET_WINDOWS
        _mkdir(partialDir.c_str());
#else
        mkdir(partialDir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
#endif
    }
}

void SetUp()
{
    testCollectionDir = pystring::os::path::join(TESTS_OUTPUT_DIR, "test_anim");
    createDirectory(testCollectionDir);
}

string getTestDirectoryName(const std::string& name)
{
    return pystring::os::path::join(testCollectionDir, name);
}

void createTestDirectory(const std::string& name) {
    currentTestDir = getTestDirectoryName(name);
    createDirectory(currentTestDir);
}

string getFullFilePath(const std::string& name)
{
    if (!currentTestDir.empty())
    {
        return pystring::os::path::join(currentTestDir, name);
    }
    else
    {
        return name;
    }
}

void saveData(const vector<Array1<double>>& data, const std::string& filename)
{
    ofstream outfile(filename, ios::trunc);
    for (int i = 0; i < data.size(); ++i)
    {
        for (int j = 0; j < data[i].size(); ++j)
            outfile << data[i][j] << " ";
        outfile << '\n';
    }
    outfile.close();
}



int idx = 0;

vector<Array1<double>> x_coords;
vector<Array1<double>> y_coords;
vector<Array1<double>> z_coords;

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);

    glPointSize(2);


    Array1<double> cur_x = x_coords[idx], cur_y = y_coords[idx];
    glBegin(GL_POINTS);
    for (int i = 0; i < cur_x.size(); ++i)
    {
        float x = cur_x[i] * 100 + 300;
        float y = cur_y[i] * 100 + 100;
        //cout << x << " " << y << endl;
        glVertex3f(x, y, 0.0f);
    }
    glEnd();


    /*glLineWidth(2);
    glBegin(GL_LINE_STRIP);
        for (int i = 0; i < cur_x.size(); ++i)
        {
            float x = cur_x[i] * 20 + 300;
            float y = cur_y[i] * 20 + 100;
            glVertex3f(x, y, 0.0f);
        }
    glEnd();*/
    //清空命令缓冲区并交换帧缓存
    glutSwapBuffers();

}


void Initial()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  //清屏颜色
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 600.0, 0.0, 600.0);   //投影到裁剪窗大小：世界
}


void TimerFunction(int value)
{
    if (idx == x_coords.size() - 1)
        idx = 0;
    else
        idx += 1;

    glutPostRedisplay();
    glutTimerFunc(1000, TimerFunction, value);
}

int main(int argc, char** argv)
{

    /*vector<Array1<double>> coords_x;
    for (int j = 0; j < 20; ++j)
    {
        Array1<double> x(1000);
        for (int i = 0; i < 1000; ++i)
        {
            x[i] = j + kPiD * i;
        }
        coords_x.push_back(x);
    }

    saveData(coords_x, "test.txt");*/

    //TestMassSpring(x_coords, y_coords);
    //TestParticleSystem(x_coords, y_coords);

    TestSphSolver3(x_coords, y_coords, z_coords);

    saveData(x_coords, "test_x.txt");
    saveData(y_coords, "test_y.txt");
    saveData(z_coords, "test_z.txt");
    //TestParticleSystem(x_coords, y_coords);

    /*glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(600, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Create Dot");

    glutDisplayFunc(RenderScene);
    glutTimerFunc(1000, TimerFunction, 1);
    Initial();
    glutMainLoop();*/




    //saveData(t.constAccessor(), "data.#line2,x.npy");
    //saveData(data.constAccessor(), "data.#line2,y.npy");
    return 0;
}
