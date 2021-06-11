#include "Tooltip.h"
#include "../Framework/Utils.h"
#include <UnigineGame.h>
#include "../Framework/Components/VRPlayerSpawner.h"
#include <string>


REGISTER_COMPONENT(Tooltip);


void Tooltip::setVisible(bool state)
{
	if (obj_line)
		obj_line->setEnabled(state);

	if (direction_hint)
		direction_hint->setEnabled(state);

	info.get()->setEnabled(state);
	end_point.get()->setEnabled(state);

	current_state = state;
}

void Tooltip::set_scale_and_offset()
{
	BoundBox text_bound = tooltip_text->getBoundBox();
	vec3 background_scale = text_bound.getMax() - text_bound.getMin();

	background_scale.z = background_scale.y;
	background_scale.y = 1;

	background_scale += frame_offset.get();
	background_scale.y = 1;

	vec3 positions = background_scale / 2;

	background_node.get()->setScale(vec3::ONE);

	substrate.get()->setScale(background_scale);
	substrate.get()->setPosition(Vec3(0, frame_offset.get().y, 0));

	left_edge.get()->setPosition(Vec3(-positions.x + half_frame_width, 0, 0));
	left_edge.get()->setScale(vec3(1, 1, background_scale.z));

	right_edge.get()->setPosition(Vec3(positions.x - half_frame_width, 0, 0));
	right_edge.get()->setScale(vec3(1, 1, background_scale.z));

	upper_edge.get()->setPosition(Vec3(0, 0, positions.z - half_frame_width));
	upper_edge.get()->setScale(vec3(background_scale.x, 1, 1));

	bottom_edge.get()->setPosition(Vec3(0, 0, -positions.z + half_frame_width));
	bottom_edge.get()->setScale(vec3(background_scale.x, 1, 1));

	tooltip_text->setPosition(Vec3(-positions.x + frame_offset.get().x / 2, 0, positions.z - frame_offset.get().z / 2));

	switch (start_point_pos)
	{
		case 0: start_point.get()->setPosition(Vec3(0, 0, -positions.z)); start_tangent_direction = AXIS_NZ; break;
		case 1: start_point.get()->setPosition(Vec3(0, 0, positions.z));  start_tangent_direction = AXIS_Z;  break;
		case 2: start_point.get()->setPosition(Vec3(-positions.x, 0, 0)); start_tangent_direction = AXIS_NX; break;
		case 3: start_point.get()->setPosition(Vec3(positions.x, 0, 0));  start_tangent_direction = AXIS_X;  break;
	}
}

void Tooltip::init()
{
	NodePtr spawn_node = World::getNodeByName("spawn_point");
	vr_player = ComponentSystem::get()->getComponent<VRPlayerSpawner>(spawn_node)->getSpawnedPlayer();
	head = vr_player->getHead();

	if (!start_point)
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: start point is null, please check node %s\n", node->getName());
#endif
		return;
	}

	if (!end_point)
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: end point is null, please check node %s\n", node->getName());
#endif
		return;
	}

	if (!text)
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: text is null, please check node %s\n", node->getName());
#endif
		return;
	}

	tooltip_text = checked_ptr_cast<ObjectText>(text.get());
	if (!tooltip_text)
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: text node is not ObjectText, please check node %s\n", node->getName());
#endif
		return;
	}

	if (!background_node)
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: background node is null, please check node %s\n", node->getName());
#endif
		return;
	}

	if (!substrate || !upper_edge || !left_edge || !bottom_edge || !right_edge)
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: some elements of frame is null, please check node %s\n", node->getName());
#endif
		return;
	}

	substrate_obj = static_ptr_cast<Object>(substrate.get());

	scale_factor = (max_scale.get() - min_scale.get()) / (max_distance.get() - min_distance.get());

	if (!FileSystem::isFileExist(direction_hint_ref.get()))
	{
#if DEBUG
		Log::error("TOOLTIP ERROR: direction_hint_ref is empty, please check node %s\n", node->getName());
#endif
		return;
	}

	direction_hint = NodeReference::create(direction_hint_ref.get());

	NodePtr hint = direction_hint->getReference();
	arrow_hint = hint->getChild(0);
	text_hint = checked_ptr_cast<ObjectText>(hint->getChild(1));

	end_point_offset = 0.015f * end_point.get()->getScale().x;

	set_scale_and_offset();

	if (use_line.get())
	{
		// create object
		obj_line = ObjectMeshDynamic::create();
		obj_line->setMaterial(line_material.get(), 0);
		obj_line->setSurfaceProperty("surface_base", 0);
		obj_line->setCastShadow(0, 0);
		obj_line->setCastWorldShadow(0, 0);
		obj_line->setIntersection(0, 0);
		obj_line->setCollision(0, 0);

		// create surface
		obj_line->clearVertex();
		obj_line->clearIndices();
		for (int i = 1; i <= line_quads; i++)
		{
			obj_line->addTriangleQuads(1);
			obj_line->addVertex(vec3::ZERO); obj_line->addTexCoord(vec4(0, 0, 0, 0));
			obj_line->addVertex(vec3::ZERO); obj_line->addTexCoord(vec4(1, 0, 0, 0));
			obj_line->addVertex(vec3::ZERO); obj_line->addTexCoord(vec4(1, 1, 0, 0));
			obj_line->addVertex(vec3::ZERO); obj_line->addTexCoord(vec4(0, 1, 0, 0));
		}
		obj_line->updateBounds();
		obj_line->updateTangents();
		obj_line->flushVertex();
		obj_line->flushIndices();

		obj_line->setEnabled(0);
	}

	Visualizer::setEnabled(1);
}

