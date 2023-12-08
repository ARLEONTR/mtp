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

/* This file hooks MTP into the rest of the Linux kernel.
 */

#include "mtpimpl.h"

#ifndef __UNIT_TEST__
MODULE_LICENSE("GPL");
#endif

#define DRV_VERSION "0.01"
#define DRV_RELDATE "Nov 25, 2023"

MODULE_AUTHOR("Ertan Onur");
MODULE_DESCRIPTION("MTP: Mesh Transport Protocol"
		   " v" DRV_VERSION " (" DRV_RELDATE ")");
MODULE_VERSION(DRV_VERSION);

/* Not yet sure what these variables are for */
long sysctl_MTP_mem[3] __read_mostly;
int sysctl_MTP_rmem_min __read_mostly;
int sysctl_MTP_wmem_min __read_mostly;
static bool exiting = false;
extern struct MTP *mtpinst;

/* This structure defines functions that handle various operations on
 * MTP sockets.
 */
const struct proto_ops MTP_proto_ops = {
	.family = PF_INET,
	.owner = THIS_MODULE,
	.release = inet_release,
	.bind = MTP_bind,
	.connect = inet_dgram_connect,
	.socketpair = sock_no_socketpair,
	.accept = sock_no_accept,
	.getname = inet_getname,
	.poll = MTP_poll,
	.ioctl = inet_ioctl,
	.listen = sock_no_listen,
	.shutdown = MTP_shutdown,
	.setsockopt = sock_common_setsockopt,
	.getsockopt = sock_common_getsockopt,
	.sendmsg = inet_sendmsg,
	.recvmsg = inet_recvmsg,
	.mmap = sock_no_mmap,
	.set_peek_off = sk_set_peek_off,
};

const struct proto_ops MTPv6_proto_ops = {
	.family = PF_INET6,
	.owner = THIS_MODULE,
	.release = inet6_release,
	.bind = MTP_bind,
	.connect = inet_dgram_connect,
	.socketpair = sock_no_socketpair,
	.accept = sock_no_accept,
	.getname = inet6_getname,
	.poll = MTP_poll,
	.ioctl = inet6_ioctl,
	.listen = sock_no_listen,
	.shutdown = MTP_shutdown,
	.setsockopt = sock_common_setsockopt,
	.getsockopt = sock_common_getsockopt,
	.sendmsg = inet_sendmsg,
	.recvmsg = inet_recvmsg,
	.mmap = sock_no_mmap,
	.set_peek_off = sk_set_peek_off,
};

/* This structure also defines functions that handle various operations
 * on MTP sockets. However, these functions are lower-level than those
 * in MTP_proto_ops: they are specific to the PF_INET or PF_INET6
 * protocol family, and in many cases they are invoked by functions in
 * MTP_proto_ops. Most of these functions have MTP-specific implementations.
 */
struct proto MTP_prot = {
	.name = "MTP",
	.owner = THIS_MODULE,
	.close = MTP_close,
	.connect = ip4_datagram_connect,
	.disconnect = MTP_disconnect,
	.ioctl = MTP_ioctl,
	.init = MTP_socket,
	.destroy = 0,
	.setsockopt = MTP_setsockopt,
	.getsockopt = MTP_getsockopt,
	.sendmsg = MTP_sendmsg,
	.recvmsg = MTP_recvmsg,
	.backlog_rcv = MTP_backlog_rcv,
	.release_cb = ip4_datagram_release_cb,
	.hash = MTP_hash,
	.unhash = MTP_unhash,
	.rehash = MTP_rehash,
	.get_port = MTP_get_port,
	.sysctl_mem = sysctl_MTP_mem,
	.sysctl_wmem = &sysctl_MTP_wmem_min,
	.sysctl_rmem = &sysctl_MTP_rmem_min,
	.obj_size = sizeof(struct MTP_sock),
	.diag_destroy = MTP_diag_destroy,
	.no_autobind = 1,
};

