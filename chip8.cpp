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
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

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
        int delayTimer{};
        int soundTimer{};
        uint16_t input_key{};
        uint32_t display[VIDEO_HEIGHT*VIDEO_WIDTH]{};
        uint16_t opcode{};    


        uint8_t keypad[16]{};



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
        void OP_5xy0();
        void OP_6xkk();
        void OP_7xkk();
        void OP_8xy0();
        void OP_8xy1();
        void OP_8xy2();
        void OP_8xy3();
        void OP_8xy4();
        void OP_8xy5();
        void OP_8xy6();
        void OP_8xy7();
        void OP_8xyE();
        void OP_9xy0();
        void OP_Annn();
        void OP_Bnnn();
        void OP_Dxyn();
        void OP_Ex9E();
        void OP_ExA1();
        void OP_Fx07();        
        void OP_Fx0A();
        void OP_Fx15();
        void OP_Fx18();
        void OP_Fx1E();
        void OP_Fx29();        
        void OP_Fx33();
        void OP_Fx55();
        void OP_Fx65();
        void OP_Cxkk();
};


void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index_register + i];
	}
}

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index_register + i] = registers[i];
	}
}

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[index_register + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index_register + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index_register] = value % 10;
}

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];

	index_register = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	index_register += registers[Vx];
}

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	soundTimer = registers[Vx];
}

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delayTimer = registers[Vx];
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;
}

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0; 

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index_register + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &display[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = 0x00F1u & byte; // <- TO DO: randbyte
}

void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}


void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

	index_register = address;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		pc += 2;
	}
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}


void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sub = registers[Vy] - registers[Vx];


	if(registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sub & 0xFFu;
}


void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t least = (Vx & 0x1u);

	if (least == 0x1u)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vx] >> 1u;
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sub = registers[Vx] - registers[Vy];

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sub & 0xFFu;
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy3(){

    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t Vy = (opcode >> 4u & 0x000Fu);

    registers[Vx] = registers[Vx] ^ registers[Vy];

}

void Chip8::OP_8xy2(){

    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t Vy = (opcode >> 4u & 0x000Fu);

    registers[Vx] = registers[Vx] & registers[Vy];

}

void Chip8::OP_8xy1(){

    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t Vy = (opcode >> 4u & 0x000Fu);

    registers[Vx] = registers[Vx] | registers[Vy];

}


void Chip8::OP_8xy0(){

    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t Vy = (opcode >> 4u & 0x000Fu);

    registers[Vx] = registers[Vy];

}

void Chip8::OP_7xkk(){

    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t kk = (opcode & 0x00FFu);

    registers[Vx] = registers[Vx] + kk; 

}

void Chip8::OP_6xkk(){

    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t kk = (opcode & 0x00FFu);

    registers[Vx] = kk;
    

}




void Chip8::OP_5xy0(){
    
    uint16_t Vx = (opcode >> 8u & 0x000Fu);
    uint16_t Vy = (opcode >> 4u & 0x000Fu);

    if (registers[Vx] == registers[Vy]) {
        ++pc;
        ++pc;   
    }
    

}

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


void Chip8::Cycle()
{
	// Fetch
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Increment the PC before we execute anything
	pc += 2;

	// Decode and Execute
	((*this).*(table[(opcode & 0xF000u) >> 12u]))();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
	{
		--soundTimer;
	}
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