#include "ScaleAnimation.h"


REGISTER_COMPONENT(ScaleAnimation);


void ScaleAnimation::update()
{
	if (time < 1)
	{
		time = saturate(time + (Game::getIFps() / Game::getScale()) / duration.get());

		vec3 cur_scale = lerp(from_scale, to_scale, time);

		if (hierarchy_based.get())
		{
			for (int i = 0; i < node->getNumChildren(); i++)
				node->getChild(i)->setScale(cur_scale);
		}
		else
		{
			for (int i = 0; i < nodes.size(); i++)
			{
				if (!nodes[i])
					continue;

				nodes[i].get()->setScale(cur_scale);
			}
		}
	}
}

void ScaleAnimation::on_init()
{
	if ((!nodes.size() && !hierarchy_based.get()) || (hierarchy_based.get() && !node->getNumChildren()))
	{
#if DEBUG
		Log::message("Scale Animation: nodes array is empty, please check %s\n", node->getName());
#endif
		return;
	}

	for (int i = 0; i < nodes.size(); i++)
		if (!nodes[i])
		{
#if DEBUG
			Log::error("Scale Animation error: node #%d is null, please check %s\n", i + 1, node->getName());
#endif
		}
		else
			nodes[i]->setScale(initial_scale.get());

	if (hierarchy_based.get())
		for (int i = 0; i < node->getNumChildren(); i++)
			node->getChild(i)->setScale(initial_scale.get());

	time = 1;
}

void ScaleAnimation::on_activate()
{
	from_scale = initial_scale.get();
	to_scale = final_scale.get();

	time = 1 - time;
}

void ScaleAnimation::on_deactivate()
{
	from_scale = final_scale.get();
	to_scale = initial_scale.get();

	time = 1 - time;
}