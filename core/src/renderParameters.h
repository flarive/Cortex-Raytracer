#pragma once

#include <iostream>
#include <sstream>

using namespace std;

class renderParameters
{
public:

	bool quietMode = false;

	double ratio = 16.0 / 9.0;
	unsigned int width = 256;
	unsigned int height = static_cast<unsigned int>(width / ratio);
	unsigned int samplePerPixel = 100;
	unsigned int recursionMaxDepth = 100;
	std::string sceneName;
	std::string saveFilePath;

	static renderParameters getArgs(int argc, char* argv[])
	{
		renderParameters params;

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
				else if (param == "width" && !value.empty())
				{
					params.width = stoul(value, 0, 10);
				}
				else if (param == "height" && !value.empty())
				{
					params.height = stoul(value, 0, 10);
				}
				else if (param == "ratio" && !value.empty())
				{
					double p1 = 0, p2 = 0;

					stringstream test(value);
					string segment;

					unsigned int loop = 0;
					while (getline(test, segment, ':'))
					{
						if (loop == 0)
						{
							p1 = stoul(segment, 0, 10);
						}
						else if (loop == 1)
						{
							p2 = stoul(segment, 0, 10);
						}

						loop++;
					}

					if (p1 > 0 && p2 > 0)
					{
						params.ratio = p1 / p2;
					}
				}
				else if (param == "spp" && !value.empty())
				{
					params.samplePerPixel = stoul(value, 0, 10);
				}
				else if (param == "maxdepth" && !value.empty())
				{
					params.recursionMaxDepth = stoul(value, 0, 10);
				}
				else if (param == "scene" && !value.empty())
				{
					params.sceneName = value;
				}
				else if (param == "save" && !value.empty())
				{
					params.saveFilePath = value;
				}
			}
		}

		return params;
	}
};