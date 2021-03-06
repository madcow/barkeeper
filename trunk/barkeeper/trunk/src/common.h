#ifndef  COMMON_H
#define  COMMON_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

//       =========
//       CONSTANTS
//       =========

#define  DEBUG                     // Enable debug mode
#define  DEFAULT_PORT       23     // Default server socket port
#define  MAX_PATH           4096   // Cannot rely on PATH_MAX from limits.h
#define  MAX_ARG_LEN        1024   // Command line argument length limit
#define  MAX_MSG_LEN        2048   // Log message length limit
#define  MAX_REQ_LEN        2048   // Client request length limit
#define  MAX_HASH_SIZE      4096   // Number of possible hashes
#define  MAX_KEY_LEN        64     // Hashtable key limit

//       ========
//       TYPEDEFS
//       ========

typedef  uint32_t           u32;   // Guaranteed 32 bit data type
typedef  uint16_t           u16;   // Guaranteed 16 bit data type
typedef  unsigned char      byte;  // uint8_t could be a non-character
                                   //   type and therefore break aliasing
//       ======
//       ERRORS
//       ======

#define  E_NOIMPL           "Not implemented."
#define  E_ASSERT           "Assertion failure."
#define  E_NOMEM            "Memory allocation failure."
#define  E_NOSOCK           "Could not bind to network port."
#define  E_FSINIT           "Could not initialize filesystem."
#define  E_ARGVAL           "Unknown command line argument."
#define  E_THREAD           "Could not create request thread."
#define  E_ACCEPT           "Could not accept client request."
#define  E_RECEIV           "Could not receive client request."
#define  E_ANSWER           "Could not answer client request."
#define  E_REQVAL           "Command not found. Try 'help'."
#define  E_ACCESS           "Authentication required."
#define  E_NOCRED           "Invalid login credentials."
#define  E_CMDARG           "Missing command argument."
#define  E_KEYLEN           "Barcode exceeds length limit."
#define  E_NOKEY            "Barcode does not exist."
#define  E_EXISTS           "Barcode already exists."
#define  E_NONE             "NONE"
#define  E_OK               "OK"

//       =======
//       UTILITY
//       =======

#define  BIT(n)             (1UL << (n))      // Bitmask for nth bit
#define  UNUSED(sym)        ((void)(sym))     // Suppress -Wunused warnings
#define  Assert(exp)        UNUSED(0)         // DEBUG: Runtime assertion
#define  AS_(exp)           Assert(exp)       // Assertion shorthands:
#define  AS_NEQ_NULL(p)     AS_NEQ_(p, NULL)  // - Valid pointer
#define  AS_GEQ_ZERO(n)     AS_GEQ_(n, 0)     // - Non-negative value
#define  AS_GTH_ZERO(n)     AS_GTH_(n, 0)     // - Value greater zero
#define  AS_NEQ_(l, r)      AS_((l) != (r))   // - Inequality
#define  AS_GEQ_(l, r)      AS_((l) >= (r))   // - Greater/equal
#define  AS_GTH_(l, r)      AS_((l) >  (r))   // - Greater

//       ======
//       MEMORY
//       ======

#define  Allocate(n)        _Allocate(n)      // Replacement for malloc()
#define  Free(ptr)          _Free(ptr)        // Replacement for free()
#define  MemCheck()         UNUSED(0)         // DEBUG: Leak report

//       =======
//       LOGGING
//       =======

void     Info(const char    * fmt, ...);      // Info message
void     Warning(const char * fmt, ...);      // Warning message
void     Error(const char   * fmt, ...);      // Non-recoverable error
#define  Verbose(...)       UNUSED(0)         // DEBUG: Verbose message

//       ==========
//       DEBUG MODE
//       ==========

#ifdef   DEBUG              // DEBUG MODE has multiple effects:
#undef   Allocate           // - Tracks all memory allocations
#undef   Verbose            // - Outputs verbose log messages
#undef   Assert             // - Enables runtime assertions
#undef   MemCheck           // - Prints detailed leak reports
#undef   Free

