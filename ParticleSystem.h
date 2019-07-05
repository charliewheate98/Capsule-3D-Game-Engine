#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <ctime>
#include <random>
#include "Instance.h"

// Global formulas
inline float randF(float min, float max) { return ((max - min)*((float)rand() / RAND_MAX)) + min; }
inline float avrgf(float min, float max) { return (min + max) / 2; }
inline float avrgv(glm::vec2 range) { return (range.x + range.y) / 2; }


// Abstract particle struct
struct Particle
{
	float			_alpha;
	float			_alpha_lerp;
	float			_life;
	float			_life_count;
	float			_velocity;
	float			_speed;
	float			_wave;
	float			_wave_count;

	glm::vec2		_pos;

	unsigned int	_loc_alpha;
	unsigned int	_loc_pos;

	inline Particle() :
		_alpha(0.0f),
		_life(0.0f),
		_life_count(0.0f),
		_wave(0.0f),
		_wave_count(0.0f),
		_alpha_lerp(0.0f),
		_velocity(0.0f),
		_speed(0.0f),
		_pos(glm::vec2(0.0f)) {}

	inline virtual void update(double delta) = 0;
};

// Directional particle struct
struct DirectionalParticle : public Particle
{
	glm::vec2	_dir;

	inline DirectionalParticle(glm::vec2 pos, glm::vec2 dir, float life, float speed, float wave, float alpha_lerp)
	{
		_life = life;
		_wave = wave;
		_alpha_lerp = alpha_lerp;
		_speed = speed;
		_pos = pos;
		_dir = dir;
	}

	inline virtual void update(double delta)
	{
		// Update position
		_velocity = _speed * (float)delta;
		_pos += _velocity * _dir;
		_pos.x += sin(_wave_count) * _wave;

		// Update alpha lerp
		_alpha += (sin((glm::distance(_life_count, _life))) * _alpha_lerp) * (float)delta;

		// Update life span
		_life_count += _velocity;

		// Update wave count
		_wave_count += 0.01f * (_velocity * 1000.0f);
	}
};

// Point particle struct
struct PointParticle : public Particle
{
	float	_radius;

	inline PointParticle(glm::vec2 pos, float radius, float life, float speed, float wave, float alpha_lerp)
	{
		_life = life;
		_wave = wave;
		_alpha_lerp = alpha_lerp;
		_speed = speed;
		_pos = pos;
		_radius = radius;
	}

	inline virtual void update(double delta)
	{
		// Update position
		_velocity = _speed * (float)delta;
		_pos += (glm::vec2(sin(_velocity * 1000.0f), cos(_velocity * 1000.0f)) *
			glm::vec2(sin(_wave_count), cos(_wave_count))) * _radius;

		// Update alpha lerp
		_alpha += (sin((glm::distance(_life_count, _life))) * _alpha_lerp) * (float)delta;

		// Update life span
		_life_count += _velocity;

		// Update wave count
		_wave != 0.0f ? _wave_count += 0.01f * (_velocity * 1000.0f) : _wave_count = 1.0f;
	}
};


// Struct redefinitions
typedef	Particle				_pt_x;
typedef DirectionalParticle		_pt_dir;
typedef PointParticle			_pt_point;


// Abstract particle system class
class ParticleSystem
{
protected:
	bool					_particles_divised;
	float					_particle_indices;
	float					_particle_divisor;
	std::vector<_pt_x*>		_particles;
	t_instance*				_instance;

public:
	unsigned int			_num_particles;

	glm::vec3				_position;
	glm::vec2				_spread;
	glm::vec2				_spawn_delay;
	glm::vec2				_speed;
	glm::vec2				_life_span;
	glm::vec2				_alpha_lerp;
	glm::vec2				_wave;

	inline ParticleSystem() :
		_particles_divised(false),
		_particle_indices(0),
		_particle_divisor(0.0f),
		_position(glm::vec3(0.0f)),
		_spread(glm::vec2(0.0f)),
		_spawn_delay(glm::vec2(0.0f)),
		_speed(glm::vec2(0.0f)),
		_life_span(glm::vec2(0.0f)),
		_alpha_lerp(glm::vec2(0.0f)),
		_wave(glm::vec2(0.0f)) {}

	inline virtual void update(double delta) = 0;
	inline virtual void render() = 0;

protected:
	inline virtual void respawn(_pt_x *p) = 0;

