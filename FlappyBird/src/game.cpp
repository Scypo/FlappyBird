#include "Game.h"
#include<fstream>
#include<iostream>

FBGame::FBGame()
	:
	pipeStartDist(80.0f, 512.0f - 80.0f - 116.0f - Pipe::gap)
{
	wnd = std::make_unique<sl::Window>("Flappy Bird", 288, 512, 0);
	gfx = std::make_unique<sl::Graphics>(wnd.get());
	ed = std::make_unique<sl::EventDispatcher>(&kbd, nullptr, wnd.get());
}

void FBGame::LoadAssets()
{
	// load flappy
	FlappyBird::textures[0] = gfx->LoadTexture("assets/sprites/yellowbird-downflap.png");
	FlappyBird::textures[1] = gfx->LoadTexture("assets/sprites/yellowbird-midflap.png");
	FlappyBird::textures[2] = gfx->LoadTexture("assets/sprites/yellowbird-upflap.png");
	//load pipes
	Pipe::texture = gfx->LoadTexture("assets/sprites/pipe-green.png");
	//load background
	background = gfx->LoadTexture("assets/sprites/background-day.png");
	base = gfx->LoadTexture("assets/sprites/base.png");
	//load numbers
	for (int i = 0; i < 10; i++)
	{
		numbers[i] = gfx->LoadTexture("assets/sprites/" + std::to_string(i) + ".png");
	}
}

void FBGame::Run()
{
	LoadAssets();
	Init();
	
	while (wnd->IsRunning())
	{
		while (wnd->IsRunning() && kbd.IsEmpty() && !autoStart)
		{
			ed->PollEvents();
			//wait for input
			Draw();
		}
		Restart();
		ft.Mark();
		
		while (wnd->IsRunning() && !ShouldRestart())
		{
			ed->PollEvents();
			float dt = ft.Mark();
			HandleCollisions();
			Update(dt);
			Draw();
		}
		maxScore = std::max(maxScore, currentScore);
		ProcessDeath();
		while (wnd->IsRunning() && kbd.IsEmpty() && !autoStart)
		{
			ed->PollEvents();
			//wait for input
			Draw();
		}
	}
}

void FBGame::HandleCollisions()
{
	for (FlappyBird& fb : birds)
	{
		if (fb.dead)continue;
		if (fb.pos.y + fb.size.y > 512.0f - 112.0f) //check if hit the ground
		{
			fb.dead = true;
			continue;
		}
		for (Pipe& pipe : pipes) //check if hit a pipe
		{
			if (fb.pos.x + FlappyBird::size.x > pipe.x && fb.pos.x < pipe.x + Pipe::width && 
				(fb.pos.y < pipe.gapY || fb.pos.y + FlappyBird::size.y > pipe.gapY + Pipe::gap))
			{
				fb.dead = true;
			}
		}
	}
}

void FBGame::Restart()
{
	for (FlappyBird& fb : birds) //restart birds pos
	{
		fb = FlappyBird();	
	}
	for (Pipe& pipe : pipes) //move pipes behind so they can be regenerated
	{
		pipe.x = -100;
	}
	HandlePipes();
	currentScore = 0;
}

void FBGame::MovePipe(Pipe& pipe, float x)
{
	pipe.x = x;
	pipe.gapY = pipeStartDist(rng);
	currentScore++;
}

void FBGame::HandlePipes()
{
	float furthestPipe = 0.0f;
	for (Pipe& pipe : pipes)
	{
		furthestPipe = std::max(furthestPipe, pipe.x);
	}
	for (Pipe& pipe : pipes)
	{
		if (pipe.x < -pipe.width) //move pipe outside of screen
		{
			MovePipe(pipe, furthestPipe + Pipe::width + 200.0f);
			furthestPipe = pipe.x;
		}
	}
}

void FBGame::Draw()
{
	gfx->BeginFrame();
	gfx->BeginView();
	//find start position of score sprites
	int digits = 0;
	int dummy = currentScore;
	while (dummy)
	{
		dummy /= 10;
		digits++;
	}
	if (currentScore == 0)
	{
		digits = 1;
	}
	float xEnd = 288.0f * 0.5f + digits * 24.0f * 0.5f - 24.0f;
	dummy = currentScore;
	//draw score
	gfx->SetDrawDepth(4.0f);
	for (int i = 0; i < digits; i++)
	{
		gfx->DrawTexture(xEnd - i * 24.0f, 54.0f, numbers[dummy % 10]);
		dummy /= 10;
	}
	//draw birds
	gfx->SetDrawDepth(3.0f);
	for (FlappyBird& fb : birds)
	{
		fb.Draw(*gfx);
	}
	//draw pipes
	gfx->SetDrawDepth(1.0f);
	for (Pipe& pipe : pipes)
	{
		pipe.Draw(*gfx);
	}
	//draw background
	gfx->SetDrawDepth(0.0f);
	gfx->DrawTexture(0, 0, background);
	gfx->SetDrawDepth(2.0f);
	gfx->DrawTexture(baseX, 512.0f - 112.0f, base);
	gfx->DrawTexture(baseX + 336.0f, 512.0f - 112.0f, base);
	gfx->EndView();
	gfx->EndFrame();
}

