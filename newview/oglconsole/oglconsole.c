/* oglconsole -- gpl license here */

/* This strategy seems to offer the convenience of zero-configuration, but
 * obviously it also offers defining GLHEADERINCLUDE */
#ifdef GLHEADERINCLUDE
#  include GLHEADERINCLUDE
#else
#  ifdef __MACH__
#    include <OpenGL/gl.h>
#  else
#    include <GL/gl.h>
#  endif
#endif

#include "oglconsole.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define C ((_OGLCONSOLE_Console*)console)

/* OGLCONSOLE font */
#include "ConsoleFont.c"
#define FIRST_CHARACTER ' '
#define LAST_CHARACTER  '~'

#define CHAR_PIXEL_W 6
#define CHAR_PIXEL_H 13
#define CHAR_WIDTH 0.0234375 /* ogl tex coords */
#define CHAR_HEIGHT 0.203125 /* ogl tex coords */

/* This is how long the animation should take to make the transition between
 * "hidden" and "visible" console visibility modes (expressed in milliseconds) */
#define SLIDE_MS 700

/* If we don't know how to retrieve the time then we can just use a number of
 * frames to divide up the time it takes to transition between "hidden" and
 * "visible" console visibility modes */
#define SLIDE_STEPS 25

GLuint OGLCONSOLE_glFontHandle = 0;
int OGLCONSOLE_CreateFont()
{
    {int err=glGetError();if(err)printf("GL ERROR: %i\n",err);}
#ifdef DEBUG
    puts("Creating OGLCONSOLE font");
#endif
   
    /* Get a font index from OpenGL */
    glGenTextures(1, &OGLCONSOLE_glFontHandle);
    {int err=glGetError();if(err)printf("glGenTextures() error: %i\n",err);}
    
    /* Select our font */
    glBindTexture(GL_TEXTURE_2D, OGLCONSOLE_glFontHandle);
    {int err=glGetError();if(err)printf("glBindTexture() error: %i\n",err);}

    /* Set some parameters i guess */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Upload our font */
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB,
            OGLCONSOLE_FontData.width, OGLCONSOLE_FontData.height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, OGLCONSOLE_FontData.pixel_data);

    {int err=glGetError();if(err)printf("glTexImage2D() error: %i\n",err);}
    
#ifdef DEBUG
    puts("Created  OGLCONSOLE font");
#endif
    return 1;
}

/* TODO: Expose these macros to the user? */

/* This is the longest command line that the user can enter TODO: Make dynamic
 * so that the user can enter any length line */
#define MAX_INPUT_LENGTH 256

/* This is the number of command line entries that the console will remember (so
 * that the user can use the up/down keys to see and easily re-execute his past
 * commands) */
#define MAX_HISTORY_COUNT 25

/* This is the default number of lines for the console to remember (that is to
 * say, the user can scroll up and down to see what has been printed to the
 * console in the past, and this is the number of those lines, plus the number
 * of lines shown on the screen at all times) */
#define DEFAULT_MAX_LINES 100

/* OGLCONSOLE console structure */
typedef struct
{
    GLdouble mvMatrix[16];
    int mvMatrixUse;

    GLdouble pMatrix[16];
    int pMatrixUse;

    /* Screen+scrollback lines (console output) */
    char *lines;
    int maxLines, lineQueueIndex, lineScrollIndex;

    /* History scrollback (command input) */
    char history[MAX_HISTORY_COUNT][MAX_INPUT_LENGTH];
    int historyQueueIndex, historyScrollIndex;

    /* Current input line */
    char inputLine[MAX_INPUT_LENGTH];
    int inputCursorPos, inputLineLength;

    /* Rows and columns of text to display */
    int textWidth, textHeight;

    /* Current column of text where new text will be inserted */
    char *outputCursor;
    int outputNewline;

    /* Width and height of a single character for the GL */
    GLdouble characterWidth, characterHeight;
    
    /* Basic options */
    int visibility;

    /* Various callback functions defined by the user */
    void(*enterKeyCallback)(OGLCONSOLE_Console console, char *cmd);
    char* (*tabCompleteCallback)(OGLCONSOLE_Console console, char* partial);

} _OGLCONSOLE_Console;

