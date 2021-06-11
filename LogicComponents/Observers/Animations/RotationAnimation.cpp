#include "RotationAnimation.h"


REGISTER_COMPONENT(RotationAnimation);


void RotationAnimation::update()
{
	if (function_state && loop.get())
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			if (!nodes[i])
				continue;

			vec3 delta_rot = delta_rotation * Game::getIFps();
			quat rot = quat(delta_rot.x, delta_rot.y, delta_rot.z);

			if (hierarchy_based.get())
			{
				for (int j = 0; j < node->getNumChildren(); j++)
					node->getChild(j)->setRotation(node->getChild(j)->getRotation() * rot);
			}
			else
			{
				for (int j = 0; j < nodes.size(); j++)
				{
					if (!nodes[j])
						continue;

					nodes[j].get()->setRotation(nodes[j].get()->getRotation() * rot);
				}
			}
		}
	}
	else if (time < 1)
	{
		time = saturate(time + (Game::getIFps() / Game::getScale()) / duration.get());

		vec3 cur_rot = lerp(from_rotation, to_rotation, time);
		quat rot = quat(cur_rot.x, cur_rot.y, cur_rot.z);

		if (hierarchy_based.get())
		{
			for (int i = 0; i < node->getNumChildren(); i++)
				node->getChild(i)->setRotation(rot);
		}
		else
		{
			for (int i = 0; i < nodes.size(); i++)
			{
				if (!nodes[i])
					continue;

				nodes[i].get()->setRotation(rot);
			}
		}
	}
}

void RotationAnimation::on_init()
{
	if ((!nodes.size() && !hierarchy_based.get()) || (hierarchy_based.get() && !node->getNumChildren()))
	{
#if DEBUG
		Log::message("Rotation Animation: nodes array is empty, please check %s\n", node->getName());
#endif
		return;
	}

	quat init_rot = quat(initial_rotation.get().x, initial_rotation.get().y, initial_rotation.get().z);

	for (int i = 0; i < nodes.size(); i++)
		if (!nodes[i])
		{
#if DEBUG
			Log::error("Rotation Animation error: node #%d is null, please check %s\n", i + 1, node->getName());
#endif
		}
		else if (!loop.get())
			nodes[i]->setRotation(init_rot);

	if (hierarchy_based.get() && !loop.get())
		for (int i = 0; i < node->getNumChildren(); i++)
			node->getChild(i)->setRotation(init_rot);

	if (axis_x.get())
		delta_rotation.x = velocity.get();

	if (axis_y.get())
		delta_rotation.y = velocity.get();

	if (axis_z.get())
		delta_rotation.z = velocity.get();

	function_state = 0;
	time = 1;
}

void RotationAnimation::on_activate()
{
	from_rotation = initial_rotation.get();
	to_rotation = final_rotation.get();

	time = 1 - time;
	function_state = 1;
}

void RotationAnimation::on_deactivate()
{
	from_rotation = final_rotation.get();
	to_rotation = initial_rotation.get();

	time = 1 - time;
	function_state = 0;
}