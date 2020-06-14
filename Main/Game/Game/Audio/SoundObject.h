#include "SoundManager.h"
#include <Windows.h>
#include <vector>
#include <string>

class SoundObject {
	/*
	struct SoundInfo
	{
		int frequency;
		int channels;
		int bitrate;
		int bitsPerChannel;
	};
	*/
private:
	//int preloadBuffersCount = 3;
	//int remainBuffers = 0;

public:
	//bool PreloadBuffer(int bufferID);
	//void LoadAllSounds();
	int SetSource(int bufferID, int sourceID, ALfloat const* sourcePos, ALfloat const* sourceVel, ALfloat const* sourceDir, bool loop);
	int SetSource(std::string bufferFilename, int sourceID, ALfloat const* sourcePos, ALfloat const* sourceVel, ALfloat const* sourceDir, bool loop);
	void PlaySource(int sourceID);
	void StopSound(int sourceID);
	void PauseSound(int sourceID);
	int setListener();
	int CreateSource(ALuint& source, std::string bufferFilename, ALfloat const* sourcePos, ALfloat const* sourceVel, ALfloat const* sourceDir, bool loop);
	void PlaySource(ALuint source);
};