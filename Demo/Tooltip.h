#pragma once
#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include "../Framework/Components/Players/VRPlayer.h"
#include <UnigineWorlds.h>


using namespace Unigine;
using namespace Math;


class Tooltip : public Unigine::ComponentBase
{
public:
	COMPONENT(Tooltip, Unigine::ComponentBase);
	COMPONENT_INIT(init, 1);
	COMPONENT_POST_UPDATE(update, 1);

	PROP_NAME("tooltip");
	PROP_AUTOSAVE(1);

	PROP_PARAM(Node, info);
	PROP_PARAM(Node, start_point);
	PROP_PARAM(Node, end_point);
	PROP_PARAM(Node, text);
	PROP_PARAM(Node, background_node);
	PROP_PARAM(Node, substrate);
	PROP_PARAM(Node, left_edge);
	PROP_PARAM(Node, upper_edge);
	PROP_PARAM(Node, right_edge);
	PROP_PARAM(Node, bottom_edge);
	PROP_PARAM(Float, min_distance);
	PROP_PARAM(Float, max_distance);
	PROP_PARAM(Float, min_scale, 1);
	PROP_PARAM(Float, max_scale, 1);
	PROP_PARAM(Vec3, frame_offset, vec3(0.2, 0.001, 0.1));
	PROP_PARAM(Switch, start_point_pos, 0, "bottom,up,left,right");
	PROP_PARAM(File, direction_hint_ref);
	PROP_PARAM(Material, line_material);
	PROP_PARAM(Toggle, use_line, 1);

	void setVisible(bool state);

private:
	void init();
	void update();

	void set_scale_and_offset();

	const float half_frame_width = 0.00225f;
	const float speed_reaction = 20.0f;

	VRPlayer *vr_player;
	NodePtr head;
	NodePtr hand;

	ObjectPtr substrate_obj;

	ObjectTextPtr tooltip_text;
	NodeReferencePtr direction_hint;
	NodePtr arrow_hint;
	ObjectTextPtr text_hint;

	int start_tangent_direction;

	String distance_str;

	float scale_factor;
	float end_point_offset;
	int current_state;

	ObjectMeshDynamicPtr obj_line;
	int line_quads = 20;
};

