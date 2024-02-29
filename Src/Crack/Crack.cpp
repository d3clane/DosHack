#include <stdio.h>
#include <assert.h>
#include <SDL.h>

int Crack(FILE* crackFile);
void ShowScreen();

int main(const int argc, const char* argv[])
{
    FILE* crackFile = nullptr;

    if (argc == 1)
        crackFile = fopen("password.com", "rb+");
    else
        crackFile = fopen(argv[1], "rb+");
    
    if (crackFile == nullptr)
    {
        printf("Incorrect file name, please retry");

        return 0;
    }

    if (Crack(crackFile) == 0)
        return -1;
        
    ShowScreen();

    fclose(crackFile);
}

int Crack(FILE* crackFile)
{
    assert(crackFile);

    static const int NOP = 144;

    int okStatus = fseek(crackFile, 0x55L, SEEK_SET);
    if (okStatus == -1)  return 0;

    okStatus = fputc(NOP, crackFile);
    if (okStatus == EOF) return 0;

    okStatus = fputc(NOP, crackFile);
    if (okStatus == EOF) return 0;

    okStatus = fputc(NOP, crackFile);
    if (okStatus == EOF) return 0;

    return 1;
}

void ShowScreen()
{
    static const size_t width  = 512;
    static const size_t height = 211;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Casino", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          width, height, SDL_WINDOW_SHOWN);

    SDL_Surface* tree  = SDL_LoadBMP("SDLAssets/imgs/Vzlom.bmp");

    if (!tree || !window)
        return;

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if (!window_surface)
        return;

    SDL_BlitSurface(tree, NULL, window_surface, NULL);

    SDL_Event event;
    SDL_PollEvent(&event);

    SDL_UpdateWindowSurface(window);

    SDL_Delay(5000);
}
