########################################################################
# This makefile is created by python script, please not edit it :)     #
########################################################################

include Makefile.rules

LIB_SRC = $(wildcard ${SRC_DIR}/comm/*.c) \
		  $(wildcard ${SRC_DIR}/crypt/*.c) \
		  $(wildcard ${SRC_DIR}/http/*.c) \
		  $(wildcard ${SRC_DIR}/json/*.c) \
		  $(wildcard ${SRC_DIR}/list/*.c) \
		  $(wildcard ${SRC_DIR}/log/*.c) \
		  $(wildcard ${SRC_DIR}/mem/*.c) \
		  $(wildcard ${SRC_DIR}/msgq/*.c) \
		  $(wildcard ${SRC_DIR}/net/*.c) \
		  $(wildcard ${SRC_DIR}/shm/*.c)

LIB_OBJS = $(patsubst %.c,%.o,${LIB_SRC})

TARGET_SRC = $(wildcard ${SRC_DIR}/*.c) \
			 $(wildcard ${SRC_DIR}/test/*.c)

TARGET_OBJS = $(patsubst %.c,%.o,${TARGET_SRC})

$(LIB_TARGET): $(LIB_OBJS)
	$(LIB) $@ $^

$(TARGET): $(LIB_OBJS) $(TARGET_OBJS)
	$(LINK) -o $@ $^ $(CXXLIBS)

${SRC_DIR}/comm/%.o:${SRC_DIR}/comm/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/crypt/%.o:${SRC_DIR}/crypt/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/http/%.o:${SRC_DIR}/http/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/json/%.o:${SRC_DIR}/json/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/list/%.o:${SRC_DIR}/list/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/log/%.o:${SRC_DIR}/log/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/mem/%.o:${SRC_DIR}/mem/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/msgq/%.o:${SRC_DIR}/msgq/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/net/%.o:${SRC_DIR}/net/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/shm/%.o:${SRC_DIR}/shm/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

${SRC_DIR}/%.o:${SRC_DIR}/%.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

$(OBJ_DIR)/test/*.o: $(SRC_DIR)/test/*.c
	$(CPP) $(CXXFLAGS) -c  $< -o $@

clean:
	rm -f $(LIB_OBJS) $(TARGET_OBJS) $(TARGET) $(LIB_TARGET)

install:
	cp $(LIB_TARGET) $(INSTALL_DIR)/lib
