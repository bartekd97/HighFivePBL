#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../Utility/Logger.h"
#include "MeshFileLoader.h"
#include "SkinningData.h"

// For converting between ASSIMP and glm
static inline glm::vec3 vec3_cast(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D& v) { return glm::vec2(v.x, v.y); }
static inline glm::quat quat_cast(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 mat4_cast(const aiMatrix3x3& m) { return glm::transpose(glm::make_mat3(&m.a1)); }

namespace {
	/*
	aiNode* FindNodeForMesh(aiNode* rootNode, unsigned int meshIndex)
	{
		if (rootNode->mNumMeshes > 0)
		{
			for (int i = 0; i < rootNode->mNumMeshes; i++)
				if (rootNode->mMeshes[i] == meshIndex)
					return rootNode;
		}

		aiNode* node;
		for (int i = 0; i < rootNode->mNumChildren; i++)
			if ((node = FindNodeForMesh(rootNode->mChildren[i], meshIndex)) != NULL)
				return node;
		return NULL;
	}
	*/

	void ProcessNodeForSkinning(
		std::vector<SkinningData::Node>& nodes,
		std::vector<glm::mat4>& boneOffsets,
		std::vector<aiNode*>& indices,
		aiNode* node)
	{
		auto getNodeIndex = [&indices](aiNode* node)
		{
			for (int i = 0; i < indices.size(); i++)
				if (indices[i] == node)
					return i;

			assert(1 != 1 && "THIS SHOULD NEVER BE REACHED");
		};

		if (node->mParent != NULL) {
			int parentIndex = getNodeIndex(node->mParent);
			int cCount = nodes[parentIndex].childCount;
			nodes[parentIndex].childIndices[cCount] = nodes.size();
			nodes[parentIndex].childCount++;
		}
		SkinningData::Node n;
		n.name = std::string(node->mName.C_Str());
		n.nodeTransform = mat4_cast(node->mTransformation);

		nodes.push_back(n);
		indices.push_back(node);
		for (int i = 0; i < node->mNumChildren; i++)
			ProcessNodeForSkinning(nodes, boneOffsets, indices, node->mChildren[i]);
	}
}

MeshFileLoader::MeshFileLoader(std::string filepath)
{
	const unsigned int aiFlags =
		aiProcess_Triangulate |
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		//aiProcess_PreTransformVertices |
		//aiProcess_RemoveRedundantMaterials |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_ImproveCacheLocality |
		aiProcess_LimitBoneWeights | // for max 4 bones per vertex
		aiProcess_GenBoundingBoxes; // for later usage (AABB bounding box)

	if (importer.ReadFile(filepath, aiFlags) == NULL)
	{
		LogError("MeshFileLoader::MeshFileLoader(): Cannot read '{}'. Reason: {}", filepath, importer.GetErrorString());
	}

	this->filepath = filepath;
}

bool MeshFileLoader::ReadMeshData(std::vector<Vertex>& vertices, std::vector<unsigned>& indices, AABBStruct& AABB)
{
	const aiScene* scene = importer.GetScene();
	if (scene == NULL)
		return false;

	if (scene->mNumMeshes > 1)
	{
		// TODO: improve mesh loader
		LogWarning("MeshFileLoader::ReadMeshData(): More than 1 mesh in '{}', please take care of it", filepath);
	}
	aiMesh* mesh = scene->mMeshes[0];
	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces * 3);

	/*
	aiNode* meshNodeTmp = FindNodeForMesh(scene->mRootNode, 0);

	aiMatrix4x4 meshTrans = meshNodeTmp->mTransformation;
	while (meshNodeTmp->mParent != NULL)
	{
		meshNodeTmp = meshNodeTmp->mParent;
		meshTrans = meshNodeTmp->mTransformation * meshTrans;
	}
	*/

	Vertex v;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		v.position = vec3_cast(mesh->mVertices[i]);
		v.uv = vec2_cast(mesh->mTextureCoords[0][i]);
		v.normal = vec3_cast(mesh->mNormals[i]);
		v.tangent = vec3_cast(mesh->mTangents[i]);
		v.bitangent = vec3_cast(mesh->mBitangents[i]);
		vertices.push_back(v);
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	AABB.min = vec3_cast(mesh->mAABB.mMin);
	AABB.max = vec3_cast(mesh->mAABB.mMax);

	return true;
}

