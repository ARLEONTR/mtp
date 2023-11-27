/* Copyright (c) 2023-Infinity ARLEON TECHNOLOGIES
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

/* This file handles MTP soc.
 */

#include "mtpimpl.h"

/**
 * MTP_sock_init() - Constructor for MTP_sock objects. This function
 * initializes only the parts of the socket that are owned by MTP.
 * @mtpsk:    Object to initialize.
 * @MTP:   MTP implementation that will manage the socket.
 *
 * Return: always 0 (success).
 */
void MTP_sock_init(struct MTP_sock *mtpsk, struct MTP *mtp)
{
//	struct MTP_socktab *socktab = &mtp->port_map;
	//int i;
/*
	//spin_lock_bh(&socktab->write_lock);
	//atomic_set(&mtpsk->protect_count, 0);
	//spin_lock_init(&mtpsk->lock);
	mtpsk->last_locker = "none";
	atomic_set(&mtpsk->protect_count, 0);
	mtpsk->mtp = mtp;
	mtpsk->ip_header_length = (mtpsk->inet.sk.sk_family == AF_INET)
			? MTP_IPV4_HEADER_LENGTH : MTP_IPV6_HEADER_LENGTH;
	mtpsk->shutdown = false;
	while (1) {
		if (mtp->next_client_port < MTP_MIN_DEFAULT_PORT) {
			mtp->next_client_port = MTP_MIN_DEFAULT_PORT;
		}
		if (!mtp(socktab, mtp->next_client_port)) {
			break;
		}
		mtp->next_client_port++;
	}
	mtpsk->port = mtp->next_client_port;
	mtpsk->inet.inet_num = mtpsk->port;
	mtpsk->inet.inet_sport = htons(mtpsk->port);
	
	mtp->next_client_port++;
	mtpsk->socktab_links.sock = mtpsk;
	
	hlist_add_head_rcu(&mtpsk->socktab_links.hash_links,
			&socktab->buckets[mtp_port_hash(mtpsk->port)]);
	INIT_LIST_HEAD(&mtpsk->active_rpcs);
	INIT_LIST_HEAD(&mtpsk->dead_rpcs);
	mtpsk->dead_skbs = 0;
	INIT_LIST_HEAD(&mtpsk->waiting_for_bufs);
	INIT_LIST_HEAD(&mtpsk->ready_requests);
	INIT_LIST_HEAD(&mtpsk->ready_responses);
	INIT_LIST_HEAD(&mtpsk->request_interests);
	INIT_LIST_HEAD(&mtpsk->response_interests);
	for (i = 0; i < MTP_CLIENT_RPC_BUCKETS; i++) {
		struct mtp_rpc_bucket *bucket = &mtpsk->client_rpc_buckets[i];
		spin_lock_init(&bucket->lock);
		INIT_HLIST_HEAD(&bucket->rpcs);
	}
	for (i = 0; i < MTP_SERVER_RPC_BUCKETS; i++) {
		struct mtp_rpc_bucket *bucket = &mtpsk->server_rpc_buckets[i];
		spin_lock_init(&bucket->lock);
		INIT_HLIST_HEAD(&bucket->rpcs);
	}
	memset(&mtpsk->buffer_pool, 0, sizeof(mtpsk->buffer_pool));
	
	spin_unlock_bh(&socktab->write_lock);
	*/
}
