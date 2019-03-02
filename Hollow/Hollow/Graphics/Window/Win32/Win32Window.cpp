#include "Win32Window.h"

HWND Win32Window::hWnd = nullptr;

Win32Window::Win32Window(HINSTANCE hInst, int width, int height)
	: hInst(hInst)
{
	// Creating window class
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = _HandleMsgSetup;
	windowClass.cbClsExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = "DirectXAppClassName";
	RegisterClassEx(&windowClass);

	// Center positions
	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	// Creating rectangle
	RECT windowRect;
	windowRect.left = centerScreenX;
	windowRect.right = width + windowRect.left;
	windowRect.top = centerScreenY;
	windowRect.bottom = height + windowRect.top;
	AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	hWnd = CreateWindow("DirectXAppClassName", "Hollow", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		windowRect.left, windowRect.top,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInst, this);

	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0;   // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = 0;

	RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
}

LRESULT WINAPI Win32Window::_HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Win32Window* const pWnd = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Win32Window::_HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Win32Window::_HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to window class
	Win32Window* const pWnd = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Win32Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	LRESULT result = 0;
	switch (msg)
	{
	case WM_INPUT:
	{
		UINT dataSize;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT * input = reinterpret_cast<RAWINPUT *>(rawData.get());
				if (input->header.dwType == RIM_TYPEMOUSE)
				{
					InputManager::mx = input->data.mouse.lLastX;
					InputManager::my = input->data.mouse.lLastY;
				}
			}
		}
	} break;
	case WM_CLOSE:
	{
		OutputDebugStringA("WM_CLOSE\n");
		PostQuitMessage(0);
	} break;
	case WM_KEYDOWN:
	{
		InputManager::SetKeyboardKeyActive(static_cast<eKeyCodes>(wParam), true);
	} break;
	case WM_KEYUP:
	{
		InputManager::SetKeyboardKeyActive(static_cast<eKeyCodes>(wParam), false);
	} break;
	// Mouse events handling
	case WM_LBUTTONDOWN:
	{
		InputManager::SetMouseButtonActive(eMouseKeyCodes::MOUSE_LEFT, true);
	} break;
	case WM_RBUTTONDOWN:
	{
		InputManager::SetMouseButtonActive(eMouseKeyCodes::MOUSE_RIGHT, true);
	} break;
	case WM_MBUTTONDOWN:
	{
		InputManager::SetMouseButtonActive(eMouseKeyCodes::MOUSE_MIDDLE, true);
	} break;
	case WM_LBUTTONUP:
	{
		InputManager::SetMouseButtonActive(eMouseKeyCodes::MOUSE_LEFT, false);
	} break;
	case WM_RBUTTONUP:
	{
		InputManager::SetMouseButtonActive(eMouseKeyCodes::MOUSE_RIGHT, false);
	} break;
	case WM_MBUTTONUP:
	{
		InputManager::SetMouseButtonActive(eMouseKeyCodes::MOUSE_MIDDLE, false);
	} break;
	case WM_MOUSEMOVE:
	{
		InputManager::mcx = GET_X_LPARAM(lParam);
		InputManager::mcy = GET_Y_LPARAM(lParam);
	} break;
	default:
		result = DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return result;
}

bool Win32Window::ProcessMessage()
{
	MSG message;
	while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			SetIsClosed(true);
			return false;
		}
	}
	return true;
}

HWND * Win32Window::getHWND()
{
	return &hWnd;
}

void Win32Window::m_UpdateWindowState()
{

}