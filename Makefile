build:
	clang++ main.cpp Vector2Operators.cpp logger.cpp $(pkg-config --libs --cflags raylib) -o MyGame --std=c++17
