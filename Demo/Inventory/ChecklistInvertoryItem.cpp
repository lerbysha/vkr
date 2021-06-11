#include "ChecklistInvertoryItem.h"
#include <UnigineInput.h>


REGISTER_COMPONENT(ChecklistInvertoryItem);


void ChecklistInvertoryItem::setMissionInfo(LogicController *logic_controller)
{
	statistics.get()->setEnabled(0);
	selection_frame.get()->setEnabled(0);

	for (int i = 0; i < items_handlers_id.size(); i++)
		checklist_items[i]->removeOnStateChangedListener(items_handlers_id[i]);
	items_handlers_id.clear();

	Vector<ChecklistItem *> all_checklist_items;
	getComponentsInChildren<ChecklistItem>(logic_controller->getNode(), all_checklist_items);

	// only active checklist items are used
	checklist_items.clear();
	for (int i = 0; i < all_checklist_items.size(); i++)
		if (all_checklist_items[i]->isEnabled())
			checklist_items.push_back(all_checklist_items[i]);

	for (int i = 0; i < checklist_items.size(); i++)
	{
		int id = checklist_items[i]->setOnStateChangedListener(std::bind(&ChecklistInvertoryItem::on_items_state_changed, this, std::placeholders::_1, std::placeholders::_2));
		items_handlers_id.push_back(id);
	}

	// set tasks description in checklist
	String str;
	for (int i = 0; i < tasks_info.size(); i++)
	{
		if (i < checklist_items.size())
		{
			str.clear();
			str.append(std::to_string(i + 1).c_str());

			i + 1 < 10 ? str.append("   ") : str.append(" ");

			str.append(checklist_items[i]->getDescription());

			tasks_info[i]->getText()->setText(str);
			tasks_info[i]->getMark()->setEnabled(0);
		}
		else
			tasks_info[i]->getNode()->setEnabled(false);
	}
}

void ChecklistInvertoryItem::setEnabledChecklist(bool state)
{
	for (int i = 0; i < tasks_info.size(); i++)
		tasks_info[i]->getNode()->setEnabled(state);

	mission_info.get()->setEnabled(state);
	statistics.get()->setEnabled(state);
	selection_frame.get()->setEnabled(state);
	restart_button.get()->setEnabled(state);
	details_button.get()->setEnabled(state);
	good_end.get()->setEnabled(state);
	bad_end.get()->setEnabled(state);

	for (int i = 0; i < items_handlers_id.size(); i++)
		checklist_items[i]->removeOnStateChangedListener(items_handlers_id[i]);
	items_handlers_id.clear();

	in_progress_index = -1;
}

void ChecklistInvertoryItem::showAccident(const char *message)
{
	mission_info.get()->setEnabled(0);
	statistics.get()->setEnabled(1);
	details_button.get()->setEnabled(0);

	good_end.get()->setEnabled(0);
	bad_end.get()->setEnabled(1);

	// show error message
	NodePtr error_message = bad_end.get()->getChild(1);
	if (error_message)
	{
		ObjectTextPtr message_text = checked_ptr_cast<ObjectText>(error_message);
		if (message_text)
		{
			String str = "<center>";
			str.append(message);
			str.append("</center>");

			message_text->setText(str);
		}
	}
	else
	{
#if DEBUG
		Log::error("Checklist Inventory Item error: can't find text field in bad_end node\n");
#endif
	}
}

void ChecklistInvertoryItem::showMissionFinised(const char *message)
{
	mission_info.get()->setEnabled(0);
	statistics.get()->setEnabled(1);

	good_end.get()->setEnabled(1);
	bad_end.get()->setEnabled(0);

	// get score and errors count, also mark incorrect and correct answers in checklist
	float result = 0;
	float points = 0;
	int error_count = 0;
	for (int i = 0; i < checklist_items.size(); i++)
	{
		bool res = checklist_items[i]->getScore(points);
		if (!res)
		{
			error_count++;
			tasks_info[i]->getMark()->setMaterial(bad_material.get(), 0);
		}
		else
			tasks_info[i]->getMark()->setMaterial(good_material.get(), 0);

		result += points;
	}

	NodePtr score_result = good_end.get()->getChild(1);
	if (score_result)
	{
		ObjectTextPtr score_text = checked_ptr_cast<ObjectText>(score_result);
		if (score_text)
			score_text->setText(std::to_string(ftoi(result)).c_str());
		else
		{
#if DEBUG
			Log::error("Checklist Inventory Item error: can't cast score result to ObjectText\n");
#endif
		}
	}
	else
	{
#if DEBUG
		Log::error("Checklist Inventory Item error: can't find score result text field in good_end node\n");
#endif
	}

	NodePtr errors_result = good_end.get()->getChild(3);
	if (errors_result)
	{
		ObjectTextPtr errors_text = checked_ptr_cast<ObjectText>(errors_result);
		if (errors_text)
			errors_text->setText(std::to_string(error_count).c_str());
		else
		{
#if DEBUG
			Log::error("Checklist Inventory Item error: can't cast errors result to ObjectText\n");
#endif
		}
	}
	else
	{
#if DEBUG
		Log::error("Checklist Inventory Item error: can't find errors result text field in good_end node\n");
#endif
	}
}

