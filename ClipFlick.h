#ifndef __CLIP_FLICK_H__
#define __CLIP_FLICK_H__

#include <vector>

#include "Clip.h"
#include "GBufferData.h"

class ClipFlick : public Clip
{
private:
	uint8_t _nr_frames;
	double  _speed;

	uniform _u_texture;
	uniform _u_model;

	std::vector<unsigned int> _frames;
public:
	inline ClipFlick(unsigned int _shader_program, uint8_t nr_frames, double speed, std::vector<unsigned int> frames)
	{
		Create(_shader_program, nr_frames, speed, frames);
	}

	inline ~ClipFlick() 
	{
		_frames.clear();

		glDeleteProgram(shader_program);
	}

	inline void Create(unsigned int _shader_program, uint8_t nr_frames, double speed, std::vector<unsigned int> frames)
	{
		 shader_program = _shader_program;
		_nr_frames		= nr_frames;
		_speed			= speed;
		_frames			= frames;

		_u_model = glGetUniformLocation(shader_program, "model");
		_u_texture = glGetUniformLocation(shader_program, "screenTexture");

		_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		_sca = glm::vec3(1.0f, 1.0f, 1.0f);
		_rot = glm::vec3(0.0f, 0.0f, 1.0f);

		_timer = new Timer(speed);
	}

	inline void Update(double delta) 
	{
		if (_isLooping == false)
		{
			if (_timer->seconds() >= _nr_frames)
			{
				_timer->reset(_speed);
				_timer->freq_ = 0.0;
			}
		}

		if (_timer->seconds() > 0.0) _isPlaying = true; 
		else _isPlaying = false;
	}

	inline void Render() 
	{
		glUseProgram(shader_program);

		glm::mat4 mod = glm::translate(_pos) * glm::scale(_sca) * glm::rotate(_ang, _rot);

		glUniformMatrix4fv(_u_model, 1, GL_FALSE, glm::value_ptr(mod));
		glUniform1i(_u_texture, 0);

		glActiveTexture(GL_TEXTURE0);

		_isLooping == true ? glBindTexture(GL_TEXTURE_2D, _frames[(int)_timer->seconds() % _nr_frames]) 
			: glBindTexture(GL_TEXTURE_2D, _frames[(int)_timer->seconds()]);

		_screen_rect->Render(1);
	}
};

#endif