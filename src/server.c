#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "log.h"

int init_server() {
	struct sockaddr_in serv_addr;
	// create socket
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		log_fatal("Failed to create socket");
		fputs("failed to create socket", stderr);
		exit(1);
	}
	log_debug("Socket created successfully");

	// assign ip & port
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT_NUM);
	log_debug("Server configured to listen on port %d", PORT_NUM);

	// bind socket to ip
	if (bind(sfd, (SA *)&serv_addr, sizeof(serv_addr)) != 0) {
		log_fatal("Failed to bind socket to port %d", PORT_NUM);
		fputs("failed to bind socket", stderr);
		exit(1);
	}
	log_debug("Socket bound successfully");

	// server listen
	if (listen(sfd, 5) != 0) {
		log_fatal("Failed to start listening");
		fputs("listen failed", stderr);
		exit(1);
	}

	log_info("Server started successfully on port %d", PORT_NUM);
	puts("server started");
	return sfd;
}

int accept_connection(int sfd) {
	struct sockaddr_in client_addr;
	unsigned int len = sizeof(struct sockaddr_in);
	int cfd = accept(sfd, (SA *)&client_addr, &len);
	if (cfd < 0) {
		log_error("Failed to accept client connection");
		fputs("failed to accept connection", stderr);
		exit(1);
	}
	log_debug("Connection accepted, client fd: %d", cfd);
	return cfd;
}

void close_socket(int sockfd) { 
	close(sockfd); 
	log_trace("Socket %d closed", sockfd);
}

void close_client(int cfd) {
	log_debug("Closing client connection (fd: %d)", cfd);
	shutdown(cfd, SHUT_RDWR);
	close(cfd);
}

char *readline(int cfd) {
	char *msg = dmalloc(1024 * sizeof(char));
	read(cfd, msg, 1024);
	msg[strlen(msg) - 1] = '\0';
	log_trace("Read from client fd %d: %s", cfd, msg);
	return msg;
}

void writeline(int cfd, char *msg) {
	char *tmp = dmalloc((strlen(msg) + 2) * sizeof(char));
	sprintf(tmp, "%s\n", msg);
	write(cfd, tmp, strlen(tmp) + 1);
	log_trace("Written to client fd %d: %s", cfd, msg);
	free(tmp);
}

int rediskw(int cfd, HashTable *ht) {
	log_debug("Started processing commands for client fd: %d", cfd);
	while (1) {
		char *msg = readline(cfd);
		msg[strcspn(msg, "\n")] = 0;
		log_debug("Received command: %s", msg);
		
		Command *cmd = parse(msg);
		if (cmd->type == UNKNOWN) {
			log_warn("Unknown command received from client fd: %d", cfd);
		}
		
		char *resp = interpret(ht, cmd);
		log_debug("Command processed, response type: %c", *resp);
		
		switch (*resp) {
		case 'q':
			log_info("Client requested to quit, fd: %d", cfd);
			return 0;
		case 'x':
			log_info("Server shutdown requested by client fd: %d", cfd);
			return 1;
		default:
			writeline(cfd, resp);
		}
		free(msg);
	}
	return 0;
}
