#include <AL/alut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <Windows.h>
#include <locale>
#include <codecvt>
#include "../Utility/Utility.h"
#include "../Utility/Logger.h"
#include <tinyxml2.h>
#include <unordered_map>
#include <stdexcept>
#include <memory>

class SoundManager {

//#define NUM_BUFFERS = 64;
//#define NUM_SOURCES = 16;

	struct SoundBuffer {
		int id;
		bool isFree;
		std::string filename;
		ALuint buffer; //actual buffer for alut function
	};

	struct SoundSource {
		int id;
		bool isFree;
		ALuint source; //actual source
	};

	struct SoundInfo {
		int id;
		std::string name;
	};

private:
	//int numBuffers = 64;
	int numSources = 16;
	//ALuint buffers[NUM_BUFFERS];
	//std::vector <SoundBuffer> buffers;
	//std::vector <SoundSource> sources;
	//vector <ALuint> freeBuffers;
	//vector <ALuint> freeSources;
	std::string soundsFolderPath = "Data/Assets/Sounds/";
	ALenum error;
	const char* CONFIG_FILE = "Data/Assets/Sounds/config.xml";
	bool Initialized = false;
	//std::vector<SoundInfo> soundInformation;

public:

	friend class SoundObject;

	void InitSoundManager();
	void ExitSoundManager();
	int GenerateBuffersSources();
	std::vector <SoundBuffer> GetBuffers();
	std::vector <SoundSource> GetSources();
	//void FreeSource();
	//int GetNumBuffers();
	int GetNumSources();
	void GetAllWavFilesNamesWithinFolder();
	std::vector <SoundBuffer> buffers;
	std::vector <SoundSource> sources;
	std::vector <SoundInfo> soundInformation;
	SoundSource GetSource(int sourceID);
	SoundBuffer GetBuffer(int bufferID);
	SoundBuffer GetBuffer(std::string bufferFilename);
	ALenum getError();
	void SetSoundBufferIsFree(int bufferID, bool isFree);
	void SetSoundSourceIsFree(int sourceID, bool isFree);
	SoundSource GetFreeSource();
	int GenerateBuffers();
	ALuint GetActualBuffer(std::string filename, ALuint &buffer);
};