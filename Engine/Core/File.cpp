#include "pch.h"
#include "File.h"

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
		std::error_code ec;
		std::filesystem::current_path(path, ec);
		// print error if error code is true
		if (ec)
			std::cerr << ec.message() << std::endl;
		

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

	bool WriteTextFile(const std::string& path, const std::string& data, bool append)
	{
		// create input file stream
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
