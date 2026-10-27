# HOGN for macOS — builds HOGN.app with the Command Line Tools (no Xcode needed).
#
#   make            build build/HOGN.app
#   make run        build and launch it
#   make install    copy it to /Applications
#   make clean
#
# Universal build (Apple silicon + Intel), as used for releases:
#   make ARCHS="-arch arm64 -arch x86_64"

APP      := build/HOGN.app
BIN      := $(APP)/Contents/MacOS/HOGN
RES      := $(APP)/Contents/Resources
OBJDIR   := build/obj

SRCS_MM  := $(wildcard src/*.mm)
SRCS_CPP := $(wildcard src/*.cpp) ../HOGN/qrcodegen.cpp
OBJS     := $(patsubst src/%.mm,$(OBJDIR)/%.o,$(SRCS_MM)) \
            $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(filter src/%,$(SRCS_CPP))) \
            $(OBJDIR)/qrcodegen.o

CXX      := clang++
ARCHS    ?=
CXXFLAGS := $(ARCHS) -std=c++17 -O2 -Wall -Wno-unused-function -mmacosx-version-min=12.0 -MMD -MP
MMFLAGS  := $(CXXFLAGS) -fobjc-arc
LDFLAGS  := $(ARCHS) -mmacosx-version-min=12.0 \
            -framework Cocoa -framework Carbon -framework ApplicationServices \
            -framework UniformTypeIdentifiers

ICONS    := $(RES)/hands.png $(RES)/hand_gray.png $(RES)/HOGN.icns

.PHONY: all run install clean

all: $(APP)

$(APP): $(BIN) $(APP)/Contents/Info.plist $(ICONS)
	# Ad-hoc signature with a requirement tied to the bundle id instead of the
	# per-build cdhash, so Accessibility / Input Monitoring grants survive rebuilds.
	codesign --force --sign - --identifier top.hogn.HOGN \
		-r='designated => identifier "top.hogn.HOGN"' $(APP)

$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: src/%.mm
	@mkdir -p $(OBJDIR)
	$(CXX) $(MMFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/qrcodegen.o: ../HOGN/qrcodegen.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(APP)/Contents/Info.plist: resources/Info.plist
	@mkdir -p $(dir $@)
	cp $< $@

# Menu bar icons, converted from the Windows .ico files.
$(RES)/%.png: ../HOGN/%.ico
	@mkdir -p $(RES)
	sips -s format png -z 36 36 $< --out $@ >/dev/null

$(RES)/HOGN.icns: ../HOGN/hands.ico
	@mkdir -p $(RES) build/HOGN.iconset
	for s in 16 32 128 256 512; do \
		sips -s format png -z $$s $$s $< --out build/HOGN.iconset/icon_$${s}x$${s}.png >/dev/null; \
		d=$$((s*2)); sips -s format png -z $$d $$d $< --out build/HOGN.iconset/icon_$${s}x$${s}@2x.png >/dev/null; \
	done
	iconutil -c icns build/HOGN.iconset -o $@

run: all
	open $(APP)

install: all
	rm -rf /Applications/HOGN.app
	cp -R $(APP) /Applications/

clean:
	rm -rf build

-include $(OBJS:.o=.d)