struct proto MTPv6_prot = {
	.name = "MTPv6",
	.owner = THIS_MODULE,
	.close = MTP_close,
	.connect = ip6_datagram_connect,
	.disconnect = MTP_disconnect,
	.ioctl = MTP_ioctl,
	.init = MTP_socket,
	.destroy = 0,
	.setsockopt = MTP_setsockopt,
	.getsockopt = MTP_getsockopt,
	.sendmsg = MTP_sendmsg,
	.recvmsg = MTP_recvmsg,
	.backlog_rcv = MTP_backlog_rcv,
	.release_cb = ip6_datagram_release_cb,
	.hash = MTP_hash,
	.unhash = MTP_unhash,
	.rehash = MTP_rehash,
	.get_port = MTP_get_port,
	.sysctl_mem = sysctl_MTP_mem,
	.sysctl_wmem = &sysctl_MTP_wmem_min,
	.sysctl_rmem = &sysctl_MTP_rmem_min,

	/* IPv6 data comes *after* MTP's data, and isn't included in
	 * struct MTP_sock.
	 */
	.obj_size = sizeof(struct MTP_sock) + sizeof(struct ipv6_pinfo),
	.diag_destroy = MTP_diag_destroy,
	.no_autobind = 1,
};

/* Top-level structure describing the MTP protocol. */
struct inet_protosw MTP_protosw = {
	.type = SOCK_DGRAM,
	.protocol = IPPROTO_MTP,
	.prot = &MTP_prot,
	.ops = &MTP_proto_ops,
	.flags = INET_PROTOSW_REUSE,
};

struct inet_protosw MTPv6_protosw = {
	.type = SOCK_DGRAM,
	.protocol = IPPROTO_MTP,
	.prot = &MTPv6_prot,
	.ops = &MTPv6_proto_ops,
	.flags = INET_PROTOSW_REUSE,
};

/* This structure is used by IP to deliver incoming MTP packets to us. */
static struct net_protocol MTP_protocol = {
	.handler = MTP_softirq,
	.err_handler = MTP_err_handler_v4,
	.no_policy = 1,
};

static struct inet6_protocol MTPv6_protocol = {
	.handler = MTP_softirq,
	.err_handler = MTP_err_handler_v6,
	.flags = INET6_PROTO_NOPOLICY | INET6_PROTO_FINAL,
};

/* Describes file operations implemented for /proc/net/MTP_metrics. */
static const struct proc_ops MTP_metrics_pops = {
	.proc_open = MTP_metrics_open,
	.proc_read = MTP_metrics_read,
	.proc_lseek = MTP_metrics_lseek,
	.proc_release = MTP_metrics_release,
};

/* Used to remove /proc/net/MTP_metrics when the module is unloaded. */
static struct proc_dir_entry *metrics_dir_entry = NULL;

static int action;

/* Used to configure sysctl access to MTP configuration parameters.*/
static struct ctl_table MTP_ctl_table[] = { { .procname = "action",
					      .data = &action,
					      .maxlen = sizeof(int),
					      .mode = 0644,
					      .proc_handler = MTP_dointvec },
					    {} };

/* Used to remove sysctl values when the module is unloaded. */
static struct ctl_table_header *MTP_ctl_header;

static DECLARE_COMPLETION(timer_thread_done);

/**
 * MTP_load() - invoked when this module is loaded into the Linux kernel
 * Return: 0 on success, otherwise a negative errno.
 */
