all: find_line gen_points plot

find_line: main.cpp
	$(CXX) $^ -o $@.elf

gen_points: gen_points.cpp
	$(CXX) $^ -o $@.elf

plot: plot.cpp
	$(CXX) $^ -lGL -lGLU -lglut -o $@.elf

