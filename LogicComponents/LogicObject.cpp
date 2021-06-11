#include "LogicObject.h"


REGISTER_COMPONENT(LogicObject);


LogicObject::States LogicObject::getState()
{
	return state;
}

void LogicObject::setInitialState()
{
	state_is_locked = 0;
	sequence_step = 0;
	sequence_recovery = 0;
	state = States::None;

	set_state((States)initial_state.get(), true);
	on_set_initial_state();
}

int LogicObject::checkAny(Vector<LogicObject *> &objects, Vector<States> &states)
{
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->getState() == states[i])
			return 1;

	return 0;
}

int LogicObject::checkAll(Vector<LogicObject *> &objects, Vector<States> &states)
{
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->getState() != states[i])
			return 0;

	return 1;
}

void LogicObject::on_init()
{
	set_logic_objects_pairs<LogicObject>();
	setInitialState();
}

void LogicObject::on_shutdown()
{

}

void LogicObject::on_set_initial_state()
{

}

void LogicObject::set_state(LogicObject::States new_state, bool force)
{
	if (!isEnabled())
		return;

	if ((state != new_state && !state_is_locked) || force)
	{
		state = new_state;

		on_changed(state);
		on_state_changed(this, state);

		if (lock_state.get() && (States)lockable_state.get() == state)
			state_is_locked = 1;
	}
}

void LogicObject::init()
{
	on_init();
}

void LogicObject::shutdown()
{
	on_shutdown();

	for (int i = 0; i < handlers_id.size(); i++)
		logic_objects[i]->removeOnStateChangedListener(handlers_id[i]);
	handlers_id.clear();

	logic_objects.clear();
	needed_states.clear();

	on_state_changed.clear();
}

void LogicObject::on_changed(States new_state)
{

}

void LogicObject::on_state_changed_handler(LogicObject *object, States object_state)
{
	if (sequence_recovery)
		return;

	int result = check_state(object, object_state);
	if (result)
		set_state((States)final_state.get());
	else
	{
		// for New -> InProgress -> Done sequence not returned in initial state
		if (state == States::InProgress)
			return;

		set_state((States)initial_state.get());
	}

	on_object_changed(object, object_state);
}

int LogicObject::check_state(LogicObject *object, States object_state)
{
	switch ((Functions)function.get())
	{
		case Functions::All:      return all();
		case Functions::Any:      return any();
		case Functions::Sequence: return sequence(object, object_state);
	}

	return 0;
}

int LogicObject::any()
{
	for (int i = 0; i < logic_objects.size(); i++)
		if (logic_objects[i]->state == needed_states[i])
			return 1;

	return 0;
}

int LogicObject::all()
{
	for (int i = 0; i < logic_objects.size(); i++)
		if (logic_objects[i]->getState() != needed_states[i])
			return 0;

	return 1;
}

int LogicObject::sequence(LogicObject *object, States object_state)
{
	int step = logic_objects.findIndex(object);
	if (step == -1 || step != sequence_step || needed_states[sequence_step] != object_state)
	{
		sequence_recovery = 1;
		for (int i = 0; i < logic_objects.size(); i++)
			logic_objects[i]->setInitialState();

		sequence_recovery = 0;

		sequence_step = 0;
		return 0;
	}

	++sequence_step;

	if (sequence_step == logic_objects.size())
		return 1;

	return 0;
}