#include "Observer.h"


void Observer::init()
{
	for (int i = 0; i < handlers_id.size(); i++)
		logic_objects[i]->removeOnStateChangedListener(handlers_id[i]);
	handlers_id.clear();

	logic_objects.clear();
	needed_states.clear();

	logic_objects.reserve(objects.size());
	needed_states.reserve(objects.size());

	for (int i = 0; i < objects.size(); i++)
	{
		if (!objects[i]->object)
		{
#if DEBUG
			Log::error("Observer error: object #%d is null, please check %s\n", i + 1, node->getName());
#endif
			continue;
		}

		LogicObject *logic_object = getComponent<LogicObject>(objects[i]->object.get());
		if (!logic_object)
		{
#if DEBUG
			Log::error("Observer error: object #%d not contain LogicObject component, please check %s\n", i + 1, node->getName());
#endif
			continue;
		}

		logic_objects.push_back(logic_object);
		needed_states.push_back((LogicObject::States)objects[i]->state.get());
	}

	handlers_id.reserve(logic_objects.size());
	for (int i = 0; i < logic_objects.size(); i++)
	{
		int id = logic_objects[i]->setOnStateChangedListener(std::bind(&Observer::on_state_changed_handler, this, std::placeholders::_1, std::placeholders::_2));
		handlers_id.push_back(id);
	}

	switch ((LogicObject::Functions)function.get())
	{
		case LogicObject::Functions::Any: initial_result = LogicObject::checkAny(logic_objects, needed_states); break;
		case LogicObject::Functions::All: initial_result = LogicObject::checkAll(logic_objects, needed_states); break;
		default: break;
	}

	last_result = initial_result;
	on_init();
}

void Observer::shutdown()
{
	on_shutdown();

	for (int i = 0; i < handlers_id.size(); i++)
		logic_objects[i]->removeOnStateChangedListener(handlers_id[i]);
	handlers_id.clear();

	logic_objects.clear();
	needed_states.clear();
}

void Observer::on_state_changed_handler(LogicObject *object, LogicObject::States object_state)
{
	int result = 0;
	switch ((LogicObject::Functions)function.get())
	{
		case LogicObject::Functions::Any: result = LogicObject::checkAny(logic_objects, needed_states); break;
		case LogicObject::Functions::All: result = LogicObject::checkAll(logic_objects, needed_states); break;
		default: break;
	}

	if (result != last_result)
	{
		last_result = result;
		result ? on_activate() : on_deactivate();
	}
}