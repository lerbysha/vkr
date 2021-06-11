#pragma once
#include "../../LogicComponents/LogicObject.h"


class ChecklistItem : public LogicObject
{
public:
	COMPONENT(ChecklistItem, LogicObject);

	PROP_NAME("checklist_item");
	PROP_AUTOSAVE(0);

	PROP_PARAM(String, description);
	PROP_PARAM(Float, success_points);
	PROP_PARAM(Float, failures_points);
	PROP_PARAM(Node, broken_object);

	const char *getDescription();
	void setDoneState(States state);
	bool getScore(float &points);

private:
	void on_set_initial_state() override;

	LogicObject *broken;

#if DEBUG
	void on_changed(States new_state) override
	{
		Log::message("Checklist item: %s is %d\n", getDescription(), getState());
	}
#endif

	States saved_state;

};