	inline void restoreDefaults(_pt_x *p)
	{
		p->_life_count = 0.0f;
		p->_velocity = 0.0f;
		p->_alpha = 0.0f;
		p->_life = randF(_life_span.x, _life_span.y);
		p->_speed = randF(_speed.x, _speed.y);
		p->_wave = randF(_wave.x, _wave.y);
		p->_alpha_lerp = randF(_alpha_lerp.x, _alpha_lerp.y);
	}

	inline void updateParticles(double delta)
	{
		// Divise particles
		if (!_particles_divised)
		{
			_particle_indices += _particle_divisor * (float)delta;
			if (_particle_indices >= _particles.size())
			{
				_particle_indices = (float)_particles.size();
				_particles_divised = true;
			}
		}

		for (unsigned int i = 0; i < _particle_indices; i++)
		{
			// Update particles
			_particles[i]->update(delta);

			// Check for particle life span
			if (_particles[i]->_life_count >= _particles[i]->_life)
				respawn(_particles[i]);
		}
	}
};

// Directional particle system
class DirectionalParticleSystem : public ParticleSystem
{
private:
	GLuint view;
	GLuint proj;

	GLuint _sampler;
	GLuint _samplerID;

	GLuint cam_u;
	GLuint cam_r;
public:
	inline DirectionalParticleSystem(GLuint shader_program,
		glm::vec2	spread,
		glm::vec3	position,
		glm::vec2	spawn_delay,
		glm::vec2	life_span,
		glm::vec2	speed,
		glm::vec2	wave,
		glm::vec2	alpha_lerp)
	{
		// Assign temp data
		_position = position;
		_spawn_delay = spawn_delay;
		_spread = spread;
		_life_span = life_span;
		_speed = speed;
		_wave = wave;
		_alpha_lerp = alpha_lerp;

		// Calculate particle count;
		_particle_divisor = (avrgv(life_span) / avrgv(spawn_delay)) * avrgv(speed);
		_num_particles = static_cast<unsigned int>(avrgv(life_span) / avrgv(spawn_delay));

		// Seed before randomisation
		srand(static_cast<unsigned int>(time(NULL)));

		// Create particles
		for (unsigned int i = 0; i < _num_particles; i++)
		{
			std::string	index = std::to_string(i);
			_particles.push_back(new _pt_dir(
				glm::vec2(randF(position.x - _spread.x, position.x + _spread.x), position.y),
				glm::vec2(randF(-_spread.y, _spread.y), 1.0f),
				randF(life_span.x, life_span.y),
				randF(speed.x, speed.y),
				randF(wave.x, wave.y),
				randF(alpha_lerp.x, alpha_lerp.y)));

			_particles[i]->_loc_alpha = glGetUniformLocation(shader_program, ("particles[" + index + "]._alpha").c_str());
			_particles[i]->_loc_pos = glGetUniformLocation(shader_program, ("particles[" + index + "]._pos").c_str());
		}

		view = glGetUniformLocation(shader_program, "view");
		proj = glGetUniformLocation(shader_program, "proj");

		cam_u = glGetUniformLocation(shader_program, "cam_up");
		cam_r = glGetUniformLocation(shader_program, "cam_right");

		_sampler = glGetUniformLocation(shader_program, "sprite");

		int width;
		int height;
		int nr_components;

		unsigned char* data = stbi_load("test.png", &width, &height, &nr_components, 0);

		glGenTextures(1, &_samplerID);
		glBindTexture(GL_TEXTURE_2D, _samplerID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(data);


		// Create instance
		_instance = new t_instance(_num_particles);
	}

	inline ~DirectionalParticleSystem()
	{
		delete _instance;
		_particles.clear();
	}

	inline virtual void update(double delta)
	{
		updateParticles(delta);
	}

	inline virtual void render()
	{
		for (unsigned int i = 0; i < _num_particles; i++)
		{
			glUniform1f(_particles[i]->_loc_alpha, _particles[i]->_alpha);
			glUniform2fv(_particles[i]->_loc_pos, 1, glm::value_ptr(_particles[i]->_pos));
		}

		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(Content::_map->GetCamera()->GetViewMatrix()));
		glUniformMatrix4fv(proj, 1, GL_FALSE, glm::value_ptr(Content::_map->GetCamera()->GetProjectionMatrix()));

		glUniform3f(cam_u, Content::_map->GetCamera()->GetUp().x, Content::_map->GetCamera()->GetUp().y, Content::_map->GetCamera()->GetUp().z);
		glUniform3f(cam_r, Content::_map->GetCamera()->GetRight().x, Content::_map->GetCamera()->GetRight().y, Content::_map->GetCamera()->GetRight().z);

		glUniform1i(_sampler, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _samplerID);

		_instance->render();
	}

