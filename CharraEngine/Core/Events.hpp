#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>

namespace Charra
{
	enum class InputCode
	{
		NO_EVENT             = 0X00,
		
		// The following are relevant keyboard and mouse inputs,
		// values match win32 inputs
		LEFT_MOUSE_BUTTON    = 0X01,
		RIGHT_MOUSE_BUTTON   = 0X02,
		CANCEL               = 0X03,
		MIDDLE_MOUSE_BUTTON  = 0X04,
		X1_MOUSE_BUTTON      = 0X05,
		X2_MOUSE_BUTTON      = 0X05,
		BACKSPACE_KEY        = 0X08,
		TAB_KEY              = 0X09,
		CLEAR_KEY            = 0X0C,
		RETURN_KEY           = 0X0D,
		SHIFT_KEY            = 0X10,
		CONTROL_KEY          = 0X11,
		ALT_KEY              = 0X12,
		PAUSE_KEY            = 0X13,
		CAPS_LOCK_KEY        = 0X14,
		ESCAPE_KEY           = 0X1B,
		SPACE_KEY            = 0X20,
		PAGE_UP_KEY          = 0X21,
		PAGE_DOWN_KEY        = 0X22,
		END_KEY              = 0X23,
		HOME_KEY             = 0X24,
		LEFT_KEY             = 0X25,
		UP_KEY               = 0X26,
		RIGHT_KEY            = 0X27,
		DOWN_KEY             = 0X28,
		SELECT_KEY           = 0X29,
		PRINT_KEY            = 0X2A,
		EXECUTE_KEY          = 0X2B,
		PRINT_SCREEN_KEY     = 0X2C,
		INSERT_KEY           = 0X2D,
		DELETE_KEY           = 0X2E,
		ZERO_KEY             = 0X30,
		ONE_KEY              = 0X31,
		TWO_KEY              = 0X32,
		THREE_KEY            = 0X33,
		FOUR_KEY             = 0X34,
		FIVE_KEY             = 0X35,
		SIX_KEY              = 0X36,
		SEVEN_KEY            = 0X37,
		EIGHT_KEY            = 0X38,
		NINE_KEY             = 0X39,
		A_KEY                = 0X41,
		B_KEY                = 0X42,
		C_KEY                = 0X43,
		D_KEY                = 0X44,
		E_KEY                = 0X45,
		F_KEY                = 0X46,
		G_KEY                = 0X47,
		H_KEY                = 0X48,
		I_KEY                = 0X49,
		J_KEY                = 0X4A,
		K_KEY                = 0X4B,
		L_KEY                = 0X4C,
		M_KEY                = 0X4D, 
		N_KEY                = 0X4E,
		O_KEY                = 0X4F,
		P_KEY                = 0X50,
		Q_KEY                = 0X51,
		R_KEY                = 0X52,
		S_KEY                = 0X53,
		T_KEY                = 0X54,
		U_KEY                = 0X55,
		V_KEY                = 0X56,
		W_KEY                = 0X57,
		X_KEY                = 0X58,
		Y_KEY                = 0X59,
		Z_KEY                = 0X5A,
		LEFT_SUPER_KEY       = 0X5B,
		RIGHT_SUPER_KEY      = 0X5C,
		APPLICATION_KEY      = 0X5D,
		NUMPAD_ZERO_KEY      = 0X60,
		NUMPAD_ONE_KEY       = 0X61,
		NUMPAD_TWO_KEY       = 0X62,
		NUMPAD_THREE_KEY     = 0X63,
		NUMPAD_FOUR_KEY      = 0X64,
		NUMPAD_FIVE_KEY      = 0X65,
		NUMPAD_SIX_KEY       = 0X66,
		NUMPAD_SEVEN_KEY     = 0X67,
		NUMPAD_EIGHT_KEY     = 0X68,
		NUMPAD_NINE_KEY      = 0X69,
		NUMPAD_MULITPLY_KEY  = 0X6A,
		NUMPAD_ADD_KEY       = 0X6B,
		NUMPAD_SEPARATOR_KEY = 0X6C,
		NUMPAD_SUBTRACT_KEY  = 0X6D,
		NUMPAD_DECIMAL_KEY   = 0X6E,
		NUMPAD_DIVIDE_KEY    = 0X6F,
		F1_KEY               = 0X70,
		F2_KEY               = 0X71,
		F3_KEY               = 0X72,
		F4_KEY               = 0X73,
		F5_KEY               = 0X74,
		F6_KEY               = 0X75,
		F7_KEY               = 0X76,
		F8_KEY               = 0X77,
		F9_KEY               = 0X78,
		F10_KEY              = 0X79,
		F11_KEY              = 0X7A,
		F12_KEY              = 0X7B,
		F13_KEY              = 0X7C,
		F14_KEY              = 0X7D,
		F15_KEY              = 0X7E,
		F16_KEY              = 0X7F,
		F17_KEY              = 0X80,
		F18_KEY              = 0X81,
		F19_KEY              = 0X82,
		F20_KEY              = 0X83,
		F21_KEY              = 0X84,
		F22_KEY              = 0X85,
		F23_KEY              = 0X86,
		F24_KEY              = 0X87,
		LEFT_SHIFT_KEY       = 0XA0,
		RIGHT_SHIFT_KEY      = 0XA1,
		LEFT_CONTROL_KEY     = 0XA2,
		RIGHT_CONTROL_KEY    = 0XA3,
		LEFT_MENU_KEY        = 0XA4,
		RIGHT_MENU_KEY       = 0XA5,
		ALL_EVENTS
	};

