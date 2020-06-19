#include <AL/alut.h>
#include "SoundManager.h"
#include <tinyxml2.h>
#include <iostream>

using namespace tinyxml2;

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

	//GenerateBuffersSources();
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

void SoundManager::GetAllWavFilesNamesWithinFolder()
{
	if (Initialized)
	{
		LogWarning("SoundManager::Initialize(): Already initialized");
		return;
	}

	tinyxml2::XMLDocument doc;
	auto loaded = doc.LoadFile(CONFIG_FILE);
	if (loaded != XML_SUCCESS)
	{
		LogError("SoundManager::Initialize(): Cannot load xml config file. Code: {}", loaded);
		return;
	}

	auto root = doc.FirstChildElement();
	if (root == nullptr)
	{
		LogError("SoundManager::Initialize(): Cannot find root element in XML config file.");
		return;
	}

	int i = 0;
	for (XMLElement* node = root->FirstChildElement("sound");
		node != nullptr;
		node = node->NextSiblingElement("sound"), i++)
	{
		const XMLAttribute* attrID = node->FindAttribute("id");
		const XMLAttribute* attrName = node->FindAttribute("name");

		if (attrID == nullptr || attrName == nullptr)
		{
			LogWarning("SoundManager::Initialize(): Invalid shader node at #{}", i);
			continue;
		}

		int soundID = std::stoi(attrID->Value());
		std::string soundName = attrName->Value();

		SoundInfo soundInfo;
		soundInfo.id = soundID;
		soundInfo.name = soundName;
		this->soundInformation.push_back(soundInfo);
	}

	Initialized = true;

	LogInfo("SoundManager initialized.");
}

int SoundManager::GenerateBuffersSources()
{
	GetAllWavFilesNamesWithinFolder();

	//pregenerate buffers
	for (int i = 0; i < soundInformation.size(); i++)
	{
		SoundBuffer buffer;
		buffer.id = soundInformation.at(i).id;
		buffer.isFree = true;
		std::string path = soundsFolderPath + soundInformation.at(i).name;
		buffer.filename = soundInformation.at(i).name;
		alGenBuffers((ALuint)1, &buffer.buffer);
		buffer.buffer = alutCreateBufferFromFile(path.c_str());
		this->buffers.push_back(buffer);
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers or alutCreateBufferFromFile: %d", error);
		return 0;
	}
	/*
	for (int i = 0; i < soundInformation.size(); i++)
	{
		this->buffers.at(i).buffer = alutCreateBufferFromFile(this->buffers.at(i).filename.c_str());
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alutCreateBufferFromFile : %d", error);
		for (int i = 0; i < soundInformation.size(); i++)
		{
			alDeleteBuffers(soundInformation.size(), &buffers.at(i).buffer);
		}
		
		return 0;
	}
	*/
	for (int i = 0; i < numSources; i++)
	{
		SoundSource source;
		//alGenSources((ALuint)1, &source.id);
		source.id = i;
		source.isFree = true;
		alGenSources((ALuint)1, &source.source);
		this->sources.push_back(source);
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources : %d", error);
		return 0;
	}

}

//inicjalizacja threadow????

int SoundManager::GenerateBuffers()
{
	GetAllWavFilesNamesWithinFolder();

	//pregenerate buffers
	for (int i = 0; i < soundInformation.size(); i++)
	{
		SoundBuffer buffer;
		buffer.id = soundInformation.at(i).id;
		buffer.isFree = true;
		std::string path = soundsFolderPath + soundInformation.at(i).name;
		buffer.filename = soundInformation.at(i).name;
		alGenBuffers((ALuint)1, &buffer.buffer);
		buffer.buffer = alutCreateBufferFromFile(path.c_str());
		this->buffers.push_back(buffer);
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers or alutCreateBufferFromFile: %d", error);
		return 0;
	}
	/*
	for (int i = 0; i < soundInformation.size(); i++)
	{
		this->buffers.at(i).buffer = alutCreateBufferFromFile(this->buffers.at(i).filename.c_str());
	}
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alutCreateBufferFromFile : %d", error);
		for (int i = 0; i < soundInformation.size(); i++)
		{
			alDeleteBuffers(soundInformation.size(), &buffers.at(i).buffer);
		}

		return 0;
	}
	*/
}

std::vector <SoundManager::SoundBuffer> SoundManager::GetBuffers()
{
	return this->buffers;
}

std::vector<SoundManager::SoundSource> SoundManager::GetSources()
{
	return this->sources;
}
/*
int SoundManager::GetNumBuffers()
{
	return this->numBuffers;
}
*/
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

ALuint SoundManager::GetActualBuffer(std::string filename, ALuint &buffer)
{
	for (int i = 0; i < buffers.size(); i++)
	{
		if (buffers.at(i).filename == filename)
		{
			buffer = buffers.at(i).buffer;
			std::cout << buffer;
			return buffer;
		}
	}
}
