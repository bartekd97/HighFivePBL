#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vector>

class AudioController 
{
public:
	void init_al();
	void exit_al();
	int generateBuffers();
	int loadSound();
	ALuint setSource(ALfloat const* sourcePos, ALfloat const* sourceVel, ALfloat const* sourceDir);
	int setListener();
	int playSound(ALuint source, ALint source_state);

private:
	ALenum error;
	int NUM_BUFFERS = 10; //monkaW
	ALuint buffers[10];   // randomowa liczba zostaje tylko na razie monkaS
	ALenum format = AL_FORMAT_MONO16;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;
	ALvoid* data;

};


