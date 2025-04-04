#include "common.h"
#include "log.h"
#include "log_init.h"
#include <stdio.h>

int main(int argc, char **argv) {
	log_init_default();
	log_info("HyperKV CLI Starting");
	
	int cfd = connect_server(LOCALHOST, PORT_NUM);
	log_info("Connected to HyperKV server at %s:%d", LOCALHOST, PORT_NUM);
	
	repl(cfd);
	log_info("HyperKV CLI Terminating");
	
	return 0;
}