/* To save code, I've gone with an imperative "modal" kind of interface */
_OGLCONSOLE_Console *programConsole = NULL;

/* This console is the console currently receiving user input */
_OGLCONSOLE_Console *userConsole = NULL;

/* Set the callback for a console */
void OGLCONSOLE_EnterKey(void(*cbfun)(OGLCONSOLE_Console console, char *cmd))
{
    programConsole->enterKeyCallback = cbfun;
}

void OGLCONSOLE_TabComplete(char*(*cbfun)(OGLCONSOLE_Console console, char*partial))
{
    programConsole->tabCompleteCallback = cbfun;
}

char* OGLCONSOLE_DefaultTabCompleteCallback(OGLCONSOLE_Console console, char *cmd)
{
    return "";
}

void OGLCONSOLE_DefaultEnterKeyCallback(OGLCONSOLE_Console console, char *cmd)
{
    OGLCONSOLE_Output(console,
            "No enter key callback is registered for this console!\n");
}

OGLCONSOLE_Console OGLCONSOLE_Create()
{
    _OGLCONSOLE_Console *console;
    GLint viewport[4];

    /* If font hasn't been created, we create it */
    if (!glIsTexture(OGLCONSOLE_glFontHandle))
        OGLCONSOLE_CreateFont();

    /* Allocate memory for our console */
    console = (void*)malloc(sizeof(_OGLCONSOLE_Console));
 
    /* Textual dimensions */
    glGetIntegerv(GL_VIEWPORT, viewport);
    console->textWidth = viewport[2] / CHAR_PIXEL_W;
    console->textHeight = viewport[3] / CHAR_PIXEL_H;
    console->characterWidth = 1.0 / console->textWidth;
    console->characterHeight = 1.0 / console->textHeight;

    /* Different values have different meanings for xMatrixUse:
        0) Do not change the matrix before rendering
        1) Upload the console's matrix before rendering
        2) Multiply the console's matrix before rendering */

    /* Initialize its projection matrix */
    console->pMatrixUse = 1;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0, console->textWidth, 0, console->textHeight, -1, 1);
    glGetDoublev(GL_MODELVIEW_MATRIX, console->pMatrix);
    glPopMatrix();

    /* Initialize its modelview matrix */
    console->mvMatrixUse = 1;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(-1, -1, 0);
    glScaled(2,2,1);
    glGetDoublev(GL_MODELVIEW_MATRIX, console->mvMatrix);
    glPopMatrix();

    /* Screen and scrollback lines */
    /* This is the total number of screen lines in memory (start blank) */
    console->maxLines = DEFAULT_MAX_LINES;
    /* Allocate space for text */
    console->lines = (char*)malloc(console->maxLines*(console->textWidth+1));
    /* Initialize to empty strings */
    memset(console->lines, 0, console->maxLines*(console->textWidth+1));
    /* This variable represents whether or not a newline has been left */
    console->outputNewline = 0;
    /* This cursor points to the X pos where console output is next destined */
    console->outputCursor = console->lines;
    /* This cursor points to what line console output is next destined for */
    console->lineQueueIndex = 0;
    /* This cursor points to what line the console view is scrolled to */
    console->lineScrollIndex = console->maxLines - console->textHeight + 1;

    /* Initialize the user's input (command line) */
    console->inputLineLength = 0;
    console->inputCursorPos = 0;
    console->inputLine[0] = '\0';

    /* History lines */
    memset(console->history, 0, MAX_INPUT_LENGTH * MAX_HISTORY_COUNT);
    console->historyQueueIndex = 0;
    console->historyScrollIndex = -1;

    /* Callbacks */
    console->enterKeyCallback = OGLCONSOLE_DefaultEnterKeyCallback;
    console->tabCompleteCallback = OGLCONSOLE_DefaultTabCompleteCallback;

    /* The console starts life invisible */
    console->visibility = 0;

    /* If no consoles existed before, we select this one for convenience */
    if (!programConsole) programConsole = console;
    if (!userConsole) userConsole = console;






    /* Return the opaque pointer to the programmer */
    return (OGLCONSOLE_Console)console;
}

