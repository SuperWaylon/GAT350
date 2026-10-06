#include "pch.h"
#include "Json.h"
#include "Core/File.h"

namespace nu::json
{
    bool Load(const std::string& filename, document_t& document) 
    {
        // read the file into a string
        std::string buffer;
        if (!ReadTextFile(filename, buffer)) 
        {
            std::cerr << "Could not read file:" << filename << std::endl;
            return false;
        }

        // convert the string into a json stream
        std::stringstream stream(buffer);
        rapidjson::IStreamWrapper istream(stream);

        // set the json document from the stream
        document.ParseStream(istream);

        // check if the parse was successful
        if (document.HasParseError())
        {
            std::cerr << "Could not parse JSON: " << filename << std::endl;
            std::cerr << "Error: " << rapidjson::GetParseError_En(document.GetParseError()) << std::endl;

            return false;
        }

        // check that the root value is an object, not an array/string/number/etc.
        if (!document.IsObject())
        {
            std::cerr << "JSON root is not an object: " << filename << std::endl;
            return false;
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, int& data, bool required)
    {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsInt()) 
        {
            if (required)
                std::cerr << "Could not read JSON value (int): " << name << std::endl;
            return false;
        }

        // get the data
        data = value[name.c_str()].GetInt();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, unsigned int& data, bool required)
    {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsUint())
        {
            if (required)
                std::cerr << "Could not read JSON value (unsigned int): " << name << std::endl;
            return false;
        }

        // get the data
        data = value[name.c_str()].GetUint();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, bool& data, bool required)
    {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsBool()) 
        {
            if (required)
                std::cerr << "Could not read JSON value (bool): " << name << std::endl;
            return false;
        }

        // get the data
        data = value[name.c_str()].GetBool();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, float& data, bool required)
    {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsNumber())
        {
            if (required)
                std::cerr << "Could not read JSON value (float): " << name << std::endl;
            return false;
        }

        // get the data
        data = value[name.c_str()].GetFloat();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, std::string& data, bool required)
    {
        // check if the value has the "<name>" and the correct data type
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsString())
        {
            if (required)
                std::cerr << "Could not read JSON value (string): " << name << std::endl;
            return false;
        }

        // get the data
        data = value[name.c_str()].GetString();

        return true;
    }

    bool Read(const value_t& value, const std::string& name, Vector2& data, bool required)
    {
        // check if the value has the "<name>" and is an array with 2 elements
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsArray() || value[name.c_str()].Size() != 2) 
        {
            if (required)
                std::cerr << "Could not read JSON value (Vector2): " << name << std::endl;
            return false;
        }

        // get json array object
        auto& array = value[name.c_str()];
        // get array values, iterate through each element
        for (rapidjson::SizeType i = 0; i < array.Size(); i++) 
        {
            if (!array[i].IsNumber()) 
            {
                std::cerr << "Could not read JSON value (Vector2): " << name << std::endl;
                return false;
            }

            // get the data
            data[i] = array[i].GetFloat();
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, Vector3& data, bool required)
    {
        // check if the value has the "<name>" and is an array with 3 elements
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsArray() || value[name.c_str()].Size() != 3)
        {
            if (required)
                std::cerr << "Could not read JSON value (Vector3): " << name << std::endl;
            return false;
        }

        // get json array object
        auto& array = value[name.c_str()];
        // get array values, iterate through each element
        for (rapidjson::SizeType i = 0; i < array.Size(); i++)
        {
            if (!array[i].IsNumber())
            {
                std::cerr << "Could not read JSON value (Vector3): " << name << std::endl;
                return false;
            }

            // get the data
            data[i] = array[i].GetFloat();
        }

        return true;
    }

    bool Read(const value_t& value, const std::string& name, std::vector<int>& data, bool required)
    {
        // check if the value has the "<name>" and is an array with 3 elements
        if (!value.HasMember(name.c_str()) || !value[name.c_str()].IsArray())
        {
            if (required)
                std::cerr << "Could not read JSON value (std::vector<int>): " << name << std::endl;
            return false;
        }

        // get json array object
        auto& array = value[name.c_str()];
        // get array values, iterate through each element
        for (rapidjson::SizeType i = 0; i < array.Size(); i++)
        {
            if (!array[i].IsInt())
            {
                std::cerr << "Could not read JSON value (std::vector<int>): " << name << std::endl;
                return false;
            }

            // get the data
            data.push_back(array[i].GetInt());
        }

        return true;
    }
}