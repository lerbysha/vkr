#pragma once
#include "../LogicObject.h"


class Task : public LogicObject
{
public:
	COMPONENT(Task, LogicObject);

	PROP_NAME("task");
	PROP_AUTOSAVE(0);

	PROP_PARAM(String, description);

	const char *getDescription();
	void inProgress();

private:

#if DEBUG
	void on_changed(States new_state) override
	{
		Log::message("TASK: %s is %d\n", getDescription(), getState());
	}
#endif
};