#include "VCMgmtEnvUpdater.h"

VCMgmtEnvUpdater::VCMgmtEnvUpdater() : parent(nullptr), type(-1)
{
}


VCMgmtEnvUpdater::~VCMgmtEnvUpdater()
{}

void VCMgmtEnvUpdater::init(ADSRUpdater *parent, int type)
{
	this->parent = parent;
	this->type = type;
}

void VCMgmtEnvUpdater::onUpdateWithValue(float value)
{
	switch (type)
	{
	case 0:
		parent->setAttack(value);
		break;
	case 1:
		parent->setDecay(value);
		break;
	case 2:
		parent->setRelease(value);
		break;
	default:
		break;
	}
}
