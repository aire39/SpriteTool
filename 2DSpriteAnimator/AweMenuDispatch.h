#ifndef AWEMENUDISPATCH
#define AWEMENUDISPATCH

#include <Awesomium\WebView.h>
#include <Awesomium\STLHelpers.h>
#include <iostream>

#include "AppEvent.h"

class AWEMenuDispatch : public Awesomium::JSMethodHandler
{
	private:
		void onSayHello(Awesomium::WebView * caller, const Awesomium::JSArray& arg) // test function
		{
			std::cout << arg.At(0).ToInteger() << "  " << arg.At(1).ToString() << std::endl;
			std::cout << "Hello!" << std::endl;
		}

		AppEvent * appevent;

	public:

		AWEMenuDispatch(AppEvent * appevent) : appevent(appevent) {}

		void OnMethodCall(Awesomium::WebView * caller, unsigned int remote_id, const Awesomium::WebString& str, const Awesomium::JSArray& arg)
		{
			if (str == Awesomium::WSLit("sayHello"))
				onSayHello(caller, arg);

			if (str == Awesomium::WSLit("openFile"))
				appevent->onOpenFile(caller, arg);

			if (str == Awesomium::WSLit("Quit"))
				appevent->exit();

			if (str == Awesomium::WSLit("SelectionMode"))
				appevent->setControlPath(arg.At(0).ToInteger() );

		}

		Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView * caller, unsigned int remote_id, const Awesomium::WebString& str, const Awesomium::JSArray& arg)
		{
			std::cout << "Hello" << std::endl;
			return Awesomium::JSValue::Undefined();
		}
};

#endif