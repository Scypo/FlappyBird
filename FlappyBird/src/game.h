#pragma once
#include<ScypLib/ScypLib.h>
#include<array>
#include<vector>
#include<random>
#include<memory>
#include"Actors.h"
#include"NeuralNet.h"

class FBGame
{
public:
	FBGame();
	void Run();
protected:
	void LoadAssets();
	void HandleCollisions();
	void Restart();
	void MovePipe(Pipe& pipe, float x);
	void HandlePipes();
	void Draw();
	void Update(float dt);
protected:
	virtual void Init() = 0;
	virtual void ProcessInput() = 0;
	virtual bool ShouldRestart() = 0;
	virtual void ProcessDeath() = 0;
	virtual void SaveData() = 0;
protected:
	std::unique_ptr<sl::Window> wnd = nullptr;
	std::unique_ptr<sl::EventDispatcher> ed = nullptr;
	std::unique_ptr<sl::Graphics> gfx = nullptr;
	sl::Keyboard kbd;
	sl::FrameTimer ft;

	std::vector<FlappyBird> birds;
	std::array<Pipe, 3> pipes;
	std::mt19937 rng;
	std::uniform_real_distribution<float> pipeStartDist;

	sl::Texture* background = nullptr;
	sl::Texture* base = nullptr;
	std::array<sl::Texture*, 10> numbers = { nullptr };

	bool autoStart = false;
	int currentScore = 0;
	int maxScore = 0;
};

class PlayableGame : public FBGame
{
private:
	void ProcessInput() override;
	void Init() override;
	bool ShouldRestart() override;
	void ProcessDeath() override {};
	void SaveData() override {};
};

class TrainingGame : public FBGame
{
private:
	void ProcessInput() override;
	void Init() override;
	bool ShouldRestart() override;
	void ProcessDeath() override;
	void SaveData() override;
	std::vector<float> GetInput(const FlappyBird& bird, const Pipe& pipe) const;
private:
	const size_t birdCount = 100;
	const size_t inputs = 3;
	size_t generation = 0;
	size_t eliteBird = 0;
	std::vector<NeuralNet> nets;

	std::vector<int> scores;
};