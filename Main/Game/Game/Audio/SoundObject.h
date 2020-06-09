#include "SoundManager.h"
#include <Windows.h>
#include <vector>
#include <string>

class SoundObject {

	struct SoundInfo
	{
		int frequency;
		int channels;
		int bitrate;
		int bitsPerChannel;
	};

private:
	int preloadBuffersCount = 3;
	int remainBuffers = 0;

public:
	bool PreloadBuffer(int bufferID);
	void LoadAllSounds();
	void LoadAndPlaySound(int bufferID);
	void Update();
	void Stop();

};