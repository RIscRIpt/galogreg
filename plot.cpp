#include <iostream>
#include <vector>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/freeglut.h> //glut.h extension for fonts

double LEFT = -1.0;
double RIGHT = +1.0;

double BOTTOM = -1.0;
double TOP = +1.0;

struct point {
    double x, y;
};

struct cpoint : point {
    bool kind;
};

struct fitness_history_entry {
    double avg, max;
};

struct line {
    double A, B, C;

    double getX(double y) const {
        // Ax + By + C = 0
        return (-B * y - C) / A;
    }

    double getY(double x) const {
        // Ax + By + C = 0
        return (-A * x - C) / B;
    }
};

line best_line;
std::vector<point> points1;
std::vector<point> points2;

void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(LEFT, RIGHT, BOTTOM, TOP);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_DOUBLE, sizeof(point), points1.data());
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, points1.size());
    //glDisableClientState(GL_VERTEX_ARRAY);

    //glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_DOUBLE, sizeof(point), points2.data());
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, points2.size());
    glDisableClientState(GL_VERTEX_ARRAY);

    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(10.0);
    glBegin(GL_LINES);
        glVertex2d(LEFT, best_line.getY(LEFT));
        glVertex2d(RIGHT, best_line.getY(RIGHT));
    glEnd();

    glFlush();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(640, 480);
    glutCreateWindow("Plot");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    size_t npoints;
    std::cin >> npoints;
    for (size_t i = 0; i < npoints; i++) {
        cpoint p;
        std::cin >> p.kind >> p.x >> p.y;
        if (p.kind)
            points1.emplace_back(p);
        else
            points2.emplace_back(p);

        LEFT = std::min(LEFT, p.x);
        RIGHT = std::max(RIGHT, p.x);
        TOP = std::max(TOP, p.y);
        BOTTOM = std::min(BOTTOM, p.y);
    }

    double unit = 1.0;
    LEFT -= unit;
    RIGHT += unit;
    TOP += unit;
    BOTTOM -= unit;

    while (true) {
        glutPostRedisplay();
        glutMainLoopEvent();

        fitness_history_entry history_entry;
        std::cin
            >> history_entry.avg
            >> history_entry.max;

        if (std::cin.fail())
            break;

        std::cin
            >> best_line.A
            >> best_line.B
            >> best_line.C;

        std::cout
            << history_entry.avg << '\t'
            << history_entry.max << '\t'
            << best_line.A << ' '
            << best_line.B << ' '
            << best_line.C << std::endl;
    }

    glutMainLoop();

    return 0;
}

