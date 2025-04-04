#include "common.h"
#include "log.h"
#include "log_init.h"
#include <stdio.h>
#include <stdlib.h>

static void print_intro() {
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
	printf("%s", intro);
	printf("\n");
	printf("       Developed by: Shikhar Soni\n");
	printf("\n");
}

static void close_server(int sfd, HashTable *ht) {
	log_info("Shutting down server");
	close_socket(sfd);
	htable_free(ht);
	log_info("Server shutdown complete");
	exit(0);
}

int main(int argc, char **argv) {
	// Initialize logging with development configuration 
	// (could be based on command line args in a real system)
	log_init_development();
	
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
