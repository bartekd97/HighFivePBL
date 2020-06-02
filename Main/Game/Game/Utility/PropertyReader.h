#pragma once

#include <string>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <glm/glm.hpp>
#include <tinyxml2.h>
#include "Logger.h"

class PropertyReader
{
private:
	std::unordered_map<std::string, std::string> raw;
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, int> ints;
	std::unordered_map<std::string, bool> bools;
	std::unordered_map<std::string, glm::vec2> vecs2;
	std::unordered_map<std::string, glm::vec3> vecs3;
	std::unordered_map<std::string, glm::vec4> vecs4;

public:
	PropertyReader() {}
	PropertyReader(std::unordered_map<std::string, std::string>& data) : raw(data) {}
	PropertyReader(tinyxml2::XMLElement* xmlPropertyGroup)
	{
		int j = 0;
		for (tinyxml2::XMLElement* property = xmlPropertyGroup->FirstChildElement("property");
			property != nullptr;
			property = property->NextSiblingElement("property"), j++)
		{
			const char* propertyName = property->Attribute("name");
			const char* propertyValue = property->Attribute("value");

			if (propertyName == nullptr || propertyValue == nullptr)
			{
				LogWarning("PropertyReader::PropertyReader(): Invalid property node at #{}", j);
				continue;
			}

			raw[propertyName] = propertyValue;
		}
	}

	inline bool GetString(std::string&& name, std::string& target, std::string defaultValue = "")
	{
		static std::unordered_map<std::string, std::string>::iterator __strval;
		if ((__strval = raw.find(name)) != raw.end()) {
			target = __strval->second; return true;
		}
		else { target = defaultValue; return false; }
	}

	inline bool GetFloat(std::string&& name, float& target, float defaultValue = 0.0f)
	{
		static std::unordered_map<std::string, float>::iterator __floatval;
		static std::unordered_map<std::string, std::string>::iterator __floatstrval;

		if ((__floatval = floats.find(name)) != floats.end()) {
			target = __floatval->second; return true;
		}
		else {
			if ((__floatstrval = raw.find(name)) != raw.end()) {
				static float __floattmp;
				try { __floattmp = std::stof(__floatstrval->second.c_str(), NULL); }
				catch (std::invalid_argument ex) {
					floats[name] = defaultValue;
					LogWarning("PropertyReader::GetFloat(): Cannot parse '{}': {}. Using: {}", name, __floatstrval->second, defaultValue);
					target = defaultValue;
					return false;
				}
				floats[name] = __floattmp;
				target = __floattmp;
				return true;
			}
			else {
				//floats[name] = defaultValue;
				target = defaultValue;
				return false;
			}
		}
	}
	inline bool GetInt(std::string&& name, int& target, int defaultValue = 0)
	{
		static std::unordered_map<std::string, int>::iterator __intval;
		static std::unordered_map<std::string, std::string>::iterator __intstrval;

		if ((__intval = ints.find(name)) != ints.end()) {
			target = __intval->second; return true;
		}
		else {
			if ((__intstrval = raw.find(name)) != raw.end()) {
				static int __inttmp;
				try { __inttmp = std::stoi(__intstrval->second.c_str(), NULL); }
				catch (std::invalid_argument ex) {
					ints[name] = defaultValue;
					LogWarning("PropertyReader::GetInt(): Cannot parse '{}': {}. Using: {}", name, __intstrval->second, defaultValue);
					target = defaultValue;
					return false;
				}
				ints[name] = __inttmp;
				target = __inttmp;
				return true;
			}
			else {
				//ints[name] = defaultValue;
				target = defaultValue;
				return false;
			}
		}
	}
	inline bool GetBool(std::string&& name, bool& target, bool defaultValue = false)
	{
		static std::unordered_map<std::string, bool>::iterator __boolval;
		static std::unordered_map<std::string, std::string>::iterator __boolstrval;
		//std::transform(name.begin(), name.end(), name.begin(), std::tolower);

		if ((__boolval = bools.find(name)) != bools.end()) {
			target = __boolval->second; return true;
		}
		else {
			if ((__boolstrval = raw.find(name)) != raw.end()) {
				static bool __booltmp;
				__booltmp = __boolstrval->second == "true";
				bools[name] = __booltmp;
				target = __booltmp;
				return true;
			}
			else {
				//ints[name] = defaultValue;
				target = defaultValue;
				return false;
			}
		}
	}
	inline bool GetVec2(std::string&& name, glm::vec2& target, glm::vec2 defaultValue = { 0.0f,0.0f })
	{
		static std::unordered_map<std::string, glm::vec2>::iterator __vec2val;
		static std::unordered_map<std::string, std::string>::iterator __vec2strval;

		if ((__vec2val = vecs2.find(name)) != vecs2.end()) {
			target = __vec2val->second; return true;
		}
		else {
			if ((__vec2strval = raw.find(name)) != raw.end()) {
				static glm::vec3 __vec3tmp;
				if (!Utility::TryConvertStringToVec2(__vec2strval->second, target)) {
					vecs2[name] = defaultValue;
					LogWarning("PropertyReader::GetVec2(): Cannot parse '{}': {}. Using: {}", name, __vec2strval->second, defaultValue);
					target = defaultValue;
					return false;
				}
				else {
					vecs2[name] = target;
					return true;
				}
			}
			else {
				target = defaultValue;
				return false;
			}
		}
	}
	inline bool GetVec3(std::string&& name, glm::vec3& target, glm::vec3 defaultValue = {0.0f,0.0f,0.0f})
	{
		static std::unordered_map<std::string, glm::vec3>::iterator __vec3val;
		static std::unordered_map<std::string, std::string>::iterator __vec3strval;

		if ((__vec3val = vecs3.find(name)) != vecs3.end()) {
			target = __vec3val->second; return true;
		}
		else {
			if ((__vec3strval = raw.find(name)) != raw.end()) {
				static glm::vec3 __vec3tmp;
				if (!Utility::TryConvertStringToVec3(__vec3strval->second, target)) {
					vecs3[name] = defaultValue;
					LogWarning("PropertyReader::GetVec3(): Cannot parse '{}': {}. Using: {}", name, __vec3strval->second, defaultValue);
					target = defaultValue;
					return false;
				}
				else {
					vecs3[name] = target;
					return true;
				}
			}
			else {
				target = defaultValue;
				return false;
			}
		}
	}
	inline bool GetVec4(std::string&& name, glm::vec4& target, glm::vec4 defaultValue = { 0.0f,0.0f,0.0f,0.0f })
	{
		static std::unordered_map<std::string, glm::vec4>::iterator __vec4val;
		static std::unordered_map<std::string, std::string>::iterator __vec4strval;

		if ((__vec4val = vecs4.find(name)) != vecs4.end()) {
			target = __vec4val->second; return true;
		}
		else {
			if ((__vec4strval = raw.find(name)) != raw.end()) {
				static glm::vec3 __vec3tmp;
				if (!Utility::TryConvertStringToVec4(__vec4strval->second, target)) {
					vecs4[name] = defaultValue;
					LogWarning("PropertyReader::GetVec4(): Cannot parse '{}': {}. Using: {}", name, __vec4strval->second, defaultValue);
					target = defaultValue;
					return false;
				}
				else {
					vecs4[name] = target;
					return true;
				}
			}
			else {
				target = defaultValue;
				return false;
			}
		}
	}

	inline std::unordered_map<std::string, std::string> GetRawCopy() const { return raw; }
};

