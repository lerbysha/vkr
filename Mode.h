#pragma once

class Mode
{
public:
	virtual ~Mode() {}

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void shutdown() = 0;
};