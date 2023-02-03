CC = g++

BUILD_DIR = build

CFLAGS = -std=c++11


OBJECTS = \
	$(BUILD_DIR)/segment.o \


SenderSensitivities = \
	sender.cpp \
	segment.hpp \


SegmentSensitivities = \
	segment.cpp \
	segment.hpp \

RouterSensitivities = \
	router.cpp \
	segment.hpp \


all: $(BUILD_DIR) sender.out router.out


$(BUILD_DIR):
	rm -Rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)


sender.out: $(BUILD_DIR)/sender.o $(OBJECTS)
	$(CC) $(CFLAGS) -o sender.out $(BUILD_DIR)/sender.o $(OBJECTS)


router.out: $(BUILD_DIR)/router.o $(OBJECTS)
	$(CC) $(CFLAGS) -o router.out $(BUILD_DIR)/router.o $(OBJECTS)


$(BUILD_DIR)/segment.o: $(SegmentSensitivities)
	$(CC) $(CFLAGS) -c segment.cpp -o $(BUILD_DIR)/segment.o


$(BUILD_DIR)/sender.o: $(SenderSensitivities)
	$(CC) $(CFLAGS) -c sender.cpp -o $(BUILD_DIR)/sender.o


$(BUILD_DIR)/router.o: $(RouterSensitivities)
	$(CC) $(CFLAGS) -c router.cpp -o $(BUILD_DIR)/router.o


.PHONY: clean

clean:
	rm -rf $(BUILD_DIR) *.o *.out
