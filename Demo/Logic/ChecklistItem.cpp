#include "ChecklistItem.h"


REGISTER_COMPONENT(ChecklistItem);


const char *ChecklistItem::getDescription()
{
	return description.get();
}

void ChecklistItem::setDoneState(States new_state)
{
	saved_state = new_state;
	set_state(States::Done);
}

bool ChecklistItem::getScore(float &points)
{
	if (getState() != States::Done || (!broken && saved_state == States::Broken))
	{
#if DEBUG
		Log::message("Error: %s\n", node->getName());
#endif
		points = failures_points.get();
		return false;
	}

	if (broken)
	{
		if (broken->getState() == saved_state)
		{
			points = success_points.get();
			return true;
		}

#if DEBUG
		Log::message("Error: %s\n", node->getName());
#endif
		points = failures_points.get();
		return false;
	}

	points = success_points.get();
	return true;
}

void ChecklistItem::on_set_initial_state()
{
	saved_state = States::None;

	broken = nullptr;
	if (broken_object)
	{
		broken = getComponent<LogicObject>(broken_object);
		if (!broken)
		{
#if DEBUG
			Log::message("Checklist Item error: broken object not contain Logic Object component, please check %s\n", node->getName());
#endif
			return;
		}
	}
}