#include "InspectionManager.h"


REGISTER_COMPONENT(InspectionManager);


void InspectionManager::init()
{
	if (!spawn_point)
	{
#if DEBUG
		Log::error("Inspection Logic error: spawn point is null, please check %s\n", node->getName());
#endif
		return;
	}

	VRPlayerSpawner *spawner = getComponent<VRPlayerSpawner>(spawn_point.get());
	if (!spawner)
	{
#if DEBUG
		Log::error("Inspection Logic error: spawn point not contain VRPlayerSpawner component, please check %s\n", node->getName());
#endif
		return;
	}

	if (activator)
		activator_component = getComponent<LogicObject>(activator.get());

	vr_player = spawner->getSpawnedPlayer();
	head = vr_player->getHead();

	projection_matrix = perspective(fov.get(), 1, z_near.get(), z_far.get());
	modelview_matrix = head->getIWorldTransform();

	sqr_z_far = z_far.get() * z_far.get();

	frustum = UNIGINE_BOUND_FRUSTUM();
	frustum.set(projection_matrix, modelview_matrix);

	last_inspected = nullptr;
	inspection_time = 0;
	intersection = WorldIntersection::create();

	current_time = 0;
	inspection_indicator.get()->setEnabled(0);
	slider_max_scale = inspection_slider.get()->getScale().x;
	slider_height = inspection_slider.get()->getScale().y;
	slider_pos = inspection_slider.get()->getPosition();
}

void InspectionManager::update()
{
	if (!force_enable && activator_component && activator_component->getState() != (LogicObject::States)activator_component->final_state.get())
	{
		if (!last_inspected)
			return;

		inspection_indicator.get()->setEnabled(0);
		last_inspected = nullptr;

		return;
	}

	frustum.set(projection_matrix, head->getIWorldTransform());

	objects.clear();
	int res = World::getIntersection(frustum, objects);
	if (res)
	{
		still_inspected = false;

		for (int i = 0; i < objects.size(); i++)
		{
			ObjInspectable *inspectable = getComponent<ObjInspectable>(objects[i]);
			if (inspectable)
			{
				box = objects[i]->getBoundBox();
				Vec3 local_pos = (Vec3)(box.getMin() + (box.getMax() - box.getMin()) * 0.5f);

				local_transform = Mat4::IDENTITY;
				setTransformPosition(local_transform, local_pos);
				local_transform = inspectable->getNode()->getWorldTransform() * local_transform;

				ObjectPtr obj = World::getIntersection(head->getWorldPosition(), local_transform.getColumn3(3), 0xFFFFFF, intersection);
				if (obj && objects[i] != obj)
					continue;

				Vec3 dist = local_transform.getColumn3(3) - head->getWorldPosition();
				if (dist.length2() < sqr_z_far)
				{
					still_inspected = true;

					if (inspectable != last_inspected)
					{
						inspection_time = 0;
						last_inspected = inspectable;

						inspection_indicator.get()->setEnabled(1);
						inspection_slider.get()->setWorldScale(vec3(FLT_EPSILON, slider_height, 1));
						inspection_slider.get()->setPosition(slider_pos);
					}
					else
					{
						inspection_time += Game::getIFps();

						float scale = slider_max_scale * inspection_time / hold_duration.get();
						inspection_slider.get()->setWorldScale(vec3(slider_max_scale - scale, slider_height, 1));

						if (inspection_time > hold_duration.get())
						{
							inspectable->inspectIt(vr_player, 1);
							inspection_time = 0;

							inspection_indicator.get()->setEnabled(0);
						}
					}
				}

				break;
			}
		}

		if (!still_inspected && last_inspected)
		{
			last_inspected->inspectIt(vr_player, 0);
			last_inspected = nullptr;

			inspection_indicator.get()->setEnabled(0);
		}
	}

	Vec3 pos = head->getWorldPosition();
	pos += (Vec3)head->getWorldDirection(AXIS_NZ) * 0.4f;

	inspection_indicator.get()->setWorldPosition(pos);
	inspection_indicator.get()->setWorldDirection(head->getWorldDirection(AXIS_Z), head->getWorldDirection(AXIS_Y), AXIS_NY);
}

void InspectionManager::shutdown()
{
	objects.clear();
}