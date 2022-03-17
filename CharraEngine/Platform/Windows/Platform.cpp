#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

#include "Platform/platform.hpp"
#include "Platform/Timer.hpp"

#include <iostream>
#include <vector>

#include "Core/Application.hpp"
#include "Core/Events.hpp"
#include "Core/Logging.hpp"

#include <Windows.h>
#include <windowsx.h>
#include <wincon.h>
#include <dwmapi.h>
#include <timeapi.h>

#pragma comment(lib, "winmm.lib") /* include winmm.lib */
#pragma comment(lib, "Dwmapi.lib") /* include 	Dwmapi.lib */

namespace Charra
{
	namespace Platform
	{
		struct ResizeData
		{
			bool resizing = false;
			RECT windowDimensions;
			uint32_t startX;
			uint32_t startY;
			LRESULT startingArea;
			iVec2 moveOffset;
		};

		struct PlatformData
		{
			HINSTANCE instance;
			std::vector<HWND> windows;
			uint32_t currentIdGen = 0; // For unique id
			std::vector<uint32_t> windowIds;
			std::vector<LPCSTR> windowNames;
			std::vector<iVec2> windowDimensions;
			LPCSTR applicationName;
			WNDCLASS windowClass;

			Charra::Events *eventHandler;
			LARGE_INTEGER timerFrequency;

			ResizeData resizeData;

			HCURSOR cursorNS;
			HCURSOR cursorWE;
			HCURSOR cursorNW;
			HCURSOR cursorNE;
			HCURSOR cursorDefault;
			HCURSOR cursorSizeAll;
		};

		static PlatformData g_platformData;

		std::vector<const char*> getExtensions()
        {
            std::vector<const char*> result;
            result.push_back("VK_KHR_win32_surface");
            result.push_back("VK_KHR_surface");
            result.push_back("VK_EXT_debug_utils");
            return result;
        }
        
        VkSurfaceKHR getSurface(VkInstance instance)
        {
            VkWin32SurfaceCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.pNext;
            createInfo.flags;
            createInfo.hinstance = g_platformData.instance;
            createInfo.hwnd = g_platformData.windows.back();

			VkSurfaceKHR surface;
            vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface);
			return surface;
        }

		std::vector<char> readFile(const char* fileName, uint32_t* fileSize)
		{
			HANDLE file = NULL;
			DWORD fileLength = 0;
			file = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			fileLength = GetFileSize(file, NULL);

			std::vector<char> fileContents(fileLength);
			DWORD fileRead;
			ReadFile(file, fileContents.data(), fileLength, &fileRead, NULL);

			DeleteFile(fileName);

			*fileSize = static_cast<uint32_t>(fileRead);

			CHARRA_LOG_ERROR(*fileSize != fileContents.size(), "Only part of the file was read");

			// TODO error checking, and much much much more to do with file io
			return fileContents;
		}

		void createWindow(const std::string& windowName, iVec2 size, iVec2 position)
		{
			// TODO drag and drop files is changed here check msdn extended window styles

			HWND window = CreateWindowExA(
				0,
				g_platformData.applicationName,
				windowName.c_str(),
				WS_POPUP|WS_VISIBLE|WS_SYSMENU,
				position.x,
				position.y,
				size.width,
				size.height,
				0,
				0,
				g_platformData.instance,
				0);

			CHARRA_LOG_ERROR(window == NULL, "Win32 could not create the window");
			
			ShowWindow(window, SW_NORMAL);

			g_platformData.windows.push_back(window);
			// This should be a unique ID
			g_platformData.windowIds.push_back(g_platformData.currentIdGen++);
			g_platformData.windowNames.push_back(windowName.c_str());
			g_platformData.windowDimensions.push_back({0,0});
		}

