#pragma once
#include "../LogicObject.h"


using namespace Unigine;


class Observer : public Unigine::ComponentBase
{
public:
	COMPONENT(Observer, Unigine::ComponentBase);
	COMPONENT_INIT(init, 2);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Switch, function, 0, "any,all");
	PROP_ARRAY_STRUCT(LogicObject::ObjectAndState, objects);

	virtual void disable() { };

protected:
	virtual void on_init() {};
	virtual void on_activate() {};
	virtual void on_deactivate() {};
	virtual void on_shutdown() {};

	int last_result;

private:
	void init();
	void shutdown();

	int initial_result;

	void on_state_changed_handler(LogicObject *object, LogicObject::States object_state);
	Vector<int> handlers_id;

	Vector<LogicObject *> logic_objects;
	Vector<LogicObject::States> needed_states;
};