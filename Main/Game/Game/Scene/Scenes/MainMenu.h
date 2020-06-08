#pragma once

#include "../IScene.h"

class MainMenuScene : public IScene
{
public:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
};