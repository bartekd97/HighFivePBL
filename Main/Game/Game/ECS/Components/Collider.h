#pragma once

struct Collider
{
	enum ColliderTypes
	{
		DYNAMIC,
		STATIC
	};

	enum ColliderShapes
	{
		CIRCLE
	};

	ColliderTypes type;
	ColliderShapes shape;
};
