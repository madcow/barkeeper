#include "common.h"

static void    Usage(void);
static void    Configure(int argc, char **argv);
static void    Import(const char *filename);
static int     Run(void);
static void    Shutdown(void);

static bool    do_fork;  // Daemon mode
static bool    do_kill;  // Shutdown active
static int     telport;  // External telnet port
static int     webport;  // External http port
static char *  file;     // Data file path

static void Usage(void)
{
	Print( // Output usage information for command-line arguments
	"Usage: barkeeper [ -d | -k | -v | -h ] [ -f filename ] [ -p port ] \n"
	"See 'man barkeeper' for more details about command-line options    \n"
	"                                                                   \n"
	"  -d, --daemon   run process in background                         \n"
	"  -k, --kill     stop active background process                    \n"
	"  -r, --restart  restart active daemon, same as --kill --daemon    \n"
	"  -f, --file     import csv data from file, can be done while the  \n"
	"                 daemon process is running in the background       \n"
	"  -p, --port     set external telnet port                          \n"
	"  -w, --http     set external http port                            \n"
	"  -v, --verbose  enable verbose logging                            \n"
	"  -h, --help     print this help text                              ");
}

static void Configure(int argc, char **argv)
{
	cvar_t cvar;

	telport  = BASE_TEL_PORT;
	webport  = BASE_WEB_PORT;
	file     = BASE_FILE;

	if (CFG_ParseFile(CONFPATH) < 0)
		Error(E_GETCFG);

	if (CFG_ParseArgs(argc, argv) < 0)
		Error(E_GETARG);

	while (CFG_Next(&cvar)) {
		switch (cvar.id) {

		// -v, --verbose
		// verbose = <cbool>
		case T_CFG_VERBOSE:
			verbose = CBOOL(cvar);
			break;

		// -d, --daemon
		// daemon = <cbool>
		case T_CFG_DAEMON:
			do_fork = CBOOL(cvar);
			break;

		// -k, --kill
		// kill = <cbool>
		case T_CFG_KILL:
			do_kill = CBOOL(cvar);
			break;

		// -p, --port
		// port = <cnum>
		case T_CFG_PORT:
			telport = CNUM(cvar);
			break;

		// -w, --http
		// http = <cnum>
		case T_CFG_HTTP:
			webport = CNUM(cvar);
			break;

		// -f, --file
		// file = <cstr>
		case T_CFG_FILE:
			file = CSTR(cvar);
			break;

		// -r, --restart
		// restart = <cbool>
		case T_CFG_RESTART:
			if (CBOOL(cvar) == true) {
				do_kill = true;
				do_fork = true;
			}
			break;

		// -h, --help
		// help = <cbool>
		case T_CFG_HELP:
			Usage();
			exit(0);
		}
	}
}

static int Run(void)
{
	Info("Running in %s mode...", (do_fork)
	     ? "daemon" : "interactive");

	if (file != NULL)
		Import(file);

	if (NET_Init(telport, webport) < 0)
		Error(E_NOSOCK);

	return 0;
}

static void Import(const char *filename)
{
	Info("Importing '%s'...", filename);
}

static void Shutdown(void)
{
	Info("Shutting down...");
}

int main(int argc, char **argv)
{
	Configure(argc, argv);
	Verbose("Setting log level to verbose...");

	if (do_kill) {
		Shutdown();
		if (!do_fork)
			return 0;
	}

	return Run();
}
