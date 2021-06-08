#include "ParameterHolder.h"
#include <stdlib.h>
#include <string.h>

ParameterHolder::ParameterHolder()
{
}

ParameterHolder::~ParameterHolder()
{
}

void ParameterHolder::addParameter(PluginParameter param)
{
	params.push_back(param);
}

PluginParameter *ParameterHolder::getParameterById(int id)
{
	for (int i = params.size() - 1; i >= 0; i--)
	{
		if (params[i].getId() == id)
		{
			return &params[i];
		}
	}
	return nullptr;
}

PluginParameter *ParameterHolder::getParameterByIndex(int index)
{
	if (index >= 0 && index < params.size())
	{
		return &params[index];
	}
	return nullptr;
}

int ParameterHolder::serialize(char **writeBuffer)
{
	auto s = serializeToString();
	char *buf = (char *)malloc(s.size());
	memcpy(buf, s.c_str(), s.size());
	*writeBuffer = buf;
	return s.size();
}

std::string ParameterHolder::serializeToString()
{
	char tempBuf[100];
	std::string s;
	for (int i = 0; i < params.size(); i++)
	{
		PluginParameter *p = &params[i];
		s.append(p->serializeToString());
		/*int bytesWritten = p->serialize(tempBuf);
			s.append(tempBuf, bytesWritten);*/
	}
	s.append("End");
	return s;
}

void ParameterHolder::deserialize(const char *readBuffer)
{
	// Set initial value to parameters that were not read from the buffer
	for (int i = 0; i < params.size(); i++)
	{
		params[i].setInitialValue();
	}
	int idx = 0;
	int readBytes;
	do
	{
		int id;
		float value;
		readBytes = deserializeParameter(&readBuffer[idx], &id, &value);
		if (readBytes > 0)
		{
			PluginParameter *p = getParameterById(id);
			if (p != nullptr)
			{
				p->setValue(value);
			}
		}
		idx += readBytes;
	} while (readBytes);
}