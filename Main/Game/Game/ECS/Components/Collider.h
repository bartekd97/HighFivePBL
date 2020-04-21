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
		CIRCLE,
		BOX
	};

	ColliderTypes type;
	ColliderShapes shape;
};
