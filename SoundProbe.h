#ifndef __SOUNDPROBE_H
#define __SOUNDPROBE_H

#include <al.h>
#include <alc.h>
#include <vector>
#include "Camera.h"

class SoundProbe
{
private:
	unsigned int _sourceID;
public:
	SoundProbe(unsigned int _bufferID)
	{
		alGenSources(1, &_sourceID);
		alSourcei(_sourceID, AL_BUFFER, _bufferID);

		alSourcef(_sourceID, AL_ROLLOFF_FACTOR, 1);
		alSourcef(_sourceID, AL_REFERENCE_DISTANCE, 6);
		alSourcef(_sourceID, AL_MAX_DISTANCE, 15);

		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	}

	~SoundProbe() {}

	inline void setVolume(ALint volume) { alSourcei(_sourceID, AL_GAIN, volume); }
	inline void setPitch(ALint pitch) { alSourcei(_sourceID, AL_PITCH, pitch); }
	inline void setPosition(glm::vec3 pos) { alSource3f(_sourceID, AL_POSITION, pos.x, pos.y, pos.z); }
	inline void setLooping(ALint loop) { alSourcei(_sourceID, AL_LOOPING, loop); }

	inline void play()  { alSourcePlay(_sourceID); }
	inline void stop()  { alSourceStop(_sourceID); }
	inline void pause() { alSourcePlay(_sourceID); }
	inline void rewind() { alSourceRewind(_sourceID); }

	inline void updateListener(Camera* camera)
	{
		float f[] = { 1, 0, 0, 0, 1, 0 };
		alListenerfv(AL_ORIENTATION, f);
		alListener3f(AL_POSITION, camera->GetPosition().x, camera->GetPosition().y, camera->GetPosition().z);
	}
};

#endif
