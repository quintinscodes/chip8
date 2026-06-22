#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <fstream>
#include <iostream>
#include <iomanip> // Necesario para hex y setfill


uint8_t fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;

const unsigned int V0 = 0;
const unsigned int V1 = 1;
const unsigned int V2 = 2;
const unsigned int V3 = 3;
const unsigned int V4 = 4;
const unsigned int V5 = 5;
const unsigned int V6 = 6;
const unsigned int V7 = 7;
const unsigned int V8 = 8;
const unsigned int V9 = 9;
const unsigned int VA = 10;
const unsigned int VB = 11;
const unsigned int VC = 12;
const unsigned int VD = 13;
const unsigned int VE = 14;
const unsigned int VF = 15;


class Chip8 {
    public:
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint16_t index_register{};
        uint16_t pc = { START_ADDRESS };
        uint8_t stack[16]{};
        uint8_t sp{};
        int delay{};
        int sound_timer{};
        uint16_t input_key{};
        uint32_t display[64*32]{};
        uint16_t opcode{};    




        void LoadROM_and_Font(const char* filename);
        
        void Cycle();


        /*instructions*/

        void OP_0nnn();
        void OP_1nnn();
        void OP_00E0();
        void OP_00EE();
        void OP_2nnn();
        void OP_3xkk();
        void OP_4xkk();

        
        
};      
void Chip8::OP_0nnn(){
    pc = (opcode & 0x0FFFu);
}

void Chip8::OP_1nnn(){
    pc = (opcode & 0x0FFFu);
}
void Chip8::OP_00E0(){
    memset(display, 0, sizeof(display));
}

void Chip8::OP_00EE(){
        --sp;
        pc = stack[sp];
}

void Chip8::OP_2nnn(){
    stack[sp] = pc;
    ++sp;
    pc = (opcode & 0x0FFFu);
}


void Chip8::OP_3xkk(){
    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t kk = (opcode & 0x00FFu);

    if (registers[Vx] == kk) {
        ++pc;
        ++pc;
    }
}

void Chip8::OP_4xkk(){
    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t kk = (opcode & 0x00FFu);

    if (registers[Vx] != kk) {
        ++pc;
        ++pc;
    }
}


void Chip8::LoadROM_and_Font(char const* filename){
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;

        for (long i = 0; i<80; ++i){

            memory[FONTSET_START_ADDRESS + i] = fontset[i];

        }
	}
}

void Chip8::Cycle(){

}

int init_and_render_sdl(SDL_Window** window, SDL_Renderer** renderer){
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Error Init: %s", SDL_GetError());
        return 3;
    }

    if (!SDL_CreateWindowAndRenderer("CHIP-8 Emulator", 640, 480, 0, window, renderer)) {
        SDL_Log("Error Window/Renderer: %s", SDL_GetError());
        return 3;
    }

    
    return 0;
}

int create_surface_and_texture(SDL_Texture** texture, SDL_Renderer** renderer, const char* path){


    SDL_Surface *surface = SDL_LoadBMP(path);
    if (!surface) {
        SDL_Log("Error BMP: %s", SDL_GetError());
        return 3; 
    }

    *texture = SDL_CreateTextureFromSurface(*renderer, surface);
    SDL_DestroySurface(surface);

    if (!*texture) { 
        SDL_Log("Error Texture: %s", SDL_GetError());
        return 3;
    }


    return 0;

}



void end_sdl(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture){
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}


int main(int argc, char *argv[])
{
    Chip8 chip;
    char const* rom_name = "logo.ch8";
    chip.LoadROM_and_Font(rom_name);



    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Event event;
    


    bool done = false;
    bool space_pressed = false;
    bool a_pressed = false;



    if (init_and_render_sdl(&window, &renderer) != 0) {
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                std::cout << "Tecla: " << event.key.key << std::endl;
                
                if (event.key.key == SDLK_SPACE) {
                    std::cout << "Dibujando..." << std::endl;
                    space_pressed=true;
                } else if (event.key.key == SDLK_A) {
                    a_pressed = true;
                }
            }
        }

        if(space_pressed){
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // Fondo gris oscuro
            SDL_RenderClear(renderer);

            create_surface_and_texture(&texture, &renderer, "sample.bmp");

            SDL_RenderTexture(renderer, texture, NULL, NULL);

            SDL_RenderPresent(renderer);
            
            SDL_Delay(16);
            
            space_pressed = false;

        } else if (a_pressed) {
            
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255); // Fondo gris oscuro
            SDL_RenderClear(renderer);

            create_surface_and_texture(&texture, &renderer, "snail.bmp");

            SDL_RenderTexture(renderer, texture, NULL, NULL);

            SDL_RenderPresent(renderer);
            
            SDL_Delay(16);

            a_pressed = false;

        }

        
    }

    end_sdl(window, renderer, texture);
    return 0;
}