static int __init MTP_load(void)
{
	int status;

#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_load\n");
#endif

	status = proto_register(&MTP_prot, 1);
	if (status != 0) {
		printk(KERN_ERR "proto_register failed for MTP_prot: %d\n",
		       status);
		goto out;
	}
	status = proto_register(&MTPv6_prot, 1);
	if (status != 0) {
		printk(KERN_ERR "proto_register failed for MTPv6_prot: %d\n",
		       status);
		goto out;
	}
	inet_register_protosw(&MTP_protosw);
	inet6_register_protosw(&MTPv6_protosw);
	status = inet_add_protocol(&MTP_protocol, IPPROTO_MTP);
	if (status != 0) {
		printk(KERN_ERR "inet_add_protocol failed in MTP_load: %d\n",
		       status);
		goto out_cleanup;
	}
	status = inet6_add_protocol(&MTPv6_protocol, IPPROTO_MTP);
	if (status != 0) {
		printk(KERN_ERR "inet6_add_protocol failed in MTP_load: %d\n",
		       status);
		goto out_cleanup;
	}

	// Initialize the MTP instance and relevant threads for handling packet i/o
	mtpinst = MTP_init();
	if (!mtpinst)
		goto out_cleanup;

	metrics_dir_entry = proc_create("MTP_metrics", S_IRUGO,
					init_net.proc_net, &MTP_metrics_pops);
	if (!metrics_dir_entry) {
		printk(KERN_ERR "couldn't create /proc/net/MTP_metrics\n");
		status = -ENOMEM;
		goto out_cleanup;
	}

	MTP_ctl_header =
		register_net_sysctl(&init_net, "net/MTP", MTP_ctl_table);
	if (!MTP_ctl_header) {
		printk(KERN_ERR "couldn't register MTP sysctl parameters\n");
		status = -ENOMEM;
		goto out_cleanup;
	}

	/*
		status = MTP_offload_init();
		if (status != 0) {
			printk(KERN_ERR "MTP couldn't init offloads\n");
			goto out_cleanup;
		}
	*/
	return 0;

out_cleanup:
	// MTP_offload_end();
	unregister_net_sysctl_table(MTP_ctl_header);
	proc_remove(metrics_dir_entry);
	// MTP_destroy(MTP);
	inet_del_protocol(&MTP_protocol, IPPROTO_MTP);
	inet_unregister_protosw(&MTP_protosw);
	inet6_del_protocol(&MTPv6_protocol, IPPROTO_MTP);
	inet6_unregister_protosw(&MTPv6_protosw);
	proto_unregister(&MTP_prot);
	proto_unregister(&MTPv6_prot);
out:
	return status;
}

/**
 * MTP_unload() - invoked when this module is unloaded from the Linux kernel.
 */
static void __exit MTP_unload(void)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_unload\n");
#endif

	printk(KERN_NOTICE "MTP module unloading\n");
	exiting = true;

	// if (MTP_offload_end() != 0)
	//	printk(KERN_ERR "MTP couldn't stop offloads\n");
	// wait_for_completion(&timer_thread_done);
	unregister_net_sysctl_table(MTP_ctl_header);
	proc_remove(metrics_dir_entry);
	// MTP_destroy(MTP);
	inet_del_protocol(&MTP_protocol, IPPROTO_MTP);
	inet_unregister_protosw(&MTP_protosw);
	inet6_del_protocol(&MTPv6_protocol, IPPROTO_MTP);
	inet6_unregister_protosw(&MTPv6_protosw);
	proto_unregister(&MTP_prot);
	proto_unregister(&MTPv6_prot);
}

module_init(MTP_load);
module_exit(MTP_unload);

/**
 * MTP_bind() - Implements the bind system call for MTP sockets: associates
 * a well-known service port with a socket. Unlike other AF_INET6 protocols,
 * there is no need to invoke this system call for sockets that are only
 * used as clients.
 * @sock:     Socket on which the system call was invoked.
 * @addr:    Contains the desired port number.
 * @addr_len: Number of bytes in uaddr.
 * Return:    0 on success, otherwise a negative errno.
 */
