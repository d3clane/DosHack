#include <stdio.h>
#include <assert.h>
#include <SDL.h>

int Crack(FILE* crackFile);

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


