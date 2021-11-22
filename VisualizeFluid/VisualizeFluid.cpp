// VisualizeFluid.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

vector<vector<double>> x_coords, y_coords, z_coords;

int idx;

void ReadData(vector<vector<double>>& data, const std::string& filename)
{
    ifstream infile(filename, ios::in);
    char* line = new char[200000];
    while (!infile.eof())
    {
        infile.getline(line, 200000);
        stringstream sstream;
        sstream << line;
        double num;
        vector<double> tmp;
        while (sstream >> num)
            tmp.push_back(num);
        if (tmp.size() != 0)
            data.push_back(tmp);
    }
}

void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLoadIdentity();  //加载单位矩阵
    gluLookAt(2.0, 0.0, 2.0, 0.85, 0, 0, 0.0, 1.0, 0.0);

    glPointSize(1);

    vector<double> cur_x = x_coords[idx], cur_y = y_coords[idx], cur_z = z_coords[idx];
    glBegin(GL_POINTS);
    for (int i = 0; i < cur_x.size(); ++i)
    {
        float x = cur_x[i] * 2 - 1;
        float y = cur_y[i] * 2 - 1;
        float z = -cur_z[i] - 1;
        //cout << x << " " << y << " " << z << endl;
        //glPushMatrix();
        //glTranslatef(x, y, z);
        //glutSolidSphere(0.01, 10, 10);
        //glPopMatrix();
        glVertex3f(x, y, z);
    }
    glEnd();



    glutSwapBuffers();

}


void Initial()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  //清屏颜色
    glViewport(0, 0, 600, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.0, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 2.9, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void TimerFunction(int value)
{
    if (idx == x_coords.size() - 1)
        idx = 0;
    else
        idx += 1;

    glutPostRedisplay();
    glutTimerFunc(100, TimerFunction, value);
}


int main(int argc, char** argv)
{
    ReadData(x_coords, "test_x_0025.txt");
    ReadData(y_coords, "test_y_0025.txt");
    ReadData(z_coords, "test_z_0025.txt");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Create Dot");

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(reshape);
    glutTimerFunc(100, TimerFunction, 1);
    Initial();
    glutMainLoop();
}