#pragma once

#include <iostream>
#include <sstream>
#include <filesystem>

using namespace std;

class parameters
{
public:

	bool quietMode = false;

	std::string inputpath;
	std::string outputpath;
	bool hdr = false;


	static parameters getArgs(int argc, char* argv[])
	{
		parameters params;

		int count;
		for (count = 0; count < argc; count++)
		{
			string arg = argv[count];

			if (arg.starts_with("-"))
			{
				string param = arg.substr(1);
				string value = argv[count + 1];

				if (param == "quiet")
				{
					params.quietMode = true;
				}
				else if (param == "input")
				{
					std::filesystem::path dir(std::filesystem::current_path());
					std::filesystem::path file(value.c_str());
					std::filesystem::path fullexternalProgramPath = dir / file;

					auto fullAbsPath = std::filesystem::absolute(fullexternalProgramPath);
					params.inputpath = fullAbsPath.string();
				}
				else if (param == "output")
				{
					std::filesystem::path dir(std::filesystem::current_path());
					std::filesystem::path file(value.c_str());
					std::filesystem::path fullexternalProgramPath = dir / file;

					auto fullAbsPath = std::filesystem::absolute(fullexternalProgramPath);
					params.outputpath = fullAbsPath.string();
				}
				else if (param == "hdr")
				{
					params.hdr = stoul(value, 0, 10);
				}
			}
		}

		return params;
	}
};