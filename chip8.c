#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>

#include"SDL.h"

typedef struct{
	SDL_Window *window;
	SDL_Renderer *renderer;
}sdl_t;

typedef struct{
	uint32_t window_width;
	uint32_t window_height;
	uint32_t fg_color;
	uint32_t bg_color;
	uint32_t scale_factor;
} config_t;

// Emulator state
typedef enum{
	QUIT ,
	RUNNING,
	PAUSED,

} emulator_state_t;

// CHIP8 Machine object
typedef struct{
	emulator_state_t state;


} chip8_t;


// Initialize SDL
bool init_sdl(sdl_t *sdl,const config_t config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError());
        return false;
	}

    sdl->window = SDL_CreateWindow("CHIP8 Emulator", SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				config.window_width * config.scale_factor,
				config.window_height * config.scale_factor,
				0);

    if(!sdl->window){
	SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError());
	return false;
	}

	sdl->renderer = SDL_CreateRenderer(sdl->window,-1,SDL_RENDERER_ACCELERATED);
	if(!sdl->renderer){
	SDL_Log("Could not create SDL renderer %s\n", SDL_GetError());
        return false;
	}


	return true;
}

// Set up initial emulator configu ration from passed in arguments
bool set_config_from_args(config_t *config,const int argc,char **argv){
	
	// Set defaults
	*config = (config_t){
		.window_width=64,
		.window_height=32,
		.fg_color = 0xFFFFFFFF,
		.bg_color = 0xFFFF00FF,
		.scale_factor = 20,
	};

	// Override defaults from passed in arguments
	for(int i=1; i< argc;i++){
		(void)argv[i]; // Prevent compiler error from unused variables argc/argv
	}

	return true;

}

// Initialize CHIP8 machine
bool init_chip8(chip8_t *chip8){
	chip8->state = RUNNING;

	return true;

}

// Final cleanup
void final_cleanup(const sdl_t sdl) {
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
    	SDL_Quit(); // Shut down SDL subsystem
}

// Clear screen / SDL Window to background color
void clear_screen(const sdl_t sdl,const config_t config){
	const uint8_t r = (config.bg_color >> 24) & 0xFF;
	const uint8_t g = (config.bg_color >> 16) & 0xFF;
	const uint8_t b = (config.bg_color >> 8) & 0xFF;
	const uint8_t a = (config.bg_color >> 0) & 0xFF;
	SDL_SetRenderDrawColor(sdl.renderer,r,g,b,a);
	SDL_RenderClear(sdl.renderer);

}

// Update window with any changes
void update_screen(const sdl_t sdl){
	SDL_RenderPresent(sdl.renderer);

}


void handle_input(chip8_t *chip8){
	SDL_Event event;

	while (SDL_PollEvent(&event)){
		switch(event.type){
			case SDL_QUIT:
				// Exit window; End program
				chip8->state = QUIT;
				return;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_ESCAPE:
						chip8->state = QUIT;
						return;
					default:
						break;
				}

				break;	
			case SDL_KEYUP:
				break;
			default:
				break;
		
		}
	}

}

int main(int argc, char **argv){
        // Initialize emulator configuration/options
        config_t config={0};
	if(!set_config_from_args(&config,argc,argv)) exit(EXIT_FAILURE);

	// Initialize SDL
	sdl_t sdl={0};
	if(!init_sdl(&sdl,config)) exit(EXIT_FAILURE);
	
	// Initialize CHIP8 machine
	chip8_t chip8 = {0};
	if(!init_chip8(&chip8)) exit(EXIT_FAILURE);

	// Initialize screen clear to background color
	clear_screen(sdl,config);

	// Main emulator loop
	while (chip8.state != QUIT){
		// Handle user input
		handle_input(&chip8);
		
		//Get_time();

		// Delay for 60hz/60fps
		SDL_Delay(16);

		// Update window with changes
		update_screen(sdl);
	}

	//Final cleanup
	final_cleanup(sdl);

	exit(EXIT_SUCCESS);
}


