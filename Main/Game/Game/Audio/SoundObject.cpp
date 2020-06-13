#include "SoundObject.h"
/*
bool SoundObject::PreloadBuffer(int bufferID) {
	std::vector decompressBuffer; 
	this->soundFileWrapper->DecompressStream(decompressBuffer, this->settings.loop); 
	if (decompressBuffer.size() == 0) 
	{ 
		//nothing more to read 
		return false; 
	} 
	//now we fill loaded data to our buffer 
	alBufferData(bufferID, this->sound.format, &decompressBuffer[0], static_cast(decompressBuffer.size()), this->sound.freqency); 
	return true; 
} 

void SoundObject::LoadAllSounds()
{
	//std::vector <std::string> soundNames = SoundManager::GetAllWavFilesNamesWithinFolder(soundsFolderPath);
	for (int i = 0; i < soundNames.size(); i++)
	{
		SoundManager::GetBuffers().at(i).filename = soundNames.at(i);
		SoundManager::GetBuffers().at(i).buffer = alutCreateBufferFromFile(soundNames.at(i));
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alutLoadWAVFile exciting_sound.wav : %d", error);
			//Delete Buffers 
			alDeleteBuffers(NUM_BUFFERS, buffers);
			return 0;
		}
	}
}
*/
int SoundObject::SetSource(int sourceID, int bufferID, ALfloat const* sourcePos, ALfloat const* sourceVel, ALfloat const* sourceDir, bool loop)
{
	//get error
	//SoundManager error;
	ALenum error;// = error.getError();
	
	//get source
	SoundManager chosenSource;
	ALuint source = chosenSource.GetSource(sourceID).source;

	//get buffer
	SoundManager chosenBuffer;
	ALuint buffer = chosenBuffer.GetBuffer(bufferID).buffer;

	if (chosenSource.GetSource(sourceID).isFree == false)
	{
		return -1;
	}
	else
	{
		chosenSource.SetSoundSourceIsFree(sourceID, false);
	}

	//atach buffer to source
	alSourcei(source, AL_BUFFER, buffer);

	if (loop == true)
	{
		alSourcei(source, AL_LOOPING, AL_TRUE);
	}
	else
	{
		alSourcei(source, AL_LOOPING, AL_FALSE);
	}

	alSourcefv(source, AL_POSITION, sourcePos); //pos do przemyslenia
	alSourcefv(source, AL_VELOCITY, sourceVel); //velocity do przemyslenia
	alSourcefv(source, AL_DIRECTION, sourceDir); //dir do przemyslenia
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcefv : %d", error);
		return 0;
	}

	return 0;
}

int SoundObject::SetSource(std::string bufferFilename, int sourceID, ALfloat const* sourcePos, ALfloat const* sourceVel, ALfloat const* sourceDir, bool loop)
{
	//get error
	//SoundManager error;
	ALenum error;// = error.getError();

	//get source
	SoundManager chosenSource;
	ALuint source = chosenSource.GetSource(sourceID).source;

	//get buffer
	SoundManager chosenBuffer;
	ALuint buffer = chosenBuffer.GetBuffer(bufferFilename).buffer;

	if (chosenSource.GetSource(sourceID).isFree == false)
	{
		return -1;
	}
	else
	{
		chosenSource.SetSoundSourceIsFree(sourceID, false);
	}

	//atach buffer to source
	alSourcei(source, AL_BUFFER, buffer);

	if (loop == true)
	{
		alSourcei(source, AL_LOOPING, AL_TRUE);
	}
	else
	{
		alSourcei(source, AL_LOOPING, AL_FALSE);
	}

	alSourcefv(source, AL_POSITION, sourcePos); //pos do przemyslenia
	alSourcefv(source, AL_VELOCITY, sourceVel); //velocity do przemyslenia
	alSourcefv(source, AL_DIRECTION, sourceDir); //dir do przemyslenia
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcefv : %d", error);
		return 0;
	}

	return 0;
}

void SoundObject::PlaySource(int sourceID)
{
	SoundManager chosenSource; 
	ALuint source = chosenSource.GetSource(sourceID).source;
	//ALuint source = SoundManager::GetSources()[sourceID].source;
	alSourcePlay(source);
}

void SoundObject::StopSound(int sourceID)
{
	SoundManager chosenSource;
	ALuint source = chosenSource.GetSource(sourceID).source;
	//ALuint source = SoundManager::GetSources()[sourceID].source;
	alSourceStop(source);
}

void SoundObject::PauseSound(int sourceID)
{
	SoundManager chosenSource;
	ALuint source = chosenSource.GetSource(sourceID).source;
	//ALuint source = SoundManager::GetSources()[sourceID].source;
	alSourcePause(source);
}

int SoundObject::setListener()
{
	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	SoundManager sm;
	ALenum error = sm.getError();

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alListenerfv pos : %d", error);
		return 0;
	}
	alListener3f(AL_VELOCITY, 0, 0, 0);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alListenerfv  vel : %d", error);
		return 0;
	}
	alListenerfv(AL_ORIENTATION, listenerOri);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alListenerfv ori : %d", error);
		return 0;
	}
}
