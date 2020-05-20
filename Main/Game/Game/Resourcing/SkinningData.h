#pragma once

#include <string>
#include <memory>
#include <vector>
#include <span>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <tsl/robin_map.h>

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
	const glm::mat4 InverseModelTransform;
private:
	std::vector<Node> nodes;
	std::vector<glm::mat4> boneOffsets;
	std::vector<glm::mat4> inversedBoneOffsets;
	tsl::robin_map<std::string, int> boneIndices;

	SkinningData(
		std::vector<Node>& nodes, std::vector<glm::mat4>& boneOffsets,
		glm::mat4& inverseRootTransform, glm::mat4& inverseModelTransform)
		: nodes(nodes), boneOffsets(boneOffsets),
		InverseRootTransform(inverseRootTransform), InverseModelTransform(inverseModelTransform)
	{
		inversedBoneOffsets.reserve(boneOffsets.size());
		for (auto& offset : boneOffsets)
			inversedBoneOffsets.emplace_back(glm::inverse(offset));

		for (int i = 0; i < this->nodes.size(); i++)
		{
			std::string name = this->nodes[i].name;
			int index = this->nodes[i].boneIndex;
			boneIndices[name] = index;
		}
	}

public:
	inline static std::shared_ptr<SkinningData> Create(
		std::vector<Node>& nodes, std::vector<glm::mat4>& boneOffsets,
		glm::mat4& inverseRootTransform, glm::mat4& inverseModelTransform)
	{
		return std::shared_ptr<SkinningData>(new SkinningData(nodes, boneOffsets, inverseRootTransform, inverseModelTransform));
	}

	inline const std::span<SkinningData::Node> GetNodes() { return std::span<SkinningData::Node>(nodes); }
	inline const int GetBoneIndex(const std::string& name) {
		auto it = boneIndices.find(name);
		return (it == boneIndices.end()) ? -1 : it->second;
	}
	inline const std::span<glm::mat4> GetBoneOffsets() { return std::span<glm::mat4>(boneOffsets); }
	inline const std::span<glm::mat4> GetInversedBoneOffsets() { return std::span<glm::mat4>(inversedBoneOffsets); }
	inline const int NumBones() const { return boneOffsets.size(); }
};

