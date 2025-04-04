#ifndef LOG_INIT_H
#define LOG_INIT_H

// Initialize logging with default settings (INFO level to stderr)
void log_init_default(void);

// Initialize logging for production environment
// Logs WARNING and above to stderr, INFO and above to file
void log_init_production(void);

// Initialize logging for development with verbose output
// Logs DEBUG and above to both stderr and file
void log_init_development(void);

// Initialize logging for testing environment
// Completely disables all logs
void log_init_testing(void);

// Initialize logging based on environment variables:
// HYPERKV_LOG_LEVEL: Can be TRACE, DEBUG, INFO, WARN, ERROR, FATAL, DEV, PROD, or TEST
// HYPERKV_LOG_QUIET: Set to 1, true, TRUE, or yes to suppress stdout logging
void log_init_from_env(void);

#endif /* LOG_INIT_H */