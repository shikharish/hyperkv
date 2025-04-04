#include "common.h"
#include "log.h"
#include "log_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage() {
	printf("Usage: hyperkv-cli [options]\n");
	printf("Options:\n");
	printf("  --dev         Run in development mode with debug logs\n");
	printf("  --prod        Run in production mode with minimal logs\n");
	printf("  --test        Run in test mode with no logs\n");
	printf("  --help        Display this help message\n");
}

int main(int argc, char **argv) {
	// First check environment variables
	char *env_log_level = getenv("HYPERKV_LOG_LEVEL");
	if (env_log_level != NULL) {
		log_init_from_env();
	} else {
		// Process command-line arguments for log level
		if (argc > 1) {
			if (strcmp(argv[1], "--dev") == 0) {
				log_init_development();
			} else if (strcmp(argv[1], "--prod") == 0) {
				log_init_production();
			} else if (strcmp(argv[1], "--test") == 0) {
				log_init_testing();
			} else if (strcmp(argv[1], "--help") == 0) {
				print_usage();
				return 0;
			} else {
				printf("Unknown option: %s\n", argv[1]);
				print_usage();
				return 1;
			}
		} else {
			// Default to standard info level
			log_init_default();
		}
	}
	
	log_info("HyperKV CLI Starting");
	
	int cfd = connect_server(LOCALHOST, PORT_NUM);
	log_info("Connected to HyperKV server at %s:%d", LOCALHOST, PORT_NUM);
	
	repl(cfd);
	log_info("HyperKV CLI Terminating");
	
	return 0;
}
