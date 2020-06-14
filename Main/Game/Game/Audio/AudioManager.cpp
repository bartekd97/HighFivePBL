#include <AL/alut.h>
#include "AudioManager.h"
//#include <string>
#include "../InputManager.h"
#include "../Utility/Logger.h"
#include <tinyxml2.h>

using namespace tinyxml2;

void AudioManager::Init_al()
{
	ALCdevice* dev = NULL;
	ALCcontext* ctx = NULL;

	const char* defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

	dev = alcOpenDevice(defname);
	ctx = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(ctx);
	alutInitWithoutContext(NULL, NULL);
	error = alGetError(); //clear error code

	PregenerateBuffers();
}

void AudioManager::Exit_al()
{
	ALCdevice* dev = NULL;
	ALCcontext* ctx = NULL;
	ctx = alcGetCurrentContext();
	dev = alcGetContextsDevice(ctx);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alutExit();
	alcCloseDevice(dev);
}

void AudioManager::GetAllWavFilesNamesWithinFolder()
{
	if (Initialized)
	{
		LogWarning("AudioController::Initialize(): Already initialized");
		return;
	}

	tinyxml2::XMLDocument doc;
	auto loaded = doc.LoadFile(CONFIG_FILE);
	if (loaded != XML_SUCCESS)
	{
		LogError("AudioController::Initialize(): Cannot load xml config file. Code: {}", loaded);
		return;
	}

	auto root = doc.FirstChildElement();
	if (root == nullptr)
	{
		LogError("AudioController::Initialize(): Cannot find root element in XML config file.");
		return;
	}

	int i = 0;
	for (XMLElement* node = root->FirstChildElement("sound");
		node != nullptr;
		node = node->NextSiblingElement("sound"), i++)
	{
		const XMLAttribute* attrName = node->FindAttribute("name");
		const XMLAttribute* attrPath = node->FindAttribute("path");

		if (attrPath == nullptr || attrName == nullptr)
		{
			LogWarning("AudioController::Initialize(): Invalid sound node at #{}", i);
			continue;
		}

		std::string soundName = attrName->Value();
		std::string soundPath = attrPath->Value();

		SoundInfo soundInfo;
		soundInfo.name = soundName;
		soundInfo.path = soundPath;
		this->soundInformation.push_back(soundInfo);
	}

	Initialized = true;

	LogInfo("SoundManager initialized.");
}

int AudioManager::GenerateBuffer(ALuint &buffer)
{
	//generete buffers
	alGenBuffers(1, &buffer);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers : %d", error);
		return -1;
	}

	return 0;
}

int AudioManager::GenerateBufferAndLoadSound(ALuint& buffer, std::string name)
{
	//generate buffer
	alGenBuffers(1, &buffer);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenBuffers : %d", error);
		return -1;
	}

	std::string path;
	for (int i = 0; i < soundInformation.size(); i++)
	{
		if (soundInformation.at(i).name == name)
		{
			path = soundInformation.at(i).path;
		}
	}

	const char* path1 = path.c_str();

	//load the sound
	buffer = alutCreateBufferFromFile(path1);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alutLoadWAVFile exciting_sound.wav : %d", error);
		//Delete Buffers 
		alDeleteBuffers(1, &buffer);
		return -1;
	}

	SoundBuffer sb;
	sb.buffer = buffer;
	sb.soundName = name;
	this->buffers.push_back(sb);

	return 0;
}

int AudioManager::LoadSound(std::string soundName, ALuint& buffer)
{
	std::string path;

	for (int i = 0; i < soundInformation.size(); i++)
	{
		if (soundInformation.at(i).name == soundName)
		{
			path = soundInformation.at(i).path;
		}
	}

	const char* path1 = path.c_str();

	//load the sound
	buffer = alutCreateBufferFromFile(path1);

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alutLoadWAVFile exciting_sound.wav : %d", error);
		//Delete Buffers 
		alDeleteBuffers(1, &buffer);
		return -1;
	}
	return 0;
}