bool MeshFileLoader::ReadBoneData(std::vector<VertexBoneData>& data, std::shared_ptr<SkinningData>& skinning)
{
	const aiScene* scene = importer.GetScene();
	if (scene == NULL)
		return false;

	if (scene->mNumMeshes > 1)
	{
		// TODO: improve mesh loader
		LogWarning("MeshFileLoader::ReadBoneData(): More than 1 mesh in '{}', please take care of it", filepath);
	}
	aiMesh* mesh = scene->mMeshes[0];

	// read data

	data.resize(mesh->mNumVertices);
	auto addWeight = [](VertexBoneData& vertex, int bone, float weight)
	{
		for (int i = 0; i < 4; i++)
		{
			if (vertex.weights[i] == 0.0f)
			{
				vertex.bones[i] = bone;
				vertex.weights[i] = weight;
				return;
			}
		}
		assert(1 == 1 && "THIS SHOULD NEVER BE REACHED");
	};

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		aiBone* bone = mesh->mBones[boneIndex];
		for (int i = 0; i < bone->mNumWeights; i++)
		{
			addWeight(data.at(bone->mWeights[i].mVertexId), boneIndex, bone->mWeights[i].mWeight);
		}
	}

	// read skinning

	std::vector<SkinningData::Node> nodes;
	std::vector<glm::mat4> boneOffsets;
	boneOffsets.resize(mesh->mNumBones, glm::mat4(1.0f));

	std::vector<aiNode*> indices; // tmp
	ProcessNodeForSkinning(nodes, boneOffsets, indices, scene->mRootNode);
	for (int bi = 0; bi < mesh->mNumBones; bi++)
	{
		for (auto& node : nodes)
		{
			if (node.name == mesh->mBones[bi]->mName.C_Str())
			{
				node.boneIndex = bi;
				boneOffsets[bi] = mat4_cast(mesh->mBones[bi]->mOffsetMatrix);
				break;
			}
		}
	}


	glm::mat inverseRootTransform = glm::inverse(mat4_cast(scene->mRootNode->mTransformation));
	glm::mat inverseModelTransform = glm::inverse(mat4_cast(
		scene->mRootNode->FindNode(mesh->mName)->mTransformation
		));
	skinning = SkinningData::Create(nodes, boneOffsets, inverseRootTransform, inverseModelTransform);

	return true;
}


bool MeshFileLoader::ReadAnimation(std::shared_ptr<AnimationClip>& animation)
{
	const aiScene* scene = importer.GetScene();
	if (scene == NULL)
		return false;

	if (scene->mNumAnimations > 1)
	{
		// TODO: improve mesh loader
		LogWarning("MeshFileLoader::ReadAnimation(): More than 1 animation in '{}', please take care of it", filepath);
	}
	aiAnimation* anim = scene->mAnimations[0];

	float ticks = anim->mTicksPerSecond == 0.0f ? 25.0f : anim->mTicksPerSecond;
	animation = AnimationClip::Create(1000.0f / ticks, anim->mDuration);

	std::vector<AnimationClip::Channel::KeyVec3> positions;
	std::vector<AnimationClip::Channel::KeyQuat> rotations;
	std::vector<AnimationClip::Channel::KeyVec3> scales;
	positions.reserve(AnimationClip::Channel::MAX_KEYS);
	rotations.reserve(AnimationClip::Channel::MAX_KEYS);
	scales.reserve(AnimationClip::Channel::MAX_KEYS);

	for (int i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* channel = anim->mChannels[i];

		positions.clear();
		rotations.clear();
		scales.clear();

		int k;
		for (k = 0; k < channel->mNumPositionKeys; k++)
			positions.push_back({ (float)channel->mPositionKeys[k].mTime, vec3_cast(channel->mPositionKeys[k].mValue) });
		for (k = 0; k < channel->mNumRotationKeys; k++)
			rotations.push_back({ (float)channel->mRotationKeys[k].mTime, quat_cast(channel->mRotationKeys[k].mValue) });
		for (k = 0; k < channel->mNumScalingKeys; k++)
			scales.push_back({ (float)channel->mScalingKeys[k].mTime, vec3_cast(channel->mScalingKeys[k].mValue) });
	
		auto aChannel = AnimationClip::Channel::Create(positions, rotations, scales);
		animation->AddChannel(std::string(channel->mNodeName.C_Str()), aChannel);
	}

	return true;
}