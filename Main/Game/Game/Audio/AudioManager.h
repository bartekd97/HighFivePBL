#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <string>
#include <vector>

namespace AudioManager
{
	/*
	struct SoundInfo {
		std::string name;
		std::string path;
	};

	struct SoundBuffer {
		ALuint buffer;
		std::string soundName;
	};

	std::vector <SoundInfo> soundInformation;
	std::vector <SoundBuffer> buffers;
	*/
	void Init_al();
	void Exit_al();
	void GetAllWavFilesNamesWithinFolder();
	int GenerateBuffer(ALuint& buffer);
	int SetSource(ALuint &source, ALuint &buffer, float sourcePosX, float sourcePosY, float sourcePosZ, 
									ALfloat const* sourceVel, ALfloat const* sourceDir, ALboolean loop);
	int SetListener();
	int PlaySoundFromSource(ALuint source);
	int LoadSound(std::string soundName, ALuint& buffer);
	int GenerateBufferAndLoadSound(ALuint& buffer, std::string name);
	int AttachSourceBuffer(ALuint& buffer, ALuint& source);
	int SetSource(ALuint& source, float sourcePosX, float sourcePosY, float sourcePosZ, ALboolean loop);
	void PregenerateBuffers();
	ALuint GetBuffer(std::string soundName);
	int SetSource(ALuint& source, ALuint& buffer, ALboolean loop);
	void CreateDefaultSourceAndPlay(ALuint &source, std::string soundName, bool loop, float volume = 1.0f);
	void PlayBackground(std::string soundName, float volume);
	void StopBackground();
	void StopSource(ALuint& source);
	void PauseSource(ALuint& source);
	void ContinuePlayingSource(ALuint& source);
	void RewindSource(ALuint& source);
	void SetSourceVolume(ALuint& source, float volume);
	ALint GetSourceState(ALuint &source);

	//ALenum error;
	//bool Initialized = false;
	//const char* CONFIG_FILE = "Data/Assets/Sounds/config.xml";
};


