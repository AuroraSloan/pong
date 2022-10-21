#pragma once
#define SDL_MAIN_HANDLED
#include <vector>
#include "SDL.h"

struct Vector2 { float x; float y; };
struct Ball { Vector2 pos; Vector2 vel; };
struct Paddle { Vector2 pos; int dir; };

class Pong
{
	SDL_Window*		_window;
	SDL_Renderer*	_renderer;
	bool			_isRunning;
	Uint32			_ticks;
	Paddle			_LPaddle;
	Paddle			_RPaddle;
	std::vector<Ball> _balls;

	static const int _thickness = 15;
	static const int _paddleH = 100;

public:
	Pong();
	~Pong();

	bool	InitializeGame();
	void	GameLoop();
	void	StopGame();

private:

	void	ProcessInput();
	void	UpdateGame();
	void	GenerateOutput();

	void	_updatePaddlePosition(Paddle& paddle, float deltaTime);
	float	_padBallDiff(const Paddle& paddle, Vector2& ballPos);

};
