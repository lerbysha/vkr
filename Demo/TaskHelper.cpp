#include "TaskHelper.h"


REGISTER_COMPONENT(TaskHelper);


void TaskHelper::setTrainingMode(int state)
{
	if (!state)
	{
		need_glow = false;
		tooltip_component->setVisible(0);
	}

	is_training_mode = state;
}

void TaskHelper::disable()
{
	tooltip_component->setVisible(0);

	for (int i = 0; i < glowing_objects.size(); i++)
	{
		if (objects[i])
			objects[i]->setEnabled(0, glowing_objects[i]->surface_number.get());
	}
}

void TaskHelper::update()
{
	if (!need_glow)
		return;

	for (int i = 0; i < glowing_objects.size(); i++)
	{
		if (objects[i])
			objects[i]->getMaterial(glowing_objects[i]->surface_number.get())->setParameterFloat("emission_scale", 1 + Math::sin(2.0f * time));
	}

	time += Game::getIFps();
}

void TaskHelper::on_init()
{
	if (!tooltip)
	{
#if DEBUG
		Log::error("Task Helper error: tooltip is null, please check %s\n", node->getName());
#endif
		return;
	}

	tooltip_component = getComponent<Tooltip>(tooltip.get());
	if (!tooltip_component)
	{
#if DEBUG
		Log::error("Task Helper error: tooltip not contain Tooltip component, please check %s\n", node->getName());
#endif
		return;
	}

	if (!glowing_objects.size())
	{
#if DEBUG
		Log::message("Task Helper: glowing objects array is empty, please check %s\n", node->getName());
#endif
	}

	for (int i = 0; i < glowing_objects.size(); i++)
		if (!glowing_objects[i]->object)
		{
#if DEBUG
			Log::error("Task Helper error: glowing object #%d is null, please check %s\n", i + 1, node->getName());
#endif
		}

	tooltip_component->setVisible(0);
	for (int i = 0; i < glowing_objects.size(); i++)
	{
		ObjectPtr obj = checked_ptr_cast<Object>(glowing_objects[i]->object.get());
		if (obj)
		{
			obj->getMaterial(glowing_objects[i]->surface_number.get())->setParameterFloat("emission_scale", 0);
			obj->setEnabled(0, glowing_objects[i]->surface_number.get());
		}

		objects.push_back(obj);
	}

	is_training_mode = 0;
	need_glow = false;
}

void TaskHelper::on_activate()
{
	if (!is_training_mode)
		return;

	tooltip_component->setVisible(1);
	need_glow = true;
	time = 0;

	for (int i = 0; i < glowing_objects.size(); i++)
	{
		if (objects[i])
			objects[i]->setEnabled(1, glowing_objects[i]->surface_number.get());
	}
}

void TaskHelper::on_deactivate()
{
	if (!is_training_mode)
		return;

	tooltip_component->setVisible(0);
	need_glow = false;

	for (int i = 0; i < glowing_objects.size(); i++)
	{
		if (objects[i])
		{
			objects[i]->getMaterial(glowing_objects[i]->surface_number.get())->setParameterFloat("emission_scale", 0);
			objects[i]->setEnabled(0, glowing_objects[i]->surface_number.get());
		}
	}

#if DEBUG
	Log::message("TASK HELPER: %s is 0\n", node->getName());
#endif
}

void TaskHelper::on_shutdown()
{
	objects.clear();
}