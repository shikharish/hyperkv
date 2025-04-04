#include "test.h"
#include "../src/log.h"
#include "../src/log_init.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#define MINIUNIT_MAIN
#include "miniunit.h"
#define B 1000000000.0

int main(int argc, char **argv) {
	// First check environment variables - they have lower precedence than command line args
	char *env_log_level = getenv("HYPERKV_LOG_LEVEL");
	if (env_log_level != NULL) {
		log_init_from_env();
	} else {
		// Process command-line arguments for log level - they override environment variables
		if (argc > 1) {
			if (strcmp(argv[1], "--test") == 0) {
				// Disable all logs for testing
				log_init_testing();
			} else if (strcmp(argv[1], "--dev") == 0) {
				// Developer mode with full logs
				log_init_development();
			} else if (strcmp(argv[1], "--prod") == 0) {
				// Production mode with minimal logs
				log_init_production();
			}
		} else {
			// Default to testing mode when running tests (no logs)
			log_init_testing();
		}
	}

	double dur;
	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	test_htable();
	test_parser();
	test_interpret();
	clock_gettime(CLOCK_REALTIME, &end);
	dur = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / B;
	printf("test duration = %lf\n", dur);
	return 0;
}
