#include <AudioVisualizer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


int main(int argc, char* argv[])
{
	// Initialize SDL with video
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	// Create an SDL window
	SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);

	// if failed to create a window
	if (!window)
	{
		// we'll print an error message and exit
		std::cerr << "Error failed to create window!\n";
		return 1;
	}

	// Create an OpenGL context (so we can use OpenGL functions)
	SDL_GLContext context = SDL_GL_CreateContext(window);

	// if we failed to create a context
	if (!context)
	{
		// we'll print out an error message and exit
		std::cerr << "Error failed to create a context\n!";
		return 2;
	}

	Music music;
	if (music.load("Memory Reboot.mp3", 1))
	{
		music.play();
	}
	else
	{
		std::cout << "failed to load music into Music Player" << std::endl;
	}

	SDL_Event event;	 // used to store any events from the OS
	bool running = true; // used to determine if we're running the game

	glClearColor(0, 0, 0, 1);
	while (running)
	{
		// poll for events from SDL
		while (SDL_PollEvent(&event))
		{
			// determine if the user still wants to have the window open
			// (this basically checks if the user has pressed 'X')
			running = event.type != SDL_QUIT;
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_q)
				{
					std::cout << "Pressed Q key,Toggling Loop!" << std::endl;
					music.toggleLooping();
				}
				if (event.key.keysym.sym == SDLK_w)
				{
					music.getInstance()->setVolume(music.getInstance()->getVolume() + 10);
					std::cout << "Increasing Volume by 10" << std::endl;
				}
				if (event.key.keysym.sym == SDLK_s)
				{
					music.getInstance()->setVolume(music.getInstance()->getVolume() - 10);
					std::cout << "Decreasing Volume by 10" << std::endl;
				}
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		// Swap OpenGL buffers
		SDL_GL_SwapWindow(window);
	}


	// Destroy the context
	SDL_GL_DeleteContext(context);

	// Destroy the window
	SDL_DestroyWindow(window);

	// And quit SDL
	SDL_Quit();

	return 0;
}