void ChecklistInvertoryItem::show_details()
{
	mission_info.get()->setEnabled(1);
	selection_frame.get()->setEnabled(0);
	statistics.get()->setEnabled(0);
}

void ChecklistInvertoryItem::set_slider(ObjectPtr &slider, float scale)
{
	slider->setScale(vec3(scale, button_height, 1.0f));
	slider->setPosition(Vec3((-button_width + scale) / 2, button_y, 0));
}

void ChecklistInvertoryItem::checklist_init()
{
	if (!mission_info)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: mission info is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!statistics)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: statistics is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!selection_frame)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: selection frame is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!restart_button)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: restart button is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!details_button)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: details button is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!good_end)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: good end is null, please check %s\n", node->getName());
#endif
		return;
	}

	if (!bad_end)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: bad end is null, please check %s\n", node->getName());
#endif
		return;
	}

	getComponentsInChildren<ChecklistTaskInfo>(node, tasks_info);

	intersection = WorldIntersection::create();
	is_vr_mode = VRPlayer::isVRPluginLoaded();
	in_progress_index = -1;
	was_clicked = false;
	restart_current_time = 0;
	buttons_current_time = 0;

	if (good_button && bad_button)
	{
		ObjectPtr obj = checked_ptr_cast<Object>(good_button.get());
		if (obj)
			obj->setMaterialParameterFloat4("emission_color", vec4(0, 0.5, 0, 0.5), 0);
		else
		{
#if DEBUG
			Log::error("Checlist Inventory Item error: can't cast good button to Object, please check %s\n", node->getName());
#endif
		}


		obj = checked_ptr_cast<Object>(bad_button.get());
		if (obj)
			obj->setMaterialParameterFloat4("emission_color", vec4(0.5, 0, 0, 0.25), 0);
		else
		{
#if DEBUG
			Log::error("Checlist Inventory Item error: can't cast bad button to Object, please check %s\n", node->getName());
#endif
		}
	}
	else
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: good or bad button is null, please check %s\n", node->getName());
#endif
	}

	if (!good_slider_node || !bad_slider_node || !restart_slider_node || !details_slider_node)
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: some slider is null, please check %s\n", node->getName());
#endif
	}

	restart_slider = checked_ptr_cast<Object>(restart_slider_node.get());
	if (restart_slider)
		set_slider(restart_slider, min_button_width);
	else
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: can't cast restart slider to Object, please check %s\n", node->getName());
#endif
	}

	details_slider = checked_ptr_cast<Object>(details_slider_node.get());
	if (details_slider)
		set_slider(details_slider, min_button_width);
	else
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: can't cast details slider to Object, please check %s\n", node->getName());
#endif
	}

	good_slider = checked_ptr_cast<Object>(good_slider_node.get());
	if (good_slider)
		set_slider(good_slider, min_button_width);
	else
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: can't cast good slider to Object, please check %s\n", node->getName());
#endif
	}

	bad_slider = checked_ptr_cast<Object>(bad_slider_node.get());
	if (bad_slider)
		set_slider(bad_slider, min_button_width);
	else
	{
#if DEBUG
		Log::error("Checlist Inventory Item error: can't cast bad slider to Object, please check %s\n", node->getName());
#endif
	}

	setEnabledChecklist(0);
}


void ChecklistInvertoryItem::checklist_shutdown()
{

}

ObjectPtr ChecklistInvertoryItem::get_hit_object()
{
	Vec3 direction;
	Vec3 p1;
	Vec3 p2;

	if (is_vr_mode)
	{
		if (grab_hand_num == 0)
		{
			direction = (Vec3)right_glove_pointer.get()->getWorldDirection(AXIS_Y);
			p1 = right_glove_pointer.get()->getWorldPosition() - direction * 0.1;
		}
		else
		{
			direction = (Vec3)left_glove_pointer.get()->getWorldDirection(AXIS_Y);
			p1 = left_glove_pointer.get()->getWorldPosition() - direction * 0.1;
		}

		p2 = p1 + direction * 0.15;
	}
	else
	{
		ivec2 mouse_pos = Input::getMouseCoord();
		direction = (Vec3)vr_player->getPlayer()->getDirectionFromScreen(mouse_pos.x, mouse_pos.y, App::getWidth(), App::getHeight());;
		p1 = vr_player->getPlayer()->getCamera()->getPosition();
		p2 = p1 + direction;
	}

	return World::getIntersection(p1, p2, Masks::ChecklistInteractive, intersection);
}

