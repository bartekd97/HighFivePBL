#pragma once

class IScene
{
public:
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;
};