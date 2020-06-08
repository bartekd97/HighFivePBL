#include <AL/alut.h>
#include "SoundManager.h"

void SoundManager::InitSoundManager()
{
	ALCdevice* dev = NULL;
	ALCcontext* ctx = NULL;

	const char* defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	dev = alcOpenDevice(defname);
	ctx = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(ctx);
	alutInitWithoutContext(NULL, NULL);
	error = alGetError(); //clear error code

	generateBuffersSources();
}

void SoundManager::ExitSoundManager()
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
}

vector<std::string> SoundManager::GetAllWavFilesNamesWithinFolder(std::string folder)
{
	vector<std::string> names;
	std::string search_path = folder + "/*.wav";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

int SoundManager::GenerateBuffersSources()
{
	vector<std::string> soundNames = GetAllWavFilesNamesWithinFolder(soundsFolderPath);

	//pregenerate buffers
	for (int i = 0; i < numBuffers; i++)
	{
		SoundBuffer buffer;
		alGenBuffer((ALuint)1, &buffer.id);
		this->buffers.push_back(buffer);
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers : %d", error);
		return 0;
	}

	//fill buffers with additional data
	for (int i = 0; i < soundNames.size(); i++)
	{
		buffers.at(i).id = i;
		buffers.at(i).isFree = false;
		buffers.at(i).filename = soundsFolderPath + soundNames.at(i);
	}

	for (int i = 0; i < numSources; i++)
	{
		SoundSource source;
		alGenSources((ALuint)1, &source.id);
		this->sources.push_back(source);
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources : %d", error);
		return 0;
	}

	for (uint32 i = 0; i < this->buffers.size(); i++) 
	{ 
		this->freeBuffers.push_back(&this->buffers); 
	} 

	for (uint32 i = 0; i < this->sources.size(); i++)
	{
		this->freeSources.push_back(&this->sources);
	}

}

//inicjalizacja threadow????

vector <ALuint> SoundManager::GetBuffers()
{
	return buffers;
}

vector <ALuint> SoundManager::GetSources()
{
	return sources;
}

void SoundManager::FreeSource()
{

}

int SoundManager::GetNumBuffers()
{
	return numBuffers;
}
int SoundManager::GetNumSources()
{
	return numSources;
}

