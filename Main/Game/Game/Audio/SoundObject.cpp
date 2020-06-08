#include "SoundObject.h"

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
	vector <std::string> soundNames = GetAllWavFilesNamesWithinFolder(soundsFolderPath);
	for (int i = 0; i < soundNames.size(); i++)
	{
		SoundManager::GetBuffers().at(i).filename = soundNames.at(i);
		SoundManager::GetBuffers().at(i) = alutCreateBufferFromFile(soundNames.at(i));
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alutLoadWAVFile exciting_sound.wav : %d", error);
			//Delete Buffers 
			alDeleteBuffers(NUM_BUFFERS, buffers);
			return 0;
		}
	}
}

void SoundObject::LoadAndPlaySound(int bufferID)
{
	this->source = SoundManager::GetInstance()->GetFreeSource();
	alSourcef(this->source->id, AL_PITCH, this->settings.pitch);
	alSourcef(this->source->id, AL_GAIN, this->settings.gain);
	alSource3f(this->source->id, AL_POSITION, this->settings.pos.X, this->settings.pos.X, this->settings.pos.X);
	alSource3f(this->source->id, AL_VELOCITY, this->settings.velocity.X, this->settings.velocity.Y, this->settings.velocity.Z);
	alSourcei(this->source->id, AL_LOOPING, false);
	this->remainBuffers = 0;
	for (int i = 0; i < preloadBuffersCount; i++)
	{
		if (this->buffers == NULL)
		{
			continue; //buffer not used, do not add it to the queue 
		}
		alSourceQueueBuffers(this->source->id, 1, &this->buffers->id);
		this->remainBuffers++;
	}
	alSourcePlay(this->source->id);
	this->state = PLAYING;
}


void SoundObject::Update() {
	if (this->state != PLAYING) 
	{ 
		//sound is not playing (PAUSED / STOPPED) do not update 
		return; 
	} 
	int buffersProcessed = 0; 
	alGetSourcei(this->source->id, AL_BUFFERS_PROCESSED, &buffersProcessed); 
	// check to see if we have a buffer to deQ 
	if (buffersProcessed > 0) 
	{ 
		if (buffersProcessed > 1) 
		{ 
			//we have processed more than 1 buffer since last call of Update method 
			//we should probably reload more buffers than just the one (not supported yet) 
			//MyUtils::Logger::LogInfo("Processed more than 1 buffer since last Update"); 
			printf("Processed more than 1 buffer since last Update");
		} 
		// remove the buffer form the source 
		uint32 bufferID; 
		alSourceUnqueueBuffers(this->source->id, 1, &bufferID); 
		// fill the buffer up and reQ! 
		// if we cant fill it up then we are finished 
		// in which case we dont need to re-Q 
		// return NO if we dont have more buffers to Q 
		if (this->state == STOPPED) 
		{ 
			//put it back - sound is not playing anymore 
			alSourceQueueBuffers(this->source->id, 1, &bufferID); 
			return; 
		} 
		//call method to load data to buffer 
		//see method in section - Creating sound 
		if (this->PreloadBuffer(bufferID) == false) 
		{ 
			this->remainBuffers--; 
		} 
		//put the newly filled buffer back (at the end of the queue) 
		alSourceQueueBuffers(this->source->id, 1, &bufferID); 
	} 
	if (this->remainBuffers <= 0) 
	{ 
		//no more buffers remain - stop sound automatically 
		this->Stop(); 
	} 
}

void SoundObject::Stop()
{
	alSourceStop(this->source->id); 
	//Remove buffers from queue 
	for (int i = 0; i < preloadBuffersCount; i++)
	{ 
		if (this->buffers == NULL) 
		{ 
			continue; 
		} 
		alSourceUnqueueBuffers(this->source->id, 1, &this->buffers->id); 
	} 
	//Free the source 
	SoundManager::GetInstance()->FreeSource(this->source); 
	//solving the "glitch" in the sound - release buffers and aquire them again 
	for (uint32 i = 0; i < preloadBuffersCount; i++)
	{ 
		SoundManager::GetInstance()->FreeBuffer(this->buffers); 
		SoundBuffer * buf = SoundManager::GetInstance()->GetFreeBuffer(); 
		if (buf == NULL) 
		{ 
			//MyUtils::Logger::LogWarning("Not enought free sound-buffers"); 
			continue; 
		} 
		this->buffers = buf; 
	} 
	//Preload data again ...
}