int MTP_bind(struct socket *sock, struct sockaddr *addr, int addr_len)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_bind\n");
#endif
	// struct MTP_sock *hsk = MTP_sk(sock->sk);
	sockaddr_in_union *addr_in = (sockaddr_in_union *)addr;
	int port;

	if (unlikely(addr->sa_family != sock->sk->sk_family)) {
		return -EAFNOSUPPORT;
	}
	if (addr_in->in6.sin6_family == AF_INET6) {
		if (addr_len < sizeof(struct sockaddr_in6)) {
			return -EINVAL;
		}
		port = ntohs(addr_in->in4.sin_port);
	} else if (addr_in->in4.sin_family == AF_INET) {
		if (addr_len < sizeof(struct sockaddr_in)) {
			return -EINVAL;
		}
		port = ntohs(addr_in->in6.sin6_port);
	}
	// return MTP_sock_bind(&MTP->port_map, hsk, port);
	return 0;
}

/**
 * MTP_close() - Invoked when close system call is invoked on a MTP socket.
 * @sk:      Socket being closed
 * @timeout: ??
 */
void MTP_close(struct sock *sk, long timeout)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_close\n");
#endif
	// struct MTP_sock *hsk = MTP_sk(sk);
	// MTP_sock_destroy(hsk);
	sk_common_release(sk);
	// tt_record1("closed socket, port %d\n", hsk->port);
	// if (hsk->MTP->freeze_type == SOCKET_CLOSE)
	//	tt_freeze();
}

/**
 * MTP_shutdown() - Implements the shutdown system call for MTP sockets.
 * @sock:      Socket to shut down.
 * @how:     Ignored: for other sockets, can independently shut down
 *           sending and receiving, but for MTP any shutdown will
 *           shut down everything.
 *
 * Return: 0 on success, otherwise a negative errno.
 */
int MTP_shutdown(struct socket *sock, int how)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_shutdown\n");
#endif
	// MTP_sock_shutdown(MTP_sk(sock->sk));
	return 0;
}

/**
 * MTP_disconnect() - Invoked when disconnect system call is invoked on a
 * MTP socket.
 * @sk:    Socket to disconnect
 * @flags: ??
 *
 * Return: 0 on success, otherwise a negative errno.
 */
int MTP_disconnect(struct sock *sk, int flags)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_disconnect\n");
#endif
	printk(KERN_WARNING "unimplemented disconnect invoked on MTP socket\n");
	return -ENOSYS;
}

/**
 * MTP_ioc_abort() - The top-level function for the ioctl that implements
 * the MTP_abort user-level API.
 * @sk:       Socket for this request.
 * @arg:      Used to pass information from user space.
 *
 * Return: 0 on success, otherwise a negative errno.
 */
int MTP_ioc_abort(struct sock *sk, unsigned long arg)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_ioc_abort\n");
#endif
	int ret = 0;
	return ret;
}

/**
 * MTP_ioctl() - Implements the ioctl system call for MTP sockets.
 * @sk:    Socket on which the system call was invoked.
 * @cmd:   Identifier for a particular ioctl operation.
 * @arg:   Operation-specific argument; typically the address of a block
 *         of data in user address space.
 *
 * Return: 0 on success, otherwise a negative errno.
 */
int MTP_ioctl(struct sock *sk, int cmd, int *arg)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_ioctl\n");
#endif
	int result = 0;
	return result;
}

/**
 * MTP_socket() - Implements the socket(2) system call for sockets.
 * @sk:    Socket on which the system call was invoked. The non-MTP
 *         parts have already been initialized.
 *
 * Return: always 0 (success).
 */
int MTP_socket(struct sock *sk)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_socket\n");
#endif
	struct MTP_sock *mtpsk = MTP_sk(sk);
	MTP_sock_init(mtpsk, mtpinst);
	return 0;
}

/**
 * MTP_setsockopt() - Implements the getsockopt system call for MTP sockets.
 * @sk:      Socket on which the system call was invoked.
 * @level:   Level at which the operation should be handled; will always
 *           be IPPROTO_MTP.
 * @optname: Identifies a particular setsockopt operation.
 * @optval:  Address in user space of information about the option.
 * @optlen:  Number of bytes of data at @optval.
 * Return:   0 on success, otherwise a negative errno.
 */
