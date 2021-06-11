#pragma once
#include "../LogicObject.h"


class Accident : public LogicObject
{
public:
	COMPONENT(Accident, LogicObject);

	PROP_NAME("accident");
	PROP_AUTOSAVE(0);

	PROP_PARAM(String, description);

	const char *getDescription();

private:

#if DEBUG
	void on_changed(States new_state) override
	{
		Log::message("ACCIDENT: %s is %d\n", getDescription(), new_state);
	}
#endif
};