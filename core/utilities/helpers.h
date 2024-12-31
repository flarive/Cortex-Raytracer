#pragma once

#include <iostream>
#include <sstream>
#include <filesystem>

class helpers
{
public:
    static double getRatio(const char* value)
    {
        double p1 = 0, p2 = 0;

        std::stringstream test(value);
        std::string segment;

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
            return p1 / p2;
        }

        return 0.0;
    }
};

struct case_insensitive_traits : public std::char_traits<char>
{
    static bool eq(char c1, char c2) {
        return std::toupper(c1) == std::toupper(c2);
    }
    static bool lt(char c1, char c2) {
        return std::toupper(c1) < std::toupper(c2);
    }
    static int compare(const char* s1, const char* s2, std::size_t n) {
        while (n-- != 0) {
            if (lt(*s1, *s2)) return -1;
            if (lt(*s2, *s1)) return 1;
            ++s1; ++s2;
        }
        return 0;
    }
};