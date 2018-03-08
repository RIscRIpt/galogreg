all: gen_points.elf find_line.elf plot.elf plot_stats.elf

find_line.elf: main.cpp
	$(CXX) $^ -o $@

gen_points.elf: gen_points.cpp
	$(CXX) $^ -o $@

plot.elf: plot.cpp
	$(CXX) $^ -lGL -lGLU -lglut -o $@

plot_stats.elf: plot_stats.cpp
	$(CXX) $^ -lGL -lGLU -lglut -o $@

