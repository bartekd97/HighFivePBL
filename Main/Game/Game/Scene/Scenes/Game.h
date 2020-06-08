#pragma once

#include "../IScene.h"

class GameScene : public IScene
{
public:
	virtual void OnLoad() override;
	virtual void OnUnload() override;
};