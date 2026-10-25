.PHONY: all

all: lib_src_cmd cmd_cmd

lib_src_cmd:
	$(MAKE) -C ./src/lib_src 
	
	
cmd_cmd:
	$(MAKE) -C ./src/cmd
	
	
clean:
	$(MAKE) -C ./src/lib_src clean
	$(MAKE) -C ./src/cmd clean
