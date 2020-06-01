#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

class AudioController 
{
public:
	void init_al();
	void exit_al();
	int generateBuffers();
	int loadSound();
	int setSources();
	int setListener(ALfloat const* listenerPos, ALfloat const* listenerVel, ALfloat const* listenerOri);

private:
	ALenum error;
	int NUM_BUFFERS = 10; //monkaW
	ALuint buffers[10];   // randomowa liczba zostaje tylko na razie monkaS
	ALenum format;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;
	ALvoid* data;

};


