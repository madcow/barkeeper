#ifndef ERRORS_H
#define ERRORS_H

#define E_ASSERT  "Assertion failure"
#define E_ENOMEM  "Memory allocation failure"
#define E_DBINIT  "Cannot initialize database"
#define E_NOROOT  "Root privileges required"
#define E_SIGNAL  "Cannot register signal handlers"
#define E_FDOPEN  "Cannot get file handle"
#define E_EFSIZE  "File size is too large"
#define E_SETPID  "Unable to access pid lock"
#define E_LOCKED  "Pid file already locked"
#define E_UNLOCK  "Unable to unlock pid file"
#define E_ACTIVE  "Daemon is already running"
#define E_NOFORK  "Unable to fork process"
#define E_EXPECT  "Expected character"
#define E_NOREAD  "Unable to read file"
#define E_NOTNUM  "Invalid numeric value"
#define E_NOTSTR  "Invalid string value"
#define E_NOTBOL  "Invalid boolean value"
#define E_SPACES  "Invalid whitespace"
#define E_GETKEY  "Invalid config key"
#define E_GETVAL  "Invalid config value"
#define E_CFGNUM  "Too many config values"
#define E_CFGLEN  "Config value too long"
#define E_OPTYPE  "Unknown command-line option"
#define E_OPTVAL  "Required argument value missing"
#define E_ENOVAL  "Required config value missing"
#define E_GETCFG  "Check the config file for errors"
#define E_GETARG  "See --help for more information"
#define E_NOSOCK  "Unable to bind to socket"

#endif // ERRORS_H