/* This functoin is only used internally; the user ultimately invokes this
 * function through either a call to Destroy() or Quit(); the purpose of this
 * mechanism is to warn the user if he has explicitly destroyed a console that
 * was engaged in operation at the time they destroyed it (the only two
 * operations a console can be engaged in are receiving programmer interaction,
 * or receiving end-user interaction. fyi, "user" always refers to the
 * programmer, end-user refers to the real end-user) */
void OGLCONSOLE_DestroyReal(OGLCONSOLE_Console console, int warn)
{
    free(C);

    if (warn)
    {
        if (programConsole == C)
        {
            fprintf(stderr, 
            "Warning: OGLCONSOLE you just destroyed the programConsole!\n");
            programConsole = NULL;
        }

        if (userConsole == C)
        {
            fprintf(stderr,
            "Warning: OGLCONSOLE you just destroyed the userConsole!\n");
            userConsole = NULL;
        }
    }
}

/* The user can call this function to free a console. A warning is printed to
 * stderr if the user has destroyed a console destroyed while it was receiving
 * input, see DestroyReal() for details TODO: there are currently no semantics
 * under consideration for explicitly destroying an 'engaged' console WITHOUT
 * having a warning issued, nor is there a way to deselect a console without
 * also selecting a new one as of yet */
void OGLCONSOLE_Destroy(OGLCONSOLE_Console console)
{
    OGLCONSOLE_DestroyReal(console, 1);
}

/* This function frees all of the consoles that the library is actively aware
 * of, and is intended to be used by programs that have only a single console;
 * technically this particular function will free up to two consoles, but don't
 * count on it because that may change; no warnings are issued by this function */
void OGLCONSOLE_Quit()
{
    OGLCONSOLE_DestroyReal((void*)programConsole, 0);
    if (programConsole != userConsole)
        OGLCONSOLE_DestroyReal((void*)userConsole, 0);
    programConsole = NULL;
    userConsole = NULL;
}

/* Select a console to receive input from the user (read: programmer); this
 * function is intended for use by applications which create MORE THAN ONE
 * console, since every newly created console is automatically selected for
 * programmer input ('programmer input' refers to calling any function which
 * does not specify a console explicitly in its parameter list. the console
 * which is 'engaged' by 'programmer input' at the time of calling one of these
 * functions is the console that function will operate on */
void OGLCONSOLE_EditConsole(OGLCONSOLE_Console console) { programConsole = C; }

/* Show or hide a console */
void OGLCONSOLE_SetVisibility(int visible)
{
}

/* Get current configuration information about a console */
void OGLCONSOLE_Info()
{
    puts("TODO: print some console info here");
}

/* This routine is meant for applications with a single console, if you use
 * multiple consoles in your program, use Render() instead */
void OGLCONSOLE_Draw() { OGLCONSOLE_Render((void*)userConsole); }

/* Internal functions for drawing text. You don't want these, do you? */
void OGLCONSOLE_DrawString(char *s, double x, double y, double w, double h,
        double z);
void OGLCONSOLE_DrawWrapString(char *s, double x, double y, double w, double h,
        double z, int wrap);
void OGLCONSOLE_DrawCharacter(int c, double x, double y, double w, double h,
        double z);

/* This function draws a single specific console; if you only use one console in
 * your program, use Draw() instead */
void OGLCONSOLE_Render(OGLCONSOLE_Console console)
{
    /* Don't render hidden console */
    if (C->visibility == 0) return;

    // TODO - move this to a seperate function, invoke only on window resize
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    C->textWidth = viewport[2] / CHAR_PIXEL_W;
    C->textHeight = viewport[3] / CHAR_PIXEL_H;
    C->characterWidth = 1.0 / C->textWidth;
    C->characterHeight = 1.0 / C->textHeight;
    C->lines = (char*)realloc((void*)C->lines,C->maxLines*(C->textWidth+1));
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0, C->textWidth, 0, C->textHeight, -1, 1);
    glGetDoublev(GL_MODELVIEW_MATRIX, C->pMatrix);
    glPopMatrix();

    /* Initialize its modelview matrix */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(-1, -1, 0);
    glScaled(2,2,1);
    glGetDoublev(GL_MODELVIEW_MATRIX, C->mvMatrix);
    glPopMatrix();



    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(C->pMatrix);
 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(C->mvMatrix);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

