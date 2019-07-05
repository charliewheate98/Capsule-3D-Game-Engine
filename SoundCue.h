#ifndef __SOUNDQUEUE_H
#define __SOUNDQUEUE_H

#include <al.h>
#include <alc.h>
#include <vector>
#include "Camera.h"

class SoundCue
{
private:
	unsigned int _sourceID;
public:
	SoundCue(unsigned int _bufferID)
	{
		alGenSources(1, &_sourceID);

		alSourcei(_sourceID, AL_BUFFER, _bufferID);
		alSourcei(_sourceID, AL_LOOPING, AL_TRUE);
	}

	~SoundCue() {}

	inline void setVolume(ALint volume) { alSourcei(_sourceID, AL_GAIN, volume); }
	inline void setLooping(ALint loop) { alSourcei(_sourceID, AL_LOOPING, loop); }

	inline void play() { alSourcePlay(_sourceID); }
	inline void stop() { alSourceStop(_sourceID); }
	inline void pause() { alSourcePlay(_sourceID); }
	inline void rewind() { alSourceRewind(_sourceID); }
};

#endif