int AudioManager::SetSource(ALuint &source, ALuint& buffer, float sourcePosX, float sourcePosY, float sourcePosZ,
								ALfloat const* sourceVel, ALfloat const* sourceDir, ALboolean loop)
{
	// Generate the sources 
	alGenSources(1, &source);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources : %d", error);
		return -1;
	}

	//atach buffers to sources
	alSourcei(source, AL_BUFFER, buffer);

	if (loop == true)
	{
		alSourcei(source, AL_LOOPING, AL_TRUE);
	}
	else
	{
		alSourcei(source, AL_LOOPING, AL_FALSE);
	}

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcei : %d", error);
		return -1;
	}

	float sourcePos[] = { sourcePosX, sourcePosY, sourcePosZ };

	alSourcefv(source, AL_POSITION, sourcePos); 
	alSourcefv(source, AL_VELOCITY, sourceVel); 
	alSourcefv(source, AL_DIRECTION, sourceDir); 
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcefv : %d", error);
		return -1;
	}

	return 0;
}

int AudioManager::AttachSourceBuffer(ALuint& source, ALuint& buffer)
{
	alSourcei(source, AL_BUFFER, buffer);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcei attach : %d", error);
		return -1;
	}
	return 0;
}

int AudioManager::SetSource(ALuint& source, ALuint& buffer, ALboolean loop)
{
	// Generate the sources 
	alGenSources(1, &source);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources : %d", error);
		return -1;
	}

	//atach buffers to sources
	alSourcei(source, AL_BUFFER, buffer);

	if (loop == true)
	{
		alSourcei(source, AL_LOOPING, AL_TRUE);
	}
	else
	{
		alSourcei(source, AL_LOOPING, AL_FALSE);
	}

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcei : %d", error);
		return -1;
	}

	float sourcePos[] = { 0.0f, 0.0f, 0.0f };
	float sourceVel[] = { 0.0f, 0.0f, 0.0f };
	float sourceDir[] = { 0.0f, 0.0f, 0.0f };

	alSourcefv(source, AL_POSITION, sourcePos); 
	alSourcefv(source, AL_VELOCITY, sourceVel); 
	alSourcefv(source, AL_DIRECTION, sourceDir); 
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcefv : %d", error);
		return -1;
	}

	return 0;
}

int AudioManager::SetSource(ALuint& source, float sourcePosX, float sourcePosY, float sourcePosZ, ALboolean loop)
{
	// Generate the sources 
	alGenSources(1, &source);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alGenSources : %d", error);
		return -1;
	}

	if (loop == true)
	{
		alSourcei(source, AL_LOOPING, AL_TRUE);
	}
	else
	{
		alSourcei(source, AL_LOOPING, AL_FALSE);
	}

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcei : %d", error);
		return -1;
	}

	float sourcePos[] = { sourcePosX, sourcePosY, sourcePosZ };
	float sourceVel[] = { 0.0f, 0.0f, 0.0f };
	float sourceDir[] = { 0.0f, 0.0f, 0.0f };

	alSourcefv(source, AL_POSITION, sourcePos); 
	alSourcefv(source, AL_VELOCITY, sourceVel); 
	alSourcefv(source, AL_DIRECTION, sourceDir); 
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alSourcefv : %d", error);
		return -1;
	}

	return 0;
}

int AudioManager::SetListener()
{
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alListenerfv pos : %d", error);
		return -1;
	}
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		printf("alListenerfv  vel : %d", error);
		return -1;
	}
	return 0;
}

int AudioManager::PlaySoundFromSource(ALuint source)
{
	alSourcePlay(source);
	return 0;
}

void AudioManager::PregenerateBuffers()
{
	//read xml
	GetAllWavFilesNamesWithinFolder();

	//generate buffers and load files
	for (int i = 0; i < soundInformation.size(); i++)
	{
		ALuint buffer;
		GenerateBufferAndLoadSound(buffer, soundInformation.at(i).name);
	}
}

ALuint AudioManager::GetBuffer(std::string soundName)
{
	for (int i = 0; i < buffers.size(); i++)
	{
		if (buffers.at(i).soundName == soundName)
		{
			return buffers.at(i).buffer;
		}
	}
	return -1;
}

void AudioManager::CreateDefaultSourceAndPlay(std::string soundName, bool loop)
{
	//create source
	ALuint source;

	//get pregenerated buffer
	ALuint buffer = GetBuffer(soundName);

	//attach source to buffer and set default values
	SetSource(source, buffer, loop);

	//play source
	PlaySoundFromSource(source);
}
