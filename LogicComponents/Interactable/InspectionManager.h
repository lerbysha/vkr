#pragma once
#include <UnigineComponentSystem.h>
#include "../../Framework/Components/Objects/ObjInspectable.h"
#include "../../Framework/Components/VRPlayerSpawner.h"
#include <UnigineBounds.h>
#include "../LogicObject.h"


class InspectionManager : public Unigine::ComponentBase
{
public:
	COMPONENT(InspectionManager, Unigine::ComponentBase);
	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_NAME("inspection_manager");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Node, spawn_point);
	PROP_PARAM(Float, fov);
	PROP_PARAM(Float, z_near);
	PROP_PARAM(Float, z_far);
	PROP_PARAM(Float, hold_duration);
	PROP_PARAM(Node, activator);
	PROP_PARAM(Node, inspection_indicator);
	PROP_PARAM(Node, inspection_slider);

	bool force_enable = false;

private:
	void init();
	void update();
	void shutdown();

	mat4 projection_matrix;
	Mat4 modelview_matrix;
	float sqr_z_far;

	UNIGINE_BOUND_FRUSTUM frustum;
	BoundBox box;
	Mat4 local_transform;
	Vector<ObjectPtr> objects;
	WorldIntersectionPtr intersection;

	ObjInspectable *last_inspected;
	bool still_inspected;
	float inspection_time;

	LogicObject *activator_component;

	VRPlayer *vr_player;
	NodePtr head;
	float current_time;
	float slider_max_scale;
	float slider_height;
	Vec3 slider_pos;
};