/*    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);*/

    /* TODO: This SHOULD become an option at some point because the
     * infrastructure for "real" consoles in the game (like you could walk up to
     * a computer terminal and manipulate a console on a computer using
     * oglconsole) already exists; you'd want depth testing in that case */
    glDisable(GL_DEPTH_TEST);

    /* Render hiding / showing console in a special manner. Zero means hidden. 1
     * means visible. All other values are traveling toward zero or one. TODO:
     * Make this time dependent */
    if (C->visibility != 1)
    {
        double d; /* bra size */
        int v = C->visibility;

        /* Count down in both directions */
        if (v < 0)
        {
            v ^= -1;
            C->visibility++;
        }
        else
        {
            v = SLIDE_STEPS - v;
            C->visibility--;
        }

        d = 0.04 * v;
        glTranslated(0, 1-d, 0);
    }

    /* First we draw our console's background TODO: Add something fancy? */
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    glColor4d(.1,0,0, 0.5);

    glBegin(GL_QUADS);
    glVertex3d(0,0,0);
    glVertex3d(1,0,0);
    glVertex3d(1,1,0);
    glVertex3d(0,1,0);
    glEnd();

    // Change blend mode for drawing text
    glBlendFunc(GL_ONE, GL_ONE);

    /* Select the console font */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, OGLCONSOLE_glFontHandle);

    /* Recolor text */
    glColor3d(0,1,0);

    /* Render console contents */
    glBegin(GL_QUADS);
    {
        /* Graphical line, and line in lines[] */
        int gLine, tLine = C->lineScrollIndex;

        /* Iterate through each line being displayed */
        for (gLine = 0; gLine < C->textHeight; gLine++)
        {
            /* Draw this line of text adjusting for user scrolling up/down */
            OGLCONSOLE_DrawString(C->lines + (tLine * C->textWidth),
                    0,
                    (C->textHeight - gLine) * C->characterHeight,
                    C->characterWidth,
                    C->characterHeight,
                    0);

            /* Grab next line of text using wheel-queue wrapping */
            if (++tLine >= C->maxLines) tLine = 0;
        }

        /* Here we draw the current commandline, it will either be a line from
         * the command history or the line being edited atm */
        if (C->historyScrollIndex >= 0)
        {
            glColor3d(1,0,0);
            OGLCONSOLE_DrawString(
                    C->history[C->historyScrollIndex],
                    0, 0,
                    C->characterWidth,
                    C->characterHeight,
                    0);
        }
        else
        {

            /* Draw input line cyan */
            glColor3d(1,0,0);
            OGLCONSOLE_DrawCharacter(']'-' ',0, 0, C->characterWidth, C->characterHeight,0);

            glColor3d(0,1,1);
            OGLCONSOLE_DrawString(C->inputLine,
                    2*C->characterWidth, 0,
                    C->characterWidth,
                    C->characterHeight,
                    0);

            /* Draw cursor beige */
            glColor3d(1,1,.5);
            OGLCONSOLE_DrawCharacter('_'-' ',
                    (C->inputCursorPos+2) * C->characterWidth, 0,
                    C->characterWidth,
                    C->characterHeight,
                    0);
        }
    }
    glEnd();

    /* Relinquish our rendering settings */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
}

/* Issue rendering commands for a single a string */
void OGLCONSOLE_DrawString(char *s, double x, double y, double w, double h,
        double z)
{
    while (*s)
    {
        OGLCONSOLE_DrawCharacter(*s-' ', x, y, w, h, z);
        s++;
        x += w;
    }
}

/* Issue rendering commands for a single a string */
void OGLCONSOLE_DrawWrapString(char *s, double x, double y, double w, double h,
        double z, int wrap)
{
    int pos = 0;
    double X = x;

    while (*s)
    {
        OGLCONSOLE_DrawCharacter(*s-' ', X, y, w, h, z);
        s++;
        X += w;

        if (++pos >= wrap)
        {
            pos = 0;
            y += h;
            X = x;
        }
    }
}

