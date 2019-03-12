#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <limits.h>
#include "calc_message.h"


int main (int argc, char* argv[])
{
	int     rcvid;         // indicates who we should reply to
	int     chid;          // the channel ID

	client_send_t msg;
	server_response_t smsg;

	// create a channel
	chid = ChannelCreate (0);
	if (chid == -1)
	{
		perror("failed to create the channel.");
		exit (EXIT_FAILURE);
	}

	printf("Server PID : %d\n", getpid());
	// this is typical of a server:  it runs forever
	while (1) {
		// get the message, and print it
		rcvid = MsgReceive (chid, &msg, sizeof (msg),
				NULL);
		smsg.error = SRVR_OK;

		if(msg.operator == '+')
			if ((msg.right_hand > 0 &&  msg.left_hand > INT_MAX - msg.right_hand) ||
			    (msg.right_hand < 0 &&  msg.left_hand < INT_MIN - msg.right_hand))
			{
				printf("here");
				smsg.error = SRVR_OVERFLOW;
			}
			else
			{
				smsg.answer = msg.left_hand + msg.right_hand;

			}

		else if(msg.operator == '-')
			smsg.answer = (double) msg.left_hand - (double) msg.right_hand;
		else if(msg.operator == 'x')
			smsg.answer = (double) msg.left_hand * (double) msg.right_hand;
		else if(msg.operator == '/' )
			if(msg.right_hand != 0)
				if(msg.left_hand != 0)
					smsg.answer = (double) msg.left_hand / (double) msg.right_hand;
				else
					smsg.error = SRVR_UNDEFINED;
			else
				smsg.error = SRVR_UNDEFINED;
		else
			smsg.error = SRVR_INVALID_OPERATOR;

		if(smsg.error == SRVR_UNDEFINED)
			strcpy(smsg.errorMsg, "undefined\n");

		else if(smsg.error == SRVR_INVALID_OPERATOR)
			strcpy(smsg.errorMsg, "The server could not calculate because of invalid operator\n");

		else if(smsg.error == SRVR_OVERFLOW)
			strcpy(smsg.errorMsg, "The server could not calculate due to overflow. Please try again\n");

		printf("Server PID : %d\n", getpid());

		// now, prepare the reply.  We reuse "message"
		MsgReply (rcvid, EOK, &smsg, sizeof (smsg));
	}

	// destroy the channel when done
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