	inline virtual void respawn(_pt_x* p)
	{
		restoreDefaults(p);
		p->_pos = glm::vec2(randF(_position.x - _spread.x, _position.x + _spread.x), _position.y);
		((_pt_dir*)p)->_dir = glm::vec2(randF(-_spread.y, _spread.y), 1.0f);
	}
};

// Point particle system
class PointParticleSystem : public ParticleSystem
{
private:
	GLuint view;
	GLuint proj;

	GLuint _sampler;
	GLuint _samplerID;

	GLuint cam_u;
	GLuint cam_r;
public:
	inline PointParticleSystem(GLuint shader_program,
		glm::vec2	spread,
		glm::vec3	position,
		glm::vec2	spawn_delay,
		glm::vec2	life_span,
		glm::vec2	speed,
		glm::vec2	wave,
		glm::vec2	alpha_lerp)
	{
		// Assign temp data
		_position = position;
		_spawn_delay = spawn_delay;
		_spread = spread;
		_life_span = life_span;
		_speed = speed;
		_wave = wave;
		_alpha_lerp = alpha_lerp;

		// Calculate particle count;
		_particle_divisor = (avrgv(life_span) / avrgv(spawn_delay)) * avrgv(speed);
		_num_particles = static_cast<unsigned int>(avrgv(life_span) / avrgv(spawn_delay));

		// Seed before randomisation
		srand(static_cast<unsigned int>(time(NULL)));

		// Create particles
		for (unsigned int i = 0; i < _num_particles; i++)
		{
			std::string	index = std::to_string(i);
			_particles.push_back(new _pt_point(
				glm::vec2(randF(position.x - _spread.x, position.x + _spread.x),
					randF(position.y - _spread.y, _spread.y)),
				randF(spread.x, spread.y),
				randF(life_span.x, life_span.y),
				randF(speed.x, speed.y),
				randF(wave.x, wave.y),
				randF(alpha_lerp.x, alpha_lerp.y)));

			_particles[i]->_loc_alpha = glGetUniformLocation(shader_program, ("particles[" + index + "]._alpha").c_str());
			_particles[i]->_loc_pos = glGetUniformLocation(shader_program, ("particles[" + index + "]._pos").c_str());
		}

		view     = glGetUniformLocation(shader_program, "view");
		proj     = glGetUniformLocation(shader_program, "proj");
		cam_u    = glGetUniformLocation(shader_program, "cam_up");
		cam_r    = glGetUniformLocation(shader_program, "cam_right");
		_sampler = glGetUniformLocation(shader_program, "sprite");
	
		int width;
		int height;
		int nr_components;

		unsigned char* data = stbi_load("thruster.png", &width, &height, &nr_components, 0);

		glGenTextures(1, &_samplerID);
		glBindTexture(GL_TEXTURE_2D, _samplerID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(data);

		// Create instance
		_instance = new t_instance(_num_particles);
	}

	inline ~PointParticleSystem()
	{
		delete _instance;
		_particles.clear();
	}

	inline virtual void update(double delta)
	{
		updateParticles(delta);
	}

	inline virtual void render()
	{
		for (unsigned int i = 0; i < _num_particles; i++)
		{
			glUniform1f(_particles[i]->_loc_alpha, _particles[i]->_alpha);
			glUniform3fv(_particles[i]->_loc_pos, 1, glm::value_ptr(_particles[i]->_pos));
		}

		glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(Content::_map->GetCamera()->GetViewMatrix()));
		glUniformMatrix4fv(proj, 1, GL_FALSE, glm::value_ptr(Content::_map->GetCamera()->GetProjectionMatrix()));

		glUniform3f(cam_u, Content::_map->GetCamera()->GetUp().x, Content::_map->GetCamera()->GetUp().y, Content::_map->GetCamera()->GetUp().z);
		glUniform3f(cam_r, Content::_map->GetCamera()->GetRight().x, Content::_map->GetCamera()->GetRight().y, Content::_map->GetCamera()->GetRight().z);

		glUniform1i(_sampler, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _samplerID);

		_instance->render();

	}

	inline virtual void respawn(_pt_x *p)
	{
		restoreDefaults(p);
		((_pt_point*)p)->_radius = randF(_spread.x, _spread.y);
		p->_pos = glm::vec2(randF(_position.x - _spread.x, _position.x + _spread.x), randF(_position.y - _spread.y, _position.y + _spread.y));
	}
};


// Class redefinitions
typedef ParticleSystem				PS_X;
typedef DirectionalParticleSystem	PS_Dir;
typedef PointParticleSystem			PS_Point;


#endif