#include <AL/alut.h>
#include "AudioManager.h"
//#include <string>
#include "../InputManager.h"
#include "../Utility/Logger.h"
#include <tinyxml2.h>

using namespace tinyxml2;

namespace AudioManager
{
	ALuint sourceBackground;
	ALuint sourceMovement;
	ALuint sourceGhost;
	ALuint sourceObstacles;
	ALuint sourceIntro;

	ALenum error;
	bool Initialized = false;
	const char* CONFIG_FILE = "Data/Assets/Sounds/config.xml";

	struct SoundInfo {
		std::string name;
		std::string path;
	};

	struct SoundBuffer {
		ALuint buffer;
		std::string soundName;
	};

	struct SoundSource {
		ALuint source;
		std::string sourceName;
	};

	std::vector <SoundInfo> soundInformation;
	std::vector <SoundBuffer> buffers;
	std::vector <ALuint> sources;
	int sourceInd;


	void Init_al()
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

		InitSources();
	}

	void Exit_al()
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

	void GetAllWavFilesNamesWithinFolder()
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
			soundInformation.push_back(soundInfo);
		}

		Initialized = true;

		LogInfo("SoundManager initialized.");
	}

	int GenerateBuffer(ALuint& buffer)
	{
		//generete buffers
		alGenBuffers(1, &buffer);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alGenBuffers : %d", error);
			printf("\n");
			return -1;
		}

		return 0;
	}

	int GenerateBufferAndLoadSound(ALuint& buffer, std::string name)
	{
		//generate buffer
		alGenBuffers(1, &buffer);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alGenBuffers : %d", error);
			printf("\n");
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
			printf("alutLoadWAVFile : %d", error);
			printf("\n");
			//Delete Buffers 
			alDeleteBuffers(1, &buffer);
			return -1;
		}

		SoundBuffer sb;
		sb.buffer = buffer;
		sb.soundName = name;
		buffers.push_back(sb);

		return 0;
	}

	int InitSources()
	{
		for (int i = 0; i < 50; i++)
		{
			ALuint source;
			alGenSources(1, &source);
			sources.push_back(source);
		}
		sourceInd = 0;
		InitSource(sourceMovement);
		InitSource(sourceBackground);
		InitSource(sourceGhost);
		InitSource(sourceObstacles);
		InitSource(sourceIntro);

		return 0;
	}

	int LoadSound(std::string soundName, ALuint& buffer)
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
			printf("alutLoadWAVFile : %d", error);
			printf("\n");
			//Delete Buffers 
			alDeleteBuffers(1, &buffer);
			return -1;
		}
		return 0;
	}

	int SetSource(ALuint& source, ALuint& buffer, float sourcePosX, float sourcePosY, float sourcePosZ,
		ALfloat const* sourceVel, ALfloat const* sourceDir, ALboolean loop)
	{
		// Generate the sources 
		alGenSources(1, &source);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alGenSources : %d", error);
			printf("\n");
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
			printf("\n");
			return -1;
		}

		return 0;
	}

	int AttachSourceBuffer(ALuint& source, ALuint& buffer)
	{
		alSourcei(source, AL_BUFFER, buffer);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alSourcei attach : %d", error);
			printf("\n");
			return -1;
		}
		return 0;
	}

	int ClearSource(ALuint& source)
	{
		alSourcei(source, AL_BUFFER, NULL);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("ClearSources : %d", error);
			printf("\n");
			return -1;
		}
		return 0;
	}

	int DeleteSource(ALuint& source)
	{
		ClearSource(source);
		alDeleteSources(1, &source);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("DeleteSource : %d", error);
			printf("\n");
			return -1;
		}
		return 0;
	}

	int SetSource(ALuint& source, ALuint& buffer, ALboolean loop)
	{
		// Generate the sources 
		alGenSources(1, &source);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			for (int i = 0; i < buffers.size(); i++)
			{
				if (buffer == buffers[i].buffer)
				{
					printf(buffers[i].soundName.c_str());
				}
			}
			printf("alGenSources : %d", error);
			printf("\n");
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
			printf("\n");
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
			printf("\n");
			return -1;
		}

		return 0;
	}

	int SetSource(ALuint& source, float sourcePosX, float sourcePosY, float sourcePosZ, ALboolean loop)
	{
		// Generate the sources 
		alGenSources(1, &source);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alGenSources : %d", error);
			printf("\n");
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
			printf("\n");
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
			printf("\n");
			return -1;
		}

		return 0;
	}

	int InitSource(ALuint& source)
	{
		// Generate the sources 
		alGenSources(1, &source);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alInitGenSources : %d", error);
			printf("\n");
			return -1;
		}

		return 0;
	}

	int SetExistingSource(ALuint& source, ALuint& buffer, ALboolean loop)
	{

		// Generate the sources 
	/*	alGenSources(1, &source);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alGenSources : %d", error);
			printf("\n");
			return -1;
		}*/
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
			printf("\n");
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
			printf("\n");
			return -1;
		}

		return 0;
	}

	int SetListener()
	{
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alListenerfv pos : %d", error);
			printf("\n");
			return -1;
		}
		alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			printf("alListenerfv  vel : %d", error);
			printf("\n");
			return -1;
		}
		return 0;
	}

	int PlaySoundFromSource(ALuint& source)
	{
		alSourcePlay(source);
		return 0;
	}

	void PregenerateBuffers()
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

	ALuint GetBuffer(std::string soundName)
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

	/*ALuint GetSource(std::string sourceName)
	{
		for (int i = 0; i < sources.size(); i++)
		{
			if (sources.at(i).sourceName == sourceName)
			{
				return sources.at(i).source;
			}
		}
		return -1;
	}*/

	void CreateDefaultSourceAndPlay(ALuint& source, std::string soundName, bool loop, float volume)
	{
		//create source
		//ALuint source;

		//get pregenerated buffer
		ALuint buffer = GetBuffer(soundName);

		//attach source to buffer and set default values
		SetSource(source, buffer, loop);

		//set volume
		SetSourceVolume(source, volume);

		//play source
		PlaySoundFromSource(source);
	}

	void PlayFromDefaultSource(std::string soundName, bool loop, float volume)
	{
		ClearSource(sources[sourceInd]);
		//get pregenerated buffer
		ALuint buffer = GetBuffer(soundName);

		//attach source to buffer and set default values
		SetExistingSource(sources[sourceInd], buffer, loop);

		//set volume
		SetSourceVolume(sources[sourceInd], volume);

		//play source
		PlaySoundFromSource(sources[sourceInd]);
		sourceInd++;
		if (sourceInd > 49)
		{
			sourceInd = 0;
		}
	}


	void SetSoundInSource(ALuint& source, std::string soundName, bool loop, float volume)
	{
		//StopSource(sourceBackground);

		//get pregenerated buffer
		ALuint buffer = GetBuffer(soundName);

		//attach source to buffer and set default values
		SetExistingSource(source, buffer, loop);

		//set volume
		SetSourceVolume(source, volume);
	}

	void PlayBackground(std::string soundName, float volume)
	{
		//StopSource(sourceBackground);
		ClearSource(sourceBackground);
		//get pregenerated buffer
		ALuint buffer = GetBuffer(soundName);

		//attach source to buffer and set default values
		SetExistingSource(sourceBackground, buffer, true);

		//set volume
		SetSourceVolume(sourceBackground, volume);

		//play source
		PlaySoundFromSource(sourceBackground);
	}

	void StopBackground()
	{
		alSourceStop(sourceBackground);
	}

	void PlayMovement(std::string soundName, float volume)
	{
		//StopSource(sourceBackground);
		ClearSource(sourceMovement);

		//get pregenerated buffer
		ALuint buffer = GetBuffer(soundName);

		//attach source to buffer and set default values
		SetExistingSource(sourceMovement, buffer, true);

		//set volume
		SetSourceVolume(sourceMovement, volume);

		//play source
		PlaySoundFromSource(sourceMovement);
	}

	void PlayMovement()
	{
		//play source
		PlaySoundFromSource(sourceMovement);
	}

	void SetMovementSound(std::string soundName, float volume)
	{
		//StopSource(sourceBackground);

		//get pregenerated buffer
		ALuint buffer = GetBuffer(soundName);

		//attach source to buffer and set default values
		SetExistingSource(sourceMovement, buffer, true);

		//set volume
		SetSourceVolume(sourceMovement, volume);
	}

	void StopMovement()
	{
		alSourceStop(sourceMovement);
	}

	void PlayGhost()
	{
		ClearSource(sourceMovement);

		//get pregenerated buffer
		ALuint buffer = GetBuffer("ghost");

		//attach source to buffer and set default values
		SetExistingSource(sourceGhost, buffer, false);

		//set volume
		SetSourceVolume(sourceGhost, 1.0f);

		//play source
		PlaySoundFromSource(sourceGhost);
	}

	void StopGhost()
	{
		StopSource(sourceGhost);
	}

	void PlayIntro()
	{
		ClearSource(sourceIntro);

		//get pregenerated buffer
		ALuint buffer = GetBuffer("intro");

		//attach source to buffer and set default values
		SetExistingSource(sourceIntro, buffer, false);

		//set volume
		SetSourceVolume(sourceIntro, 1.0f);

		//play source
		PlaySoundFromSource(sourceIntro);
	}

	void StopIntro()
	{
		StopSource(sourceIntro);
	}

	void StopSource(ALuint& source)
	{
		alSourceStop(source);
	}

	void PauseSource(ALuint& source)
	{
		alSourcePause(source);
	}

	void ContinuePlayingSource(ALuint& source)
	{
		PlaySoundFromSource(source);
	}

	void RewindSource(ALuint& source)
	{
		alSourceRewind(source);
	}

	void SetSourceVolume(ALuint& source, float volume)
	{
		alSourcef(source, AL_GAIN, volume);
	}

	ALint GetSourceState(ALuint& source)
	{
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		return state;
	}
}
