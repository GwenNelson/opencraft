/* Two toner is a program that will convert a rasterized font file in a very
 * specific format (a long string of 256 characters) and turn them into a
 * C source code file containing a two-tone array representing the font
 * digested. Also, only character zero, and characters ! through ~ will be
 * digested.
*/

#include "ConsoleFont.c"

int main(int argc, char **argv)
{
    for (int x=0; x < OGLCONSOLE_FontData.width;  x++)
    for (int y=0; y < OGLCONSOLE_FontData.height; y++)
    {
        
    }
}

