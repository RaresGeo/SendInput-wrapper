#pragma once
#include <Windows.h>
#include <chrono>

class sendinput_wrapper
{
private:
	INPUT mouse;
	INPUT kb;

	std::chrono::high_resolution_clock time;
public:
	sendinput_wrapper()
	{
		// Keyboard
		kb.type = INPUT_KEYBOARD;
		kb.ki.wScan = 0;
		kb.ki.time = 0;
		kb.ki.dwExtraInfo = 0;

		// Mouse
		mouse.type = INPUT_MOUSE;
		mouse.mi.mouseData = 0;
		mouse.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
		mouse.mi.time = 0;
		mouse.mi.dwExtraInfo = 0;
	}

	void pause(int delay = 1)
	{
		auto start = time.now();
		while ((time.now() - start) < std::chrono::milliseconds(delay))
		{

		}
	}

	bool parse_key(WORD &key)
	{
		// ascii table and virtual key codes are a little different, we need to check this
		if (key >= 97 && key <= 122)
		{
			key -= 32; // Turns ascii into respective virtual key code
			return false; // key is lower case, we don't have to use shift
		}

		// Is upper case, we need to work with shift
		// Virtual key code for 'a' is 65, (ascii code for 'A') so we don't need to change it
		else if (key >= 65 && key <= 90)
			return true;

		else  // key is not a letter, we'll check if we still have to use shift
		{
			if (key == '?')
			{
				key = VK_OEM_2;
				return true;
			}
			else if (key == '/')
			{
				key = VK_OEM_2;
				return false;
			}
			else return false;
		}
	}

	void key_down(WORD key, int delay = 1)
	{
		bool shift = parse_key(key);

		// We can check caps lock state with GetKeyState(VK_CAPITAL), in order to always type letters corectly. But that will mess with shift symbols like / ?
		// One way to circumvent that is to have two different parse functions for different uses of shift, but we'll go with Occam's razor
		if(shift)
			key_down(VK_SHIFT, 0);

		kb.ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
		kb.ki.wVk = key;
		kb.ki.dwFlags = KEYEVENTF_SCANCODE;

		SendInput(1, &kb, sizeof(INPUT));

		// Disable shift
		if(shift)
			key_up(VK_SHIFT, 0);

		pause(delay);

	}

	void key_up(WORD key, int delay = 1)
	{
		kb.ki.wScan = MapVirtualKey(key, MAPVK_VK_TO_VSC);
		kb.ki.wVk = key;
		kb.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		SendInput(1, &kb, sizeof(INPUT));

		pause(delay);
	}

	void key_tap(WORD key, int hold_time = 1, int delay = 1)
	{
		key_down(key, hold_time);
		key_up(key, delay);
	}

	void mouse_move(LONG x, LONG y)
	{
		mouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_MOVE);
		mouse.mi.dx = x;
		mouse.mi.dy = y;
		SendInput(1, &mouse, sizeof(INPUT));
	}

	void mouse_move_r(LONG x, LONG y) // relative to current mouse position
	{
		mouse.mi.dwFlags = (MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_MOVE);
		mouse.mi.dx = x;
		mouse.mi.dy = y;
		SendInput(1, &mouse, sizeof(INPUT));
	}

	void lmb_down(int delay = 1)
	{
		mouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
		SendInput(1, &mouse, sizeof(INPUT));

		pause(delay);
	}

	void lmb_up(int delay = 1)
	{
		mouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
		SendInput(1, &mouse, sizeof(INPUT));

		pause(delay);
	}

	void rmb_down(int delay = 1)
	{
		mouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN);
		SendInput(1, &mouse, sizeof(INPUT));

		pause(delay);
	}

	void rmb_up(int delay = 1)
	{
		mouse.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTUP);
		SendInput(1, &mouse, sizeof(INPUT));

		pause(delay);
	}

	void lmb_click(int hold_time = 1, int delay = 1)
	{
		lmb_down(hold_time);
		lmb_up(delay);
	}

	void rmb_click(int hold_time = 1, int delay = 1)
	{
		rmb_down(hold_time);
		rmb_up(delay);
	}

	void macro(char* s, int typing_speed = 1)
	{
		for (int i = 0; i < strlen(s); i++)
			key_tap(s[i], typing_speed);
	}
};