#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <string>
#include <vector>

class AudioManager
{
	struct SoundInfo {
		std::string name;
		std::string path;
	};

	struct SoundBuffer {
		ALuint buffer;
		std::string soundName;
	};

public:
	std::vector <SoundInfo> soundInformation;
	std::vector <SoundBuffer> buffers;
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
	void CreateDefaultSourceAndPlay(std::string soundName, bool loop);

private:
	ALenum error;
	bool Initialized = false;
	const char* CONFIG_FILE = "Data/Assets/Sounds/config.xml";
};


