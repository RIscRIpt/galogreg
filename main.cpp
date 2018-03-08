#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <cmath>
#include <random>
#include <chrono>

std::default_random_engine random_engine(std::chrono::system_clock::now().time_since_epoch().count());

struct line;
struct cline;
struct point;
struct cpoint;

struct fitness_history_entry {
    double min, avg, max;
};

struct line {
    double A, B, C;
    line() = default;
    line(line const&) = default;
    line(double A, double B, double C)
        : A(A), B(B), C(C)
    {}
};

struct cline : line {
    bool S;
    cline() = default;
    cline(cline const&) = default;
    cline& operator=(cline const&) = default;
    cline(double A, double B, double C, bool S)
        : line(A, B, C), S(S)
    {}
    double get_fitness(std::vector<cpoint> const &points) const;
    void mutate();
};

struct point {
    double x, y;
    double distance_to_line(line const &l) const;
};

struct cpoint : point {
    bool kind;

    bool is_on_valid_line_side(cline const &l) const;
};

double point::distance_to_line(line const &l) const {
    return std::abs(l.A * x + l.B * y + l.C) / std::sqrt(l.A * l.A + l.B * l.B);
}

bool cpoint::is_on_valid_line_side(cline const &l) const {
    double w = l.A * x + l.B * y + l.C;
    if (l.S) {
        return (w >= 0.0 && kind) || (w < 0.0 && !kind);
    } else {
        return (w >= 0.0 && !kind) || (w < 0.0 && kind);
    }
}

double cline::get_fitness(std::vector<cpoint> const &points) const {
    double errors = 0.0;
    for (auto const &p : points) {
        if (!p.is_on_valid_line_side(*this))
         errors += std::pow(p.distance_to_line(*this), 2.0);
    }
    double fitness =  1 / errors;
    if (fitness > 10e100)
        fitness = 10e100;
    return fitness;
}

void cline::mutate() {
    std::uniform_int_distribution<> random_mutation(0, 3);
    std::normal_distribution<> normal_distribution(0, 10);
    switch (random_mutation(random_engine)) {
        case 0:
            A += normal_distribution(random_engine);
            break;
        case 1:
            B += normal_distribution(random_engine);
            break;
        case 2:
            C += normal_distribution(random_engine);
            break;
        case 3:
            S = !S;
            break;
    }
}

bool in_optimum(std::vector<fitness_history_entry> const &fitness_history) {
    const long optimum_history_length = 20;
    double avg_mean = 0.0;
    for (
        auto he = fitness_history.rbegin();
        he != fitness_history.rend()
            && he - fitness_history.rbegin() < optimum_history_length;
        ++he)
    {
        avg_mean += he->max;
    }
    const long history_length = std::min(optimum_history_length, static_cast<long>(fitness_history.size()));
    avg_mean /= history_length;

    double sd = 0.0;
    for (
        auto he = fitness_history.rbegin();
        he != fitness_history.rend()
            && std::distance(he, fitness_history.rbegin()) < optimum_history_length;
        ++he)
    {
        sd += pow(he->max - avg_mean, 2.0);
    }
    sd = sqrt(sd / (history_length - 1));

    return history_length == optimum_history_length && sd < 0.5;
}

std::vector<cline> crossover_clines(cline &first_parent, cline &second_parent) {
    std::vector<cline> children;
    std::uniform_int_distribution<> random_coeff(0, 3);
    switch (random_coeff(random_engine)) {
        case 0:
        {
            double minA = std::min(first_parent.A, second_parent.A);
            double maxA = std::max(first_parent.A, second_parent.A);
            double dA = maxA - minA;
            children.emplace_back(minA - 0.5 * dA , first_parent  . B , first_parent  . C , first_parent.S  ) ;
            children.emplace_back(minA - 0.5 * dA , second_parent . B , second_parent . C , second_parent.S ) ;
            children.emplace_back(minA + 0.5 * dA , first_parent  . B , first_parent  . C , first_parent.S  ) ;
            children.emplace_back(minA + 0.5 * dA , second_parent . B , second_parent . C , second_parent.S ) ;
            children.emplace_back(maxA + 0.5 * dA , first_parent  . B , first_parent  . C , first_parent.S  ) ;
            children.emplace_back(maxA + 0.5 * dA , second_parent . B , second_parent . C , second_parent.S ) ;
            break;
        }
        case 1:
        {
            double minB = std::min(first_parent.B, second_parent.B);
            double maxB = std::max(first_parent.B, second_parent.B);
            double dB = maxB - minB;
            children.emplace_back(first_parent  . A , minB - 0.5 * dB , first_parent  . C , first_parent.S  ) ;
            children.emplace_back(second_parent . A , minB - 0.5 * dB , second_parent . C , second_parent.S ) ;
            children.emplace_back(first_parent  . A , minB + 0.5 * dB , first_parent  . C , first_parent.S  ) ;
            children.emplace_back(second_parent . A , minB + 0.5 * dB , second_parent . C , second_parent.S ) ;
            children.emplace_back(first_parent  . A , maxB + 0.5 * dB , first_parent  . C , first_parent.S  ) ;
            children.emplace_back(second_parent . A , maxB + 0.5 * dB , second_parent . C , second_parent.S ) ;
            break;
        }
        case 2:
        {
            double minC = std::min(first_parent.C, second_parent.C);
            double maxC = std::max(first_parent.C, second_parent.C);
            double dC = maxC - minC;
            children.emplace_back(first_parent  . A , first_parent  . B , minC - 0.5 * dC , first_parent.S  ) ;
            children.emplace_back(second_parent . A , second_parent . B , minC - 0.5 * dC , second_parent.S ) ;
            children.emplace_back(first_parent  . A , first_parent  . B , minC + 0.5 * dC , first_parent.S  ) ;
            children.emplace_back(second_parent . A , second_parent . B , minC + 0.5 * dC , second_parent.S ) ;
            children.emplace_back(first_parent  . A , first_parent  . B , maxC + 0.5 * dC , first_parent.S  ) ;
            children.emplace_back(second_parent . A , second_parent . B , maxC + 0.5 * dC , second_parent.S ) ;
        }
        case 3:
            children.emplace_back(first_parent  . A , first_parent  . B , first_parent  . C , !first_parent.S  ) ;
            children.emplace_back(second_parent . A , second_parent . B , second_parent . C , !second_parent.S ) ;
            break;
    }
    return children;
}

