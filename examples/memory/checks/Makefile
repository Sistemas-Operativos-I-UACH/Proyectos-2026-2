TARGET_DIR = target

.PHONY: all checks dyn vuln dyn_vuln debug debug_dyn clean

dirs: target
	mkdir target

all: $(TARGET_DIR)/memory_checks $(TARGET_DIR)/dynamic_memory
checks: $(TARGET_DIR)/memory_checks
dyn: $(TARGET_DIR)/dynamic_memory


$(TARGET_DIR)/memory_checks: memory_checks.c | $(TARGET_DIR)
	gcc -ggdb memory_checks.c -O0 -o target/memory_checks

$(TARGET_DIR)/dynamic_memory: dynamic_memory.c | $(TARGET_DIR)
	gcc -ggdb dynamic_memory.c -O0 -o target/dynamic_memory

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

vuln: $(TARGET_DIR)
	echo "Creating vulnerable progam"
	gcc -Wno-stack-protector -O0 -z execstack -D_FORTIFY_SOURCE=o -ggdb memory_checks.c -o $(TARGET_DIR)/memory_checks

dyn_vuln: $(TARGET_DIR)
	echo "Creating vulnerable progam"
	gcc -Wno-stack-protector -O -z execstack -D_FORTIFY_SOURCE=o -ggdb dynamic_memory.c -o $(TARGET_DIR)/dynamic_memory

debug: $(TARGET_DIR)
	gdb --args $(TARGET_DIR)/memory_checks

debug_dyn: $(TARGET_DIR)
	gdb --args $(TARGET_DIR)/dynamic_memory

debug_dyn_vuln: dyn_vuln
	gdb --args $(TARGET_DIR)/dynamic_memory

clean:
	rm -rf $(TARGET_DIR)
