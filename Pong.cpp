#include "Pong.h"

Pong::Pong() : _window(nullptr), _renderer(nullptr), _isRunning(true), _ticks(0), _LPaddle(), _RPaddle(), _balls() {}
Pong::~Pong() {}

bool	Pong::InitializeGame() {
	// initialize SDL
	int	sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return (false);
	}

	// Create window
	_window = SDL_CreateWindow("Jade's Pong Game", 100, 100, 1024, 768, 0);
	if (!_window) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return (false);
	}

	// Create renderer
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!_renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return (false);
	}

	// Initialize game objs
	_LPaddle = { {5.0f, 768.0f / 2.0f}, 0 };
	_RPaddle = { {1024 - 5.0f, 768.0f / 2.0f}, 0 };
	_balls.push_back({ {1024.0f / 2.0f, 768.0f / 2.0f}, {-200.0f, 235.0f} });
	//_balls.push_back({ {1024.0f / 2.0f, 768.0f / 2.0f}, {200.0f, 235.0f} });

	return (true);
}

void	Pong::GameLoop() {

	while (_isRunning) {
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void	Pong::ProcessInput() {
	SDL_Event event;

	// check events and change game status if X button is clicked
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			_isRunning = false;
			break;
		}
	}

	// get state of keyboard change game status if esc is pressed
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		_isRunning = false;
	}

	// change dir of left paddle based on key input
	_LPaddle.dir = 0;
	if (state[SDL_SCANCODE_W]) {
		_LPaddle.dir -= 1;
	}
	if (state[SDL_SCANCODE_S]) {
		_LPaddle.dir += 1;
	}

	// change dir of right paddle based on key input
	_RPaddle.dir = 0;
	if (state[SDL_SCANCODE_I]) {
		_RPaddle.dir -= 1;
	}
	if (state[SDL_SCANCODE_K]) {
		_RPaddle.dir += 1;
	}
}

void	Pong::_updatePaddlePosition(Paddle& paddle, float deltaTime) {
	// Move paddle pos if direction was changed
	if (paddle.dir != 0) {

		paddle.pos.y += paddle.dir * 300.0f * deltaTime;

		if (paddle.pos.y < (_thickness + _paddleH / 2.0f)) {
			paddle.pos.y = _thickness + _paddleH / 2.0f;
		}
		else if (paddle.pos.y > (768.0f - _paddleH / 2.0f - _thickness)) {
			paddle.pos.y = 768.0f - _paddleH / 2.0f - _thickness;
		}
	}
}

float	Pong::_padBallDiff(const Paddle& paddle, Vector2& ballPos) {

	// decide relative position of ball and paddle
	float diff = paddle.pos.y - ballPos.y;

	return ((diff > 0.0f) ? diff : -diff);
}

void	Pong::UpdateGame() {
	float	diff;

	// wait until 16ms has passed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), _ticks + 16)) { ; }

	// decide deslta time in seconds
	float deltaTime = (SDL_GetTicks() - _ticks) / 1000.0f;

	// revert delta time in case too high (debugging)
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}

	// get ticks (ms since SDL_INIT())
	_ticks = SDL_GetTicks();

	// update paddle pos based on dir
	_updatePaddlePosition(_LPaddle, deltaTime);
	_updatePaddlePosition(_RPaddle, deltaTime);
	
	for (std::vector<Ball>::iterator ballIt = _balls.begin(); ballIt != _balls.end(); ++ballIt) {
		// update ball pos based on vel
		ballIt->pos.x += ballIt->vel.x * deltaTime;
		ballIt->pos.y += ballIt->vel.y * deltaTime;

		// bounce if ball hits left paddle
		diff = _padBallDiff(_LPaddle, ballIt->pos);
		if (diff <= (_paddleH / 2.0f)
			&& ballIt->pos.x <= static_cast<float>(_thickness)
			&& ballIt->vel.x < 0.0f)
		{
			ballIt->vel.x *= -1.0f;
		}

		// bounce if ball hits right paddle
		diff = _padBallDiff(_RPaddle, ballIt->pos);
		if (diff <= (_paddleH / 2.0f)
			&& ballIt->pos.x >= 1024.0f - static_cast<float>(_thickness)
			&& ballIt->vel.x > 0.0f)
		{
			ballIt->vel.x *= -1.0f;
		}

		// Check if ball left the screen
		if (ballIt->pos.x <= 0.0f || ballIt->pos.x >= 1024.0f) {
			_isRunning = false;
		}

		// bounce if ball hits top wall
		if (ballIt->pos.y <= _thickness &&  ballIt->vel.y < 0.0f) {
			ballIt->vel.y *= -1.0f;
		}
		// if ball hits bottom wall
		if (ballIt->pos.y >= (768.0f - _thickness) &&  ballIt->vel.y > 0.0f) {
			ballIt->vel.y *= -1.0f;
		}
	}
}

void	Pong::GenerateOutput() {

	// render background
	SDL_SetRenderDrawColor(_renderer, 0, 0, 255, 255);
	SDL_RenderClear(_renderer);

	// set color for game objs
	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);

	// Draw top wall
	SDL_Rect wall{ 0, 0, 1024, _thickness };
	SDL_RenderFillRect(_renderer, &wall);

	// Draw bottom wall
	wall.y = 768 - _thickness;
	SDL_RenderFillRect(_renderer, &wall);

	// Draw ball
	for (std::vector<Ball>::iterator ballIt = _balls.begin(); ballIt != _balls.end(); ++ballIt) {
		SDL_Rect ball{
			static_cast<int>(ballIt->pos.x) - _thickness / 2,
			static_cast<int>(ballIt->pos.y) - _thickness / 2,
			_thickness,
			_thickness
		};
		SDL_RenderFillRect(_renderer, &ball);
	}
	
	// Draw left paddle
	SDL_Rect leftPaddle{
		static_cast<int>(_LPaddle.pos.x) - _thickness / 2,
		static_cast<int>(_LPaddle.pos.y) - _paddleH / 2,
		_thickness,
		_paddleH
	};
	SDL_RenderFillRect(_renderer, &leftPaddle);

	// Draw right paddle
	SDL_Rect rightPaddle{
		static_cast<int>(_RPaddle.pos.x) - _thickness / 2,
		static_cast<int>(_RPaddle.pos.y) - _paddleH / 2,
		_thickness,
		_paddleH
	};
	SDL_RenderFillRect(_renderer, &rightPaddle);

	// swap front and back buffer
	SDL_RenderPresent(_renderer);
}

void	Pong::StopGame() {
	SDL_DestroyWindow(_window);
	SDL_Quit();
	SDL_DestroyRenderer(_renderer);
}