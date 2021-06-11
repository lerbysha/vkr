#include "NodeSwitcher.h"


REGISTER_COMPONENT(NodeSwitcher);


void NodeSwitcher::disable()
{
	if (hierarchy_based.get())
	{
		for (int i = 0; i < node->getNumChildren(); i++)
			node->getChild(i)->setEnabled(activated.get() != 0);
	}
	else
	{
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->setEnabled(activated.get() != 0);
	}

	Observer::disable();
}

void NodeSwitcher::on_init()
{
	if ((!nodes.size() && !hierarchy_based.get()) || (hierarchy_based.get() && !node->getNumChildren()))
	{
#if DEBUG
		Log::message("Node Switcher: nodes array is empty, please check %s\n", node->getName());
#endif
		return;
	}

	if (!hierarchy_based.get())
	{
		for (int i = 0; i < nodes.size(); i++)
			if (!nodes[i])
			{
#if DEBUG
				Log::error("Node Switcher error: node #%d is null, please check %s\n", i + 1, node->getName());
#endif
			}
			else
				nodes[i].get()->setEnabled(activated.get() != 0);
	}
	else
	{
		for (int i = 0; i < node->getNumChildren(); i++)
			node->getChild(i)->setEnabled(activated.get() != 0);
	}
}

void NodeSwitcher::on_activate()
{
	if (hierarchy_based.get())
	{
		for (int i = 0; i < node->getNumChildren(); i++)
			node->getChild(i)->setEnabled(!activated.get());
	}
	else
	{
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->setEnabled(!activated.get());
	}

#if DEBUG
	Log::message("Nodes is activate\n");
#endif
}

void NodeSwitcher::on_deactivate()
{
	if (hierarchy_based.get())
	{
		for (int i = 0; i < node->getNumChildren(); i++)
			node->getChild(i)->setEnabled(activated.get() != 0);
	}
	else
	{
		for (int i = 0; i < nodes.size(); i++)
			nodes[i]->setEnabled(activated.get() != 0);
	}

#if DEBUG
	Log::message("Nodes is deactivate\n");
#endif
}