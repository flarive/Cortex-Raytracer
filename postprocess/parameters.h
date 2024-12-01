#pragma once

#include "effects.h"
#include "effects/effect.h"


#include <sstream>
#include <filesystem>

using namespace std;

class parameters
{
public:

	bool quietMode = false;

	string inputpath;
	string outputpath;

	short fx_index;
	pmap fx_args;

	static parameters getArgs(int argc, char* argv[])
	{
		parameters params;

		int loop = 2;
		for (int count = 0; count < argc; count++)
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
					params.fx_index = stoi(value, 0, 10);

					if (params.fx_index > 0)
						getEffectArgs(argc, argv, loop, params);
				}
			}

			loop++;
		}

		return params;
	}

	static void getEffectArgs(int argc, char* argv[], int loop, parameters& params)
	{
		for (int scount = loop; scount < argc; scount++)
		{
			string arg = argv[scount];

			if (arg.starts_with("-"))
			{
				string param = arg.substr(1);
				string value = argv[scount + 1];

				params.fx_args.emplace(param, stof(value));
			}
		}
	}
};