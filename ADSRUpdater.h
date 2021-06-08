#pragma once
class ADSRUpdater
{
public:
	virtual void setAttack(float) {}
	virtual void setDecay(float) {}
	virtual void setSustain(float) {}
	virtual void setRelease(float) {}
	ADSRUpdater();
	~ADSRUpdater();
};