/* Issue rendering commands for a single character */
void OGLCONSOLE_DrawCharacter(int c, double x, double y, double w, double h,
        double z)
{
//  static int message = 0;
    double cx, cy, cX, cY;
    
//    if (c < FIRST_CHARACTER || c > LAST_CHARACTER)
//        c = (c - FIRST_CHARACTER) % (LAST_CHARACTER - FIRST_CHARACTER);
//    else c -= FIRST_CHARACTER;

    cx = (c % 42) * CHAR_WIDTH;
    cy = 1.0 - (c / 42) * CHAR_HEIGHT;
    cX = cx + CHAR_WIDTH;
    cY = 1.0 - (c / 42 + 1) * CHAR_HEIGHT;

/*  if (message != c)
    {
        printf("For %i we got %f, %f\n", c, x, y);
        message = c;
    }*/

    /* This should occur outside of this function for optimiation TODO: MOVE IT */
    glTexCoord2d(cx, cy); glVertex3d(x,   y,   z);
    glTexCoord2d(cX, cy); glVertex3d(x+w, y,   z);
    glTexCoord2d(cX, cY); glVertex3d(x+w, y+h, z);
    glTexCoord2d(cx, cY); glVertex3d(x,   y+h, z);
}

/* This is the final, internal function for printing text to a console */
void OGLCONSOLE_Output(OGLCONSOLE_Console console, const char *s, ...)
{
    va_list argument;

    /* cache some console properties */
    int lineQueueIndex = C->lineQueueIndex;
    int lineScrollIndex = C->lineScrollIndex;
    int textWidth = C->textWidth;
    int maxLines = C->maxLines;

    /* String buffer */
    char output[4096];

    /* string copy cursors */
    char *consoleCursor, *outputCursor = output;

    /* Acrue arguments in argument list */
    va_start(argument, s);
    vsnprintf(output, 4096, s, argument);
    va_end(argument);



    /* This cursor tells us where in the console display we are currently
     * copying text into from the "output" string */
    consoleCursor = C->outputCursor;

    while (*outputCursor)
    {
        /* Here we check to see if any conditions require console line
         * advancement. These two conditions are:
            1) Hitting the end of the screen
            2) Getting a newlien character */
        if((C->outputNewline) ||
            (consoleCursor - (C->lines + lineQueueIndex * textWidth))
                >= (textWidth - 1))
        {
            C->outputNewline = 0;

            //puts("incrementing to the next line");

            /* Inrement text-line index, with wrapping */
            if (++lineQueueIndex >= maxLines)
                lineQueueIndex = 0;

            /* Scroll the console display one line TODO: Don't scroll if the console is
             * currently scrolled away from the end of output? */
            if (++lineScrollIndex >= maxLines)
                lineScrollIndex = 0;

            /* Reposition the cursor at the beginning of the new line */
            consoleCursor = C->lines + lineQueueIndex * C->textWidth;
        }
        
        /* If we encounter a newline character, we set the newline flag, which
         * tells the console to advance one line before it prints the next
         * character. The reason we do it this way is to defer line-advancement,
         * and thus we needn't suffer through a needless blank line between
         * console output and the command line, wasting precious screen
         * real-estate */
        if (*outputCursor == '\n')
        {
            C->outputNewline = 1;
            outputCursor++;
            continue;
        }

        /* copy a single character */
        *(consoleCursor++) = *(outputCursor++);
    }

    /* Unless we're at the very end of our current line, we finish up by capping
     * a NULL terminator on the current line */
    if (consoleCursor != C->lines + (lineQueueIndex+1) *C->textWidth -1)
        *consoleCursor = '\0';

    /* Restore cached values */
    C->lineQueueIndex = lineQueueIndex;
    C->lineScrollIndex = lineScrollIndex;
    C->outputCursor = consoleCursor; // TODO: confusing variable names

    /* old way of copying the text into the console */
    //strcpy(C->lines[C->lineQueueIndex], output);
#ifdef DEBUG
    printf("Copied \"%s\" into line %i\n", output, C->lineQueueIndex);
#endif
}

/* Mono-Console Users: print text to the console; multi-console users use
 * Output() */
void OGLCONSOLE_Print(char *s, ...)
{
    va_list argument;
    char output[4096];

    /* Acrue arguments in argument list */
    va_start(argument, s);
    vsnprintf(output, 4096, s, argument);
    va_end(argument);

    /* TODO: Find some way to pass the va_list arguments to OGLCONSOLE_Output
     * so that we don't waste extra time with the "%s" bullshit */
    OGLCONSOLE_Output((OGLCONSOLE_Console)userConsole, "%s", output);
}

