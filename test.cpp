#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <sfmlimgui.h>

int main()
{
	sf::RenderWindow win(sf::VideoMode(800, 800, 32), "imgui test",
		sf::Style::Default);
	win.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(win);

	while (win.isOpen())
	{
		sf::Event e;
		while (win.pollEvent(e))
		{
			ImGui::SFML::ProcessEvent(e);
			if (e.type == sf::Event::Closed || (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape))
			{
				win.close();
				break;
			}
		}
		ImGui::SFML::NewFrame();

		bool tmp = false;
		ImGui::ShowTestWindow(&tmp);

		win.clear(sf::Color(200, 100, 50));
		ImGui::Render();
		win.display();
	}
	ImGui::SFML::Shutdown();

	return EXIT_SUCCESS;
}