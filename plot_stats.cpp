#include <iostream>
#include <deque>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/freeglut.h> //glut.h extension for fonts

double LEFT = 0.0;
double RIGHT = 1.0;

double BOTTOM = 0.0;
double TOP = 0.0;

struct fitness_history_entry {
    double avg, max;
};

std::deque<fitness_history_entry> fitness_history;
double fitness_history_max = TOP;

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void display() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(LEFT, RIGHT, BOTTOM, fitness_history_max);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    double Xwidth = RIGHT - LEFT;
    double Xstep = Xwidth / fitness_history.size();

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    double x = 0;
    for (auto const &e : fitness_history) {
        glVertex2d(x, e.max);
        x += Xstep;
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    x = 0;
    for (auto const &e : fitness_history) {
        glVertex2d(x, e.avg);
        x += Xstep;
    }
    glEnd();

    glFlush();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(640, 480);
    glutCreateWindow("Stats");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    while (true) {
        glutPostRedisplay();
        glutMainLoopEvent();

        fitness_history_entry history_entry;
        std::cin
            >> history_entry.avg
            >> history_entry.max;

        if (std::cin.fail())
            break;

        fitness_history.emplace_back(history_entry);
        fitness_history_max = std::max(fitness_history_max, history_entry.max * 1.1);

        if (fitness_history.size() > 100)
            fitness_history.pop_front();

        double A, B, C;
        std::cin
            >> A
            >> B
            >> C;

        std::cout
            << history_entry.avg << '\t'
            << history_entry.max << '\t'
            << A << ' '
            << B << ' '
            << C << std::endl;
    }

    glutMainLoop();

    return 0;
}

