
#include <SFML/OpenGL.hpp>
#include "sfmlimgui.h"

using namespace ImGui;

sf::Texture *SFML::fontTexture;
sf::RenderWindow *SFML::window;
sf::Clock SFML::timer;
double SFML::time = 0.0f;
bool SFML::mousePressed[3] = { false, false, false };
float SFML::mouseWheel = 0.0f;

void RenderDrawLists(ImDrawData* draw_data)
{
	if (draw_data->CmdListsCount == 0)
		return;

	sf::Vector2u winSize = SFML::window->getSize();
	ImGuiIO& io = ImGui::GetIO();

	SFML::window->pushGLStates();

	GLint last_texture;
	GLint last_viewport[4];
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_VIEWPORT, last_viewport);

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);

	glViewport(0, 0, (GLsizei)winSize.x, (GLsizei)winSize.y);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
		const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
		glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
		glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				sf::Texture::bind(SFML::fontTexture);
				glScissor((int)pcmd->ClipRect.x,
					(int)(winSize.y - pcmd->ClipRect.w),
					(int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
					(int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer);
			}
			idx_buffer += pcmd->ElemCount;
		}
	}
	#undef OFFSETOF

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);

	SFML::window->popGLStates();
	SFML::window->resetGLStates();
}

void SFML::Init(sf::RenderWindow &window)
{
	SFML::window = &window;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
	io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
	io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
	io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
	io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
	io.KeyMap[ImGuiKey_PageUp] = sf::Keyboard::PageUp;
	io.KeyMap[ImGuiKey_PageDown] = sf::Keyboard::PageDown;
	io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
	io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
	io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
	io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
	io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
	io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
	io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
	io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
	io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
	io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
	io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
	io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

	io.RenderDrawListsFn = RenderDrawLists;
	//io.ImeWindowHandle = SFML::window->getSystemHandle();
}

void SFML::Shutdown()
{
	InvalidateDeviceObjects();
	ImGui::Shutdown();
}

void SFML::NewFrame()
{
	if (!fontTexture)
		CreateDeviceObjects();

	ImGuiIO &io = ImGui::GetIO();
	sf::Vector2u winSize = window->getSize();
	io.DisplaySize = ImVec2(static_cast<float>(winSize.x),
		static_cast<float>(winSize.y));

	double currentTime = timer.getElapsedTime().asSeconds();
	io.DeltaTime = time > 0.0 ? (float)(currentTime - time) :
		(float)(1.0f / 60.0f);
	time = currentTime;

	if (window->hasFocus())
	{
		sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
		io.MousePos = ImVec2((float)mousePosition.x, (float)mousePosition.y);
	}
	else
		io.MousePos = ImVec2(-1.0, -1.0);

	io.MouseDown[0] = mousePressed[0] || sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	io.MouseDown[1] = mousePressed[1] || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
	io.MouseDown[2] = mousePressed[2] || sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
	mousePressed[0] = mousePressed[1] = mousePressed[2] = false;

	io.MouseWheel = mouseWheel;
	mouseWheel = 0.0f;

	window->setMouseCursorVisible(io.MouseDrawCursor ? false : true);

	ImGui::NewFrame();
}

bool SFML::ProcessEvent(sf::Event &event)
{
	ImGuiIO &io = ImGui::GetIO();

	switch (event.type)
	{
	case sf::Event::MouseWheelMoved:
		mouseWheel += (float)event.mouseWheel.delta;
		break;
	case sf::Event::MouseButtonPressed:
		mousePressed[event.mouseButton.button] = true;
		break;
	case sf::Event::KeyPressed:
		io.KeysDown[event.key.code] = true;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeyAlt = event.key.alt;
		io.KeySuper = event.key.system;
		break;
	case sf::Event::KeyReleased:
		io.KeysDown[event.key.code] = false;
		io.KeyCtrl = event.key.control;
		io.KeyShift = event.key.shift;
		io.KeyAlt = event.key.alt;
		io.KeySuper = event.key.system;
		break;
	case sf::Event::TextEntered:
		io.AddInputCharacter(event.text.unicode);
		break;
	default:
		return false;
	}

	return true;
}

void SFML::InvalidateDeviceObjects()
{
	if (SFML::fontTexture)
	{
		ImGui::GetIO().Fonts->TexID = nullptr;
		delete fontTexture;
	}
}

bool SFML::CreateDeviceObjects()
{
	ImGuiIO &io = ImGui::GetIO();
	unsigned char *pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	fontTexture = new sf::Texture;
	fontTexture->create(width, height);
	fontTexture->update(pixels);
	io.Fonts->TexID = (void*)fontTexture;

	return true;
}
