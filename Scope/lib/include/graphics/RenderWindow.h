#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Color.h"

class RenderWindow
{
public:
	RenderWindow(const char* title, int width, int height, bool highDPI = true, Uint32 flags = 0);
	~RenderWindow(); 

	SDL_Texture* load(const char* path);
	void blend(SDL_BlendMode mode);
	void clear();
	void render(SDL_Texture* tex);
	int color(double r, double g, double b, double a = 1, bool clip = true);
	int color(Color& color);
	void line(float x1, float y1, float x2, float y2);
	void curve(SDL_FPoint* points, int count);
	void geometry(SDL_Vertex* vertices, int count);
	void rectangle(SDL_Rect* rect);
	void circle(float x, float y, float radius);
	void display();

	double get_scale();
	
	SDL_Window* sdl_window();
	SDL_GLContext gl_context();

private:
	const double scale;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_GLContext context;
	Color current_color;
};