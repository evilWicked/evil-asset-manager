#pragma once

#ifdef _WIN32

#define EVIL_WIN

//visual leak detector. 
//If you enable this note that visual leak detector sometimes reports some false
//positives due to top level objects not being deleted after the leak detection code 
#include <vld.h> 

#include <windows.h>

#elif __APPLE__
#define EVIL_OSX
#endif

#include <iostream>


//#include <vector>
//#include <string>
//#include <unordered_set>
//#include <queue>

//include the main header as you normally would
#include "evil/asset-manager/asset-manager.h"

//bring in the wrapper to bring in all the source
#include "evil/asset-manager/evil-wrapper.cpp"

//You would normally not use these

//bring in our test renderer and sprite dev and testing only!!!!
#include "evil/test-renderer/glfw-context.h"
#include "evil/test-renderer/window-manager.h"
#include "evil/test-renderer/test-sprite.h"


#include "evil/signals/slot.h"

#include "evil/timer/timer.h"

//bring in the evil test harness 
#include "evil/test-harness/test-harness.h"

//bring in the test wrapper which includes the test harness implementation files
#include "evil/test-harness/evil-wrapper.cpp"

//hack
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include "../evil-dependencies/glfw/include/GLFW/glfw3native.h"

//end hack


int main(int argc, char* argv[])
{

	evil::GLFWContext glcontext;
	evil::WindowManager manager(glcontext);

	evil::Timer timer;

	//create a slot so we can be notified if window 1 is closed - we will use that to stop 
	//our processing
	evil::Slot<const evil::WindowStateEvent &> slot_closed;
	slot_closed.attach([&](const evil::WindowStateEvent &x) {
		if(x.type == EVIL_WINDOW_EVT_CLOSE) {

			int debug = 1;
			manager.stop();
		}
	});


	evil::Slot<const evil::PointerEvent &> slot_mouse1;
	slot_mouse1.attach([](const evil::PointerEvent &x) {
		int debug = 1;
		std::cout << "Mouse event 1\n";
	});

	evil::Slot<const evil::PointerEvent &> slot_mouse2;
	slot_mouse2.attach([](const evil::PointerEvent &x) {
		int debug = 1;
		std::cout << "Mouse event 2\n";
	});


	evil::Slot<const evil::PauseNotification &> slot_paused;
	slot_paused.attach([](const evil::PauseNotification &x) {
		int debug = 1;
		std::cout << "Paused\n";
	});


	evil::eGLFWwindow *w1= (evil::eGLFWwindow *)manager.createWindow(100,100,500, 500, 500, 200, "FRED");
	w1->msigPointerEvent.add(&slot_mouse1);
	w1->msigPauseNotification.add(&slot_paused);
	w1->msigWindowStateEvent.add(&slot_closed);

	evil::eGLFWwindow *w2 = (evil::eGLFWwindow *)manager.createWindow(200, 200, 200, 300, 200, 300, "GEORGE");
	w2->msigPointerEvent.add(&slot_mouse2);

//hack experiment to see if child window works
//	HWND hwnd1 = glfwGetWin32Window(w1->glfw_window());
//	HWND hwnd2 = glfwGetWin32Window(w2->glfw_window());
//
//	SetParent(hwnd2, hwnd1);
//long style = GetWindowLongPtr(hwnd2,GWL_EXSTYLE);
//	style &=~ (WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS) ;
//style |= WS_POPUP;// | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
//SetWindowLongPtr(hwnd1,GWL_EXSTYLE,style);


	//end hack

	manager.start();
	manager.run();
	

	std::cout << "Press return to exit";
	std::getchar();

}
