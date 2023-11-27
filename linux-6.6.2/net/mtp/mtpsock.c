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
 * homa_sock_init() - Constructor for homa_sock objects. This function
 * initializes only the parts of the socket that are owned by Homa.
 * @hsk:    Object to initialize.
 * @homa:   Homa implementation that will manage the socket.
 *
 * Return: always 0 (success).
 */
void homa_sock_init(struct homa_sock *hsk, struct homa *homa)
{
	struct homa_socktab *socktab = &homa->port_map;
	int i;

	spin_lock_bh(&socktab->write_lock);
	atomic_set(&hsk->protect_count, 0);
	spin_lock_init(&hsk->lock);
	hsk->last_locker = "none";
	atomic_set(&hsk->protect_count, 0);
	hsk->homa = homa;
	hsk->ip_header_length = (hsk->inet.sk.sk_family == AF_INET)
			? HOMA_IPV4_HEADER_LENGTH : HOMA_IPV6_HEADER_LENGTH;
	hsk->shutdown = false;
	while (1) {
		if (homa->next_client_port < HOMA_MIN_DEFAULT_PORT) {
			homa->next_client_port = HOMA_MIN_DEFAULT_PORT;
		}
		if (!homa_sock_find(socktab, homa->next_client_port)) {
			break;
		}
		homa->next_client_port++;
	}
	hsk->port = homa->next_client_port;
	hsk->inet.inet_num = hsk->port;
	hsk->inet.inet_sport = htons(hsk->port);
	homa->next_client_port++;
	hsk->socktab_links.sock = hsk;
	hlist_add_head_rcu(&hsk->socktab_links.hash_links,
			&socktab->buckets[homa_port_hash(hsk->port)]);
	INIT_LIST_HEAD(&hsk->active_rpcs);
	INIT_LIST_HEAD(&hsk->dead_rpcs);
	hsk->dead_skbs = 0;
	INIT_LIST_HEAD(&hsk->waiting_for_bufs);
	INIT_LIST_HEAD(&hsk->ready_requests);
	INIT_LIST_HEAD(&hsk->ready_responses);
	INIT_LIST_HEAD(&hsk->request_interests);
	INIT_LIST_HEAD(&hsk->response_interests);
	for (i = 0; i < HOMA_CLIENT_RPC_BUCKETS; i++) {
		struct homa_rpc_bucket *bucket = &hsk->client_rpc_buckets[i];
		spin_lock_init(&bucket->lock);
		INIT_HLIST_HEAD(&bucket->rpcs);
	}
	for (i = 0; i < HOMA_SERVER_RPC_BUCKETS; i++) {
		struct homa_rpc_bucket *bucket = &hsk->server_rpc_buckets[i];
		spin_lock_init(&bucket->lock);
		INIT_HLIST_HEAD(&bucket->rpcs);
	}
	memset(&hsk->buffer_pool, 0, sizeof(hsk->buffer_pool));
	spin_unlock_bh(&socktab->write_lock);
}
