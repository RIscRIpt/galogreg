#include <iostream>
#include <random>
#include <chrono>

#define _USE_MATH_DEFINES
#include <cmath>

std::default_random_engine random_engine(std::chrono::system_clock::now().time_since_epoch().count());

struct point {
    double x, y;
};

struct circle : point {
    double radius;
};


std::istream& operator>>(std::istream &is, point &p) {
    return is >> p.x >> p.y;
}

std::istream& operator>>(std::istream &is, circle &c) {
    return is >> c.x >> c.y >> c.radius;
}


int main() {
    size_t npoints;
    circle c1, c2;
    std::cin >> npoints;
    std::cin >> c1 >> c2;

    std::uniform_int_distribution<> rndtype(1, 2);
    std::uniform_real_distribution<> rndc1r(0.0, c1.radius);
    std::uniform_real_distribution<> rndc2r(0.0, c2.radius);
    std::uniform_real_distribution<> rndangle(-M_PI, +M_PI);

    std::cout << npoints << '\n';
    while (npoints--) {
        bool type = rndtype(random_engine) == 1;
        double angle = rndangle(random_engine);
        point center = type ? c1 : c2;
        double dist = type ? rndc1r(random_engine) : rndc2r(random_engine);
        std::cout
            << type << ' '
            << center.x + dist * std::cos(angle) << ' '
            << center.y + dist * std::sin(angle) << ' '
            << '\n';
    }

    return 0;
}

