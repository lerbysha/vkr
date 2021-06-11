#pragma once
#include "../LogicObject.h"
#include "../Tasks/Task.h"


class Mission : public LogicObject
{
public:
	COMPONENT(Mission, LogicObject);

	PROP_NAME("mission");
	PROP_AUTOSAVE(0);

	PROP_PARAM(String, description);

	const char *getDescription();
	void setInProgress();

private:
	void on_init() override;
	void on_object_changed(LogicObject *object, States object_state) override;
	
#if DEBUG
	void on_changed(States new_state) override
	{
		Log::message("MISSION: %s is %d\n", getDescription(), new_state);
	}
#endif
};