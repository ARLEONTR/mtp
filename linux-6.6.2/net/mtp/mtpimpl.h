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

/* This file contains definitions that are shared across the files
 * that implement MTP for Linux.
 */

#ifndef _MTP_IMPL_H
#define _MTP_IMPL_H
#include <linux/bug.h>
#include <linux/audit.h>
#include <linux/icmp.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/proc_fs.h>
#include <linux/sched/signal.h>
#include <linux/skbuff.h>
#include <linux/version.h>
#include <linux/socket.h>
#include <net/icmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/inet_common.h>
#include <net/gro.h>

#define MTP_DEBUG

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 16, 0)
typedef unsigned int __poll_t;
#endif

struct MTP {
	int i;
};


/**
 * struct MTP_sock - Information about an open socket.
 */
struct MTP_sock {
	union {
		struct sock sock;
		struct inet_sock inet;
	};
	struct spinlock lock;
	char *last_locker;
	bool shutdown;
	__u16 port;
	int ip_header_length;
};

/**
 * Holds either an IPv4 or IPv6 address (smaller and easier to use than
 * sockaddr_storage).
 */
typedef union sockaddr_in_union {
	struct sockaddr sa;
	struct sockaddr_in in4;
	struct sockaddr_in6 in6;
} sockaddr_in_union;

static inline struct MTP_sock *MTP_sk(const struct sock *sk)
{
	return (struct MTP_sock *)sk;
}

extern int MTP_bind(struct socket *sk, struct sockaddr *addr, int addr_len);
extern void MTP_close(struct sock *sock, long timeout);
extern int MTP_disconnect(struct sock *sk, int flags);
extern int MTP_dointvec(struct ctl_table *table, int write, void __user *buffer,
			size_t *lenp, loff_t *ppos);
extern int MTP_err_handler_v4(struct sk_buff *skb, u32 info);
extern int MTP_err_handler_v6(struct sk_buff *skb, struct inet6_skb_parm *, u8,
			      u8, int, __be32);
extern int MTP_get_port(struct sock *sk, unsigned short snum);
extern int MTP_getsockopt(struct sock *sk, int level, int optname,
			  char __user *optval, int __user *option);
extern int MTP_ioctl(struct sock *sk, int cmd, int *arg);
extern int MTP_sendmsg(struct sock *sk, struct msghdr *msg, size_t len);
extern int MTP_setsockopt(struct sock *sk, int level, int optname,
			  sockptr_t __user optval, unsigned int optlen);
extern int MTP_shutdown(struct socket *sock, int how);
extern int MTP_socket(struct sock *sk);
extern int MTP_softirq(struct sk_buff *skb);
extern int MTP_hash(struct sock *sk);
extern void MTP_unhash(struct sock *sk);
extern void MTP_rehash(struct sock *sk);
extern __poll_t MTP_poll(struct file *file, struct socket *sock,
			 struct poll_table_struct *wait);
extern int MTP_recvmsg(struct sock *sk, struct msghdr *msg, size_t len,
		       int flags, int *addr_len);
extern int MTP_backlog_rcv(struct sock *sk, struct sk_buff *skb);
extern int MTP_diag_destroy(struct sock *sk, int err);

extern loff_t MTP_metrics_lseek(struct file *file, loff_t offset, int whence);
extern int MTP_metrics_open(struct inode *inode, struct file *file);
extern ssize_t MTP_metrics_read(struct file *file, char __user *buffer,
				size_t length, loff_t *offset);
extern int MTP_metrics_release(struct inode *inode, struct file *file);
extern void MTP_sock_init(struct MTP_sock *hsk, struct MTP *mtp);

#endif /* _MTP_IMPL_H */
