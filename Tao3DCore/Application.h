#pragma once
#include "DXResources.h"
#include "IDeviceNotify.h"
#include "IRender.h"
#include "IWindow.h"
#include <DirectXTK/Audio.h>
#include <DirectXTK/Keyboard.h>
#include <DirectXTK/Mouse.h>
#include <unordered_set>
namespace Tao3D
{

#define gApp	  (Application::Instance())
#define gWindow	  (Application::Instance().getWindow())
#define gMouse	  (Application::Instance().getMouse())
#define gKeyboard (Application::Instance().getKeyboard())
#define gResource (Application::Instance().getResources())
#define gDevice	  (Application::Instance().getResources().getDevice())
#define gContext  (Application::Instance().getResources().getDeviceContext())

class Application
{
public:
	Application();
	~Application() { }
	static Application& Instance();

	void init(int width, int height, LPCSTR title, bool fullScreen = false);
	void resize(int width, int height);
	void setWindow(IWindow* window);
	void setRender(IRender* render);

	void registerDeviceNotify(IDeviceNotify* notify)
	{
		m_deviceNotifys.insert(notify);
	}
	void unRegisterDeviceNotify(IDeviceNotify* notify)
	{
		m_deviceNotifys.erase(notify);
	}
	void notifyDeviceState(bool lost)
	{
		for (auto notify : m_deviceNotifys)
		{
			notify->setLost(lost);
			if (lost)
			{
				notify->onDeviceLost();
			}
			else
			{
				notify->onDeviceRestore();
			}
		}
	}

	IWindow* getWindow() const
	{
		return m_pWindow;
	}
	IRender* getRender() const
	{
		return m_pRender;
	}
	DirectX::Mouse& getMouse()
	{
		return m_mouse;
	}
	DirectX::Keyboard& getKeyboard()
	{
		return m_keyboard;
	}
	// DirectX::AudioEngine &getAudioEngine()
	//{
	//    return m_audioEngine;
	//}
	DXResources& getResources()
	{
		return m_resources;
	}
	void exec();
	void quit();

private:
	static Application* self;
	IWindow*			m_pWindow = nullptr;
	IRender*			m_pRender = nullptr;
	DirectX::Mouse		m_mouse;
	DirectX::Keyboard	m_keyboard;
	DXResources			m_resources;
	// DirectX::AudioEngine m_audioEngine;

	std::unordered_set<IDeviceNotify*> m_deviceNotifys;
};

} // namespace Tao3D
