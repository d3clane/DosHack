#include <stdio.h>

int main()
{
    FILE* outStream = fopen("input", "wb");

    const size_t NumberOfEmptyBytes = 0xFC59ul;

    for (size_t i = 0; i < NumberOfEmptyBytes; i += 2)
    {
        fputc(0xB3, outStream);
        fputc(0xD3, outStream);
    }

    fputc(0xB3, outStream);
    fputc(0xD3, outStream);

    fputc(0x0D, outStream);

    fclose(outStream);
}