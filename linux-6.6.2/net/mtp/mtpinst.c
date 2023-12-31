/** Copyright (c) 2023-Infinity ARLEON TECHNOLOGIES
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* This file hooks MTP into the rest of the Linux kernel.
 */

#include "mtpimpl.h"

struct MTP *mtpsess = NULL;

static void mtp_socket_handler(struct MTP *mtpsess)
{
	unsigned char buf[MTP_BUFFER_SIZE];

	for (;;) {
		memset(&buf, 0, MTP_BUFFER_SIZE);

		// Receive from socket and process packet

		if (signal_pending(current))
			break;
	}
}

/**
 * MTP_sock_init() - Constructor for MTP_sock objects. This function
 * initializes only the parts of the socket that are owned by MTP.
 * @mtpsk:    Object to initialize.
 * @mtpsess:   MTP implementation that will manage the socket.
 *
 * Return: always 0 (success).
 */
void MTP_sock_init(struct MTP_sock *mtpsk, struct MTP *mtpsess)
{
	int err;
	/* create a socket */
	if (((err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP,
				&mtpsk->sock)) < 0)) {
		printk(KERN_INFO MODULE_NAME
		       ": Could not create a datagram socket, error = %d\n",
		       -ENXIO);
		goto out;
	}
/*
close_and_out:
	sock_release(mtpsess->sock);
	kthread->sock = NULL;
*/
out:
	mtpsess->thread = NULL;
	mtpsess->running = 0;
}

struct MTP_session *MTP_session_init(void){

	mtpsess = kmalloc(sizeof(struct MTP_session), GFP_KERNEL);
	memset(mtpsess, 0, sizeof(struct MTP_session));

	mtpsess->thread =
		kthread_run((void *)mtp_socket_handler, NULL, MODULE_NAME);
	if (IS_ERR(mtpsess->thread)) {
		printk(KERN_INFO MODULE_NAME
		       ": unable to start kernel thread\n");
		kfree(mtpsess);
		mtpsess = NULL;
	}
	return mtpsess;
}

/**
 * MTP_init() - Constructor for MTP object. This function
 * initializes MTP data structure.
 *
 * Return: pointer to the MTP object.
 */
struct MTP *MTP_init(void)
{

	mtp = kmalloc(sizeof(struct MTP), GFP_KERNEL);
	memset(mtp, 0, sizeof(struct MTP));

	INIT_LIST_HEAD(mtp->mtp_sessions);
	
	
}
