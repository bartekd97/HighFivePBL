#include "AudioManager.h"
#include "../Utility/Logger.h"

namespace AudioManager
{
	/*bool initialized = false;

	void Init()
	{
		if (initialized)
		{
			LogWarning("AudioManager already initialized!");
			return;
		}
		ALCdevice* dev = NULL;
		ALCcontext* ctx = NULL;

		const char* defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

		dev = alcOpenDevice(defname);
		ctx = alcCreateContext(dev, NULL);
		alcMakeContextCurrent(ctx);
		alutInitWithoutContext(NULL, NULL);
		error = alGetError(); //clear error code
	}

	void AudioController::exit_al()
	{
		//alDeleteSources(NUM_SOURCES, source); 
		//alDeleteBuffers(NUM_BUFFERS, buffers);

		ALCdevice* dev = NULL;
		ALCcontext* ctx = NULL;
		ctx = alcGetCurrentContext();
		dev = alcGetContextsDevice(ctx);

		alcMakeContextCurrent(NULL);
		alcDestroyContext(ctx);
		alutExit();
		alcCloseDevice(dev);
	}*/
}
