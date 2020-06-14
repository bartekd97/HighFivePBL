#include <AL/alut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <Windows.h>
#include <locale>
#include <codecvt>

class SoundManager {

//#define NUM_BUFFERS = 64;
//#define NUM_SOURCES = 16;

//struct SoundBuffer {};
//struct SoundSource {};

private:
	int numBuffers = 64;
	int numSources = 16;
	//ALuint buffers[NUM_BUFFERS];
	//std::vector <SoundBuffer> buffers;
	//std::vector <SoundSource> sources;
	//vector <ALuint> freeBuffers;
	//vector <ALuint> freeSources;
	std::string soundsFolderPath = "Data/Assets/Sounds";
	ALenum error;

public:

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

	void InitSoundManager();
	void ExitSoundManager();
	int GenerateBuffersSources();
	std::vector <SoundBuffer> GetBuffers();
	std::vector <SoundSource> GetSources();
	//void FreeSource();
	int GetNumBuffers();
	int GetNumSources();
	std::vector<std::string> GetAllWavFilesNamesWithinFolder(std::string folder);
	std::vector <SoundBuffer> buffers;
	std::vector <SoundSource> sources;
	SoundSource GetSource(int sourceID);
	SoundBuffer GetBuffer(int bufferID);
	SoundBuffer GetBuffer(std::string bufferFilename);
	ALenum getError();
	void SetSoundBufferIsFree(int bufferID, bool isFree);
	void SetSoundSourceIsFree(int sourceID, bool isFree);
	SoundSource GetFreeSource();
};