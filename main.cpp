#include "Pong.h"

int main() {

	Pong Game;
	if (Game.InitializeGame()) {
		Game.GameLoop();
	}
	Game.StopGame();
	return 0;
}