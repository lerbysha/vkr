#include "Timer.h"

REGISTER_COMPONENT(TimerComponent);

void TimerComponent::on_init()
{
	current_time = 0;
	start_timer = false;

	set_logic_objects_pairs<LogicObject>();
	setInitialState();
}

void TimerComponent::on_changed(States in_state)
{
	if (in_state == (States)initial_state.get())
		start_timer = false;
	else if (!start_timer && in_state == (States)final_state.get())
		start_timer = true;

	current_time = 0;
}

void TimerComponent::update()
{
	if (start_timer)
	{
		if (current_time >= duration.get())
		{
#if DEBUG
			Log::message("TIMER: %s is %d\n", node->getName(), States::Done);
#endif

			set_state(States::Done);
			start_timer = false;
		}

		current_time += Game::getIFps();
	}
}