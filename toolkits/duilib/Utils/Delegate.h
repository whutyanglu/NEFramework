#ifndef __UIDELEGATE_H__
#define __UIDELEGATE_H__

#pragma once

#include "duilib/Core/Define.h"

namespace ui {

using EventCallback = std::function<bool(const EventArgs &)>;

class UILIB_API EventSource : public std::vector<EventCallback>
{
public:
	EventSource& operator += (const EventCallback & other)
	{
		push_back(other);
		return *this;
	}

	bool operator()(const EventArgs &arg) const
	{
		for (auto it : *this)
		{
			if(!it(arg)) return false;
		}

		return true;
	}

private:
	
};

typedef std::map<EventType, EventSource> EventMap;

} // namespace DuiLib

#endif // __UIDELEGATE_H__