	enum class EventType
	{
		NONE,
		MOUSE_CLICK,
		MOUSE_RELEASE,
		MOUSE_MOVE,
		KEY_DOWN,
		KEY_INPUT,
		KEY_UP,
		WINDOW_RESIZE,
		WINDOW_CLOSE,
		WINDOW_MINIMIZE,
		WINDOW_MAXIMIZE,
		APPLICATION_QUIT,
		APPLICATION_PAUSE,
		APPLICATION_RESUME,
		APPLICATION_CRASH,

		EVENT_TYPE_MAX
	};
	
	// eventType will indicate event types that are passed
	// The user is expected to return true if callback is handled and false otherwise
	// keyCode will be zero if the callback is not a keyboard input
	// eventHandle will be the same value that was return by register event so that user can easily unregister event from callback
	// data will be zero unless the callback is cursor position, where the upper 32 bits will be x, and lower 32 will be y,
	// or for text input it should be cast to wchar_t
	// private data will be nullptr unless the user specified data in registerEvent function
	typedef bool (*PFN_event_callback)(EventType type, InputCode code, uint64_t data, void* privateData);

	class Events
	{
		private: // Structs
			struct EventDetails
			{
				InputCode code;
				PFN_event_callback callback;
				void* privateData = nullptr;

				uint32_t windowIndex;
			};
			
		public:
			Events();
			~Events(); 

			// This will add a callback for an input type, and will need to be removed manually
			// WindowIndex is the window that the callback waits for, UINT32_MAX will apply to all windows
			// type is of a value in EventTypes enum,
			// code is an optional input if type is of character or key value
			// callback is the function that will be called if the event occurs
			// private data is a void pointer that will be passed back through callback and can be any user type
			void registerEventCallback(uint32_t windowIndex, EventType type, InputCode code, PFN_event_callback callback, void* privateData);

			// This will unregister the event by callback function
			void unregisterEventCallback(EventType type, PFN_event_callback callback);

			// This function will cause matching events callbacks to be fired
			// type is the eventType
			// code is the event code, can be 0 if it is not key event
			// data will be passed into the data member of the callback function
			void signalEvent(EventType type, InputCode code, uint64_t data);

			std::string& getTextBuffer() { return m_textBuffer; }
			void resetTextBuffer() { m_textBuffer.clear(); }

			void windowInFocus(uint32_t windowIndex) { m_windowInFocus = windowIndex; };

			bool isButtonDown(InputCode button);

		private: // Methods
			void updateStates(EventType type, InputCode code, uint64_t data);

		private: // Members
			// Each element of the array corresponds to an event type, this allows quicker lookup
			// and easier resizing when events are pushed and popped
			std::array<std::vector<EventDetails>, static_cast<size_t>(EventType::EVENT_TYPE_MAX) - 1> m_eventDetails;
			std::string m_textBuffer;

			uint32_t m_windowInFocus;

			// This will have some useless bytes because there are only 115 useful keys/buttons,
			// however this reduces computation
			std::array<bool, 166> m_buttonStates = {};
	};
};