cline random_cline() {
    std::uniform_real_distribution<> random(-10, +10);
    return cline(
            random(random_engine),
            random(random_engine),
            random(random_engine),
            random(random_engine) >= 0
            );
}

int main() {
    size_t const population_size = 500;

    size_t npoints;
    std::cin >> npoints;
    std::cout << npoints << '\n';
    std::vector<cpoint> points(npoints);
    for (auto &p : points) {
        std::cin >> p.kind >> p.x >> p.y;
        std::cout << p.kind << ' ' << p.x << ' ' << p.y << '\n';
    }
    std::cout << std::endl;

    std::vector<fitness_history_entry> fitness_history;
    std::vector<std::pair<cline, double>> lines;
    std::vector<cline> best_lines;

    // Initial population
    while (lines.size() < population_size) {
        lines.emplace_back(std::make_pair(random_cline(), 0.0));
    }

    do {
        // Crossover
        std::uniform_int_distribution<> random_line(0, lines.size() - 1);
        while (lines.size() < population_size * 2) {
            auto first_parent_id = random_line(random_engine);
            auto second_parent_id = random_line(random_engine);
            auto children = crossover_clines(lines[first_parent_id].first, lines[second_parent_id].first);
            for (auto const &child : children) {
                lines.emplace_back(std::make_pair(child, 0.0));
            }
        }

        // Mutation
        std::uniform_real_distribution<> random_mutation(0.0, 1.0);
        for (auto &line : lines) {
            if (random_mutation(random_engine) < 0.01) {
                line.first.mutate();
            }
        }
        // Calculate fitness stats & best line
        cline best_line;
        fitness_history_entry history_entry;
        double fitness_sum = 0.0;
        history_entry.min = std::numeric_limits<double>::max();
        history_entry.max = -std::numeric_limits<double>::max();
        for (auto &line : lines) {
            auto fitness = line.first.get_fitness(points);
            history_entry.min = std::min(history_entry.min, fitness);
            history_entry.max = std::max(history_entry.max, fitness);
            if (fitness == history_entry.max) {
                best_line = line.first;
            }
            fitness_sum += fitness;
            line.second = fitness;
        }
        history_entry.avg = fitness_sum / lines.size();
        fitness_history.push_back(history_entry);
        best_lines.emplace_back(best_line);

        // Selection
        std::vector<std::pair<cline, double>> new_lines;
        std::uniform_real_distribution<> random_selection(0.0, fitness_sum);
        /* new_lines.emplace_back(std::make_pair(best_line, history_entry.max)); */
        while (new_lines.size() < population_size) {
            auto selection = random_selection(random_engine);
            size_t selected_id = 0;
            for (; selection > 0.0 && selected_id < lines.size(); selected_id++) {
                selection -= lines[selected_id].second;
            }
            if (selected_id >= lines.size())
                selected_id = lines.size() - 1;

            new_lines.emplace_back(lines[selected_id]);
        }

        lines.swap(new_lines);

        std::cout
            << fitness_history.back().avg << ' '
            << fitness_history.back().max << ' '
            << best_line.A << ' '
            << best_line.B << ' '
            << best_line.C << std::endl;
    } while (true || !in_optimum(fitness_history));

    for (auto const &line : best_lines) {
        std::cout
            << line.get_fitness(points) << '\t'
            << line.A << ' '
            << line.B << ' '
            << line.C << '\t'
            << line.S << '\n';
    }

    return 0;
}

