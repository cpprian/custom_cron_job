build:
	@echo "Building..."
	gcc -o bin/main main.c cron_util/cron_util.c cron_util/cron_timer/cron_timer.c rtlsp/rtlsp.c util/arg_handler.c util/client_task.c util/server_task.c -lpthread

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

.PHONY: build run run_list run_cancel clear_bin clear_dump clear_log clear