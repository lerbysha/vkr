#include "Mission.h"


REGISTER_COMPONENT(Mission);


const char *Mission::getDescription()
{
	return description.get();
}

void Mission::setInProgress()
{
	for (int i = 0; i < logic_objects.size(); i++)
		if (logic_objects[i]->getState() != (States)logic_objects[i]->final_state.get())
		{
			if (logic_objects[i]->getState() == States::InProgress)
				return;

			Task *task = dynamic_cast<Task *>(logic_objects[i]);
			if (task)
			{
				task->inProgress();
				return;
			}
		}
}

void Mission::on_init()
{
	set_logic_objects_pairs<Task>();
	setInitialState();
}

void Mission::on_object_changed(LogicObject *object, States object_state)
{
	if (object_state == object->final_state.get())
		setInProgress();
}