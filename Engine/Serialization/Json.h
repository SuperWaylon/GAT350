#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"

#include <rapidjson/document.h>
#include <string>
#include <vector>

#define JSON_READ(value, data) nu::json::Read(value, #data, data)
#define JSON_READ_NAME(value, name, data) nu::json::Read(value, name, data)

#define JSON_READ_REQ(value, data) nu::json::Read(value, #data, data, true)
#define JSON_READ_NAME_REQ(value, name, data) nu::json::Read(value, name, data, true)

#define JSON_HAS(value, data)   value.HasMember(#data)
#define JSON_HAS_NAME(value, name)   value.HasMember(name)

#define JSON_GET(value, data)	value[#data]
#define JSON_GET_NAME(value, name)	value[name]

namespace nu::json
{
	using value_t = rapidjson::Value;
	using document_t = rapidjson::Document;

	bool Load(const std::string& filename, document_t& document);

	// read json data
	bool Read(const value_t& value, const std::string& name, int& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, unsigned int& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, float& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, bool& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, std::string& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, struct Vector2& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, struct Vector3& data, bool required = false);
	bool Read(const value_t& value, const std::string& name, std::vector<int>& data, bool required = false);
}
