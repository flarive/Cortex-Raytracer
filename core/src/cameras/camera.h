#pragma once

class camera
{
public:
	virtual ~camera() = default;

	virtual bool test() const;
};