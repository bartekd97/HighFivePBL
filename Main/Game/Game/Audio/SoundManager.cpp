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

	GenerateBuffersSources();
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

std::vector<std::string> SoundManager::GetAllWavFilesNamesWithinFolder(std::string folder)
{
	std::vector<std::string> names;
	//Convert string to wstring
	std::wstring search_path = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(folder);
	//std::string search_path = folder + "/*.wav";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				//convert from wide char to narrow char array
				char ch[260];
				char DefChar = ' ';
				WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, ch, 260, &DefChar, NULL);
				names.push_back(ch);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	return names;
}

int SoundManager::GenerateBuffersSources()
{
	std::vector<std::string> soundNames = GetAllWavFilesNamesWithinFolder(soundsFolderPath);

	//pregenerate buffers
	for (int i = 0; i < numBuffers; i++)
	{
		SoundBuffer buffer;
		//alGenBuffer((ALuint)1, &buffer.id);
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
		this->buffers.at(i).id = i;
		this->buffers.at(i).isFree = false;
		this->buffers.at(i).filename = soundsFolderPath + soundNames.at(i);
		alGenBuffers((ALuint)1, &buffers.at(i).buffer);
		//buffers.at(i).buffer = alutCreateBufferFromFile(soundNames.at(i));
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers : %d", error);
		return 0;
	}

	for (int i = 0; i < soundNames.size(); i++)
	{
		this->buffers.at(i).buffer = alutCreateBufferFromFile(soundNames.at(i).c_str());
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alutCreateBufferFromFile : %d", error);
		for (int i = 0; i < soundNames.size(); i++)
		{
			alDeleteBuffers(numBuffers, &buffers.at(i).buffer);
		}
		
		return 0;
	}

	for (int i = 0; i < numSources; i++)
	{
		SoundSource source;
		//alGenSources((ALuint)1, &source.id);
		this->sources.push_back(source);
	}
	//if ((error = alGetError()) != AL_NO_ERROR)
	//{
	//	printf("alGenSources : %d", error);
	//	return 0;
	//}

	for (int i = 0; i < sources.size(); i++)
	{
		this->sources.at(i).id = i;
		this->sources.at(i).isFree = true;
		alGenSources((ALuint)1, &sources.at(i).source);
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources : %d", error);
		return 0;
	}
}

//inicjalizacja threadow????

std::vector <SoundManager::SoundBuffer> SoundManager::GetBuffers()
{
	return this->buffers;
}

std::vector<SoundManager::SoundSource> SoundManager::GetSources()
{
	return this->sources;
}
/*
void SoundManager::GetFreeSource()
{

}
*/
int SoundManager::GetNumBuffers()
{
	return this->numBuffers;
}

int SoundManager::GetNumSources()
{
	return this->numSources;
}

SoundManager::SoundSource SoundManager::GetSource(int sourceID)
{
	for (int i = 0; i < sources.size(); i++)
	{
		if (sources.at(i).id == sourceID)
		{
			return sources.at(i);
			break;
		}
	}
}

SoundManager::SoundBuffer SoundManager::GetBuffer(int bufferID)
{
	for (int i = 0; i < buffers.size(); i++)
	{
		if (buffers.at(i).id == bufferID)
		{
			return buffers.at(i);
			break;
		}
	}
}

SoundManager::SoundBuffer SoundManager::GetBuffer(std::string bufferFilename)
{
	for (int i = 0; i < buffers.size(); i++)
	{
		if (buffers.at(i).filename == bufferFilename)
		{
			return buffers.at(i);
			break;
		}
	}
}

ALenum SoundManager::getError()
{
	return this->error;
}

void SoundManager::SetSoundBufferIsFree(int bufferID, bool isFree)
{
	for (int i = 0; i < buffers.size(); i++)
	{
		if (buffers.at(i).id == bufferID)
		{
			buffers.at(i).isFree = isFree;
			break;
		}
	}
}

void SoundManager::SetSoundSourceIsFree(int sourceID, bool isFree)
{
	for (int i = 0; i < sources.size(); i++)
	{
		if (sources.at(i).id == sourceID)
		{
			sources.at(i).isFree = isFree;
			break;
		}
	}
}

SoundManager::SoundSource SoundManager::GetFreeSource()
{
	int id = -1;
	for (int i = 0; i < sources.size(); i++)
	{
		if (sources.at(i).isFree == true)
		{
			id = i;
			//return sources.at(i);
			break;
		}
	}

	SetSoundSourceIsFree(id, false);
	return sources.at(id);
}

