#pragma once

#include <string>
#include <memory>
#include <vector>
#include <span>
#include <glm/glm.hpp>
#include <glad/glad.h>

class SkinningData
{
public:
	struct Node
	{
		std::string name;
		glm::mat4 nodeTransform;
		int boneIndex;
		int childCount;
		int childIndices[16];

		Node() : boneIndex(-1), childCount(0) {}
	};


public:
	const glm::mat4 InverseRootTransform;
private:
	std::vector<Node> nodes;
	std::vector<glm::mat4> boneOffsets;

	SkinningData(std::vector<Node>& nodes, std::vector<glm::mat4>& boneOffsets, glm::mat4& inverseRootTransform)
		: nodes(nodes), boneOffsets(boneOffsets), InverseRootTransform(inverseRootTransform){}

public:
	inline static std::shared_ptr<SkinningData> Create(std::vector<Node>& nodes, std::vector<glm::mat4>& boneOffsets, glm::mat4& inverseRootTransform) {
		return std::shared_ptr<SkinningData>(new SkinningData(nodes, boneOffsets, inverseRootTransform));
	}

	inline const std::span<SkinningData::Node> GetNodes() { return std::span<SkinningData::Node>(nodes); }
	inline const std::span<glm::mat4> GetBoneOffsets() { return std::span<glm::mat4>(boneOffsets); }
	inline const int NumBones() const { return boneOffsets.size(); }
};

