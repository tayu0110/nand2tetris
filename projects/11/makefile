COMPILER	= g++
CFLAGS		= -g -Wall -Wextra -Winit-self -Wno-missing-field-initializers -std=c++17
ifeq "$(shell getconf LONG_BIT)" "64"
	LDFLAGS	=
else
	LDFLAGS	=
endif
LIBS			=
INCLUDE		= -I./include
# TARGET		= ./bin/$(shell basename `readlink -f .`)
TARGET		= ./bin/JackAnalyzer.exe
SRCDIR		= ./source
ifeq "$(strip $(SRCDIR))" ""
	SRCDIR	= .
endif
SOURCES		= $(wildcard $(SRCDIR)/*.cpp)
OBJDIR		= ./obj
ifeq "$(strip $(OBJDIR))" ""
	OBJDIR	= .
endif
OBJECTS		= $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS		= $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
		$(COMPILER) -o $@ $^ $(LDFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
		-mkdir -p $(OBJDIR)
		$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
		-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)