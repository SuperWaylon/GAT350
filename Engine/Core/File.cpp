#include "pch.h"
#include "File.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nu
{
	std::string GetWorkingDirectory()
	{
		std::error_code ec;
		auto path = std::filesystem::current_path(ec);
		// print error if error code is true
		if (ec)
			std::cerr << ec.message() << std::endl;

		// if error code then return empty string else return path
		return ec ? std::string{} : path.string();
	}

	bool SetWorkingDirectory(const std::string& path)
	{
		// remember where we were so a failure can say what it tried to resolve against
		std::string before = GetWorkingDirectory();

		std::error_code ec;
		std::filesystem::current_path(path, ec);
		// print error if error code is true
		if (ec)
		{
			std::cerr << "Could not set working directory to '" << path << "' (from '" << before << "'): "
				<< ec.message() << std::endl;
		}

		return !ec;
	}

	std::string GetFilename(const std::string& path)
	{
		// create path object from string, return filename
		return std::filesystem::path{ path }.filename().string();
	}

	std::string GetFileExtension(const std::string& path)
	{
		// create path object from string, return extension
		return std::filesystem::path{ path }.extension().string();
	}

	std::string GetFilenameNoExtension(const std::string& path)
	{
		// create path object from string, return filename no extension
		return std::filesystem::path{ path }.stem().string();
	}

	bool FileExists(const std::string& path)
	{
		std::error_code ec;
		bool result = std::filesystem::exists(path, ec);
		// print error if error code is true
		if (ec)
			std::cerr << ec.message() << std::endl;

		return !ec && result;
	}

	std::vector<std::string> GetFilesInDirectory(const std::string& path)
	{
		std::vector<std::string> files;
		std::error_code ec;

		// get entries in directory
		auto iter = std::filesystem::directory_iterator(path, ec);
		// return empty vector if error code is true
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			return files;
		}

		// iterate through all entries
		for (const auto& entry : iter)
		{
			// check if entry is file and no error code
			if (entry.is_regular_file(ec) && !ec)
			{
				// add path to files
				files.push_back(entry.path().string());
			}
		}

		return files;
	}

	std::vector<std::string> GetDirectoriesIn(const std::string& path)
	{
		std::vector<std::string> directories;
		std::error_code ec;

		// get entries in directory
		auto iter = std::filesystem::directory_iterator(path, ec);
		// return empty vector if error code is true
		if (ec)
		{
			std::cerr << ec.message() << std::endl;
			return directories;
		}

		// iterate through all entries
		for (const auto& entry : iter)
		{
			// check if entry is directories and no error code
			if (entry.is_directory(ec) && !ec)
			{
				directories.push_back(entry.path().string());
			}
		}

		return directories;
	}

	bool ReadTextFile(const std::string& path, std::string& data)
	{
		// create input file stream
		std::ifstream file(path);
		// return false if file is not open
		if (!file.is_open())
			return false;

		// use string stream to read file
		std::stringstream ss;
		ss << file.rdbuf();

		// convert stream string to string
		data = ss.str();

		return true;
	}

	std::vector<uint8_t> ReadBinaryFile(const std::string& path)
	{
		// open the file in binary mode (no newline conversion) with the read position at the end (ate = "at end")
		std::ifstream file(path, std::ios::binary | std::ios::ate);

		// if the file could not be opened, print where we looked and return an empty vector
		if (!file.is_open())
		{
			std::cerr << "Could not open file '" << path << "' (cwd: " << GetWorkingDirectory() << ")" << std::endl;
			return {};
		}

		// the read position is at the end, so tellg() gives the file size in bytes
		std::streamsize size = file.tellg();

		// tellg() returns -1 on failure, and an empty file has nothing to read
		if (size <= 0)
		{
			return {};
		}

		// move the read position back to the beginning so the read starts at the first byte
		file.seekg(0, std::ios::beg);

		// create a vector large enough to hold every byte in the file
		std::vector<uint8_t> bytes(static_cast<size_t>(size));

		// read all bytes into the vector
		// read() takes a char*, so reinterpret_cast the uint8_t* from bytes.data()
		if (!file.read(reinterpret_cast<char*>(bytes.data()), size))
		{
			// the read failed, so return an empty vector
			return {};
		}

		// return the bytes read from the file
		return bytes;
	}

	bool WriteTextFile(const std::string& path, const std::string& data, bool append)
	{
		// create output file stream
		std::ios::openmode mode = append ? std::ios::app : std::ios::out;
		std::ofstream file(path, mode);
		// return false if file is not open
		if (!file.is_open())
			return false;

		// stream in string to file
		file << data;

		return true;
	}
}