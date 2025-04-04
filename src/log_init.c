#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initialize logging with default settings
void log_init_default(void) {
	// Set default log level to INFO
	log_set_level(LOG_INFO);
	log_set_quiet(false);
}

// Initialize logging for production environment
void log_init_production(void) {
	// In production, we typically want warnings and errors
	log_set_level(LOG_WARN);

	// Open a log file
	FILE *logfile = fopen("hyperkv.log", "a");
	if (logfile == NULL) {
		fprintf(stderr, "Error opening log file. Logging to stderr only.\n");
		return;
	}

	// Add file logger with all severity levels
	log_add_fp(logfile, LOG_INFO);

	log_info("Logging initialized in production mode");
}

// Initialize logging for development with verbose output
void log_init_development(void) {
	// In development, we want debug information
	log_set_level(LOG_DEBUG);
	log_set_quiet(false);

	// Open a log file for development
	FILE *logfile = fopen("hyperkv-dev.log", "a");
	if (logfile == NULL) {
		fprintf(stderr, "Error opening development log file. Logging to stderr only.\n");
		return;
	}

	// Add file logger with DEBUG severity
	log_add_fp(logfile, LOG_DEBUG);

	log_info("Logging initialized in development mode");
}