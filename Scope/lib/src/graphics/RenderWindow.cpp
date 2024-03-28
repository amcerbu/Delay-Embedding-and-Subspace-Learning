#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>
#include <cmath>

#include "RenderWindow.h"

RenderWindow::RenderWindow(const char* title, int width, int height, const bool highDPI, Uint32 flags) : 
	window(NULL), renderer(NULL), scale(highDPI ? 2 : 1)
{
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags | SDL_WINDOW_OPENGL | (highDPI ? SDL_WINDOW_ALLOW_HIGHDPI : 0));

	if (window == NULL)
	{
		std::cout << "Window failed to initialize. Error: " << SDL_GetError() << std::endl;
		std::exit(1);
	}

	context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		std::cout << "OpenGL context could not be created. Error: " << SDL_GetError() << std::endl;
		std::exit(1);
	}

 	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // framelimit
	blend(SDL_BLENDMODE_BLEND); // enable alpha
}

RenderWindow::~RenderWindow()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void RenderWindow::blend(SDL_BlendMode mode)
{
	SDL_SetRenderDrawBlendMode(renderer, mode); // enable alpha
}

SDL_Texture* RenderWindow::load(const char* path)
{
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, path);

	if (texture == NULL)
		std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;

	return texture;
}

void RenderWindow::clear()
{
	SDL_RenderClear(renderer);
}

void RenderWindow::render(SDL_Texture* tex)
{
	// glColor3f(0.7f, 1.0f, 0.3f);
	// glBegin(GL_TRIANGLES);
 //    glVertex3f(1.0f, -1.0f, 0.0f);
 //    glVertex3f(-1.0f, -1.0f, 0.0f);
 //    glVertex3f(0.0f, 1.0f, 0.0f);
	// glEnd(); // On 12/30/06, SkunkGuru <skunkguru at gmail.com> wrote:

	SDL_RenderCopy(renderer, tex, NULL, NULL);
}

int RenderWindow::color(double r, double g, double b, double a, bool clip)
{
	if (clip)
	{
		const static double before = std::nextafter(1.0, 0.0);
		r = (int)(256 * std::fmax(0, std::fmin(r, before)));
		g = (int)(256 * std::fmax(0, std::fmin(g, before)));
		b = (int)(256 * std::fmax(0, std::fmin(b, before)));
		a = (int)(256 * std::fmax(0, std::fmin(a, before)));
	}
	else
	{
		r = (int)(256 * r);
		g = (int)(256 * g);
		b = (int)(256 * b);
		a = (int)(256 * a);
	}

	return SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

int RenderWindow::color(Color& color)
{
	current_color = color;
	SDL_Color raw = color.raw();
	return SDL_SetRenderDrawColor(renderer, raw.r, raw.g, raw.b, raw.a);
}

void RenderWindow::line(float x1, float y1, float x2, float y2)
{
	SDL_RenderDrawLineF(renderer, scale * x1, scale * y1, scale * x2, scale * y2);
}

void RenderWindow::curve(SDL_FPoint* points, int count)
{
	SDL_RenderDrawLinesF(renderer, points, count);
}

void RenderWindow::geometry(SDL_Vertex* vertices, int count)
{
	SDL_RenderGeometry(renderer, NULL, vertices, count, NULL, 0);
}

void RenderWindow::rectangle(SDL_Rect* rect)
{
	SDL_RenderFillRect(renderer, rect);
}

void RenderWindow::circle(float x, float y, float radius)
{
	int resolution = 12 + (2 * M_PI * radius / 12); // segments per circle
	float x_coords[resolution];
	float y_coords[resolution];

	for (int i = 0; i < resolution; i++)
	{
		x_coords[i] = scale * (x + radius * cos(2 * M_PI * (double)i / resolution));
		y_coords[i] = scale * (y + radius * sin(2 * M_PI * (double)i / resolution));
	}

	SDL_Vertex points[resolution * 3];
	for (int j = 0; j < resolution; j++)
	{
		points[3 * j] = { SDL_FPoint{ x_coords[j], y_coords[j] }, current_color.raw(), SDL_FPoint{ 0 } };
		points[3 * j + 1] = { SDL_FPoint{ x_coords[(j + 1) % resolution], y_coords[(j + 1) % resolution] }, current_color.raw(), SDL_FPoint{ 0 } };
		points[3 * j + 2] = { SDL_FPoint{ float(scale * x), float(scale * y) }, current_color.raw(), SDL_FPoint{ 0 } };
	}

	geometry(points, resolution * 3);
}

void RenderWindow::display()
{
	SDL_RenderPresent(renderer);
}

SDL_Window* RenderWindow::sdl_window()
{
	return window;
}

SDL_GLContext RenderWindow::gl_context()
{
	return context;
}

double RenderWindow::get_scale()
{
	return scale;
}