int MTP_setsockopt(struct sock *sk, int level, int optname, sockptr_t optval,
		   unsigned int optlen)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_setsockopt\n");
#endif
	return 0;
}

/**
 * MTP_getsockopt() - Implements the getsockopt system call for MTP sockets.
 * @sk:      Socket on which the system call was invoked.
 * @level:   ??
 * @optname: Identifies a particular setsockopt operation.
 * @optval:  Address in user space where the option's value should be stored.
 * @option:  ??.
 * Return:   0 on success, otherwise a negative errno.
 */
int MTP_getsockopt(struct sock *sk, int level, int optname, char __user *optval,
		   int __user *option)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_getsockopt\n");
#endif
	printk(KERN_WARNING "unimplemented getsockopt invoked on MTP socket:"
			    " level %d, optname %d\n",
	       level, optname);
	return -EINVAL;
}

/**
 * MTP_sendmsg() - Send a request or response message on a MTP socket.
 * @sk:    Socket on which the system call was invoked.
 * @msg:   Structure describing the message to send; the msg_control
 *         field points to additional information.
 * @length:   Number of bytes of the message.
 * Return: 0 on success, otherwise a negative errno.
 */
int MTP_sendmsg(struct sock *sk, struct msghdr *msg, size_t length)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_sendmsg\n");
#endif
	int result = 0;
	return result;
}

/**
 * MTP_recvmsg() - Receive a message from a MTP socket.
 * @sk:          Socket on which the system call was invoked.
 * @msg:         Controlling information for the receive.
 * @len:         Total bytes of space available in msg->msg_iov; not used.
 * @flags:       Flags from system call, not including MSG_DONTWAIT; ignored.
 * @addr_len:    Store the length of the sender address here
 * Return:       The length of the message on success, otherwise a negative
 *               errno.
 */
int MTP_recvmsg(struct sock *sk, struct msghdr *msg, size_t len, int flags,
		int *addr_len)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_recvmsg\n");
#endif
	int result = 0;
	return result;
}

/**
 * MTP_sendpage() - ??.
 * @sk:     Socket for the operation
 * @page:   ??
 * @offset: ??
 * @size:   ??
 * @flags:  ??
 * Return:  0 on success, otherwise a negative errno.
 */
int MTP_sendpage(struct sock *sk, struct page *page, int offset, size_t size,
		 int flags)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_sendpage\n");
#endif
	printk(KERN_WARNING "unimplemented sendpage invoked on MTP socket\n");
	return -ENOSYS;
}

/**
 * MTP_hash() - ??.
 * @sk:    Socket for the operation
 * Return: ??
 */
int MTP_hash(struct sock *sk)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_hash\n");
#endif
	printk(KERN_WARNING "unimplemented hash invoked on MTP socket\n");
	return 0;
}

/**
 * MTP_unhash() - ??.
 * @sk:    Socket for the operation
 */
void MTP_unhash(struct sock *sk)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_unhash\n");
#endif
	return;
	printk(KERN_WARNING "unimplemented unhash invoked on MTP socket\n");
}

/**
 * MTP_rehash() - ??.
 * @sk:    Socket for the operation
 */
void MTP_rehash(struct sock *sk)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_rehash\n");
#endif
	printk(KERN_WARNING "unimplemented rehash invoked on MTP socket\n");
}

/**
 * MTP_get_port() - It appears that this function is called to assign a
 * default port for a socket.
 * @sk:    Socket for the operation
 * @snum:  Unclear what this is.
 * Return: Zero for success, or a negative errno for an error.
 */
int MTP_get_port(struct sock *sk, unsigned short snum)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_get_port\n");
#endif
	/* MTP always assigns ports immediately when a socket is created,
	 * so there is nothing to do here.
	 */
	return 0;
}

/**
 * MTP_diag_destroy() - ??.
 * @sk:    Socket for the operation
 * @err:   ??
 * Return: ??
 */
