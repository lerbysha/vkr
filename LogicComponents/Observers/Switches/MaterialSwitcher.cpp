#include "MaterialSwitcher.h"


REGISTER_COMPONENT(MaterialSwitcher);


void MaterialSwitcher::on_init()
{
	if ((!nodes.size() && !hierarchy_based.get()) || (hierarchy_based.get() && !node->getNumChildren()))
	{
#if DEBUG
		Log::message("Material Switcher: nodes array is empty, please check %s\n", node->getName());
#endif
		return;
	}

	MaterialPtr mat = activated.get() ? enabled_material.get() : disabled_material.get();

	if (!hierarchy_based.get())
	{
		for (int i = 0; i < nodes.size(); i++)
			if (!nodes[i])
			{
#if DEBUG
				Log::error("Material Switcher error: node #%d is null, please check %s\n", i + 1, node->getName());
#endif
			}
			else
			{
				ObjectPtr obj = checked_ptr_cast<Object>(nodes[i].get());
				if (obj)
				{
					for (int k = 0; k < obj->getNumSurfaces(); k++)
						obj->setMaterial(mat, k);
				}
			}
	}
	else
	{
		for (int i = 0; i < node->getNumChildren(); i++)
		{
			ObjectPtr obj = checked_ptr_cast<Object>(node->getChild(i));
			if (obj)
			{
				for (int k = 0; k < obj->getNumSurfaces(); k++)
					obj->setMaterial(mat, k);
			}
		}
	}
}

void MaterialSwitcher::on_activate()
{
	if (hierarchy_based.get())
	{
		for (int i = 0; i < node->getNumChildren(); i++)
		{
			ObjectPtr obj = checked_ptr_cast<Object>(node->getChild(i));
			if (obj)
			{
				for (int k = 0; k < obj->getNumSurfaces(); k++)
					obj->setMaterial(enabled_material.get(), k);
			}
		}
	}
	else
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			ObjectPtr obj = checked_ptr_cast<Object>(nodes[i].get());
			if (obj)
			{
				for (int k = 0; k < obj->getNumSurfaces(); k++)
					obj->setMaterial(enabled_material.get(), k);
			}
		}
	}

#if DEBUG
	Log::message("Materials is switched\n");
#endif
}

void MaterialSwitcher::on_deactivate()
{
	if (hierarchy_based.get())
	{
		for (int i = 0; i < node->getNumChildren(); i++)
		{
			ObjectPtr obj = checked_ptr_cast<Object>(node->getChild(i));
			if (obj)
			{
				for (int k = 0; k < obj->getNumSurfaces(); k++)
					obj->setMaterial(disabled_material.get(), k);
			}
		}
	}
	else
	{
		for (int i = 0; i < nodes.size(); i++)
		{
			ObjectPtr obj = checked_ptr_cast<Object>(nodes[i].get());
			if (obj)
			{
				for (int k = 0; k < obj->getNumSurfaces(); k++)
					obj->setMaterial(disabled_material.get(), k);
			}
		}
	}

#if DEBUG
	Log::message("Materials is switched\n");
#endif
}