#define  Verbose(...)       _Verbose(__VA_ARGS__)
#define  Assert(exp)        _Assert((exp), #exp, __FILE__, __LINE__)
#define  Allocate(n)        _AllocateDebug((n),  __FILE__, __LINE__)
#define  Free(ptr)          _FreeDebug(ptr)
#define  MemCheck()         _MemCheck()
#endif

//       ==========
//       NETWORKING
//       ==========

typedef  struct req_s req_t;
typedef  void (*req_fun_t)(req_t *req);

int      NET_Init(int port, req_fun_t func);
void     NET_Accept(void);
void     NET_Answer(req_t *req, const char *fmt, ...);
void     NET_Error(req_t *req, const char *fmt, ...);
void     NET_Shutdown(void);

struct   req_s {
         int    type;
         int    handle;
	 char * params;
         char   data[MAX_REQ_LEN];
	 bool   privileged;
};

enum     req_type {
         T_REQ_INVAL      =  -1,
	 T_REQ_EMPTY      =   0,
         T_REQ_QUERY      = 0x1,
         T_REQ_INSERT     = 0x2,
         T_REQ_DELETE     = 0x3,
         T_REQ_LIST_FULL  = 0x4,
         T_REQ_LIST_DONE  = 0x5,
         T_REQ_LIST_TODO  = 0x6,
	 T_REQ_AUTH       = 0x7,
	 T_REQ_HELP       = 0x8,
         T_REQ_CLEAR      = 0x9,
	 T_REQ_EXIT       = 0xA
};

//       ==========
//       FILESYSTEM
//       ==========

int      FS_Init(void);
void     FS_AddPath(const char *path);
int      FS_Open(const char *path);
int      FS_Read(int fd, void *dest, int n);
int      FS_Write(int fd, const void *src, int n);
void     FS_Close(int handle);
void     FS_Shutdown(void);

//       =========
//       HASHTABLE
//       =========

typedef  u32 (*ht_fun_t)(const char *);
typedef  struct ht_tab_s ht_tab_t;
typedef  struct ht_ent_s ht_ent_t;

int      Hash_Init(ht_tab_t *tab);
void     Hash_Free(ht_tab_t *tab);
int      Hash_Insert(ht_tab_t *tab, const char *key);
ht_ent_t *Hash_Get(ht_tab_t *tab, const char *key);
int      Hash_Delete(ht_tab_t *tab, const char *key);

struct   ht_tab_s {
         ht_ent_t  ** table;
	 ht_ent_t   * active;
         ht_fun_t     func;
         int          size;
};

struct   ht_ent_s {
         ht_ent_t   * next;
         char         key[MAX_KEY_LEN];
	 int          status;
};

enum     ht_ent_status {
         T_ENT_TODO,
	 T_ENT_DONE
};

//       ===========
//       COMMANDLINE
//       ===========

typedef  struct arg_s arg_t;

void     CMD_Parse(int argc, char **argv);
int      CMD_Next(arg_t *arg);

struct   arg_s {
         int type;
         union {
         	long num;
         	char str[MAX_ARG_LEN];
         } as;
};

enum     arg_type_e {
         T_ARG_PORT,
         T_ARG_PATH
};

//       ========
//       INTERNAL
//       ========

// Internal helper functions - do not use these directly.
// Use the macros without prefixed underscore instead.

void *   _Allocate(int size);
void *   _AllocateDebug(int size, const char *file, int line);
void     _Assert(bool exp, const char *text, const char *file, int line);
void     _Message(const char *pre, const char *fmt, va_list arg);
void     _Verbose(const char *fmt, ...);
void     _Free(void *ptr);
void     _FreeDebug(void *ptr);
void     _MemCheck(void);

#endif // COMMON_H
