#
# Makefile for freeglut library
#



#
# Package name and list of source files.
#

NAME=fglut
FGLUT_CSRCS = \
freeglut_callbacks.c \
freeglut_cursor.c \
freeglut_display.c \
freeglut_ext.c \
freeglut_font.c \
freeglut_glutfont_definitions.c \
freeglut_font_data.c \
freeglut_stroke_roman.c \
freeglut_stroke_mono_roman.c \
freeglut_gamemode.c \
freeglut_geometry.c \
freeglut_init.c \
freeglut_input_devices.c \
freeglut_spaceball.c \
freeglut_joystick.c \
freeglut_main.c \
freeglut_menu.c \
freeglut_misc.c \
freeglut_overlay.c \
freeglut_state.c \
freeglut_structure.c \
freeglut_teapot.c \
freeglut_videoresize.c \
freeglut_window.c

ifeq ("$(shell uname -s)", "Darwin")
  CSRCS = $(NAME).c 
else
  CSRCS=$(FGLUT_CSRCS)
endif



#
# Targets
#

OBJS=$(CSRCS:.c=.o) 



#
# C flags
#

CC=g++
BASE_CFLAGS=$(USER_CFLAGS) -Wall -I. -I..
DEBUG_CFLAGS=$(BASE_CFLAGS) -g
OPT_CFLAGS=$(BASE_CFLAGS) -O3 -DNDEBUG
CFLAGS=$(DEBUG_CFLAGS)



#
# Default rules
#

.SUFFIXES: .c .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) -c $<

.c.o:
	gcc $(CFLAGS) -c $<



#
# Set up target name
#

LIB=./lib$(NAME).a



#
# Make targets
#

opt:
	    $(MAKE) $(LIB) "CFLAGS=$(OPT_CFLAGS)" 

debug:
	    $(MAKE) $(LIB) "CFLAGS=$(DEBUG_CFLAGS)" 

$(LIB):     $(CSRCS) $(OBJS) 
	    rm -f $(LIB)
	    ar ur $(LIB) $(OBJS) $(USER_OBJS)

clean:
	    - rm -f *~ *.o $(LIB)