		static void moveWindow(uint32_t windowIndex, iVec2 position, iVec2 size)
		{
			//MoveWindow(g_platformData.windows[windowIndex], position.x, position.y, size.width, size.height, false);
			HRESULT result = SetWindowPos(g_platformData.windows[windowIndex], 0, position.x, position.y, size.width, size.height, SWP_DEFERERASE | SWP_NOZORDER | SWP_NOCOPYBITS | SWP_NOREDRAW | SWP_NOSENDCHANGING);
			
		}

		void fullscreen(int windowIndex, bool fullscreen)
		{
			// TODO
		}

		void pollEvents()
		{
			MSG msg = { };
			while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		bool shouldAppQuit()
		{
			return g_platformData.windows.size() == 0;
		}

		void printMessage(const std::string& message, ConsoleColours colour)
		{

			HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			WORD windowsColourCode;
			switch(colour)
			{
				case ConsoleColours::RED:
					windowsColourCode = FOREGROUND_RED;
					break;
				case ConsoleColours::GREEN:
					windowsColourCode = FOREGROUND_GREEN;
					break;
				case ConsoleColours::BLUE:
					windowsColourCode = FOREGROUND_BLUE;
					break;
				case ConsoleColours::YELLOW:
					windowsColourCode = FOREGROUND_RED | FOREGROUND_GREEN;
					break;
				case ConsoleColours::PURPLE:
					windowsColourCode = FOREGROUND_BLUE | FOREGROUND_RED;
					break;
				case ConsoleColours::CYAN:
					windowsColourCode = FOREGROUND_BLUE | FOREGROUND_GREEN;
					break;
				case ConsoleColours::WHITE:
					windowsColourCode = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				case ConsoleColours::BRIGHT_RED:
					windowsColourCode = FOREGROUND_RED | FOREGROUND_INTENSITY;
					break;
				case ConsoleColours::BRIGHT_GREEN:
					windowsColourCode = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					break;
				case ConsoleColours::BRIGHT_BLUE:
					windowsColourCode = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;
				case ConsoleColours::BRIGHT_YELLOW:
					windowsColourCode = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					break;
				case ConsoleColours::BRIGHT_PURPLE:
					windowsColourCode = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
					break;
				case ConsoleColours::BRIGHT_CYAN:
					windowsColourCode = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
					break;
				case ConsoleColours::BRIGHT_WHITE:
					windowsColourCode = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
					break;
			}

			SetConsoleTextAttribute(consoleHandle, windowsColourCode);
			OutputDebugStringA(message.c_str());
		}

		static void handleResizing(HWND window)
		{
			RECT dimensions = g_platformData.resizeData.windowDimensions;
			uint32_t &startX = g_platformData.resizeData.startX;
			uint32_t &startY = g_platformData.resizeData.startY;

			POINT cursor;
			GetCursorPos(&cursor);

			switch(g_platformData.resizeData.startingArea)
			{
				case HTTOP:
					dimensions.top = cursor.y;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTBOTTOM:
					dimensions.bottom = cursor.y;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTLEFT:
					dimensions.left = cursor.x;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTRIGHT:
					dimensions.right = cursor.x;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					//CHARRA_LOG_INFO(true, std::to_string(endX));
					break;
				case HTTOPLEFT:
					dimensions.top = cursor.y;
					dimensions.left = cursor.x;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTTOPRIGHT:
					dimensions.top = cursor.y;
					dimensions.right = cursor.x;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTBOTTOMRIGHT:
					dimensions.bottom = cursor.y;
					dimensions.right = cursor.x;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTBOTTOMLEFT:
					dimensions.bottom = cursor.y;
					dimensions.left = cursor.x;
					startX = cursor.x;
					startY = cursor.y;
					g_platformData.resizeData.windowDimensions = dimensions;
					break;
				case HTCAPTION:
					dimensions.left -= startX - cursor.x;// - g_platformData.resizeData.moveOffset.x;
					dimensions.right -= startX - cursor.x;
					dimensions.top -= startY - cursor.y;// - g_platformData.resizeData.moveOffset.y;
					dimensions.bottom -= startY - cursor.y;
					break;
			}

			for(int i = 0; i < g_platformData.windows.size(); i++)
			{
				if(g_platformData.windows[i] == window)
				{ 
					iVec2 position = {static_cast<uint32_t>(dimensions.left), static_cast<uint32_t>(dimensions.top)};
					iVec2 size = {static_cast<uint32_t>(dimensions.right - dimensions.left), static_cast<uint32_t>(dimensions.bottom - dimensions.top)};

					if(size.width < 50 || size.height < 50 || dimensions.right <= dimensions.left || dimensions.bottom <= dimensions.top)
					{
						return;
					}

					moveWindow(i, position, size);

					if(g_platformData.resizeData.startingArea != HTCAPTION)
					{
						g_platformData.eventHandler->signalEvent(EventType::WINDOW_RESIZE, InputCode::NO_EVENT, (uint64_t)((uint64_t)size.width << 32 | size.height));
					}

					break;
				}
			}
		}

		static LRESULT customHitTest(HWND window, int x, int y)
		{
			RECT windowSize;
			GetClientRect(window, &windowSize);
			POINT cursor = {x, y};
			int borderWidth = 4;
			int menuHeight = 10;

			RECT clientArea = {windowSize.left + borderWidth, windowSize.top + borderWidth + menuHeight, windowSize.right - borderWidth, windowSize.bottom - borderWidth};
			if(PtInRect(&clientArea, cursor))
			{
				return HTCLIENT;
			}

			RECT leftResize = {windowSize.left, windowSize.top + borderWidth, windowSize.left + borderWidth, windowSize.bottom - borderWidth};
			if(PtInRect(&leftResize, cursor))
			{
				return HTLEFT;
			}

			RECT bottomResize = {windowSize.left + borderWidth, windowSize.bottom - borderWidth, windowSize.right - borderWidth, windowSize.bottom};
			if(PtInRect(&bottomResize, cursor))
			{
				return HTBOTTOM;
			}

			RECT rightResize = {windowSize.right - borderWidth, windowSize.top + borderWidth, windowSize.right, windowSize.bottom - borderWidth};
			if(PtInRect(&rightResize, cursor))
			{
				return HTRIGHT;
			}

			RECT topResize = {windowSize.left + borderWidth, windowSize.top, windowSize.right - borderWidth, windowSize.top + borderWidth};
			if(PtInRect(&topResize, cursor))
			{
				return HTTOP;
			}

			RECT bottomLeftResize = {windowSize.left, windowSize.bottom - borderWidth, windowSize.left + borderWidth, windowSize.bottom};
			if(PtInRect(&bottomLeftResize, cursor))
			{
				return HTBOTTOMLEFT;
			}

			RECT bottomRightResize = {windowSize.right - borderWidth, windowSize.bottom - borderWidth, windowSize.right, windowSize.bottom};
			if(PtInRect(&bottomRightResize, cursor))
			{
				return HTBOTTOMRIGHT;
			}

			RECT topLeftResize = {windowSize.left, windowSize.top, windowSize.left + borderWidth, windowSize.top + borderWidth};
			if(PtInRect(&topLeftResize, cursor))
			{
				return HTTOPLEFT;
			}

			RECT topRightResize = {windowSize.right - borderWidth, windowSize.top, windowSize.right, windowSize.top + borderWidth};
			if(PtInRect(&topRightResize, cursor))
			{
				return HTTOPRIGHT;
			}

			RECT dragBar = {windowSize.left + borderWidth, windowSize.top + borderWidth, windowSize.right - borderWidth, windowSize.top + borderWidth + menuHeight};
			if(PtInRect(&dragBar, cursor))
			{
				return HTCAPTION;
			}


			return HTNOWHERE;
		}

		LRESULT WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
		{
			uint32_t mouseX;
			uint32_t mouseY;
			iVec2 newSize(0, 0);
			uint32_t result = 0;
			LRESULT hitResult;
			POINT cursorPos;


			switch(message)
			{
				//case WM_NCHITTEST:
				//	returnVal = customHitTest(window, lParam);
				//	if(result )
				//	return returnVal;

				case WM_SIZING:
					return 0;

				case WM_QUIT:
					PostQuitMessage(0);
					return 0;

				case WM_DESTROY:
					for(int i = 0; i < g_platformData.windows.size(); i++)
					{
						if(g_platformData.windows[i] == window)
						{
							g_platformData.windowDimensions.erase(g_platformData.windowDimensions.begin() + i);
							g_platformData.windowNames.erase(g_platformData.windowNames.begin() + i);
							g_platformData.windows.erase(g_platformData.windows.begin() + i);
							g_platformData.windowIds.erase(g_platformData.windowIds.begin() + i);
						}
					}
					
					DestroyWindow(window);
					
					return 0;

				case WM_LBUTTONDOWN:
					SetCapture(window);

					RECT windowDimensions;
					CHARRA_LOG_WARNING(!GetWindowRect(window, &windowDimensions), "Win32 could not get widnow rect");
					mouseX = GET_X_LPARAM(lParam);
					mouseY = GET_Y_LPARAM(lParam);

					hitResult = customHitTest(window, mouseX, mouseY);
					if(hitResult != HTCLIENT)
					{
						GetCursorPos(&cursorPos);

						g_platformData.resizeData.resizing = true;
						g_platformData.resizeData.startingArea = hitResult;
						g_platformData.resizeData.startX = cursorPos.x;
						g_platformData.resizeData.startY = cursorPos.y;
						g_platformData.resizeData.windowDimensions = windowDimensions;

						if(hitResult == HTCAPTION)
						{
							g_platformData.resizeData.moveOffset.x = mouseX;
							g_platformData.resizeData.moveOffset.y = mouseY;
						}
						return 0;
					}


					g_platformData.eventHandler->signalEvent(EventType::MOUSE_CLICK, InputCode::LEFT_MOUSE_BUTTON, lParam);
					return 0;

				case WM_NCLBUTTONUP:
				case WM_LBUTTONUP:
					ReleaseCapture();
					
					if(g_platformData.resizeData.resizing)
					{
						g_platformData.resizeData.resizing = false;
						return 0;
					}

					g_platformData.eventHandler->signalEvent(EventType::MOUSE_RELEASE, InputCode::LEFT_MOUSE_BUTTON, lParam);
					return 0;
				
				case WM_RBUTTONDOWN:
					g_platformData.eventHandler->signalEvent(EventType::MOUSE_CLICK, InputCode::RIGHT_MOUSE_BUTTON, lParam);
					return 0;
				
				case WM_RBUTTONUP:
					g_platformData.eventHandler->signalEvent(EventType::MOUSE_RELEASE, InputCode::RIGHT_MOUSE_BUTTON, lParam);
					return 0;
				
				case WM_MBUTTONDOWN:
					g_platformData.eventHandler->signalEvent(EventType::MOUSE_CLICK, InputCode::MIDDLE_MOUSE_BUTTON, lParam);
					return 0;

				case WM_MBUTTONUP:
					g_platformData.eventHandler->signalEvent(EventType::MOUSE_RELEASE, InputCode::MIDDLE_MOUSE_BUTTON, lParam);
					return 0;
				// TODO x1 x2 mouse buttons

				//case WM_NCMOUSEMOVE:	
				//	if(!g_platformData.resizeData.resizing)
				//	return DefWindowProc(window, message, wParam, lParam);

				case WM_NCMOUSEMOVE:
				case WM_MOUSEMOVE:

					if(g_platformData.resizeData.resizing)
					{
						handleResizing(window);
						return 0;
					}

					g_platformData.eventHandler->signalEvent(EventType::MOUSE_MOVE, InputCode::NO_EVENT, lParam);			
					return 0;

				case WM_KEYDOWN:
					// TODO this is temporary
					if (static_cast<InputCode>(wParam) == InputCode::F1_KEY)
					{
						DestroyWindow(window);

						for(int i = 0; i < g_platformData.windows.size(); i++)
						{
							if(g_platformData.windows[i] == window)
							{
								g_platformData.windowDimensions.erase(g_platformData.windowDimensions.begin() + i);
								g_platformData.windowNames.erase(g_platformData.windowNames.begin() + i);
								g_platformData.windows.erase(g_platformData.windows.begin() + i);
							}
						}

					}
					g_platformData.eventHandler->signalEvent(EventType::KEY_DOWN, static_cast<InputCode>(wParam), 0);
					return 0;

				case WM_CHAR:
					if(wParam == 0x000D) // windows provides CR and I need LF
					{
						wParam = (WPARAM)0x000A;
					}
					g_platformData.eventHandler->signalEvent(EventType::KEY_INPUT, InputCode::NO_EVENT, wParam);
					return 0;

				case WM_KEYUP:
					g_platformData.eventHandler->signalEvent(EventType::KEY_UP, static_cast<InputCode>(wParam), 0);
					return 0;

				case WM_SETCURSOR:
					mouseX = GET_X_LPARAM(lParam);
					mouseY = GET_Y_LPARAM(lParam);
					GetCursorPos(&cursorPos);
					ScreenToClient(window, &cursorPos);
					hitResult = customHitTest(window, cursorPos.x, cursorPos.y);

					if(hitResult == HTBOTTOM || hitResult == HTTOP)
					{
						SetCursor(g_platformData.cursorNS);
					}
					else if(hitResult == HTRIGHT || hitResult == HTLEFT)
					{
						SetCursor(g_platformData.cursorWE);
					}
					else if(hitResult == HTTOPLEFT || hitResult == HTBOTTOMRIGHT)
					{
						SetCursor(g_platformData.cursorNW);
					}
					else if(hitResult == HTTOPRIGHT || hitResult == HTBOTTOMLEFT)
					{
						SetCursor(g_platformData.cursorNE);
					}
					else if(hitResult == HTCAPTION)
					{
						SetCursor(g_platformData.cursorSizeAll);
					}
					else
					{
						SetCursor(g_platformData.cursorDefault);
					}
			}

			return DefWindowProc(window, message, wParam, lParam);
		}

		void initPlatform(LPCSTR applicationName, HINSTANCE instance, Events *eventHandler)
		{
			g_platformData.instance = instance;
			g_platformData.applicationName = applicationName;
			g_platformData.eventHandler = eventHandler;

			WNDCLASS wc{};
			wc.lpfnWndProc   = WindowProc;
			// TODO verify that instance exists
			wc.hInstance     = instance;
			wc.lpszClassName = applicationName;

			RegisterClass(&wc);

			g_platformData.windowClass = wc;

			g_platformData.cursorNS = LoadCursor(NULL, IDC_SIZENS);
			g_platformData.cursorWE = LoadCursor(NULL, IDC_SIZEWE);
			g_platformData.cursorNE = LoadCursor(NULL, IDC_SIZENESW);
			g_platformData.cursorNW = LoadCursor(NULL, IDC_SIZENWSE);
			g_platformData.cursorDefault = LoadCursor(NULL, IDC_ARROW);
			g_platformData.cursorSizeAll = LoadCursor(NULL, IDC_SIZEALL);
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	Charra::Application application;
	
	Charra::Platform::initPlatform("Charra Engine", hInst, application.getEventHandler());

	TIMECAPS resolution;
	timeGetDevCaps(&resolution, sizeof(TIMECAPS));

	timeBeginPeriod(resolution.wPeriodMin); 

	Charra::Timer::initTimer();
	
	application.run();

	timeEndPeriod(resolution.wPeriodMin);

	return 0;
}