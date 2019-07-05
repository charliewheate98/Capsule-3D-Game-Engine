#ifndef __CLIP_H__
#define __CLIP_H__

#include "Timer.h"
#include "Texture.h"
#include "SoundCue.h"
#include "SoundMaster.h"

class Clip
{
protected:
	unsigned int shader_program;

	bool _isLooping;
	bool _isPlaying;

	glm::vec3 _pos;
	glm::vec3 _sca;
	glm::vec3 _rot;
	float     _ang;

	Timer* _timer;

	SoundCue* _sndFx;
public:
	inline Clip() : _ang(0.0f) {}
	inline ~Clip() {}

	inline void SetPosition(glm::vec3 pos) { _pos = pos; }
	inline void SetScale(glm::vec3 sca) { _sca = sca; }
	inline void SetRotationAxis(glm::vec3 rot) { _rot = rot; }
	inline void SetAngle(float ang) { _ang = ang; }
	inline void SetLooping(bool val) { _isLooping = val; }
	inline bool IsPlaying(bool val) { return _isPlaying; }
	inline bool IsLooping(bool val) { return _isPlaying; }

	inline glm::vec3 GetPosition()	   { return _pos; }
	inline glm::vec3 GetScale()		   { return _sca; }
	inline glm::vec3 GetRotationAxis() { return _rot; }
	inline float     GetAngle()		   { return _ang; }

	inline void attachSound(const char* file)
	{
		unsigned int snd = SoundMaster::loadSound(file);

		_sndFx = new SoundCue(snd);
		_sndFx->play();
	}
	
	unsigned int addFrame(const char * file)
	{
		unsigned int id;

		int width;
		int height;
		int nr_comp;

		unsigned char* data = stbi_load(file, &width, &height, &nr_comp, 0);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return id;
	}

	inline virtual void Update(double delta) = 0;
	inline virtual void Render() = 0;
};

#endif