int MTP_diag_destroy(struct sock *sk, int err)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_diag_destroy\n");
#endif
	printk(KERN_WARNING
	       "unimplemented diag_destroy invoked on MTP socket\n");
	return -ENOSYS;
}

/**
 * MTP_v4_early_demux() - Invoked by IP for ??.
 * @skb:    Socket buffer.
 * Return: Always 0?
 */
int MTP_v4_early_demux(struct sk_buff *skb)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_v4_early_demux\n");
#endif
	printk(KERN_WARNING
	       "unimplemented early_demux invoked on MTP socket\n");
	return 0;
}

/**
 * MTP_v4_early_demux_handler() - invoked by IP for ??.
 * @skb:    Socket buffer.
 * @return: Always 0?
 */
int MTP_v4_early_demux_handler(struct sk_buff *skb)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_v4_early_demux_handler\n");
#endif
	printk(KERN_WARNING
	       "unimplemented early_demux_handler invoked on MTP socket\n");
	return 0;
}

/**
 * MTP_softirq() - This function is invoked at SoftIRQ level to handle
 * incoming packets.
 * @skb:   The incoming packet.
 * Return: Always 0
 */
int MTP_softirq(struct sk_buff *skb)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_softirq\n");
#endif
	printk(KERN_WARNING "unimplemented MTP_softirq\n");
	return 0;
}

/**
 * MTP_backlog_rcv() - Invoked to handle packets saved on a socket's
 * backlog because it was locked when the packets first arrived.
 * @sk:     MTP socket that owns the packet's destination port.
 * @skb:    The incoming packet. This function takes ownership of the packet
 *          (we'll delete it).
 *
 * Return:  Always returns 0.
 */
int MTP_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_backlog_rcv\n");
#endif
	printk(KERN_WARNING
	       "unimplemented backlog_rcv invoked on MTP socket\n");
	kfree_skb(skb);
	return 0;
}

/**
 * MTP_err_handler_v4() - Invoked by IP to handle an incoming error
 * packet, such as ICMP UNREACHABLE.
 * @skb:   The incoming packet.
 * @info:  Information about the error that occurred?
 *
 * Return: zero, or a negative errno if the error couldn't be handled here.
 */
int MTP_err_handler_v4(struct sk_buff *skb, u32 info)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_err_handler_v4\n");
#endif
	return 0;
}

/**
 * MTP_err_handler_v6() - Invoked by IP to handle an incoming error
 * packet, such as ICMP UNREACHABLE.
 * @skb:   The incoming packet.
 * @info:  Information about the error that occurred?
 * @opt:   options
 * @type:  type
 * @code:  code
 * @offset: offset
 *
 * Return: zero, or a negative errno if the error couldn't be handled here.
 */
int MTP_err_handler_v6(struct sk_buff *skb, struct inet6_skb_parm *opt, u8 type,
		       u8 code, int offset, __be32 info)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_err_handler_v6\n");
#endif
	return 0;
}

/**
 * MTP_poll() - Invoked by Linux as part of implementing select, poll,
 * epoll, etc.
 * @file:  Open file that is participating in a poll, select, etc.
 * @sock:  A MTP socket, associated with @file.
 * @wait:  This table will be registered with the socket, so that it
 *         is notified when the socket's ready state changes.
 *
 * Return: A mask of bits such as EPOLLIN, which indicate the current
 *         state of the socket.
 */
__poll_t MTP_poll(struct file *file, struct socket *sock,
		  struct poll_table_struct *wait)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_poll\n");
#endif
	__poll_t mask;
	mask = POLLOUT | POLLWRNORM;
	return mask;
}

/**
 * MTP_metrics_open() - This function is invoked when /proc/net/MTP_metrics is
 * opened.
 * @inode:    The inode corresponding to the file.
 * @file:     Information about the open file.
 *
 * Return: always 0.
 */
