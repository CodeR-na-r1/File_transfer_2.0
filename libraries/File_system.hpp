#pragma once

#include <filesystem>
#include <vector>
#include <string>

class File_system
{
public:
	static void files_in_dir(const std::string directory, std::vector<std::string>& files, std::vector<std::string>& directories, const std::string __relative = "")
	{
		for (const auto& it : std::filesystem::directory_iterator(directory + std::filesystem::path("/").generic_string() + __relative))
		{
			if (it.is_regular_file())
				files.push_back(__relative + it.path().filename().generic_string());
			else
			{
				directories.push_back(__relative + (it.path().filename() / "").generic_string());
				files_in_dir(directory, files, directories, __relative + (it.path().filename() / "").generic_string());
			}
		}

		return;
	}

	static void create_dirs(const std::string& path_where, const std::vector<std::string>& dirs)
	{
		std::filesystem::path _path = path_where;

		if (path_where == "") { _path = File_system::get_work_dir(); }

		_path += std::filesystem::path::preferred_separator;
		std::string path = _path.generic_string();

		for (std::string dir : dirs)
		{
			std::filesystem::create_directory(path + dir);
		}

		return;
	}

	static void create_dir(const std::string& path_where, const std::string& dir)
	{
		std::filesystem::path _path = path_where;

		if (path_where == "") { _path = File_system::get_work_dir(); }

		_path += std::filesystem::path::preferred_separator;
		std::string path = _path.generic_string();

		std::filesystem::create_directory(path + dir);

		return;
	}

	static bool is_regular_file(const std::string& name)
	{
		std::filesystem::path _path(name);

		return _path.has_filename();
	}

	static std::string get_filename(const std::string& name)
	{
		std::filesystem::path _path(name);

		return _path.filename().generic_string();
	}

	static bool exists(const std::string& name)
	{
		return std::filesystem::exists(name);
	}

	static std::string get_work_dir()
	{
		return std::filesystem::current_path().generic_string();
	}

private:
	File_system() = delete;
	~File_system() = delete;

};