void FBGame::Update(float dt)
{
	ProcessInput();
	for (FlappyBird& fb : birds)
	{
		fb.UpdatePos(dt);
	}
	for (Pipe& pipe : pipes)
	{
		pipe.UpdatePos(dt);
	}
	baseX -= 100.0f * dt;
	baseX = std::fmod(baseX, 336.0f);
	HandlePipes();
}

void PlayableGame::ProcessInput()
{
	if (!kbd.IsEmpty())
	{
		assert(!birds.empty());
		birds[0].input = true;
		kbd.Flush();
	}
}

void PlayableGame::Init()
{
	autoStart = false;
	birds.push_back(FlappyBird());
	MovePipe(pipes[0], 288.0f);
	MovePipe(pipes[1], pipes[0].x + Pipe::width + 200.0f);
	MovePipe(pipes[2], pipes[1].x + Pipe::width + 200.0f);
	currentScore = 0;
}

bool PlayableGame::ShouldRestart()
{
	assert(!birds.empty());
	return birds[0].dead;
}

void TrainingGame::ProcessInput()
{
	//find next pipe
	size_t pipe = 0;
	for (size_t i = 0; i < pipes.size(); i++)
	{
		if (pipes[i].x + Pipe::gap > 57.0f && pipes[i].x < pipes[pipe].x)
		{
			pipe = i;
		}
	}
	//pass data to neural nets and decide if should jump
	for (size_t i = 0; i < birdCount; i++)
	{
		float output = nets[i].Evaluate(GetInput(birds[i], pipes[pipe]))[0];
		output = (output + 1.0f) * 0.5f;
		if (output > 0.5f)
		{
			birds[i].input = true;
		}
	}
}

void TrainingGame::Init()
{
	autoStart = true;
	birds.resize(birdCount);
	nets.reserve(birdCount);

	std::vector<size_t> layerSize = { 4, 1 };//TODO load from file
	for (size_t i = 0; i < birdCount; i++)
	{
		nets.emplace_back(NeuralNet(inputs, layerSize));
	}

	MovePipe(pipes[0], 288.0f);
	MovePipe(pipes[1], pipes[0].x + Pipe::width + 200.0f);
	MovePipe(pipes[2], pipes[1].x + Pipe::width + 200.0f);
	currentScore = 0;

	std::cout << "Enter number of generations:" << std::endl;
	std::cin >> trainingGenCount;
	trainingGenCount--;
	system("cls");
}

bool TrainingGame::ShouldRestart()
{
	size_t sum = 0;
	for (size_t i = 0; i < birdCount; i++)
	{
		if (birds[i].dead)
		{
			sum++;
		}
	}
	return sum * 10 > birdCount * 9; //should restart if 90% of population is dead
}

void TrainingGame::ProcessDeath()
{
	system("cls");
	std::cout << "Generation Number: " << generation << std::endl;
	std::cout << "High Score: " << maxScore << std::endl;
	std::cout << "Current Score: " << currentScore << std::endl;

	if (generation <= trainingGenCount)
	{
		std::vector<size_t> alive;
		for (size_t i = 0; i < birdCount; i++)
		{
			if (!birds[i].dead)
			{
				alive.push_back(i);
			}
		}
		std::uniform_int_distribution<size_t> dist(0, alive.size() - 1);

		eliteBird = alive[dist(rng)];
		if (alive.size() >= 2 && generation != trainingGenCount)
		{
			for (size_t i = 0; i < birdCount; i++)
			{
				if (!birds[i].dead) continue;

				size_t p1 = dist(rng);
				size_t p2 = dist(rng);

				while (p1 == p2)
				{
					p2 = dist(rng);
				}
				nets[i].Evolve(nets[alive[p1]], nets[alive[p2]]);
			}
		}
	}
	if (generation == trainingGenCount)
	{
		std::swap(nets[eliteBird], nets[0]);
		generation++;
		birdCount = 1;
		birds.resize(birdCount);
		nets.resize(birdCount);
	}
	generation++;
}


std::vector<float> TrainingGame::GetInput(const FlappyBird& bird, const Pipe& pipe) const
{
	//distance from pipe, distance from top of the gap, distance from bottom of the gap
	std::vector<float> v = { pipe.x - bird.pos.x, bird.pos.y - pipe.gapY, pipe.gapY + pipe.gap - bird.pos.y - bird.size.y };
	assert(v.size() == inputs);
	return v;
}