int MTP_metrics_open(struct inode *inode, struct file *file)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_metrics_open\n");
#endif
	return 0;
}

/**
 * MTP_metrics_read() - This function is invoked to handle read kernel calls on
 * /proc/net/MTP_metrics.
 * @file:    Information about the file being read.
 * @buffer:  Address in user space of the buffer in which data from the file
 *           should be returned.
 * @length:  Number of bytes available at @buffer.
 * @offset:  Current read offset within the file.
 *
 * Return: the number of bytes returned at @buffer. 0 means the end of the
 * file was reached, and a negative number indicates an error (-errno).
 */
ssize_t MTP_metrics_read(struct file *file, char __user *buffer, size_t length,
			 loff_t *offset)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_metrics_read\n");
#endif
	size_t copied = 0;
	return copied;
}

/**
 * MTP_metrics_lseek() - This function is invoked to handle seeks on
 * /proc/net/MTP_metrics. Right now seeks are ignored: the file must be
 * read sequentially.
 * @file:    Information about the file being read.
 * @offset:  Distance to seek, in bytes
 * @whence:  Starting point from which to measure the distance to seek.
 */
loff_t MTP_metrics_lseek(struct file *file, loff_t offset, int whence)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_metrics_lseek\n");
#endif
	return 0;
}

/**
 * MTP_metrics_release() - This function is invoked when the last reference to
 * an open /proc/net/MTP_metrics is closed.  It performs cleanup.
 * @inode:    The inode corresponding to the file.
 * @file:     Information about the open file.
 *
 * Return: always 0.
 */
int MTP_metrics_release(struct inode *inode, struct file *file)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_metrics_release\n");
#endif
	return 0;
}

/**
 * MTP_dointvec() - This function is a wrapper around proc_dointvec. It is
 * invoked to read and write sysctl values and also update other values
 * that depend on the modified value.
 * @table:    sysctl table describing value to be read or written.
 * @write:    Nonzero means value is being written, 0 means read.
 * @buffer:   Address in user space of the input/output data.
 * @lenp:     Not exactly sure.
 * @ppos:     Not exactly sure.
 *
 * Return: 0 for success, nonzero for error.
 */
int MTP_dointvec(struct ctl_table *table, int write, void __user *buffer,
		 size_t *lenp, loff_t *ppos)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_dointvec\n");
#endif
	int result;
	result = proc_dointvec(table, write, buffer, lenp, ppos);
	return result;
}

/**
 * MTP_sysctl_softirq_cores() - This function is invoked to handle sysctl
 * requests for the "gen3_softirq_cores" target, which requires special
 * processing.
 * @table:    sysctl table describing value to be read or written.
 * @write:    Nonzero means value is being written, 0 means read.
 * @buffer:   Address in user space of the input/output data.
 * @lenp:     Not exactly sure.
 * @ppos:     Not exactly sure.
 *
 * Return: 0 for success, nonzero for error.
 */
int MTP_sysctl_softirq_cores(struct ctl_table *table, int write,
			     void __user *buffer, size_t *lenp, loff_t *ppos)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_sysctl_softirq_cores\n");
#endif
	int result = 0;
	return result;
}

/**
 * MTP_hrtimer() - This function is invoked by the hrtimer mechanism to
 * wake up the timer thread. Runs at IRQ level.
 * @timer:   The timer that triggered; not used.
 *
 * Return:   Always HRTIMER_RESTART.
 */
enum hrtimer_restart MTP_hrtimer(struct hrtimer *timer)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: hrtimer_restart\n");
#endif
	return HRTIMER_NORESTART;
}

/**
 * MTP_timer_main() - Top-level function for the timer thread.
 * @transportInfo:  Pointer to struct MTP.
 *
 * Return:         Always 0.
 */
int MTP_timer_main(void *transportInfo)
{
#ifdef MTP_DEBUG
	printk(KERN_NOTICE "MTP module: MTP_timer_main\n");
#endif
	return 0;
}
