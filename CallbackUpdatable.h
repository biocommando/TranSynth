#pragma once
class CallbackUpdatable
{
public:
	virtual void onUpdate(void) {}
	virtual void onUpdateWithValue(float value) {}
	CallbackUpdatable();
	~CallbackUpdatable();
};

