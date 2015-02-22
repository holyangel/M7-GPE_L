#ifndef __LINUX_LLC_H
#define __LINUX_LLC_H
/*
 * IEEE 802.2 User Interface SAPs for Linux, data structures and indicators.
 *
 * Copyright (c) 2001 by Jay Schulist <jschlst@samba.org>
 *
 * This program can be redistributed or modified under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * This program is distributed without any warranty or implied warranty
 * of merchantability or fitness for a particular purpose.
 *
 * See the GNU General Public License for more details.
 */

#include <linux/socket.h>

#define __LLC_SOCK_SIZE__ 16	
struct sockaddr_llc {
	__kernel_sa_family_t sllc_family; 
	__kernel_sa_family_t sllc_arphrd; 
	unsigned char   sllc_test;
	unsigned char   sllc_xid;
	unsigned char	sllc_ua;	
	unsigned char   sllc_sap;
	unsigned char   sllc_mac[IFHWADDRLEN];
	unsigned char   __pad[__LLC_SOCK_SIZE__ -
			      sizeof(__kernel_sa_family_t) * 2 -
			      sizeof(unsigned char) * 4 - IFHWADDRLEN];
};

enum llc_sockopts {
	LLC_OPT_UNKNOWN = 0,
	LLC_OPT_RETRY,		
	LLC_OPT_SIZE,		
	LLC_OPT_ACK_TMR_EXP,	
	LLC_OPT_P_TMR_EXP,	
	LLC_OPT_REJ_TMR_EXP,	
	LLC_OPT_BUSY_TMR_EXP,	
	LLC_OPT_TX_WIN,		
	LLC_OPT_RX_WIN,		
	LLC_OPT_PKTINFO,	
	LLC_OPT_MAX
};

#define LLC_OPT_MAX_RETRY	 100
#define LLC_OPT_MAX_SIZE	4196
#define LLC_OPT_MAX_WIN		 127
#define LLC_OPT_MAX_ACK_TMR_EXP	  60
#define LLC_OPT_MAX_P_TMR_EXP	  60
#define LLC_OPT_MAX_REJ_TMR_EXP	  60
#define LLC_OPT_MAX_BUSY_TMR_EXP  60

#define LLC_SAP_NULL	0x00		
#define LLC_SAP_LLC	0x02		
#define LLC_SAP_SNA	0x04		
#define LLC_SAP_PNM	0x0E			
#define LLC_SAP_IP	0x06		
#define LLC_SAP_BSPAN	0x42		
#define LLC_SAP_MMS	0x4E		
#define LLC_SAP_8208	0x7E		
#define LLC_SAP_3COM	0x80		
#define LLC_SAP_PRO	0x8E		
#define LLC_SAP_SNAP	0xAA		
#define LLC_SAP_BANYAN	0xBC		
#define LLC_SAP_IPX	0xE0		
#define LLC_SAP_NETBEUI	0xF0		
#define LLC_SAP_LANMGR	0xF4		
#define LLC_SAP_IMPL	0xF8		
#define LLC_SAP_DISC	0xFC		
#define LLC_SAP_OSI	0xFE		
#define LLC_SAP_LAR	0xDC		
#define LLC_SAP_RM	0xD4		
#define LLC_SAP_GLOBAL	0xFF		

struct llc_pktinfo {
	int lpi_ifindex;
	unsigned char lpi_sap;
	unsigned char lpi_mac[IFHWADDRLEN];
};

#ifdef __KERNEL__
#define LLC_SAP_DYN_START	0xC0
#define LLC_SAP_DYN_STOP	0xDE
#define LLC_SAP_DYN_TRIES	4

#define llc_ui_skb_cb(__skb) ((struct sockaddr_llc *)&((__skb)->cb[0]))
#endif 
#endif 
