#include "TextureTools.h"
#include "Resourcing/Texture.h"

static const int GRADIENT_LENGTH = 128;

template<typename T>
static T Interpolate(std::vector<T>& gradient, float level)
{
	int size = gradient.size();
	if (size == 1) return gradient[0];

	float partlen = 1.0f / (float)(size-1);
	float progress = level / partlen;
	int from = (int)progress;
	int to = from + 1;
	if (to == size)
	{
		from--;
		to--;
	}
	float t = glm::smoothstep(float(from), float(to), progress);
	
	return glm::mix(gradient[from], gradient[to], t);
}
static inline unsigned char FloatToByte(float val)
{
	return static_cast<unsigned char>(val * 254.999999f);
}

std::shared_ptr<Texture> TextureTools::GenerateGradientTexture(std::vector<float> floats)
{
	assert(floats.size() > 0 && "Empty graident");
	
	static unsigned char gradient[GRADIENT_LENGTH];
	float t;
	for (int i = 0; i < GRADIENT_LENGTH; i++)
	{
		t = float(i) / float(GRADIENT_LENGTH - 1);
		gradient[i] = FloatToByte(Interpolate(floats, t));
	}
	TextureConfig conf;
	conf.format = GL_RED;
	return TextureManager::CreateTextureFromRawData(gradient, GRADIENT_LENGTH, 1, GL_RED, conf);
}

std::shared_ptr<Texture> TextureTools::GenerateGradientTexture(std::vector<glm::vec3> vec3s)
{
	assert(vec3s.size() > 0 && "Empty graident");

	static unsigned char gradient[GRADIENT_LENGTH * 3];
	float t;
	glm::vec3 v3;
	for (int i = 0; i < GRADIENT_LENGTH * 3; i += 3)
	{
		t = float(i / 3) / float(GRADIENT_LENGTH - 1);
		v3 = Interpolate(vec3s, t);
		gradient[i] = FloatToByte(v3.r);
		gradient[i + 1] = FloatToByte(v3.g);
		gradient[i + 2] = FloatToByte(v3.b);
	}
	TextureConfig conf;
	conf.format = GL_RGB;
	return TextureManager::CreateTextureFromRawData(gradient, GRADIENT_LENGTH, 1, GL_RGB, conf);
}
