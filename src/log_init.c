#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// Initialize logging for testing environment
void log_init_testing(void) {
	// In testing, we want to disable all logs
	// Setting level to FATAL means only fatal errors will be logged
	log_set_level(LOG_FATAL);
	
	// Set quiet mode to true to suppress all console output
	log_set_quiet(true);
}

// Initialize logging using environment variables
void log_init_from_env(void) {
	char *log_level = getenv("HYPERKV_LOG_LEVEL");
	
	if (log_level != NULL) {
		if (strcmp(log_level, "DEV") == 0 || strcmp(log_level, "DEBUG") == 0) {
			log_init_development();
		} else if (strcmp(log_level, "PROD") == 0 || strcmp(log_level, "PRODUCTION") == 0) {
			log_init_production();
		} else if (strcmp(log_level, "TEST") == 0 || strcmp(log_level, "TESTING") == 0) {
			log_init_testing();
		} else if (strcmp(log_level, "TRACE") == 0) {
			log_set_level(LOG_TRACE);
		} else if (strcmp(log_level, "DEBUG") == 0) {
			log_set_level(LOG_DEBUG);
		} else if (strcmp(log_level, "INFO") == 0) {
			log_set_level(LOG_INFO);
		} else if (strcmp(log_level, "WARN") == 0 || strcmp(log_level, "WARNING") == 0) {
			log_set_level(LOG_WARN);
		} else if (strcmp(log_level, "ERROR") == 0) {
			log_set_level(LOG_ERROR);
		} else if (strcmp(log_level, "FATAL") == 0) {
			log_set_level(LOG_FATAL);
		} else {
			// Default to info level if unknown value
			log_init_default();
		}
	} else {
		// No environment variable set, use default
		log_init_default();
	}
	
	// Check for quiet mode
	char *quiet_mode = getenv("HYPERKV_LOG_QUIET");
	if (quiet_mode != NULL && (strcmp(quiet_mode, "1") == 0 || strcmp(quiet_mode, "true") == 0 || 
	                          strcmp(quiet_mode, "TRUE") == 0 || strcmp(quiet_mode, "yes") == 0)) {
		log_set_quiet(true);
	}
}