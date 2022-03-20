#include "Events.hpp"

#include "Platform/Platform.hpp"
#include "Core/logging.hpp"

namespace Charra
{
	Events::Events()
	{
		m_eventDetails.fill(std::vector<EventDetails>());
	}

	Events::~Events()
	{
		// This should call all application destroy events from here
	}

	void Events::registerEventCallback(uint32_t windowIndex, EventType type, InputCode code, PFN_event_callback callback, void* privateData)
	{
		int typeIndex = static_cast<int>(type);

		if(!callback || typeIndex == 0)
			return;
		
		m_eventDetails[typeIndex - 1].emplace_back(EventDetails(code, callback, privateData, windowIndex));
	}

	void Events::unregisterEventCallback(EventType type, PFN_event_callback eventCallback)
	{	
		int typeIndex = static_cast<int>(type);
		typeIndex--; // This accounts for NO_EVENT
		auto it = m_eventDetails[typeIndex].begin();
		for(it; it < m_eventDetails[typeIndex].end(); it++)
		{
			if(it->callback == eventCallback)
			{
				m_eventDetails[typeIndex].erase(it);
				return;
			}
		}
	}

	void Events::signalEvent(EventType type, InputCode code, uint64_t data)
	{
		int typeIndex = static_cast<int>(type);
		typeIndex--; // This accounts for NO_EVENT

		if(type == EventType::KEY_INPUT)
		{
			if(m_textBuffer.size() > 100)
			{
				m_textBuffer.clear();
			}

			if((InputCode)data == InputCode::BACKSPACE_KEY && m_textBuffer.size() > 0)
			{
				m_textBuffer.pop_back();
			}
			else
			{
				m_textBuffer += ((wchar_t)data);
			}
		}

		if(type == EventType::MOUSE_RELEASE)
		{
			// TODO handle key and mouse states and check if mouse was down first;
		}

		auto it = m_eventDetails[typeIndex].begin();
		for (it; it < m_eventDetails[typeIndex].end(); it++)
		{
			if(it->code == code || it->code == InputCode::ALL_EVENTS && it->windowIndex == m_windowInFocus)
			{
				if(it->callback(type, code, data, it->privateData))
				{
					break;
				}
				else
				{
					CHARRA_LOG_ERROR(true, "Event callback returned false");
				}
			}
		}
	}
}