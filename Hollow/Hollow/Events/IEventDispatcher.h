#pragma once
#include "Hollow/Platform.h"
#include <unordered_map>
#include "EventDelegate.h"

namespace Hollow {

	class IEventDispatcher
	{
	public:

		virtual ~IEventDispatcher()
		{}

		virtual void Dispatch(IEvent* event) = 0;

		virtual void AddEventCallback(IEventDelegate* const eventDelegate) = 0;

		virtual void RemoveEventCallback(IEventDelegate* eventDelegate) = 0;

		virtual inline size_t GetEventCallbackCount() const = 0;
	};

}