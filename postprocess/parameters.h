#pragma once

#include <sstream>
#include <filesystem>

using namespace std;


enum effects { none, bloom, glow };

class parameters
{
public:

	bool quietMode = false;

	std::string inputpath;
	std::string outputpath;

	effects effect;

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
				else if (param == "effect")
				{
					unsigned long fx = stoul(value, 0, 10);
					if (fx == 1)
						params.effect = effects::bloom;
					else if (fx == 2)
						params.effect = effects::glow;
					else
						params.effect = effects::none;
				}
			}
		}

		return params;
	}
};