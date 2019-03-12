#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>
#include "../../Lab4_server/src/calc_message.h"

int main (int argc, char* argv[])
{
	client_send_t msg;
	server_response_t smsg;
	int  coid;

	if(argc != 5 ){
		printf ("There are not enough arguments");
		exit(EXIT_FAILURE);
	}

	pid_t serverpid = atoi(argv[1]);
	msg.left_hand = atoi(argv[2]);
	msg.operator = argv[3][0];
	msg.right_hand = atoi(argv[4]);

	// establish a connection
	coid = ConnectAttach (ND_LOCAL_NODE, serverpid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		fprintf (stderr, "Couldn't ConnectAttach\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}

	// send the message
	if (MsgSend (coid, &msg, sizeof(msg) + 1, &smsg, sizeof (smsg)) == -1) {
		fprintf (stderr, "Error during MsgSend\n");
		perror (NULL);
		exit (EXIT_FAILURE);
	}

	if(smsg.error == SRVR_OK)
		printf ("The server has calculated the result of %d %c %d as %.2f \n",msg.left_hand, msg.operator, msg.right_hand, smsg.answer);
	else if(smsg.error == SRVR_UNDEFINED)
			printf ("The server has calculated the result of %d %c %d as %s\n",msg.left_hand, msg.operator, msg.right_hand, smsg.errorMsg);

	else
		printf (smsg.errorMsg);

	//Disconnect from the channel
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
