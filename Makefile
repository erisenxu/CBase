########################################################################
# This makefile is created by python script, please not edit it :)     #
########################################################################

include Makefile.rules

LIB_OBJS = $(OBJ_DIR)/ByteArray.o \
		   $(OBJ_DIR)/FileLib.o \
		   $(OBJ_DIR)/Logger.o \
		   $(OBJ_DIR)/FixMemPool.o \
		   $(OBJ_DIR)/FMsgChannel.o \
		   $(OBJ_DIR)/FMsgQ.o \
		   $(OBJ_DIR)/EpollServer.o \
		   $(OBJ_DIR)/net.o \
		   $(OBJ_DIR)/ShmMgr.o \
		   $(OBJ_DIR)/ShmTable.o

TARGET_OBJS = $(LIB_OBJS) \
			  $(OBJ_DIR)/main.o \
			  $(OBJ_DIR)/ShmTableTest.o

$(LIB_TARGET): $(LIB_OBJS)
	$(LIB) $@ $^

$(TARGET): $(TARGET_OBJS)
	$(LINK) -o $@ $^

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/ByteArray.o: $(SRC_DIR)/comm/ByteArray.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/FileLib.o: $(SRC_DIR)/comm/FileLib.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/Logger.o: $(SRC_DIR)/log/Logger.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/FixMemPool.o: $(SRC_DIR)/mem/FixMemPool.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/FMsgChannel.o: $(SRC_DIR)/msgq/FMsgChannel.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/FMsgQ.o: $(SRC_DIR)/msgq/FMsgQ.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/EpollServer.o: $(SRC_DIR)/net/EpollServer.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/net.o: $(SRC_DIR)/net/net.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/ShmMgr.o: $(SRC_DIR)/shm/ShmMgr.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/ShmTable.o: $(SRC_DIR)/shm/ShmTable.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

$(OBJ_DIR)/ShmTableTest.o: $(SRC_DIR)/test/ShmTableTest.c
	$(CPP) $^ -o $@ $(CXXFLAGS)

clean:
	rm -f $(TARGET_OBJS) $(TARGET) $(LIB_TARGET)

install:
	cp $(LIB_TARGET) $(INSTALL_DIR)/lib
