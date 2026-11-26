TARGET_DIR = target

dirs:
	echo "Creating $(TARGET_DIR) directory"
	mkdir -p target

all: dirs
	echo "Compiling simple_ps"
	gcc simple_ps.c -o $(TARGET_DIR)/simple_ps

run: all
	./$(TARGET_DIR)/simple_ps

clean:
	rm -rf $(TARGET_DIR)
