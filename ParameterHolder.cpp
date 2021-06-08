#include "ParameterHolder.h"
#include<stdlib.h>
#include<string.h>


ParameterHolder::ParameterHolder(int numOfParams)
{
	this->numOfParams = numOfParams;
	params = (PluginParameter**)malloc(sizeof(PluginParameter*) * numOfParams);
	for (int i = 0; i < numOfParams; i++)
	{
		params[i] = NULL;
	}
}


ParameterHolder::~ParameterHolder()
{
	for (int i = 0; i < numOfParams; i++)
	{
		if (params[i] != NULL) 
		{
			delete params[i];
		}
	}
	free(params);
}

PluginParameter *ParameterHolder::addParameter(PluginParameter *param)
{
	for (int i = 0; i < numOfParams; i++)
	{
		if (params[i] == NULL)
		{
			params[i] = param;
			return param;
		}
	}
	return NULL;
}


PluginParameter *ParameterHolder::getParameterById(int id)
{
	for (int i = 0; i < numOfParams; i++)
	{
		if (params[i] != NULL && params[i]->getId() == id)
		{
			return params[i];
		}
	}
	return NULL;
}

PluginParameter *ParameterHolder::getParameterByIndex(int index)
{
	if (index >= 0 && index < numOfParams)
	{
		return params[index];
	}
	return NULL;
}


int ParameterHolder::serialize(char **writeBuffer)
{
	char tempBuf[100];
	char *buf = (char*)malloc(1);
	int size = 0;
	for (int i = 0; i < numOfParams; i++)
	{
		PluginParameter *p = params[i];
		if (p != NULL)
		{
			int bytesWritten = p->serialize(tempBuf);
			int newSize = size + bytesWritten;
			buf = (char *)realloc(buf, newSize);
			memcpy(buf + size, tempBuf, bytesWritten);
			size = newSize;
		}
	}
	size += 3;
	buf = (char *)realloc(buf, size);
	buf[size - 3] = 'E'; 
	buf[size - 2] = 'n'; 
	buf[size - 1] = 'd'; 
	*writeBuffer = buf;
	return size;
}

void ParameterHolder::deserialize(char *readBuffer)
{
	PluginParameter *tempParam = new PluginParameter("", "", 0);
	char *buf = readBuffer;
	int readBytes;
	do {
		readBytes = tempParam->deserialize(buf);
		if (readBytes > 0)
		{
			PluginParameter *p = getParameterById(tempParam->getId());
			if (p != NULL)
			{
				p->setValue(tempParam->getValue());
			}
		}
		buf += readBytes;
	} while (readBytes);
	delete tempParam;
}