void ChecklistInvertoryItem::button_click(ChecklistInvertoryItem::ButtonType type)
{
	if (type == ButtonType::Restart)
	{
		on_restart();
		return;
	}

	if (type == ButtonType::Details)
	{
		show_details();
		return;
	}

	if (in_progress_index == -1)
		return;

	tasks_info[in_progress_index]->getMark()->setEnabled(1);

	if (type == ButtonType::Good)
	{
		tasks_info[in_progress_index]->getMark()->setMaterial(good_material.get(), 0);
		checklist_items[in_progress_index]->setDoneState(LogicObject::States::NotBroken);
	}
	else if (type == ButtonType::Bad)
	{
		tasks_info[in_progress_index]->getMark()->setMaterial(bad_material.get(), 0);
		checklist_items[in_progress_index]->setDoneState(LogicObject::States::Broken);
	}
}

void ChecklistInvertoryItem::checklist_update()
{
	if (!is_hold)
	{
		if (selection_frame.get()->isEnabledSelf())
			selection_frame.get()->setEnabled(0);

		return;
	}
	else if (in_progress_index != -1)
	{
		if (!selection_frame.get()->isEnabledSelf())
			selection_frame.get()->setEnabled(1);
	}

	hit_obj = get_hit_object();
	if (!hit_obj)
	{
		was_clicked = false;
		restart_current_time = 0;
		buttons_current_time = 0;

		set_slider(restart_slider, min_button_width);
		set_slider(details_slider, min_button_width);
		set_slider(good_slider, min_button_width);
		set_slider(bad_slider, min_button_width);

		movable_obj->use_hand_to_hand = true;

		return;
	}

	hit_mask = hit_obj->getIntersectionMask(0);
	movable_obj->use_hand_to_hand = false;

	vr_update();
}

void ChecklistInvertoryItem::pc_update()
{
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		switch (hit_mask)
		{
		case Masks::GoodButton:    button_click(ButtonType::Good);    break;
		case Masks::BadButton:     button_click(ButtonType::Bad);     break;
		case Masks::DetailsButton: button_click(ButtonType::Details); break;
		case Masks::RestartButton: button_click(ButtonType::Restart); break;
		}
	}
}

void ChecklistInvertoryItem::vr_update()
{
	if (was_clicked)
	{
		restart_current_time = 0;
		buttons_current_time = 0;
		return;
	}

	float current_width = 0;
	if (hit_mask != Masks::RestartButton)
	{
		buttons_current_time += Game::getIFps();
		current_width = button_width * buttons_current_time / buttons_click_time;

		// update sliders
		switch (hit_mask)
		{
		case Masks::GoodButton:    set_slider(good_slider, current_width);    break;
		case Masks::BadButton:     set_slider(bad_slider, current_width);     break;
		case Masks::DetailsButton: set_slider(details_slider, current_width); break;
		}

		// click on button
		if (buttons_current_time > buttons_click_time)
		{
			switch (hit_mask)
			{
			case Masks::GoodButton:    button_click(ButtonType::Good);    break;
			case Masks::BadButton:     button_click(ButtonType::Bad);     break;
			case Masks::DetailsButton: button_click(ButtonType::Details); break;
			}

			buttons_current_time = 0;
			was_clicked = true;
		}
	}
	else
	{
		if (restart_current_time > restart_click_time)
		{
			button_click(ButtonType::Restart);

			restart_current_time = 0;
			was_clicked = true;
		}

		restart_current_time += Game::getIFps();

		current_width = button_width * restart_current_time / restart_click_time;
		set_slider(restart_slider, current_width);
	}
}

void ChecklistInvertoryItem::on_items_state_changed(LogicObject *object, LogicObject::States object_state)
{
	// when task is done, appropriate checklist item gets InProgress state
	// but currently another checklist item may be active
	if (object_state == LogicObject::States::InProgress && in_progress_index == -1)
	{
		ChecklistItem *item = dynamic_cast<ChecklistItem *>(object);
		if (!item)
			return;

		int index = checklist_items.findIndex(object);
		if (0 <= index && index < tasks_info.size())
		{
			Vec3 pos = tasks_info[index]->getBackground()->getWorldPosition();
			selection_frame.get()->setWorldPosition(pos);
			selection_frame.get()->setEnabled(1);
			in_progress_index = index;
		}
	}
	else if (object_state == LogicObject::States::Done)
	{
		// any item in checklist has been marked, and we trying to find new InProgress checklist item
		for (int i = 0; i < checklist_items.size(); i++)
			if (checklist_items[i]->getState() == LogicObject::States::InProgress)
			{
				Vec3 pos = tasks_info[i]->getBackground()->getWorldPosition();
				selection_frame.get()->setWorldPosition(pos);
				selection_frame.get()->setEnabled(1);
				in_progress_index = i;
				return;
			}

		selection_frame.get()->setEnabled(0);
		in_progress_index = -1;
	}
}