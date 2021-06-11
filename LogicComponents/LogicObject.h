#pragma once
#include <UnigineComponentSystem.h>
#include "../Framework/Utils.h"
#include <UnigineVector.h>

using namespace Unigine;

class Task;

class LogicObject : public Unigine::ComponentBase
{
public:
	COMPONENT(LogicObject, Unigine::ComponentBase);
	COMPONENT_INIT(init, 1);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_NAME("logic_object");
	PROP_AUTOSAVE(0);

	enum States { Disabled, Enabled, Accomplished, Unaccomplished, Grabbed, Held, Used, Released, Selected, Unselected, Broken, NotBroken, New, InProgress, Done, Inspected, Uninspected, None };
	enum Functions { Any, All, Sequence };

	struct ObjectAndState : public ComponentStruct
	{
		virtual ~ObjectAndState() = default;

		PROP_PARAM(Node, object);
		PROP_PARAM(Switch, state, 0, "disabled,enabled,accomplished,unaccomplished,grabbed,held,used,released,selected,unselected,broken,not_broken,new,in_progress,done,inspected,uninspected");
	};

	PROP_PARAM(Switch, initial_state, 0, "disabled,enabled,accomplished,unaccomplished,grabbed,held,used,released,selected,unselected,broken,not_broken,new,in_progress,done,inspected,uninspected");
	PROP_PARAM(Switch, final_state, 0, "disabled,enabled,accomplished,unaccomplished,grabbed,held,used,released,selected,unselected,broken,not_broken,new,in_progress,done,inspected,uninspected");
	PROP_PARAM(Toggle, lock_state);
	PROP_PARAM(Switch, lockable_state, 0, "disabled,enabled,accomplished,unaccomplished,grabbed,held,used,released,selected,unselected,broken,not_broken,new,in_progress,done,inspected,uninspected");
	PROP_PARAM(Switch, function, 0, "any,all,sequence");
	PROP_PARAM(Toggle, hierarchy_based);
	PROP_ARRAY_STRUCT(ObjectAndState, objects);

	template<typename Functor>
	int setOnStateChangedListener(Functor &&f)
	{
		return on_state_changed.addListener(std::forward<Functor>(f));
	}

	void removeOnStateChangedListener(int id)
	{
		on_state_changed.removeListener(id);
	}

	States getState();
	virtual void setInitialState();
	virtual void disabled() {};

	static int checkAny(Vector<LogicObject *> &objects, Vector<States> &states);
	static int checkAll(Vector<LogicObject *> &objects, Vector<States> &states);

protected:
	// callbacks
	virtual void on_init();
	virtual void on_set_initial_state();
	virtual void on_changed(States state);
	virtual void on_object_changed(LogicObject *object, States object_state) {};
	virtual void on_shutdown();

	void set_state(States new_state, bool force = false);

	template<typename T>
	void set_logic_objects_pairs()
	{
		static_assert(std::is_base_of<LogicObject, T>::value, "component is not inherited from LogicObject");

		for (int i = 0; i < handlers_id.size(); i++)
			logic_objects[i]->removeOnStateChangedListener(handlers_id[i]);
		handlers_id.clear();

		logic_objects.clear();
		needed_states.clear();
		handlers_id.clear();

		if (!objects.size() && !hierarchy_based.get())
		{
#if DEBUG
			Log::message("Logic Object: objects array is empty, please check %s\n", node->getName());
#endif
			return;
		}

		if (!hierarchy_based.get())
		{
			logic_objects.reserve(objects.size());
			needed_states.reserve(objects.size());

			for (int i = 0; i < objects.size(); i++)
			{
				if (!objects[i]->object)
				{
#if DEBUG
					Log::error("Logic Object error: object #%d is null, please check %s\n", i + 1, node->getName());
#endif
					continue;
				}

				T *logic_object = getComponent<T>(objects[i]->object.get());
				if (!logic_object)
				{
#if DEBUG
					Log::error("Logic Object error: object #%d not contain %s component, please check %s\n", i + 1, typeid(T).name(), node->getName());
#endif
					continue;
				}

				logic_objects.push_back(logic_object);
				needed_states.push_back((States)objects[i]->state.get());
			}
		}
		else
		{
			Vector<T *> child_logic_objects;
			getComponentsInChildren<T>(node, child_logic_objects);

			// first component is LogicObject of the node itself
			int i = 1;
			int reserve_size = child_logic_objects.size() - 1;

			// but for mission component Mission isn't Task
			if (typeid(T *) == typeid(Task *))
			{
				if (!child_logic_objects.size())
				{
#if DEBUG
					Log::message("Logic Object: objects array is empty, please check %s\n", node->getName());
#endif
					return;
				}

				i = 0;
				reserve_size++;
			}
			else if (child_logic_objects.size() <= 1)
			{
#if DEBUG
				Log::message("Logic Object: objects array is empty, please check %s\n", node->getName());
#endif
				return;
			}

			logic_objects.reserve(reserve_size);
			needed_states.reserve(reserve_size);

			for (; i < child_logic_objects.size(); i++)
				if (child_logic_objects[i]->isEnabled())
				{
					logic_objects.push_back(child_logic_objects[i]);
					needed_states.push_back((States)child_logic_objects[i]->final_state.get());
				}
		}

		handlers_id.reserve(logic_objects.size());
		for (int i = 0; i < logic_objects.size(); i++)
		{
			int id = logic_objects[i]->setOnStateChangedListener(std::bind(&LogicObject::on_state_changed_handler, this, std::placeholders::_1, std::placeholders::_2));
			handlers_id.push_back(id);
		}
	}

	Vector<LogicObject *> logic_objects;

private:
	void init();
	void shutdown();

	void on_state_changed_handler(LogicObject *object, States object_state);
	int check_state(LogicObject *object, States object_state);
	int any();
	int all();
	int sequence(LogicObject *object, States object_state);

	States state;
	int state_is_locked;
	
	Vector<States> needed_states;
	int sequence_step;
	int sequence_recovery;

	Vector<int> handlers_id;

	Action<void(LogicObject *object, States object_state)> on_state_changed;
};