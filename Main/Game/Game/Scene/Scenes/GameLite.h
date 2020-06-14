#pragma once

#include "../IScene.h"

class GameLiteScene : public IScene
{
public:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
};