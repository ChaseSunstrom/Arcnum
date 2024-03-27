#include "control.hpp"

namespace spark
{
	std::string to_string(key key)
	{
		switch (key)
		{
		case key::SPACE: return "SPACE";
		case key::APOSTROPHE: return "APOSTROPHE";
		case key::COMMA: return "COMMA";
		case key::MINUS: return "MINUS";
		case key::PERIOD: return "PERIOD";
		case key::SLASH: return "SLASH";
		case key::_0: return "0";
		case key::_1: return "1";
		case key::_2: return "2";
		case key::_3: return "3";
		case key::_4: return "4";
		case key::_5: return "5";
		case key::_6: return "6";
		case key::_7: return "7";
		case key::_8: return "8";
		case key::_9: return "9";
		case key::SEMICOLON: return "SEMICOLON";
		case key::EQUAL: return "EQUAL";
		case key::A: return "A";
		case key::B: return "B";
		case key::C: return "C";
		case key::D: return "D";
		case key::E: return "E";
		case key::F: return "F";
		case key::G: return "G";
		case key::H: return "H";
		case key::I: return "I";
		case key::J: return "J";
		case key::K: return "K";
		case key::L: return "L";
		case key::M: return "M";
		case key::N: return "N";
		case key::O: return "O";
		case key::P: return "P";
		case key::Q: return "Q";
		case key::R: return "R";
		case key::S: return "S";
		case key::T: return "T";
		case key::U: return "U";
		case key::V: return "V";
		case key::W: return "W";
		case key::X: return "X";
		case key::Y: return "Y";
		case key::Z: return "Z";
		case key::LEFT_BRACKET: return "LEFT_BRACKET";
		case key::BACKSLASH: return "BACKSLASH";
		case key::RIGHT_BRACKET: return "RIGHT_BRACKET";
		case key::GRAVE_ACCENT: return "GRAVE_ACCENT";
		case key::ESCAPE: return "ESCAPE";
		case key::ENTER: return "ENTER";
		case key::TAB: return "TAB";
		case key::BACKSPACE: return "BACKSPACE";
		case key::INSERT: return "INSERT";
		case key::DELETE: return "DELETE";
		case key::RIGHT: return "RIGHT";
		case key::LEFT: return "LEFT";
		case key::DOWN: return "DOWN";
		case key::UP: return "UP";
		case key::PAGE_UP: return "PAGE_UP";
		case key::PAGE_DOWN: return "PAGE_DOWN";
		case key::HOME: return "HOME";
		case key::END: return "END";
		case key::CAPS_LOCK: return "CAPS_LOCK";
		case key::SCROLL_LOCK: return "SCROLL_LOCK";
		case key::NUM_LOCK: return "NUM_LOCK";
		case key::PRINT_SCREEN: return "PRINT_SCREEN";
		case key::PAUSE: return "PAUSE";
		case key::F1: return "F1";
		case key::F2: return "F2";
		case key::F3: return "F3";
		case key::F4: return "F4";
		case key::F5: return "F5";
		case key::F6: return "F6";
		case key::F7: return "F7";
		case key::F8: return "F8";
		case key::F9: return "F9";
		case key::F10: return "F10";
		case key::F11: return "F11";
		case key::F12: return "F12";
		case key::F13: return "F13";
		case key::F14: return "F14";
		case key::F15: return "F15";
		case key::F16: return "F16";
		case key::F17: return "F17";
		case key::F18: return "F18";
		case key::F19: return "F19";
		case key::F20: return "F20";
		case key::F21: return "F21";
		case key::F22: return "F22";
		case key::F23: return "F23";
		case key::F24: return "F24";
		case key::F25: return "F25";
		case key::KP_0: return "KP_0";
		case key::KP_1: return "KP_1";
		case key::KP_2: return "KP_2";
		case key::KP_3: return "KP_3";
		case key::KP_4: return "KP_4";
		case key::KP_5: return "KP_5";
		case key::KP_6: return "KP_6";
		case key::KP_7: return "KP_7";
		case key::KP_8: return "KP_8";
		case key::KP_9: return "KP_9";
		case key::KP_DECIMAL: return "KP_DECIMAL";
		case key::KP_DIVIDE: return "KP_DIVIDE";
		case key::KP_MULTIPLY: return "KP_MULTIPLY";
		case key::KP_SUBTRACT: return "KP_SUBTRACT";
		case key::KP_ADD: return "KP_ADD";
		case key::KP_ENTER: return "KP_ENTER";
		case key::KP_EQUAL: return "KP_EQUAL";
		case key::LEFT_SHIFT: return "LEFT_SHIFT";
		case key::LEFT_CONTROL: return "LEFT_CONTROL";
		case key::LEFT_ALT: return "LEFT_ALT";
		case key::LEFT_SUPER: return "LEFT_SUPER";
		case key::RIGHT_SHIFT: return "RIGHT_SHIFT";
		case key::RIGHT_CONTROL: return "RIGHT_CONTROL";
		case key::RIGHT_ALT: return "RIGHT_ALT";
		case key::RIGHT_SUPER: return "RIGHT_SUPER";
		case key::MENU: return "MENU";
		default: return "UNKNOWN KEY";
		}
	}

	std::string to_string(mouse_button button)
	{
		switch (button)
		{
		case mouse_button::LEFT_CLICK: return "LEFT_CLICK";
		case mouse_button::RIGHT_CLICK: return "RIGHT_CLICK";
		case mouse_button::MIDDLE_CLICK: return "MIDDLE_CLICK";
		case mouse_button::MOUSE_BUTTON_1: return "MOUSE_BUTTON_1";
		case mouse_button::MOUSE_BUTTON_2: return "MOUSE_BUTTON_2";
		case mouse_button::MOUSE_BUTTON_3: return "MOUSE_BUTTON_3";
		case mouse_button::MOUSE_BUTTON_4: return "MOUSE_BUTTON_4";
		case mouse_button::MOUSE_BUTTON_5: return "MOUSE_BUTTON_5";
		default: return "UNKNOWN MOUSE BUTTON";
		}
	}

	std::string to_string(controller_button button)
	{
		switch (button)
		{
		case controller_button::A: return "A";
		case controller_button::B: return "B";
		case controller_button::X: return "X";
		case controller_button::Y: return "Y";
		case controller_button::LEFT_BUMPER: return "LEFT_BUMPER";
		case controller_button::RIGHT_BUMPER: return "RIGHT_BUMPER";
		case controller_button::BACK: return "BACK";
		case controller_button::START: return "START";
		case controller_button::GUIDE: return "GUIDE";
		case controller_button::LEFT_THUMB: return "LEFT_THUMB";
		case controller_button::RIGHT_THUMB: return "RIGHT_THUMB";
		case controller_button::DPAD_UP: return "DPAD_UP";
		case controller_button::DPAD_RIGHT: return "DPAD_RIGHT";
		case controller_button::DPAD_DOWN: return "DPAD_DOWN";
		case controller_button::DPAD_LEFT: return "DPAD_LEFT";
			// No need for separate cases for CROSS, CIRCLE, SQUARE, and TRIANGLE as they are aliases
		default: return "UNKNOWN CONTROLLER BUTTON";
		}
	}

}