#if 0
/* Multi-Console Users: print text to a specific console; mono-console users use
 * Print() */
void OGLCONSOLE_Output(OGLCONSOLE_Console console, char *s)
{
    /* TODO: Chop up output to wrap text */

    /* TODO: Add auto-scroll (the commented out code here is a failed attempt,
     * my brain is too scattered to do it) */
    /* If the console isn't scrolled up, then we move the scroll point */
/*    if (C->lineQueueIndex - C->lineScrollIndex == C->textHeight)
        if (++C->lineScrollIndex - C->lineScrollIndex >= MAX_LINE_COUNT)
            C->lineScrollIndex = 0;*/

}
#endif

/* Internal encapsulation of the act for adding a command the user executed to
 * their command history for that particular console */
void OGLCONSOLE_AddHistory(_OGLCONSOLE_Console *console, char *s)
{
    if (++C->historyQueueIndex > MAX_HISTORY_COUNT)
        C->historyQueueIndex = 0;

    strcpy(C->history[C->historyQueueIndex], s);
}

void OGLCONSOLE_YankHistory(_OGLCONSOLE_Console *console)
{
    /* First we have to see if we are browsing our command history */
    if (console->historyScrollIndex != -1)
    {
        /* Copy the selected command into the current input line */
        strcpy(console->inputLine,
                console->history[console->historyScrollIndex]);

        /* Set up this shite */
        console->inputCursorPos  = 
            console->inputLineLength =
            strlen(console->inputLine);

        /* Drop out of history browsing mode */
        console->historyScrollIndex = -1;
    }
}

#ifndef OGLCONSOLE_USE_SDL
#error **************************************************************************
#error
#error
#error
#error Only SDL is supported so far: you must define the OGLCONSOLE_USE_SDL macro
#error
#error
#error
#error **************************************************************************
#endif

/* This function tries to handle the incoming SDL event. In the future there may
 * be non-SDL analogs for input systems such as GLUT. Returns true if the event
 * was handled by the console. If console is hidden, no events are handled. */
