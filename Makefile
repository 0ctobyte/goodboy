AS := clang
CC := clang++
LD := ld

PROGRAM := gb_emulator

CPP_SRCS := $(wildcard src/*.cpp)
S_SRCS := $(wildcard src/*.s)

OBJS := $(patsubst %.s,%.o,$(S_SRCS))
OBJS += $(patsubst %.cpp,%.o,$(CPP_SRCS))

INCLUDE := -Iinclude
LIBS    := -lncurses -lSystem -lc++

BASEFLAGS := -g
WARNFLAGS := -Weverything -Werror -Wno-conversion -Wno-unused-parameter -Wno-old-style-cast -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-reserved-id-macro -Wno-c++98-compat -Wno-exit-time-destructors -Wno-global-constructors -Wno-format-nonliteral -Wno-padded -Wno-missing-prototypes -Wno-weak-vtables -Wno-unused-exception-parameter
CPPFLAGS := -std=c++11 $(DEFINES) $(BASEFLAGS) $(WARNFLAGS) $(INCLUDE)
LDFLAGS := -arch x86_64 -macosx_version_min 10.11 $(LIBS)
ASFLAGS := $(BASEFLAGS) $(WARNFLAGS)

$(PROGRAM): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o: %.s Makefile
	$(AS) $(ASFLAGS) -c $< -o $@

%.o: %.cpp Makefile
	$(CC) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(PROGRAM)

