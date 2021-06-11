#pragma once
#include "../Observer.h"


class MaterialSwitcher : public Observer
{
public:
	COMPONENT(MaterialSwitcher, Observer);

	PROP_NAME("material_switcher");
	PROP_AUTOSAVE(0);

	PROP_PARAM(Material, enabled_material);
	PROP_PARAM(Material, disabled_material);

	PROP_PARAM(Toggle, hierarchy_based);
	PROP_PARAM(Toggle, activated);
	PROP_ARRAY(Node, nodes);

private:
	void on_init() override;
	void on_activate() override;
	void on_deactivate() override;
};