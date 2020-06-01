#include "AudioController.h"


void AudioController::init_al()
{
	ALCdevice* dev = NULL;
	ALCcontext* ctx = NULL;

	const char* defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
		
	dev = alcOpenDevice(defname);
	ctx = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(ctx);
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
	alcCloseDevice(dev);
}

int AudioController::generateBuffers()
{
	//generete buffers
	alGenBuffers(10, buffers);

	if ((error = alGetError()) != AL_NO_ERROR) 
	{ 
		printf("alGenBuffers : %d", error); 
		return 0; 
	}
}

int AudioController::loadSound()
{
	//load the sound
	alutLoadWAVFile("exciting_sound.wav", &format, &data, &size, &freq, &loop); 
	if ((error = alGetError()) != AL_NO_ERROR) 
	{
		printf("alutLoadWAVFile exciting_sound.wav : %d", error); 
		//Delete Buffers 
		alDeleteBuffers(NUM_BUFFERS, buffers); 
		return 0; 
	}

	//load to the buffer
	alBufferData(buffers[0], format, data, size, freq); 
	if ((error = alGetError()) != AL_NO_ERROR) 
	{
		printf("alBufferData buffer 0 : %d", error); 
		// Delete buffers 
		alDeleteBuffers(NUM_BUFFERS, buffers); 
		return 0; 
	}

	//unload memory
	alutUnloadWAV(format, data, size, freq); 
	if ((error = alGetError()) != AL_NO_ERROR) 
	{
		printf("alutUnloadWAV : %d", error); 
		// Delete buffers 
		alDeleteBuffers(NUM_BUFFERS, buffers); 
		return 0; 
	}
}

int AudioController::setSources()
{
	ALuint source[10]; //fucking random number again monkaS
	// Generate the sources 
	alGenSources(10, source);
	if ((error = alGetError()) != AL_NO_ERROR) 
	{ 
		printf("alGenSources : %d", error); 
		return 0; 
	}

	//atach buffers to sources
	alSourcei(source[0], AL_BUFFER, buffers[0]); 
	if ((error = alGetError()) != AL_NO_ERROR) 
	{ 
		printf("alSourcei : %d", error); 
		return 0; 
	}

	alSourcefv(source[0], AL_POSITION, 0); //pos do przemyslenia
	alSourcefv(source[0], AL_VELOCITY, 0); //velocity do przemyslenia
	alSourcefv(source[0], AL_DIRECTION, 0); //dir do przemyslenia
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcefv : %d", error);
		return 0;
	}
}

int AudioController::setListener(ALfloat const* listenerPos, ALfloat const* listenerVel, ALfloat const* listenerOri)
{
	alListenerfv(AL_POSITION, listenerPos); //pos do przemyslenia
	alListenerfv(AL_VELOCITY, listenerVel); //vel do przemyslenia
	alListenerfv(AL_ORIENTATION, listenerOri);  //ori do przemyslenia
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alListenerfv : %d", error);
		return 0;
	}
}

	
	