void Tooltip::update()
{
	if (!current_state)
		return;

	Vec3 head_pos = head->getWorldPosition();
	Vec3 end_pos = end_point.get()->getWorldPosition();
	Vec3 start_pos = start_point.get()->getWorldPosition();

	Vec3 info_dir = info.get()->getWorldPosition() - head_pos;
	vec3 new_scale;

	float distance(static_cast<float>(info_dir.length()));
	if (min_distance.get() < distance && distance < max_distance.get())
	{
		float coeff = min_scale.get() + scale_factor * (distance - min_distance.get());
		new_scale = vec3(coeff, coeff, coeff);
	}
	else if (distance < min_distance.get())
		new_scale = vec3(min_scale.get(), min_scale.get(), min_scale.get());
	else if (max_distance.get() < distance)
		new_scale = vec3(max_scale.get(), max_scale.get(), max_scale.get());

	info.get()->setWorldDirection((vec3)info_dir.normalize(), vec3::UP, AXIS_Y);
	info.get()->setScale(new_scale);

	Vec3 head_dir = (Vec3)head->getWorldDirection(AXIS_NZ);
	Vec3 new_hint_pos = head_pos + head_dir * 0.5f;
	Vec3 new_arrow_dir = end_pos - arrow_hint->getWorldPosition();

	if (substrate_obj && !substrate_obj->isVisibleCamera())
	{
		if (!direction_hint->isEnabled())
			direction_hint->setEnabled(true);

		float k = saturate(speed_reaction * Game::getIFps());
		direction_hint->setWorldPosition(lerp(direction_hint->getWorldPosition(), new_hint_pos, k));
		arrow_hint->setWorldDirection(lerp(arrow_hint->getWorldDirection(AXIS_Y), (vec3)new_arrow_dir, k), vec3::UP, AXIS_Y);
		text_hint->setWorldDirection((vec3)head_dir, vec3::UP, AXIS_NZ);

		float arrow_distance(static_cast<float>(new_arrow_dir.length()));
		int first = ftoi(arrow_distance);
		int second = ftoi((100.0f * (arrow_distance - first)));

		distance_str.clear();
		distance_str.append(std::to_string(first).c_str());
		distance_str.append(".");

		if (second)
			distance_str.append(second < 10 ? "0" : std::to_string(second).c_str());
		else
			distance_str.append("00");

		distance_str.append(" m");

		text_hint->setText(distance_str);
	}
	else
	{
		if (direction_hint->isEnabled())
			direction_hint->setEnabled(0);

		direction_hint->setWorldPosition(new_hint_pos);
		arrow_hint->setWorldDirection((vec3)new_arrow_dir, vec3::UP, AXIS_Y);
	}

	if (use_line.get())
	{
		float inum = 1.0f / line_quads;
		float line_width = 0.005f;

		Vec3 end(end_point.get()->getWorldPosition() + Vec3(end_point.get()->getWorldDirection(AXIS_X)) * 0.015f * end_point.get()->getScale().x);
		Vec3 start(start_point.get()->getWorldPosition());

		Vec3 start_t = start - (Vec3)start_point.get()->getWorldDirection(static_cast<Math::AXIS>(start_tangent_direction)) * new_scale.x;
		Vec3 end_dir(end_point.get()->getWorldDirection(AXIS_X));
		Vec3 end_t = end_pos - end_dir * new_scale.x;

		Vec3 last_p = start;
		Vec3 last_right;
		for (int i = 1; i <= line_quads; i++)
		{
			Vec3 p = getHermiteSpline(start_t, start, end, end_t, inum * i);
			Vec3 dir = normalize(p - last_p);
			Vec3 right = cross(p - head->getWorldPosition(), dir); //cross(view_direction, dir);
			if (i == 1)
				last_right = right;

			Vec3 p0 = last_p - last_right * line_width * 0.5f;	// 0, 0
			Vec3 p1 = last_p + last_right * line_width * 0.5f;  // 1, 0
			Vec3 p2 = p + right * line_width * 0.5f;	// 1, 1
			Vec3 p3 = p - right * line_width * 0.5f;	// 0, 1

			obj_line->setVertex((i - 1) * 4, vec3(p0));
			obj_line->setVertex((i - 1) * 4 + 1, vec3(p1));
			obj_line->setVertex((i - 1) * 4 + 2, vec3(p2));
			obj_line->setVertex((i - 1) * 4 + 3, vec3(p3));

			last_p = p;
			last_right = right;
		}

		obj_line->updateBounds();
		obj_line->flushVertex();
	}
}
