/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the protocol dispatcher.
 *
 * Version:	@(#)protocol.h	1.0.2	05/07/93
 *
 * Author:	Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 *	Changes:
 *		Alan Cox	:	Added a name field and a frag handler
 *					field for later.
 *		Alan Cox	:	Cleaned up, and sorted types.
 *		Pedro Roque	:	inet6 protocols
 */
 
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <linux/in6.h>
#if IS_ENABLED(CONFIG_IPV6)
#include <linux/ipv6.h>
#endif

#define MAX_INET_PROTOS	256		


struct net_protocol {
	int			(*handler)(struct sk_buff *skb);
	void			(*err_handler)(struct sk_buff *skb, u32 info);
	int			(*gso_send_check)(struct sk_buff *skb);
	struct sk_buff	       *(*gso_segment)(struct sk_buff *skb,
					       netdev_features_t features);
	struct sk_buff	      **(*gro_receive)(struct sk_buff **head,
					       struct sk_buff *skb);
	int			(*gro_complete)(struct sk_buff *skb);
	unsigned int		no_policy:1,
				netns_ok:1;
};

#if IS_ENABLED(CONFIG_IPV6)
struct inet6_protocol {
	int	(*handler)(struct sk_buff *skb);

	void	(*err_handler)(struct sk_buff *skb,
			       struct inet6_skb_parm *opt,
			       u8 type, u8 code, int offset,
			       __be32 info);

	int	(*gso_send_check)(struct sk_buff *skb);
	struct sk_buff *(*gso_segment)(struct sk_buff *skb,
				       netdev_features_t features);
	struct sk_buff **(*gro_receive)(struct sk_buff **head,
					struct sk_buff *skb);
	int	(*gro_complete)(struct sk_buff *skb);

	unsigned int	flags;	
};

#define INET6_PROTO_NOPOLICY	0x1
#define INET6_PROTO_FINAL	0x2
#define INET6_PROTO_GSO_EXTHDR	0x4
#endif

struct inet_protosw {
	struct list_head list;

        
	unsigned short	 type;	   
	unsigned short	 protocol; 

	struct proto	 *prot;
	const struct proto_ops *ops;
  
	char             no_check;   
	unsigned char	 flags;      
};
#define INET_PROTOSW_REUSE 0x01	     
#define INET_PROTOSW_PERMANENT 0x02  
#define INET_PROTOSW_ICSK      0x04  

extern const struct net_protocol __rcu *inet_protos[MAX_INET_PROTOS];

#if IS_ENABLED(CONFIG_IPV6)
extern const struct inet6_protocol __rcu *inet6_protos[MAX_INET_PROTOS];
#endif

extern int	inet_add_protocol(const struct net_protocol *prot, unsigned char num);
extern int	inet_del_protocol(const struct net_protocol *prot, unsigned char num);
extern void	inet_register_protosw(struct inet_protosw *p);
extern void	inet_unregister_protosw(struct inet_protosw *p);

#if IS_ENABLED(CONFIG_IPV6)
extern int	inet6_add_protocol(const struct inet6_protocol *prot, unsigned char num);
extern int	inet6_del_protocol(const struct inet6_protocol *prot, unsigned char num);
extern int	inet6_register_protosw(struct inet_protosw *p);
extern void	inet6_unregister_protosw(struct inet_protosw *p);
#endif

#endif	
