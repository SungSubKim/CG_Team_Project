#ifndef __PARTICLE_H__
#define __PARTICLE_H__
#pragma once

#include "cgmath.h"
#include "cgut.h"

inline float random_range( float min, float max ){ return mix( min, max, rand()/float(RAND_MAX) ); }

struct particle_t
{
	static constexpr int MAX_PARTICLES = 5;

	vec2 pos;
	vec4 color;
	float scale;
	float life;
	float tmp_time = 0.0f;

	//optional
	float elapsed_time;
	float time_interval;

	void reset();
	void update(float x, float y, bool space, int direction);
};

inline void particle_t::reset()
{
	pos = vec2(random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f));
	color = vec4(1.0f, 1.0f, 1.0f, 1);
	scale = random_range(4.5f, 5.8f);
	life = random_range(0.01f, 0.02f);
	elapsed_time = 0.0f;
	time_interval = random_range(200.0f, 600.0f);
	tmp_time = float(glfwGetTime());
}

inline void particle_t::update(float x, float y, bool space, int direction)
{
	float times = float(glfwGetTime()) - tmp_time;
	vec2 plus = vec2(random_range(0.0f, 10.5f), random_range(0, 10.5f));

	/*switch (direction) {
		case 1:
			pos.x = x - plus.x;
			pos.y = y + plus.y;
			break;
		case 2:
			pos.x = x + plus.x;
			pos.y = y + plus.y;
			break;
		case 3:
			pos.x = x + plus.y;
			pos.y = y - plus.x;
			break;
		case 4:
			pos.x = x + plus.y;
			pos.y = y + plus.x;
			break;
		default:
			pos.x = x;
			pos.y = y;
			break;
	}*/
	pos.x = x+plus.x * cos(getModel("Character").theta);
	pos.y = y-plus.y * sin(getModel("Character").theta);
	elapsed_time += times;

	if (elapsed_time > time_interval){
		elapsed_time = 0.0f;
	}

	constexpr float life_factor = 0.05f;
	life -= life_factor * times;

	// disappear
	if (life < 0.0f)
	{
		constexpr float alpha_factor = 0.05f;
		color.a -= alpha_factor * times;
	}

	// dead
	if (space) {
		reset();
	}
}

#endif