// sfimgui.hpp

#if !defined(SFIMGUI_H_)
#define SFIMGUI_H_

#include <SFML/Graphics.hpp>
#include "imgui.h"

namespace ImGui
{
	class SFML
	{
	public:
		static sf::Texture *fontTexture;
		static sf::RenderWindow *window;
		static sf::Clock timer;
		static double time;
		static bool mousePressed[3];
		static float mouseWheel;

	public:
		static void Init(sf::RenderWindow &window);
		static void Shutdown();

		static void NewFrame();
		static bool ProcessEvent(sf::Event &event);
		
		static void InvalidateDeviceObjects();
		static bool CreateDeviceObjects();
	};
}

#endif
