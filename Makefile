build:
	clang++ main.cpp Vector2Operators.cpp logger.cpp -I/opt/homebrew/Cellar/raylib/5.5/include -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib -o MyGame --std=c++17 && ./MyGame
