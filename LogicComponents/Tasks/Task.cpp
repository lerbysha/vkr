#include "Task.h"


REGISTER_COMPONENT(Task);


const char *Task::getDescription()
{
	return description.get();
}

void Task::inProgress()
{
	set_state(States::InProgress);
}