#ifdef OGLCONSOLE_USE_SDL
#include "SDL.h"
#define KEY_BACKSPACE   SDLK_BACKSPACE
#define KEY_DELETE      SDLK_DELETE
#define KEY_RETURN      SDLK_RETURN
#define KEY_UP          SDLK_UP
#define KEY_DOWN        SDLK_DOWN
#define KEY_LEFT        SDLK_LEFT
#define KEY_RIGHT       SDLK_RIGHT
#define KEY_PAGEUP      SDLK_PAGEUP
#define KEY_PAGEDOWN    SDLK_PAGEDOWN
#define KEY_TAB         SDLK_TAB
#define KMOD_CAPITALIZE (KMOD_LSHIFT|KMOD_RSHIFT|KMOD_CAPS)
int OGLCONSOLE_SDLEvent(SDL_Event *e)
{
    char *c,*d;
    /* If the terminal is hidden we only check for show/hide key */
    if (userConsole->visibility < 1)
    {
        if (e->type == SDL_KEYDOWN && e->key.keysym.sym == '`')
        {  
            // TODO: Fetch values from OS?
            // TODO: Expose them to the program
//            SDL_EnableKeyRepeat(250, 30);
            userConsole->visibility += SLIDE_STEPS;
            return 1;
        }
        
        return 0;
    }

    /* TODO: SDL_KEYPRESS? ENABLE KEY REPEAT USING THE HIDE/SHOW FUNCTION? */
    if (e->type == SDL_KEYDOWN)
    {
        /* Reject most modifier keys TODO: Add some accelerator keys? */
        if (e->key.keysym.mod & ~(KMOD_CAPITALIZE)) return 0;

        /* Check for hide key */
        if (e->key.keysym.sym == '`')
        {
            /* Tell console to slide into closing */
            userConsole->visibility -= SLIDE_STEPS;

            /* Disable key repeat */
//            SDL_EnableKeyRepeat(0, 0);
            return 1;
        }
        if (e->key.keysym.sym == KEY_TAB) {
            char* part=userConsole->tabCompleteCallback((void*)userConsole,userConsole->inputLine);
            int i=0;
            for(i=0; i<strlen(part); i++) {
                c = userConsole->inputLine + userConsole->inputCursorPos;
                d = userConsole->inputLine + userConsole->inputLineLength + 1;

                /* Slide some of the string to the right */
                for (; d != c; d--)
                    *d = *(d-1);

                /* Insert new character */
                *c = part[i];

                /* Increment input line length counter */
                userConsole->inputLineLength++;

               /* Advance input cursor position */
                userConsole->inputCursorPos++;


            }
        }
        
        /* TODO: Find out how to handle CAPSLOCK */
        if (e->key.keysym.sym >= ' ' && e->key.keysym.sym <= '~')
        {
            int k = e->key.keysym.sym;

            /* Yank the command history if necessary */
            OGLCONSOLE_YankHistory(userConsole);

            /* Capitalize if necessary */
            if (e->key.keysym.mod & KMOD_CAPITALIZE)
            {
                static
                const int capital[] = { (int)' ', (int)'!', (int)'"', (int)'#',
                    (int)'$', (int)'%', (int)'&', (int)'"', (int)'(', (int)')',
                    (int)'*', (int)'+', (int)'<', (int)'_', (int)'>', (int)'?',
                    (int)')', (int)'!', (int)'@', (int)'#', (int)'$', (int)'%',
                    (int)'^', (int)'&', (int)'*', (int)'(', (int)':', (int)':',
                    (int)'<', (int)'+', (int)'>', (int)'?', (int)'@', (int)'A',
                    (int)'B', (int)'C', (int)'D', (int)'E', (int)'F', (int)'G',
                    (int)'H', (int)'I', (int)'J', (int)'K', (int)'L', (int)'M',
                    (int)'N', (int)'O', (int)'P', (int)'Q', (int)'R', (int)'S',
                    (int)'T', (int)'U', (int)'V', (int)'W', (int)'X', (int)'Y',
                    (int)'Z', (int)'{', (int)'|', (int)'}', (int)'^', (int)'_',
                    (int)'~', (int)'A', (int)'B', (int)'C', (int)'D', (int)'E',
                    (int)'F', (int)'G', (int)'H', (int)'I', (int)'J', (int)'K',
                    (int)'L', (int)'M', (int)'N', (int)'O', (int)'P', (int)'Q',
                    (int)'R', (int)'S', (int)'T', (int)'U', (int)'V', (int)'W',
                    (int)'X', (int)'Y', (int)'Z', (int)'{', (int)'|', (int)'}',
                    (int)'~' };

                /* If we're not explicitly holding a shift key, that means just
                 * capslock, which means we only capitalize letters */
                if ((k >= 'a' && k <= 'z') || (e->key.keysym.mod & KMOD_SHIFT))
                    k = capital[k-' '];
            }

            /* Point to the cursor position and the end of the string */
            c = userConsole->inputLine + userConsole->inputCursorPos;
            d = userConsole->inputLine + userConsole->inputLineLength + 1;

            /* Slide some of the string to the right */
            for (; d != c; d--)
                *d = *(d-1);

            /* Insert new character */
            *c = k;

            /* Increment input line length counter */
            userConsole->inputLineLength++;

            /* Advance input cursor position */
            userConsole->inputCursorPos++;

            return 1;
        }

        else if (e->key.keysym.sym == KEY_DELETE
             ||  e->key.keysym.sym == KEY_BACKSPACE)
        {
            /* Yank the command history if necessary */
            OGLCONSOLE_YankHistory(userConsole);

            /* If string is not empty */
            if (userConsole->inputLineLength)
            {
                char *end, *c;

                /* Backspace is like pressing LEFT and then DELETE */
                if (e->key.keysym.sym == KEY_BACKSPACE)
                    userConsole->inputCursorPos--;

                /* With delete we much check more than just inputLineLength */
                else if (userConsole->inputCursorPos ==
                        userConsole->inputLineLength)
                    return 1;

                c   = userConsole->inputLine +   userConsole->inputCursorPos;
                end = userConsole->inputLine + --userConsole->inputLineLength;

                while (c <= end)
                {
                    *c = *(c+1);
                    c++;
                }
            }

            return 1;
        }

        else if (e->key.keysym.sym == KEY_RETURN)
        {
            /* Yank the command history if necessary */
            OGLCONSOLE_YankHistory(userConsole);

            /* Add user's command to history */
            OGLCONSOLE_AddHistory(userConsole, userConsole->inputLine);

            /* Print user's command to the console */
            OGLCONSOLE_Output((void*)userConsole, "] %s\n", userConsole->inputLine);

            /* Invoke console's enter-key callback function */
            userConsole->enterKeyCallback((void*)userConsole,userConsole->inputLine);

            /* Erase command line */
            userConsole->inputCursorPos = 0;
            userConsole->inputLineLength = 0;
            userConsole->inputLine[0] = '\0';

            return 1;
        }

        // Page up key
        else if (e->key.keysym.sym == SDLK_PAGEUP)
        {
            userConsole->lineScrollIndex -= userConsole->textHeight / 2;

            if (userConsole->lineScrollIndex < 0)
                userConsole->lineScrollIndex += userConsole->maxLines;

            printf("scroll index = %i\n", userConsole->lineScrollIndex);
        }

        // Page down key
        else if (e->key.keysym.sym == SDLK_PAGEDOWN)
        {
            userConsole->lineScrollIndex += userConsole->textHeight / 2;

            if (userConsole->lineScrollIndex >= userConsole->maxLines)
                userConsole->lineScrollIndex -= userConsole->maxLines;

            printf("scroll index = %i\n", userConsole->lineScrollIndex);
        }

        // Arrow key up
        else if (e->key.keysym.sym == KEY_UP)
        {
            // Shift key is for scrolling the output display
            if (e->key.keysym.mod & (KMOD_LSHIFT|KMOD_RSHIFT))
            {
                if (--userConsole->lineScrollIndex < 0)
                    userConsole->lineScrollIndex = userConsole->maxLines-1;
            }

            // No shift key is for scrolling through command history
            else
            {
                // -1 means we aren't look at history yet
                if (userConsole->historyScrollIndex == -1)
                {
                    userConsole->historyScrollIndex =
                        userConsole->historyQueueIndex;
                }
                else
                {
                    // Wrap our history scrolling
                    if (--userConsole->historyScrollIndex < 0)
                        userConsole->historyScrollIndex = MAX_HISTORY_COUNT;
                }

                // If we've returned to our current position in the command
                // history, we'll just drop out of history mode
                if (userConsole->historyScrollIndex ==
                        userConsole->historyQueueIndex +1)
                    userConsole->historyScrollIndex = -1;
            }

            return 1;
        }

        // Arrow key down
        else if (e->key.keysym.sym == KEY_DOWN)
        {
            // Shift key is for scrolling the output display
            if (e->key.keysym.mod & (KMOD_LSHIFT|KMOD_RSHIFT))
            {
                if (++userConsole->lineScrollIndex >= userConsole->maxLines)
                    userConsole->lineScrollIndex = 0;
            }

            // No shift key is for scrolling through command history
            else
            {
                // -1 means we aren't look at history yet
                if (userConsole->historyScrollIndex != -1)
                {
                    // Wrap our history scrolling
                    if (++userConsole->historyScrollIndex >= MAX_HISTORY_COUNT)
                        userConsole->historyScrollIndex = 0;

                    // If we've returned to our current position in the command
                    // history, we'll just drop out of history mode
                    if (userConsole->historyScrollIndex ==
                            userConsole->historyQueueIndex +1)
                    userConsole->historyScrollIndex = -1;
                }
                else
                {
                    // TODO: be like, no bitch, there's no history down there
                }
            }
            return 1;
        }

        // Arrow key left
        else if (e->key.keysym.sym == KEY_LEFT)
        {
            /* Yank the command history if necessary */
            OGLCONSOLE_YankHistory(userConsole);

            if (userConsole->inputCursorPos > 0)
                userConsole->inputCursorPos--;

            return 1;
        }

        // Arrow key right
        else if (e->key.keysym.sym == KEY_RIGHT)
        {
            /* Yank the command history if necessary */
            OGLCONSOLE_YankHistory(userConsole);

            if (userConsole->inputCursorPos <
                userConsole->inputLineLength)
                userConsole->inputCursorPos++;

            return 1;
        }
    }

    return 0;
}
#endif

