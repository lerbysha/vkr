#pragma once
#include "../LogicObject.h"
#include <UnigineGame.h>


class TimerComponent : public LogicObject
{
public:
	COMPONENT(TimerComponent, LogicObject);
	COMPONENT_UPDATE(update);

	PROP_NAME("timer");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Float, duration);

private:
	void on_init() override;
	void on_changed(States state) override;
	void update();

	float current_time;
	bool start_timer;
};