#include "common.h"
#include "log.h"
#include "log_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_intro() {
	const char *green_color = "\033[32m"; // ANSI code for green text
	const char *reset_color = "\033[0m";  // ANSI code to reset color
	const char *intro =
		" █████   █████                                         █████   ████ █████   █████\n"
		"░░███   ░░███                                         ░░███   ███░ ░░███   ░░███ \n"
		" ░███    ░███  █████ ████ ████████   ██████  ████████  ░███  ███    ░███    ░███ \n"
		" ░███████████ ░░███ ░███ ░░███░░███ ███░░███░░███░░███ ░███████     ░███    ░███ \n"
		" ░███░░░░░███  ░███ ░███  ░███ ░███░███████  ░███ ░░░  ░███░░███    ░░███   ███  \n"
		" ░███    ░███  ░███ ░███  ░███ ░███░███░░░   ░███      ░███ ░░███    ░░░█████░   \n"
		" █████   █████ ░░███████  ░███████ ░░██████  █████     █████ ░░████    ░░███     \n"
		"░░░░░   ░░░░░   ░░░░░███  ░███░░░   ░░░░░░  ░░░░░     ░░░░░   ░░░░      ░░░      \n"
		"                ███ ░███  ░███                                                   \n"
		"               ░░██████   █████                                                  \n"
		"                ░░░░░░   ░░░░░                                                   \n";

	printf("\n");
	printf("%s%s%s", green_color, intro, reset_color);
	printf("\n");
	const char *credits = "Developed by: Shikhar Burman";
	printf("%s%s%s", green_color, credits, reset_color);
	printf("\n");
}

static void print_usage() {
	printf("Usage: hyperkv [options]\n");
	printf("Options:\n");
	printf("  --dev         Run in development mode with debug logs\n");
	printf("  --prod        Run in production mode with minimal logs\n");
	printf("  --test        Run in test mode with no logs\n");
	printf("  --help        Display this help message\n");
}

static void close_server(int sfd, HashTable *ht) {
	log_info("Shutting down server");
	close_socket(sfd);
	htable_free(ht);
	log_info("Server shutdown complete");
	exit(0);
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
				exit(0);
			} else {
				printf("Unknown option: %s\n", argv[1]);
				print_usage();
				exit(1);
			}
		} else {
			// Default to development mode with verbose logging
			log_init_development();
		}
	}

	log_info("Initializing HyperKV server");

	HashTable *ht = htable_init(HT_BASE_SIZE);
	if (ht == NULL) {
		log_fatal("Failed to initialize hash table");
		exit(1);
	}
	log_info("Hash table initialized with base size %d", HT_BASE_SIZE);

	print_intro();

	int sfd = init_server();
	log_info("Server initialized and listening on port %d", PORT_NUM);

	while (1) {
		log_debug("Waiting for incoming connections");
		int cfd = accept_connection(sfd);
		log_info("Accepted new client connection");

		int code = rediskw(cfd, ht);
		if (code == 1) {
			log_info("Received shutdown command");
			close_server(sfd, ht);
		} else {
			log_debug("Closing client connection");
			close_socket(cfd);
		}
	}
	return 0;
}
