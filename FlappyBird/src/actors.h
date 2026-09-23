#pragma once
#include<ScypLib/Vec2.h>
#include<ScypLib/Graphics.h>
struct FlappyBird
{
	const static inline sl::Vec2f size = { 34.0f, 24.0f };
	static inline sl::Texture* textures[3] = { nullptr };
	sl::Vec2f pos = { 57.0f, 244.0f };
	float velocity = 0.0f;
	bool dead = false;
	bool input = true;
	int frame = 0;
	float timeLeft = 0.25f;
	void UpdatePos(float dt)
	{
		if (input)
		{
			velocity = -250.0f; //flap
		}
		if (!dead)
		{
			velocity += 900.0f * dt;//add gravity
			velocity = std::clamp(velocity, -350.0f, 600.0f);
			pos.y += velocity * dt;
			timeLeft -= dt;
			if (timeLeft < 0)
			{
				frame++;
				frame = frame % 3;
				timeLeft = 0.25f;
			}
		}
		else 
		{
			pos.x -= 100.0f * dt;//speed of bird
		}
		input = false;
	}
	void Draw(sl::Graphics& gfx)
	{
		if(pos.x > -size.x)
		{
			float angle = 30.0f - 60 * (velocity + 350.0f) / (600.0f + 350.0f);
			gfx.DrawTexture(pos, FlappyBird::size, textures[frame], nullptr, false, false, -angle);
		}
	}
};

struct Pipe
{
	static inline float width = 52.0f;
	static inline float gap = 100.0f;
	static inline sl::Texture* texture = nullptr;
	float x = -1.0f;
	float gapY = 0.0f;
	void UpdatePos(float dt)
	{
		x -= 100.0f * dt;//speed of bird
	}
	void Draw(sl::Graphics& gfx)
	{
		gfx.DrawTexture({ x, gapY - 320.0f }, { width, 320.0f }, texture, nullptr, false, true);
		gfx.DrawTexture({ x, gapY + gap }, { width, 320.0f }, texture, nullptr, false, false);
	}
};