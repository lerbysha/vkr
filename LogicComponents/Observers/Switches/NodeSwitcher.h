#pragma once
#include "../Observer.h"


class NodeSwitcher : public Observer
{
public:
	COMPONENT(NodeSwitcher, Observer);

	PROP_NAME("node_switcher");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Toggle, hierarchy_based);
	PROP_PARAM(Toggle, activated);
	PROP_ARRAY(Node, nodes);

	void disable() override;

private:
	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;
};