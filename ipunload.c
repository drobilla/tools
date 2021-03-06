#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <jack/jack.h>
#include <jack/intclient.h>

int
main (int argc, char *argv[])
{
	char *my_name;
	char *client_name;
	jack_client_t *client;
	jack_status_t status;
	jack_intclient_t intclient;

	/* validate args */
	if ((argc < 2) || (argc > 3)) {
		fprintf (stderr, "usage: %s client-name [ server-name ]]\n",
			 argv[0]);
		return 1;
	}

	/* use `basename $0` for my own client name */
	my_name = strrchr(argv[0], '/');
	if (my_name == 0) {
		my_name = argv[0];
	} else {
		my_name++;
	}

	/* first, become a JACK client */
	if (argc > 2) {
		client = jack_client_open (my_name,
					   (JackServerName|JackNoStartServer),
					   &status, argv[2]);
	} else {
		client = jack_client_open (my_name, JackNoStartServer, &status);
	}

	if (client == NULL) {
		if (status & JackServerFailed) {
			fprintf (stderr, "JACK server not running.\n");
		} else {
			fprintf (stderr, "JACK open failed, "
				 "status = 0x%2.0x\n", status);
		}
		exit (1);
	}

	/* then, get the internal client handle */
	client_name = argv[1];

	if (jack_internal_client_handle (client, client_name, &status, &intclient) != 0) {
                if (status & JackFailure) {
                        fprintf (stderr, "client %s not found.\n", client_name);
                }
		exit (2);
	}

	/* now, unload the internal client */
	status = jack_internal_client_unload (client, intclient);
	if (status & JackFailure) {
                if (status & JackInvalidOption) {
                        fprintf (stderr, "I'm sorry Dave, I can't do that\n");
                } else if (status & JackNoSuchClient) {
			fprintf (stderr, "client %s is gone.\n",
				 client_name);
		} else {
			fprintf (stderr, "could not unload %s, "
				 "returns 0x%2.0x\n", client_name, status);
		}
		exit (3);
	} else {
		fprintf (stdout, "%s unloaded.\n", client_name);
	}

	return 0;
}
	
		
