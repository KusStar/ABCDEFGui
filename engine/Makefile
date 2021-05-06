BUILD_DIR = build
EXE = $(BUILD_DIR)/test
QUICKJS_DIR = ../quickjs
SOURCES = test.cpp
SOURCES += $(QUICKJS_DIR)/quickjs.c $(QUICKJS_DIR)/libunicode.c $(QUICKJS_DIR)/libregexp.c $(QUICKJS_DIR)/cutils.c $(QUICKJS_DIR)/quickjs-libc.c
OBJS = $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
CFLAGS = -g -Wall -I../quickjs
CFLAGS += -DCONFIG_VERSION=\"2021-03-27\"

$(BUILD_DIR)/%.o:%.cpp $(BUILD_DIR)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o:$(QUICKJS_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all:$(EXE)
	@echo Build complete
	./$(EXE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	@echo $(SOURCES)
	@echo $(OBJS)

$(EXE):$(OBJS)
	$(CXX) $(LDFLAGS) -fcommon $(LDEXPORT) -o $@ $^ $(LIBS)
	
clean:
	rm -rf $(BUILD_DIR)