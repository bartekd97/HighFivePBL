#pragma once

struct BoxCollider
{
	float width;
	float height;

	glm::vec3 bPoints[4];

	void SetWidth(float width)
	{
		this->width = width;
		RecalculateBPoints();
	}

	void SetHeight(float height)
	{
		this->height = height;
		RecalculateBPoints();
	}

	void SetWidthHeight(float width, float height)
	{
		this->width = width;
		this->height = height;
		RecalculateBPoints();
	}

	void RecalculateBPoints()
	{
		bPoints[0] = { -width / 2.0f, 0.0f, -height / 2.0f };
		bPoints[1] = { width / 2.0f, 0.0f, -height / 2.0f };
		bPoints[2] = { width / 2.0f, 0.0f, height / 2.0f };
		bPoints[3] = { -width / 2.0f, 0.0f, height / 2.0f };
	}
};
