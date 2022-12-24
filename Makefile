

build:
	@echo "Building..."
	gcc -o bin/main main.c cron_util/cron_util.c network/server/server.c network/client/client.c rtlsp/rtlsp.c util/arg_handler.c -lpthread

run: build
	@echo "Running..."
	sudo ./bin/main $(MI) $(H) $(DM) $(MO) $(DW) $(CMD) $(OUT)

clear_bin:
	@echo "Clearing bin..."
	rm -rf bin/*

clear_dump:
	@echo "Clearing dump..."
	rm -rf out/dump/dump_*

clear_log:
	@echo "Clearing log..."
	rm -rf out/log/log_*

clear: clear_bin clear_dump clear_log

.PHONY: build run clear_bin clear_dump clear_log clear