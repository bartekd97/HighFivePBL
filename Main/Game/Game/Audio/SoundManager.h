#include <AL/alut.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#include <string>
#include <Windows.h>

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

private:
	int numBuffers = 64;
	int numSources = 16;
	//ALuint buffers[NUM_BUFFERS];
	vector <ALuint> buffers;
	vector <ALuint> sources;
	vector <ALuint> freeBuffers;
	vector <ALuint> freeSources;
	std::string soundsFolderPath = "Data/Assets/Sounds";

public:
	void InitSoundManager();
	void ExitSoundManager();
	int GenerateBuffersSources();
	vector <ALuint> GetBuffers();
	vector <ALuint> GetSources();
	void FreeSource();
	int GetNumBuffers();
	int GetNumSources();
	vector<std::string> GetAllWavFilesNamesWithinFolder(std::string folder);
};