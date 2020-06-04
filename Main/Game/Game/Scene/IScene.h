#pragma once

class IScene
{
public:
	virtual void Load() = 0;
	virtual void Unload() = 0;
};