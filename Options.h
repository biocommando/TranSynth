#pragma once

#include <fstream>
#include <string>
#include <vector>

class Options
{
    std::vector<std::pair<std::string, std::string>> opts;

public:
    void read(const std::string &fname)
    {
        std::ifstream f;
        f.open(fname);
        std::string s;
        while (std::getline(f, s))
        {
            if (s.find_first_of('#') == 0)
                continue;

            auto pos = s.find_first_of('=');
            if (pos == std::string::npos)
                continue;
            opts.push_back(std::pair<std::string, std::string>(
                s.substr(0, pos), s.substr(pos + 1)));
        }
        f.close();
    }

    std::string getOption(const std::string &key, const std::string &defaultValue = "")
    {
        for (int i = opts.size() - 1; i >= 0; i--)
        {
            if (opts[i].first == key)
            {
                return opts[i].second;
            }
        }
        return defaultValue;
    }

    int getIntOption(const std::string &key, int defaultValue = 0)
    {
        const auto s = getOption(key);
        if (s == "")
            return defaultValue;
        return std::stoi(s);
    }

    float getFloatOption(const std::string &key, float defaultValue = 0)
    {
        const auto s = getOption(key);
        if (s == "")
            return defaultValue;
        return std::stof(s);
    }
};

Options *getOptions();