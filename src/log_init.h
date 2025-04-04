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

#endif /* LOG_INIT_H */