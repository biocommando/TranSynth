#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include "PluginParameter.h"

void log(PluginParameter *p)
{
	FILE *f = fopen("C:\\Users\\Käyttäjä\\Documents\\Visual Studio 2013\\Projects\\TransitionSynth\\Release\\log.txt", "a");
	char n[100], ns[8];
	p->getName(n, 100);
	p->getShortName(ns);
	fprintf(f, "Parameter [id=%d]: name=%s (shortname=%s) = %f\n", p->getId(), n, ns, p->getValue());
	fclose(f);
}

PluginParameter::PluginParameter(const char *name, const char *shortName, int id, float defaultValue)
{
	strncpy(this->name, name, 64);
	this->name[63] = 0;
	strncpy(this->shortName, shortName, 8);
	this->shortName[7] = 0;
	this->callbackUpdatable = NULL;
	this->id = id;
	value = defaultValue;
}


PluginParameter::~PluginParameter()
{
}

void PluginParameter::setOnChanged(CallbackUpdatable *callbackUpdatable)
{
	this->callbackUpdatable = callbackUpdatable;
	callbackUpdatable->onUpdateWithValue(value);
}


void PluginParameter::setValue(float value)
{
	if (fabs(value - this->value) > 1e-6)
	{
		this->value = value;
		if (callbackUpdatable != NULL)
		{
			this->callbackUpdatable->onUpdateWithValue(value);
		}
	}
}

float PluginParameter::getValue()
{
	return value;
}

int PluginParameter::getId()
{
	return id;
}

void PluginParameter::getName(char *name, int maxLen)
{
	strncpy(name, this->name, maxLen);
	name[maxLen - 1] = 0;
}

void PluginParameter::getShortName(char *name)
{
	strcpy(name, shortName);
}

/*
 * The serialized format is:
 * content			size		description
 * --- header ---
 * 0xDADA			int			magic number
 * payload size		int			size of serialized data excluding the magic number and payload size
 * --- payload ---
 * id				int			the id of the parameter
 * value			float		the value of the parameter
 */
int PluginParameter::serialize(char *writeBuffer)
{
	const int magic = 0xDADA;
	const int magicSize = sizeof(int);
	const int payloadSizeInfoSize = sizeof(int);
	const int idSize = sizeof(int);
	const int valueSize = sizeof(float);
	const int payloadSize = idSize + valueSize;
	char *write = writeBuffer;
	memcpy(write, &magic, magicSize);
	write += magicSize;
	memcpy(write, &payloadSize, idSize);
	write += payloadSizeInfoSize;
	memcpy(write, &id, idSize);
	write += idSize;
	memcpy(write, &value, valueSize);
	return magicSize + payloadSizeInfoSize + payloadSize;
}

int PluginParameter::deserialize(const char *readBuffer)
{
	const int magic = 0xDADA;
	const int magicSize = sizeof(int);
	const int idSize = sizeof(int);
	const int valueSize = sizeof(float);
	const int payloadSizeInfoSize = sizeof(int);
	const char *read = readBuffer;
	int mgc = 0;
	memcpy(&mgc, read, magicSize);
	if (mgc == magic)
	{
		read += magicSize;
		int payloadSize = 0;
		memcpy(&payloadSize, read, payloadSizeInfoSize);
		if (payloadSize >= idSize + valueSize)
		{
			read += payloadSizeInfoSize;
			memcpy(&id, read, idSize);
			read += idSize;
			memcpy(&value, read, valueSize);
			return magicSize + payloadSizeInfoSize + payloadSize;
		}
	}
	return 0;
}