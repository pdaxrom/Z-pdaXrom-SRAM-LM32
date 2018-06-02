TARGET = bootloader

OBJS = bootloader.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -f $(TARGET) $(OBJS)
