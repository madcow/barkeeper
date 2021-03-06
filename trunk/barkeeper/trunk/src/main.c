#include "common.h"

#include <unistd.h>
#include <string.h>

static ht_tab_t table;

static void HandleRequest(req_t *req);

int main(int argc, char **argv)
{
	int port;
	arg_t arg;

	if (FS_Init() < 0)
		Error(E_FSINIT);

	port = DEFAULT_PORT;
	CMD_Parse(argc, argv);

	while (CMD_Next(&arg))
		switch(arg.type) { // Parse command line flags
		case T_ARG_PORT: port = arg.as.num;      break;
		case T_ARG_PATH: FS_AddPath(arg.as.str); break;
		default:         Error(E_ARGVAL);
		}

	// Create hashtable
	Hash_Init(&table);

	// Initialize threaded request handling
	if (NET_Init(port, HandleRequest) < 0)
		Error(E_NOSOCK " %d", port);

	for (;;) // Next client
		NET_Accept();

	NET_Shutdown();
	FS_Shutdown();
	MemCheck();

	return 0;
}

static void HandleRequest(req_t *req)
{
	int n = 0;
	int status, i;
	ht_ent_t *ent;

	if ((!req->privileged)
	&& ((req->type == T_REQ_INSERT)
	|| ((req->type == T_REQ_DELETE)))) {
		NET_Error(req, E_ACCESS);
		return;
	}

	if ((!req->params[0])
	&& ((req->type == T_REQ_QUERY)
	|| ((req->type == T_REQ_INSERT))
	|| ((req->type == T_REQ_DELETE))
	|| ((req->type == T_REQ_AUTH)))) {
		NET_Error(req, E_CMDARG);
		return;
	}

	switch(req->type) {
	case T_REQ_INVAL:
		NET_Error(req, E_REQVAL);
		break;

	case T_REQ_HELP:
		NET_Answer(req,
		"[1]  query     BARCODE    Check if BARCODE exists and mark as done.\n"\
		"[2]  insert    BARCODE    Add BARCODE to database and mark as todo.\n"\
		"[3]  delete    BARCODE    Remove BARCODE from database.            \n"\
		"[4]  auth      PASSWD     Request elevated privileges.             \n"\
		"[5]  list                 Alias for 'list --todo'.                 \n"\
		"[6]            --full     Print full barcode list. Alias for 'all'.\n"\
		"[7]            --done     Print list of already scanned barcodes.  \n"\
		"[8]            --todo     Print list of still missing barcodes.    \n"\
		"[9]  help                 Print list of all valid commands.        \n"\
		"[0]  quit                 Close connection. Alias for 'exit'.        ");

		break;

	case T_REQ_AUTH:
		if (strcmp(req->params, "123")) {
			NET_Error(req, E_NOCRED);
			break; // Plaintext!
		}

		req->privileged = true;
		NET_Answer(req, E_OK);
		break;

	case T_REQ_EXIT:
		close(req->handle);
		break;

	case T_REQ_QUERY:
		if (!(ent = Hash_Get(&table, req->params))) {
			NET_Error(req, E_NOKEY);
			break;
		}

		ent->status = T_ENT_DONE;
		NET_Answer(req, E_OK);
		break;

	case T_REQ_INSERT:
		status = Hash_Insert(&table, req->params);
		if (status == -1) { NET_Error(req, E_KEYLEN); break; }
		if (status == -2) { NET_Error(req, E_EXISTS); break; }
		NET_Answer(req, E_OK);
		break;

	case T_REQ_DELETE:
		if (Hash_Delete(&table, req->params) < 0) {
			NET_Error(req, E_NOKEY);
			break;
		}

		NET_Answer(req, E_OK);
		break;

	case T_REQ_LIST_FULL:
		// FIXME: Shamefully inefficient
		for (i = 0; i < MAX_HASH_SIZE; i++) {
			if (table.table[i] != NULL) {
				ent = table.table[i]; n++;
				NET_Answer(req, "%s", ent->key);
			}
		}

		if (n == 0)
			NET_Answer(req, E_NONE);
		break;

	case T_REQ_LIST_DONE:
		// FIXME: Shamefully inefficient
		for (i = 0; i < MAX_HASH_SIZE; i++) {
			if ((table.table[i] != NULL)
			&& ((table.table[i]->status == T_ENT_DONE))) {
				ent = table.table[i]; n++;
				NET_Answer(req, "%s", ent->key);
			}
		}

		if (n == 0)
			NET_Answer(req, E_NONE);
		break;

	case T_REQ_LIST_TODO:
		// FIXME: Shamefully inefficient
		for (i = 0; i < MAX_HASH_SIZE; i++) {
			if ((table.table[i] != NULL)
			&& ((table.table[i]->status == T_ENT_TODO))) {
				ent = table.table[i]; n++;
				NET_Answer(req, "%s", ent->key);
			}
		}

		if (n == 0)
			NET_Answer(req, E_NONE);
		break;

	case T_REQ_CLEAR:
	case T_REQ_EMPTY:
		break; // TODO
	}
}
