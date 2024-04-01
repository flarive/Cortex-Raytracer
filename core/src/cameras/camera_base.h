#pragma once

class camera_base
{
public:
	virtual ~camera_base() = default;

	virtual bool test() const;

};

