/*
 * iSCSI transport class definitions
 *
 * Copyright (C) IBM Corporation, 2004
 * Copyright (C) Mike Christie, 2004 - 2005
 * Copyright (C) Dmitry Yusupov, 2004 - 2005
 * Copyright (C) Alex Aizman, 2004 - 2005
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/bsg-lib.h>
#include <linux/idr.h>
#include <net/tcp.h>
#include <scsi/scsi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsi_transport_iscsi.h>
#include <scsi/iscsi_if.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_bsg_iscsi.h>

#define ISCSI_TRANSPORT_VERSION "2.0-870"

static int dbg_session;
module_param_named(debug_session, dbg_session, int,
		   S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug_session,
		 "Turn on debugging for sessions in scsi_transport_iscsi "
		 "module. Set to 1 to turn on, and zero to turn off. Default "
		 "is off.");

static int dbg_conn;
module_param_named(debug_conn, dbg_conn, int,
		   S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug_conn,
		 "Turn on debugging for connections in scsi_transport_iscsi "
		 "module. Set to 1 to turn on, and zero to turn off. Default "
		 "is off.");

#define ISCSI_DBG_TRANS_SESSION(_session, dbg_fmt, arg...)		\
	do {								\
		if (dbg_session)					\
			iscsi_cls_session_printk(KERN_INFO, _session,	\
						 "%s: " dbg_fmt,	\
						 __func__, ##arg);	\
	} while (0);

#define ISCSI_DBG_TRANS_CONN(_conn, dbg_fmt, arg...)			\
	do {								\
		if (dbg_conn)						\
			iscsi_cls_conn_printk(KERN_INFO, _conn,		\
					      "%s: " dbg_fmt,		\
					      __func__, ##arg);	\
	} while (0);

struct iscsi_internal {
	struct scsi_transport_template t;
	struct iscsi_transport *iscsi_transport;
	struct list_head list;
	struct device dev;

	struct transport_container conn_cont;
	struct transport_container session_cont;
};

static atomic_t iscsi_session_nr; /* sysfs session id for next new session */
static struct workqueue_struct *iscsi_eh_timer_workq;

static DEFINE_IDA(iscsi_sess_ida);
/*
 * list of registered transports and lock that must
 * be held while accessing list. The iscsi_transport_lock must
 * be acquired after the rx_queue_mutex.
 */
static LIST_HEAD(iscsi_transports);
static DEFINE_SPINLOCK(iscsi_transport_lock);

#define to_iscsi_internal(tmpl) \
	container_of(tmpl, struct iscsi_internal, t)

#define dev_to_iscsi_internal(_dev) \
	container_of(_dev, struct iscsi_internal, dev)

static void iscsi_transport_release(struct device *dev)
{
	struct iscsi_internal *priv = dev_to_iscsi_internal(dev);
	kfree(priv);
}

/*
 * iscsi_transport_class represents the iscsi_transports that are
 * registered.
 */
static struct class iscsi_transport_class = {
	.name = "iscsi_transport",
	.dev_release = iscsi_transport_release,
};

static ssize_t
show_transport_handle(struct device *dev, struct device_attribute *attr,
		      char *buf)
{
	struct iscsi_internal *priv = dev_to_iscsi_internal(dev);
	return sprintf(buf, "%llu\n", (unsigned long long)iscsi_handle(priv->iscsi_transport));
}
static DEVICE_ATTR(handle, S_IRUGO, show_transport_handle, NULL);

#define show_transport_attr(name, format)				\
static ssize_t								\
show_transport_##name(struct device *dev, 				\
		      struct device_attribute *attr,char *buf)		\
{									\
	struct iscsi_internal *priv = dev_to_iscsi_internal(dev);	\
	return sprintf(buf, format"\n", priv->iscsi_transport->name);	\
}									\
static DEVICE_ATTR(name, S_IRUGO, show_transport_##name, NULL);

show_transport_attr(caps, "0x%x");

static struct attribute *iscsi_transport_attrs[] = {
	&dev_attr_handle.attr,
	&dev_attr_caps.attr,
	NULL,
};

static struct attribute_group iscsi_transport_group = {
	.attrs = iscsi_transport_attrs,
};

/*
 * iSCSI endpoint attrs
 */
#define iscsi_dev_to_endpoint(_dev) \
	container_of(_dev, struct iscsi_endpoint, dev)

#define ISCSI_ATTR(_prefix,_name,_mode,_show,_store)	\
struct device_attribute dev_attr_##_prefix##_##_name =	\
        __ATTR(_name,_mode,_show,_store)

static void iscsi_endpoint_release(struct device *dev)
{
	struct iscsi_endpoint *ep = iscsi_dev_to_endpoint(dev);
	kfree(ep);
}

static struct class iscsi_endpoint_class = {
	.name = "iscsi_endpoint",
	.dev_release = iscsi_endpoint_release,
};

static ssize_t
show_ep_handle(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct iscsi_endpoint *ep = iscsi_dev_to_endpoint(dev);
	return sprintf(buf, "%llu\n", (unsigned long long) ep->id);
}
static ISCSI_ATTR(ep, handle, S_IRUGO, show_ep_handle, NULL);

static struct attribute *iscsi_endpoint_attrs[] = {
	&dev_attr_ep_handle.attr,
	NULL,
};

static struct attribute_group iscsi_endpoint_group = {
	.attrs = iscsi_endpoint_attrs,
};

#define ISCSI_MAX_EPID -1

static int iscsi_match_epid(struct device *dev, void *data)
{
	struct iscsi_endpoint *ep = iscsi_dev_to_endpoint(dev);
	uint64_t *epid = (uint64_t *) data;

	return *epid == ep->id;
}

struct iscsi_endpoint *
iscsi_create_endpoint(int dd_size)
{
	struct device *dev;
	struct iscsi_endpoint *ep;
	uint64_t id;
	int err;

	for (id = 1; id < ISCSI_MAX_EPID; id++) {
		dev = class_find_device(&iscsi_endpoint_class, NULL, &id,
					iscsi_match_epid);
		if (!dev)
			break;
	}
	if (id == ISCSI_MAX_EPID) {
		printk(KERN_ERR "Too many connections. Max supported %u\n",
		       ISCSI_MAX_EPID - 1);
		return NULL;
	}

	ep = kzalloc(sizeof(*ep) + dd_size, GFP_KERNEL);
	if (!ep)
		return NULL;

	ep->id = id;
	ep->dev.class = &iscsi_endpoint_class;
	dev_set_name(&ep->dev, "ep-%llu", (unsigned long long) id);
	err = device_register(&ep->dev);
        if (err)
                goto free_ep;

	err = sysfs_create_group(&ep->dev.kobj, &iscsi_endpoint_group);
	if (err)
		goto unregister_dev;

	if (dd_size)
		ep->dd_data = &ep[1];
	return ep;

unregister_dev:
	device_unregister(&ep->dev);
	return NULL;

free_ep:
	kfree(ep);
	return NULL;
}
EXPORT_SYMBOL_GPL(iscsi_create_endpoint);

void iscsi_destroy_endpoint(struct iscsi_endpoint *ep)
{
	sysfs_remove_group(&ep->dev.kobj, &iscsi_endpoint_group);
	device_unregister(&ep->dev);
}
EXPORT_SYMBOL_GPL(iscsi_destroy_endpoint);

struct iscsi_endpoint *iscsi_lookup_endpoint(u64 handle)
{
	struct iscsi_endpoint *ep;
	struct device *dev;

	dev = class_find_device(&iscsi_endpoint_class, NULL, &handle,
				iscsi_match_epid);
	if (!dev)
		return NULL;

	ep = iscsi_dev_to_endpoint(dev);
	/*
	 * we can drop this now because the interface will prevent
	 * removals and lookups from racing.
	 */
	put_device(dev);
	return ep;
}
EXPORT_SYMBOL_GPL(iscsi_lookup_endpoint);

/*
 * Interface to display network param to sysfs
 */

static void iscsi_iface_release(struct device *dev)
{
	struct iscsi_iface *iface = iscsi_dev_to_iface(dev);
	struct device *parent = iface->dev.parent;

	kfree(iface);
	put_device(parent);
}


static struct class iscsi_iface_class = {
	.name = "iscsi_iface",
	.dev_release = iscsi_iface_release,
};

#define ISCSI_IFACE_ATTR(_prefix, _name, _mode, _show, _store)	\
struct device_attribute dev_attr_##_prefix##_##_name =		\
	__ATTR(_name, _mode, _show, _store)

/* iface attrs show */
#define iscsi_iface_attr_show(type, name, param_type, param)		\
static ssize_t								\
show_##type##_##name(struct device *dev, struct device_attribute *attr,	\
		     char *buf)						\
{									\
	struct iscsi_iface *iface = iscsi_dev_to_iface(dev);		\
	struct iscsi_transport *t = iface->transport;			\
	return t->get_iface_param(iface, param_type, param, buf);	\
}									\

#define iscsi_iface_net_attr(type, name, param)				\
	iscsi_iface_attr_show(type, name, ISCSI_NET_PARAM, param)	\
static ISCSI_IFACE_ATTR(type, name, S_IRUGO, show_##type##_##name, NULL);

/* generic read only ipvi4 attribute */
iscsi_iface_net_attr(ipv4_iface, ipaddress, ISCSI_NET_PARAM_IPV4_ADDR);
iscsi_iface_net_attr(ipv4_iface, gateway, ISCSI_NET_PARAM_IPV4_GW);
iscsi_iface_net_attr(ipv4_iface, subnet, ISCSI_NET_PARAM_IPV4_SUBNET);
iscsi_iface_net_attr(ipv4_iface, bootproto, ISCSI_NET_PARAM_IPV4_BOOTPROTO);

/* generic read only ipv6 attribute */
iscsi_iface_net_attr(ipv6_iface, ipaddress, ISCSI_NET_PARAM_IPV6_ADDR);
iscsi_iface_net_attr(ipv6_iface, link_local_addr, ISCSI_NET_PARAM_IPV6_LINKLOCAL);
iscsi_iface_net_attr(ipv6_iface, router_addr, ISCSI_NET_PARAM_IPV6_ROUTER);
iscsi_iface_net_attr(ipv6_iface, ipaddr_autocfg,
		     ISCSI_NET_PARAM_IPV6_ADDR_AUTOCFG);
iscsi_iface_net_attr(ipv6_iface, link_local_autocfg,
		     ISCSI_NET_PARAM_IPV6_LINKLOCAL_AUTOCFG);

/* common read only iface attribute */
iscsi_iface_net_attr(iface, enabled, ISCSI_NET_PARAM_IFACE_ENABLE);
iscsi_iface_net_attr(iface, vlan_id, ISCSI_NET_PARAM_VLAN_ID);
iscsi_iface_net_attr(iface, vlan_priority, ISCSI_NET_PARAM_VLAN_PRIORITY);
iscsi_iface_net_attr(iface, vlan_enabled, ISCSI_NET_PARAM_VLAN_ENABLED);
iscsi_iface_net_attr(iface, mtu, ISCSI_NET_PARAM_MTU);
iscsi_iface_net_attr(iface, port, ISCSI_NET_PARAM_PORT);

static umode_t iscsi_iface_attr_is_visible(struct kobject *kobj,
					  struct attribute *attr, int i)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct iscsi_iface *iface = iscsi_dev_to_iface(dev);
	struct iscsi_transport *t = iface->transport;
	int param;

	if (attr == &dev_attr_iface_enabled.attr)
		param = ISCSI_NET_PARAM_IFACE_ENABLE;
	else if (attr == &dev_attr_iface_vlan_id.attr)
		param = ISCSI_NET_PARAM_VLAN_ID;
	else if (attr == &dev_attr_iface_vlan_priority.attr)
		param = ISCSI_NET_PARAM_VLAN_PRIORITY;
	else if (attr == &dev_attr_iface_vlan_enabled.attr)
		param = ISCSI_NET_PARAM_VLAN_ENABLED;
	else if (attr == &dev_attr_iface_mtu.attr)
		param = ISCSI_NET_PARAM_MTU;
	else if (attr == &dev_attr_iface_port.attr)
		param = ISCSI_NET_PARAM_PORT;
	else if (iface->iface_type == ISCSI_IFACE_TYPE_IPV4) {
		if (attr == &dev_attr_ipv4_iface_ipaddress.attr)
			param = ISCSI_NET_PARAM_IPV4_ADDR;
		else if (attr == &dev_attr_ipv4_iface_gateway.attr)
			param = ISCSI_NET_PARAM_IPV4_GW;
		else if (attr == &dev_attr_ipv4_iface_subnet.attr)
			param = ISCSI_NET_PARAM_IPV4_SUBNET;
		else if (attr == &dev_attr_ipv4_iface_bootproto.attr)
			param = ISCSI_NET_PARAM_IPV4_BOOTPROTO;
		else
			return 0;
	} else if (iface->iface_type == ISCSI_IFACE_TYPE_IPV6) {
		if (attr == &dev_attr_ipv6_iface_ipaddress.attr)
			param = ISCSI_NET_PARAM_IPV6_ADDR;
		else if (attr == &dev_attr_ipv6_iface_link_local_addr.attr)
			param = ISCSI_NET_PARAM_IPV6_LINKLOCAL;
		else if (attr == &dev_attr_ipv6_iface_router_addr.attr)
			param = ISCSI_NET_PARAM_IPV6_ROUTER;
		else if (attr == &dev_attr_ipv6_iface_ipaddr_autocfg.attr)
			param = ISCSI_NET_PARAM_IPV6_ADDR_AUTOCFG;
		else if (attr == &dev_attr_ipv6_iface_link_local_autocfg.attr)
			param = ISCSI_NET_PARAM_IPV6_LINKLOCAL_AUTOCFG;
		else
			return 0;
	} else {
		WARN_ONCE(1, "Invalid iface attr");
		return 0;
	}

	return t->attr_is_visible(ISCSI_NET_PARAM, param);
}

static struct attribute *iscsi_iface_attrs[] = {
	&dev_attr_iface_enabled.attr,
	&dev_attr_iface_vlan_id.attr,
	&dev_attr_iface_vlan_priority.attr,
	&dev_attr_iface_vlan_enabled.attr,
	&dev_attr_ipv4_iface_ipaddress.attr,
	&dev_attr_ipv4_iface_gateway.attr,
	&dev_attr_ipv4_iface_subnet.attr,
	&dev_attr_ipv4_iface_bootproto.attr,
	&dev_attr_ipv6_iface_ipaddress.attr,
	&dev_attr_ipv6_iface_link_local_addr.attr,
	&dev_attr_ipv6_iface_router_addr.attr,
	&dev_attr_ipv6_iface_ipaddr_autocfg.attr,
	&dev_attr_ipv6_iface_link_local_autocfg.attr,
	&dev_attr_iface_mtu.attr,
	&dev_attr_iface_port.attr,
	NULL,
};

static struct attribute_group iscsi_iface_group = {
	.attrs = iscsi_iface_attrs,
	.is_visible = iscsi_iface_attr_is_visible,
};

struct iscsi_iface *
iscsi_create_iface(struct Scsi_Host *shost, struct iscsi_transport *transport,
		   uint32_t iface_type, uint32_t iface_num, int dd_size)
{
	struct iscsi_iface *iface;
	int err;

	iface = kzalloc(sizeof(*iface) + dd_size, GFP_KERNEL);
	if (!iface)
		return NULL;

	iface->transport = transport;
	iface->iface_type = iface_type;
	iface->iface_num = iface_num;
	iface->dev.release = iscsi_iface_release;
	iface->dev.class = &iscsi_iface_class;
	/* parent reference released in iscsi_iface_release */
	iface->dev.parent = get_device(&shost->shost_gendev);
	if (iface_type == ISCSI_IFACE_TYPE_IPV4)
		dev_set_name(&iface->dev, "ipv4-iface-%u-%u", shost->host_no,
			     iface_num);
	else
		dev_set_name(&iface->dev, "ipv6-iface-%u-%u", shost->host_no,
			     iface_num);

	err = device_register(&iface->dev);
	if (err)
		goto free_iface;

	err = sysfs_create_group(&iface->dev.kobj, &iscsi_iface_group);
	if (err)
		goto unreg_iface;

	if (dd_size)
		iface->dd_data = &iface[1];
	return iface;

unreg_iface:
	device_unregister(&iface->dev);
	return NULL;

free_iface:
	put_device(iface->dev.parent);
	kfree(iface);
	return NULL;
}
EXPORT_SYMBOL_GPL(iscsi_create_iface);

void iscsi_destroy_iface(struct iscsi_iface *iface)
{
	sysfs_remove_group(&iface->dev.kobj, &iscsi_iface_group);
	device_unregister(&iface->dev);
}
EXPORT_SYMBOL_GPL(iscsi_destroy_iface);

/*
 * BSG support
 */
/**
 * iscsi_bsg_host_dispatch - Dispatch command to LLD.
 * @job: bsg job to be processed
 */
static int iscsi_bsg_host_dispatch(struct bsg_job *job)
{
	struct Scsi_Host *shost = iscsi_job_to_shost(job);
	struct iscsi_bsg_request *req = job->request;
	struct iscsi_bsg_reply *reply = job->reply;
	struct iscsi_internal *i = to_iscsi_internal(shost->transportt);
	int cmdlen = sizeof(uint32_t);	/* start with length of msgcode */
	int ret;

	/* check if we have the msgcode value at least */
	if (job->request_len < sizeof(uint32_t)) {
		ret = -ENOMSG;
		goto fail_host_msg;
	}

	/* Validate the host command */
	switch (req->msgcode) {
	case ISCSI_BSG_HST_VENDOR:
		cmdlen += sizeof(struct iscsi_bsg_host_vendor);
		if ((shost->hostt->vendor_id == 0L) ||
		    (req->rqst_data.h_vendor.vendor_id !=
			shost->hostt->vendor_id)) {
			ret = -ESRCH;
			goto fail_host_msg;
		}
		break;
	default:
		ret = -EBADR;
		goto fail_host_msg;
	}

	/* check if we really have all the request data needed */
	if (job->request_len < cmdlen) {
		ret = -ENOMSG;
		goto fail_host_msg;
	}

	ret = i->iscsi_transport->bsg_request(job);
	if (!ret)
		return 0;

fail_host_msg:
	/* return the errno failure code as the only status */
	BUG_ON(job->reply_len < sizeof(uint32_t));
	reply->reply_payload_rcv_len = 0;
	reply->result = ret;
	job->reply_len = sizeof(uint32_t);
	bsg_job_done(job, ret, 0);
	return 0;
}

/**
 * iscsi_bsg_host_add - Create and add the bsg hooks to receive requests
 * @shost: shost for iscsi_host
 * @ihost: iscsi_cls_host adding the structures to
 */
static int
iscsi_bsg_host_add(struct Scsi_Host *shost, struct iscsi_cls_host *ihost)
{
	struct device *dev = &shost->shost_gendev;
	struct iscsi_internal *i = to_iscsi_internal(shost->transportt);
	struct request_queue *q;
	char bsg_name[20];
	int ret;

	if (!i->iscsi_transport->bsg_request)
		return -ENOTSUPP;

	snprintf(bsg_name, sizeof(bsg_name), "iscsi_host%d", shost->host_no);

	q = __scsi_alloc_queue(shost, bsg_request_fn);
	if (!q)
		return -ENOMEM;

	ret = bsg_setup_queue(dev, q, bsg_name, iscsi_bsg_host_dispatch, 0);
	if (ret) {
		shost_printk(KERN_ERR, shost, "bsg interface failed to "
			     "initialize - no request queue\n");
		blk_cleanup_queue(q);
		return ret;
	}

	ihost->bsg_q = q;
	return 0;
}

static int iscsi_setup_host(struct transport_container *tc, struct device *dev,
			    struct device *cdev)
{
	struct Scsi_Host *shost = dev_to_shost(dev);
	struct iscsi_cls_host *ihost = shost->shost_data;

	memset(ihost, 0, sizeof(*ihost));
	atomic_set(&ihost->nr_scans, 0);
	mutex_init(&ihost->mutex);

	iscsi_bsg_host_add(shost, ihost);
	/* ignore any bsg add error - we just can't do sgio */

	return 0;
}

static int iscsi_remove_host(struct transport_container *tc,
			     struct device *dev, struct device *cdev)
{
	struct Scsi_Host *shost = dev_to_shost(dev);
	struct iscsi_cls_host *ihost = shost->shost_data;

	if (ihost->bsg_q) {
		bsg_remove_queue(ihost->bsg_q);
		blk_cleanup_queue(ihost->bsg_q);
	}
	return 0;
}

static DECLARE_TRANSPORT_CLASS(iscsi_host_class,
			       "iscsi_host",
			       iscsi_setup_host,
			       iscsi_remove_host,
			       NULL);

static DECLARE_TRANSPORT_CLASS(iscsi_session_class,
			       "iscsi_session",
			       NULL,
			       NULL,
			       NULL);

static DECLARE_TRANSPORT_CLASS(iscsi_connection_class,
			       "iscsi_connection",
			       NULL,
			       NULL,
			       NULL);

static struct sock *nls;
static DEFINE_MUTEX(rx_queue_mutex);

static LIST_HEAD(sesslist);
static DEFINE_SPINLOCK(sesslock);
static LIST_HEAD(connlist);
static DEFINE_SPINLOCK(connlock);

static uint32_t iscsi_conn_get_sid(struct iscsi_cls_conn *conn)
{
	struct iscsi_cls_session *sess = iscsi_dev_to_session(conn->dev.parent);
	return sess->sid;
}

/*
 * Returns the matching session to a given sid
 */
static struct iscsi_cls_session *iscsi_session_lookup(uint32_t sid)
{
	unsigned long flags;
	struct iscsi_cls_session *sess;

	spin_lock_irqsave(&sesslock, flags);
	list_for_each_entry(sess, &sesslist, sess_list) {
		if (sess->sid == sid) {
			spin_unlock_irqrestore(&sesslock, flags);
			return sess;
		}
	}
	spin_unlock_irqrestore(&sesslock, flags);
	return NULL;
}

/*
 * Returns the matching connection to a given sid / cid tuple
 */
static struct iscsi_cls_conn *iscsi_conn_lookup(uint32_t sid, uint32_t cid)
{
	unsigned long flags;
	struct iscsi_cls_conn *conn;

	spin_lock_irqsave(&connlock, flags);
	list_for_each_entry(conn, &connlist, conn_list) {
		if ((conn->cid == cid) && (iscsi_conn_get_sid(conn) == sid)) {
			spin_unlock_irqrestore(&connlock, flags);
			return conn;
		}
	}
	spin_unlock_irqrestore(&connlock, flags);
	return NULL;
}

/*
 * The following functions can be used by LLDs that allocate
 * their own scsi_hosts or by software iscsi LLDs
 */
static struct {
	int value;
	char *name;
} iscsi_session_state_names[] = {
	{ ISCSI_SESSION_LOGGED_IN,	"LOGGED_IN" },
	{ ISCSI_SESSION_FAILED,		"FAILED" },
	{ ISCSI_SESSION_FREE,		"FREE" },
};

static const char *iscsi_session_state_name(int state)
{
	int i;
	char *name = NULL;

	for (i = 0; i < ARRAY_SIZE(iscsi_session_state_names); i++) {
		if (iscsi_session_state_names[i].value == state) {
			name = iscsi_session_state_names[i].name;
			break;
		}
	}
	return name;
}

int iscsi_session_chkready(struct iscsi_cls_session *session)
{
	unsigned long flags;
	int err;

	spin_lock_irqsave(&session->lock, flags);
	switch (session->state) {
	case ISCSI_SESSION_LOGGED_IN:
		err = 0;
		break;
	case ISCSI_SESSION_FAILED:
		err = DID_IMM_RETRY << 16;
		break;
	case ISCSI_SESSION_FREE:
		err = DID_TRANSPORT_FAILFAST << 16;
		break;
	default:
		err = DID_NO_CONNECT << 16;
		break;
	}
	spin_unlock_irqrestore(&session->lock, flags);
	return err;
}
EXPORT_SYMBOL_GPL(iscsi_session_chkready);

int iscsi_is_session_online(struct iscsi_cls_session *session)
{
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&session->lock, flags);
	if (session->state == ISCSI_SESSION_LOGGED_IN)
		ret = 1;
	spin_unlock_irqrestore(&session->lock, flags);
	return ret;
}
EXPORT_SYMBOL_GPL(iscsi_is_session_online);

static void iscsi_session_release(struct device *dev)
{
	struct iscsi_cls_session *session = iscsi_dev_to_session(dev);
	struct Scsi_Host *shost;

	shost = iscsi_session_to_shost(session);
	scsi_host_put(shost);
	ISCSI_DBG_TRANS_SESSION(session, "Completing session release\n");
	kfree(session);
}

int iscsi_is_session_dev(const struct device *dev)
{
	return dev->release == iscsi_session_release;
}
EXPORT_SYMBOL_GPL(iscsi_is_session_dev);

static int iscsi_iter_session_fn(struct device *dev, void *data)
{
	void (* fn) (struct iscsi_cls_session *) = data;

	if (!iscsi_is_session_dev(dev))
		return 0;
	fn(iscsi_dev_to_session(dev));
	return 0;
}

void iscsi_host_for_each_session(struct Scsi_Host *shost,
				 void (*fn)(struct iscsi_cls_session *))
{
	device_for_each_child(&shost->shost_gendev, fn,
			      iscsi_iter_session_fn);
}
EXPORT_SYMBOL_GPL(iscsi_host_for_each_session);

/**
 * iscsi_scan_finished - helper to report when running scans are done
 * @shost: scsi host
 * @time: scan run time
 *
 * This function can be used by drives like qla4xxx to report to the scsi
 * layer when the scans it kicked off at module load time are done.
 */
int iscsi_scan_finished(struct Scsi_Host *shost, unsigned long time)
{
	struct iscsi_cls_host *ihost = shost->shost_data;
	/*
	 * qla4xxx will have kicked off some session unblocks before calling
	 * scsi_scan_host, so just wait for them to complete.
	 */
	return !atomic_read(&ihost->nr_scans);
}
EXPORT_SYMBOL_GPL(iscsi_scan_finished);

struct iscsi_scan_data {
	unsigned int channel;
	unsigned int id;
	unsigned int lun;
};

static int iscsi_user_scan_session(struct device *dev, void *data)
{
	struct iscsi_scan_data *scan_data = data;
	struct iscsi_cls_session *session;
	struct Scsi_Host *shost;
	struct iscsi_cls_host *ihost;
	unsigned long flags;
	unsigned int id;

	if (!iscsi_is_session_dev(dev))
		return 0;

	session = iscsi_dev_to_session(dev);

	ISCSI_DBG_TRANS_SESSION(session, "Scanning session\n");

	shost = iscsi_session_to_shost(session);
	ihost = shost->shost_data;

	mutex_lock(&ihost->mutex);
	spin_lock_irqsave(&session->lock, flags);
	if (session->state != ISCSI_SESSION_LOGGED_IN) {
		spin_unlock_irqrestore(&session->lock, flags);
		goto user_scan_exit;
	}
	id = session->target_id;
	spin_unlock_irqrestore(&session->lock, flags);

	if (id != ISCSI_MAX_TARGET) {
		if ((scan_data->channel == SCAN_WILD_CARD ||
		     scan_data->channel == 0) &&
		    (scan_data->id == SCAN_WILD_CARD ||
		     scan_data->id == id))
			scsi_scan_target(&session->dev, 0, id,
					 scan_data->lun, 1);
	}

user_scan_exit:
	mutex_unlock(&ihost->mutex);
	ISCSI_DBG_TRANS_SESSION(session, "Completed session scan\n");
	return 0;
}

static int iscsi_user_scan(struct Scsi_Host *shost, uint channel,
			   uint id, uint lun)
{
	struct iscsi_scan_data scan_data;

	scan_data.channel = channel;
	scan_data.id = id;
	scan_data.lun = lun;

	return device_for_each_child(&shost->shost_gendev, &scan_data,
				     iscsi_user_scan_session);
}

static void iscsi_scan_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session, scan_work);
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost = shost->shost_data;
	struct iscsi_scan_data scan_data;

	scan_data.channel = 0;
	scan_data.id = SCAN_WILD_CARD;
	scan_data.lun = SCAN_WILD_CARD;

	iscsi_user_scan_session(&session->dev, &scan_data);
	atomic_dec(&ihost->nr_scans);
}

/**
 * iscsi_block_scsi_eh - block scsi eh until session state has transistioned
 * @cmd: scsi cmd passed to scsi eh handler
 *
 * If the session is down this function will wait for the recovery
 * timer to fire or for the session to be logged back in. If the
 * recovery timer fires then FAST_IO_FAIL is returned. The caller
 * should pass this error value to the scsi eh.
 */
int iscsi_block_scsi_eh(struct scsi_cmnd *cmd)
{
	struct iscsi_cls_session *session =
			starget_to_session(scsi_target(cmd->device));
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&session->lock, flags);
	while (session->state != ISCSI_SESSION_LOGGED_IN) {
		if (session->state == ISCSI_SESSION_FREE) {
			ret = FAST_IO_FAIL;
			break;
		}
		spin_unlock_irqrestore(&session->lock, flags);
		msleep(1000);
		spin_lock_irqsave(&session->lock, flags);
	}
	spin_unlock_irqrestore(&session->lock, flags);
	return ret;
}
EXPORT_SYMBOL_GPL(iscsi_block_scsi_eh);

static void session_recovery_timedout(struct work_struct *work)
{
	struct iscsi_cls_session *session =
		container_of(work, struct iscsi_cls_session,
			     recovery_work.work);
	unsigned long flags;

	iscsi_cls_session_printk(KERN_INFO, session,
				 "session recovery timed out after %d secs\n",
				 session->recovery_tmo);

	spin_lock_irqsave(&session->lock, flags);
	switch (session->state) {
	case ISCSI_SESSION_FAILED:
		session->state = ISCSI_SESSION_FREE;
		break;
	case ISCSI_SESSION_LOGGED_IN:
	case ISCSI_SESSION_FREE:
		/* we raced with the unblock's flush */
		spin_unlock_irqrestore(&session->lock, flags);
		return;
	}
	spin_unlock_irqrestore(&session->lock, flags);

	if (session->transport->session_recovery_timedout)
		session->transport->session_recovery_timedout(session);

	ISCSI_DBG_TRANS_SESSION(session, "Unblocking SCSI target\n");
	scsi_target_unblock(&session->dev);
	ISCSI_DBG_TRANS_SESSION(session, "Completed unblocking SCSI target\n");
}

static void __iscsi_unblock_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session,
				     unblock_work);
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost = shost->shost_data;
	unsigned long flags;

	ISCSI_DBG_TRANS_SESSION(session, "Unblocking session\n");
	/*
	 * The recovery and unblock work get run from the same workqueue,
	 * so try to cancel it if it was going to run after this unblock.
	 */
	cancel_delayed_work(&session->recovery_work);
	spin_lock_irqsave(&session->lock, flags);
	session->state = ISCSI_SESSION_LOGGED_IN;
	spin_unlock_irqrestore(&session->lock, flags);
	/* start IO */
	scsi_target_unblock(&session->dev);
	/*
	 * Only do kernel scanning if the driver is properly hooked into
	 * the async scanning code (drivers like iscsi_tcp do login and
	 * scanning from userspace).
	 */
	if (shost->hostt->scan_finished) {
		if (scsi_queue_work(shost, &session->scan_work))
			atomic_inc(&ihost->nr_scans);
	}
	ISCSI_DBG_TRANS_SESSION(session, "Completed unblocking session\n");
}

/**
 * iscsi_unblock_session - set a session as logged in and start IO.
 * @session: iscsi session
 *
 * Mark a session as ready to accept IO.
 */
void iscsi_unblock_session(struct iscsi_cls_session *session)
{
	queue_work(iscsi_eh_timer_workq, &session->unblock_work);
	/*
	 * make sure all the events have completed before tell the driver
	 * it is safe
	 */
	flush_workqueue(iscsi_eh_timer_workq);
}
EXPORT_SYMBOL_GPL(iscsi_unblock_session);

static void __iscsi_block_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session,
				     block_work);
	unsigned long flags;

	ISCSI_DBG_TRANS_SESSION(session, "Blocking session\n");
	spin_lock_irqsave(&session->lock, flags);
	session->state = ISCSI_SESSION_FAILED;
	spin_unlock_irqrestore(&session->lock, flags);
	scsi_target_block(&session->dev);
	ISCSI_DBG_TRANS_SESSION(session, "Completed SCSI target blocking\n");
	if (session->recovery_tmo >= 0)
		queue_delayed_work(iscsi_eh_timer_workq,
				   &session->recovery_work,
				   session->recovery_tmo * HZ);
}

void iscsi_block_session(struct iscsi_cls_session *session)
{
	queue_work(iscsi_eh_timer_workq, &session->block_work);
}
EXPORT_SYMBOL_GPL(iscsi_block_session);

static void __iscsi_unbind_session(struct work_struct *work)
{
	struct iscsi_cls_session *session =
			container_of(work, struct iscsi_cls_session,
				     unbind_work);
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost = shost->shost_data;
	unsigned long flags;
	unsigned int target_id;

	ISCSI_DBG_TRANS_SESSION(session, "Unbinding session\n");

	/* Prevent new scans and make sure scanning is not in progress */
	mutex_lock(&ihost->mutex);
	spin_lock_irqsave(&session->lock, flags);
	if (session->target_id == ISCSI_MAX_TARGET) {
		spin_unlock_irqrestore(&session->lock, flags);
		mutex_unlock(&ihost->mutex);
		return;
	}

	target_id = session->target_id;
	session->target_id = ISCSI_MAX_TARGET;
	spin_unlock_irqrestore(&session->lock, flags);
	mutex_unlock(&ihost->mutex);

	if (session->ida_used)
		ida_simple_remove(&iscsi_sess_ida, target_id);

	scsi_remove_target(&session->dev);
	iscsi_session_event(session, ISCSI_KEVENT_UNBIND_SESSION);
	ISCSI_DBG_TRANS_SESSION(session, "Completed target removal\n");
}

struct iscsi_cls_session *
iscsi_alloc_session(struct Scsi_Host *shost, struct iscsi_transport *transport,
		    int dd_size)
{
	struct iscsi_cls_session *session;

	session = kzalloc(sizeof(*session) + dd_size,
			  GFP_KERNEL);
	if (!session)
		return NULL;

	session->transport = transport;
	session->creator = -1;
	session->recovery_tmo = 120;
	session->state = ISCSI_SESSION_FREE;
	INIT_DELAYED_WORK(&session->recovery_work, session_recovery_timedout);
	INIT_LIST_HEAD(&session->sess_list);
	INIT_WORK(&session->unblock_work, __iscsi_unblock_session);
	INIT_WORK(&session->block_work, __iscsi_block_session);
	INIT_WORK(&session->unbind_work, __iscsi_unbind_session);
	INIT_WORK(&session->scan_work, iscsi_scan_session);
	spin_lock_init(&session->lock);

	/* this is released in the dev's release function */
	scsi_host_get(shost);
	session->dev.parent = &shost->shost_gendev;
	session->dev.release = iscsi_session_release;
	device_initialize(&session->dev);
	if (dd_size)
		session->dd_data = &session[1];

	ISCSI_DBG_TRANS_SESSION(session, "Completed session allocation\n");
	return session;
}
EXPORT_SYMBOL_GPL(iscsi_alloc_session);

int iscsi_add_session(struct iscsi_cls_session *session, unsigned int target_id)
{
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	struct iscsi_cls_host *ihost;
	unsigned long flags;
	int id = 0;
	int err;

	ihost = shost->shost_data;
	session->sid = atomic_add_return(1, &iscsi_session_nr);

	if (target_id == ISCSI_MAX_TARGET) {
		id = ida_simple_get(&iscsi_sess_ida, 0, 0, GFP_KERNEL);

		if (id < 0) {
			iscsi_cls_session_printk(KERN_ERR, session,
					"Failure in Target ID Allocation\n");
			return id;
		}
		session->target_id = (unsigned int)id;
		session->ida_used = true;
	} else
		session->target_id = target_id;

	dev_set_name(&session->dev, "session%u", session->sid);
	err = device_add(&session->dev);
	if (err) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "could not register session's dev\n");
		goto release_ida;
	}
	transport_register_device(&session->dev);

	spin_lock_irqsave(&sesslock, flags);
	list_add(&session->sess_list, &sesslist);
	spin_unlock_irqrestore(&sesslock, flags);

	iscsi_session_event(session, ISCSI_KEVENT_CREATE_SESSION);
	ISCSI_DBG_TRANS_SESSION(session, "Completed session adding\n");
	return 0;

release_ida:
	if (session->ida_used)
		ida_simple_remove(&iscsi_sess_ida, session->target_id);

	return err;
}
EXPORT_SYMBOL_GPL(iscsi_add_session);

/**
 * iscsi_create_session - create iscsi class session
 * @shost: scsi host
 * @transport: iscsi transport
 * @dd_size: private driver data size
 * @target_id: which target
 *
 * This can be called from a LLD or iscsi_transport.
 */
struct iscsi_cls_session *
iscsi_create_session(struct Scsi_Host *shost, struct iscsi_transport *transport,
		     int dd_size, unsigned int target_id)
{
	struct iscsi_cls_session *session;

	session = iscsi_alloc_session(shost, transport, dd_size);
	if (!session)
		return NULL;

	if (iscsi_add_session(session, target_id)) {
		iscsi_free_session(session);
		return NULL;
	}
	return session;
}
EXPORT_SYMBOL_GPL(iscsi_create_session);

static void iscsi_conn_release(struct device *dev)
{
	struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev);
	struct device *parent = conn->dev.parent;

	ISCSI_DBG_TRANS_CONN(conn, "Releasing conn\n");
	kfree(conn);
	put_device(parent);
}

static int iscsi_is_conn_dev(const struct device *dev)
{
	return dev->release == iscsi_conn_release;
}

static int iscsi_iter_destroy_conn_fn(struct device *dev, void *data)
{
	if (!iscsi_is_conn_dev(dev))
		return 0;
	return iscsi_destroy_conn(iscsi_dev_to_conn(dev));
}

void iscsi_remove_session(struct iscsi_cls_session *session)
{
	struct Scsi_Host *shost = iscsi_session_to_shost(session);
	unsigned long flags;
	int err;

	ISCSI_DBG_TRANS_SESSION(session, "Removing session\n");

	spin_lock_irqsave(&sesslock, flags);
	list_del(&session->sess_list);
	spin_unlock_irqrestore(&sesslock, flags);

	/* make sure there are no blocks/unblocks queued */
	flush_workqueue(iscsi_eh_timer_workq);
	/* make sure the timedout callout is not running */
	if (!cancel_delayed_work(&session->recovery_work))
		flush_workqueue(iscsi_eh_timer_workq);
	/*
	 * If we are blocked let commands flow again. The lld or iscsi
	 * layer should set up the queuecommand to fail commands.
	 * We assume that LLD will not be calling block/unblock while
	 * removing the session.
	 */
	spin_lock_irqsave(&session->lock, flags);
	session->state = ISCSI_SESSION_FREE;
	spin_unlock_irqrestore(&session->lock, flags);

	scsi_target_unblock(&session->dev);
	/* flush running scans then delete devices */
	scsi_flush_work(shost);
	__iscsi_unbind_session(&session->unbind_work);

	/* hw iscsi may not have removed all connections from session */
	err = device_for_each_child(&session->dev, NULL,
				    iscsi_iter_destroy_conn_fn);
	if (err)
		iscsi_cls_session_printk(KERN_ERR, session,
					 "Could not delete all connections "
					 "for session. Error %d.\n", err);

	transport_unregister_device(&session->dev);

	ISCSI_DBG_TRANS_SESSION(session, "Completing session removal\n");
	device_del(&session->dev);
}
EXPORT_SYMBOL_GPL(iscsi_remove_session);

void iscsi_free_session(struct iscsi_cls_session *session)
{
	ISCSI_DBG_TRANS_SESSION(session, "Freeing session\n");
	iscsi_session_event(session, ISCSI_KEVENT_DESTROY_SESSION);
	put_device(&session->dev);
}
EXPORT_SYMBOL_GPL(iscsi_free_session);

/**
 * iscsi_destroy_session - destroy iscsi session
 * @session: iscsi_session
 *
 * Can be called by a LLD or iscsi_transport. There must not be
 * any running connections.
 */
int iscsi_destroy_session(struct iscsi_cls_session *session)
{
	iscsi_remove_session(session);
	ISCSI_DBG_TRANS_SESSION(session, "Completing session destruction\n");
	iscsi_free_session(session);
	return 0;
}
EXPORT_SYMBOL_GPL(iscsi_destroy_session);

/**
 * iscsi_create_conn - create iscsi class connection
 * @session: iscsi cls session
 * @dd_size: private driver data size
 * @cid: connection id
 *
 * This can be called from a LLD or iscsi_transport. The connection
 * is child of the session so cid must be unique for all connections
 * on the session.
 *
 * Since we do not support MCS, cid will normally be zero. In some cases
 * for software iscsi we could be trying to preallocate a connection struct
 * in which case there could be two connection structs and cid would be
 * non-zero.
 */
struct iscsi_cls_conn *
iscsi_create_conn(struct iscsi_cls_session *session, int dd_size, uint32_t cid)
{
	struct iscsi_transport *transport = session->transport;
	struct iscsi_cls_conn *conn;
	unsigned long flags;
	int err;

	conn = kzalloc(sizeof(*conn) + dd_size, GFP_KERNEL);
	if (!conn)
		return NULL;
	if (dd_size)
		conn->dd_data = &conn[1];

	mutex_init(&conn->ep_mutex);
	INIT_LIST_HEAD(&conn->conn_list);
	conn->transport = transport;
	conn->cid = cid;

	/* this is released in the dev's release function */
	if (!get_device(&session->dev))
		goto free_conn;

	dev_set_name(&conn->dev, "connection%d:%u", session->sid, cid);
	conn->dev.parent = &session->dev;
	conn->dev.release = iscsi_conn_release;
	err = device_register(&conn->dev);
	if (err) {
		iscsi_cls_session_printk(KERN_ERR, session, "could not "
					 "register connection's dev\n");
		goto release_parent_ref;
	}
	transport_register_device(&conn->dev);

	spin_lock_irqsave(&connlock, flags);
	list_add(&conn->conn_list, &connlist);
	spin_unlock_irqrestore(&connlock, flags);

	ISCSI_DBG_TRANS_CONN(conn, "Completed conn creation\n");
	return conn;

release_parent_ref:
	put_device(&session->dev);
free_conn:
	kfree(conn);
	return NULL;
}

EXPORT_SYMBOL_GPL(iscsi_create_conn);

/**
 * iscsi_destroy_conn - destroy iscsi class connection
 * @conn: iscsi cls session
 *
 * This can be called from a LLD or iscsi_transport.
 */
int iscsi_destroy_conn(struct iscsi_cls_conn *conn)
{
	unsigned long flags;

	spin_lock_irqsave(&connlock, flags);
	list_del(&conn->conn_list);
	spin_unlock_irqrestore(&connlock, flags);

	transport_unregister_device(&conn->dev);
	ISCSI_DBG_TRANS_CONN(conn, "Completing conn destruction\n");
	device_unregister(&conn->dev);
	return 0;
}
EXPORT_SYMBOL_GPL(iscsi_destroy_conn);

/*
 * iscsi interface functions
 */
static struct iscsi_internal *
iscsi_if_transport_lookup(struct iscsi_transport *tt)
{
	struct iscsi_internal *priv;
	unsigned long flags;

	spin_lock_irqsave(&iscsi_transport_lock, flags);
	list_for_each_entry(priv, &iscsi_transports, list) {
		if (tt == priv->iscsi_transport) {
			spin_unlock_irqrestore(&iscsi_transport_lock, flags);
			return priv;
		}
	}
	spin_unlock_irqrestore(&iscsi_transport_lock, flags);
	return NULL;
}

static int
iscsi_multicast_skb(struct sk_buff *skb, uint32_t group, gfp_t gfp)
{
	return nlmsg_multicast(nls, skb, 0, group, gfp);
}

int iscsi_recv_pdu(struct iscsi_cls_conn *conn, struct iscsi_hdr *hdr,
		   char *data, uint32_t data_size)
{
	struct nlmsghdr	*nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	char *pdu;
	struct iscsi_internal *priv;
	int len = NLMSG_SPACE(sizeof(*ev) + sizeof(struct iscsi_hdr) +
			      data_size);

	priv = iscsi_if_transport_lookup(conn->transport);
	if (!priv)
		return -EINVAL;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		iscsi_conn_error_event(conn, ISCSI_ERR_CONN_FAILED);
		iscsi_cls_conn_printk(KERN_ERR, conn, "can not deliver "
				      "control PDU: OOM\n");
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	memset(ev, 0, sizeof(*ev));
	ev->transport_handle = iscsi_handle(conn->transport);
	ev->type = ISCSI_KEVENT_RECV_PDU;
	ev->r.recv_req.cid = conn->cid;
	ev->r.recv_req.sid = iscsi_conn_get_sid(conn);
	pdu = (char*)ev + sizeof(*ev);
	memcpy(pdu, hdr, sizeof(struct iscsi_hdr));
	memcpy(pdu + sizeof(struct iscsi_hdr), data, data_size);

	return iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_ATOMIC);
}
EXPORT_SYMBOL_GPL(iscsi_recv_pdu);

int iscsi_offload_mesg(struct Scsi_Host *shost,
		       struct iscsi_transport *transport, uint32_t type,
		       char *data, uint16_t data_size)
{
	struct nlmsghdr	*nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	int len = NLMSG_SPACE(sizeof(*ev) + data_size);

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_ERR "can not deliver iscsi offload message:OOM\n");
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	memset(ev, 0, sizeof(*ev));
	ev->type = type;
	ev->transport_handle = iscsi_handle(transport);
	switch (type) {
	case ISCSI_KEVENT_PATH_REQ:
		ev->r.req_path.host_no = shost->host_no;
		break;
	case ISCSI_KEVENT_IF_DOWN:
		ev->r.notify_if_down.host_no = shost->host_no;
		break;
	}

	memcpy((char *)ev + sizeof(*ev), data, data_size);

	return iscsi_multicast_skb(skb, ISCSI_NL_GRP_UIP, GFP_ATOMIC);
}
EXPORT_SYMBOL_GPL(iscsi_offload_mesg);

void iscsi_conn_error_event(struct iscsi_cls_conn *conn, enum iscsi_err error)
{
	struct nlmsghdr	*nlh;
	struct sk_buff	*skb;
	struct iscsi_uevent *ev;
	struct iscsi_internal *priv;
	int len = NLMSG_SPACE(sizeof(*ev));

	priv = iscsi_if_transport_lookup(conn->transport);
	if (!priv)
		return;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		iscsi_cls_conn_printk(KERN_ERR, conn, "gracefully ignored "
				      "conn error (%d)\n", error);
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	ev->transport_handle = iscsi_handle(conn->transport);
	ev->type = ISCSI_KEVENT_CONN_ERROR;
	ev->r.connerror.error = error;
	ev->r.connerror.cid = conn->cid;
	ev->r.connerror.sid = iscsi_conn_get_sid(conn);

	iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_ATOMIC);

	iscsi_cls_conn_printk(KERN_INFO, conn, "detected conn error (%d)\n",
			      error);
}
EXPORT_SYMBOL_GPL(iscsi_conn_error_event);

void iscsi_conn_login_event(struct iscsi_cls_conn *conn,
			    enum iscsi_conn_state state)
{
	struct nlmsghdr *nlh;
	struct sk_buff  *skb;
	struct iscsi_uevent *ev;
	struct iscsi_internal *priv;
	int len = NLMSG_SPACE(sizeof(*ev));

	priv = iscsi_if_transport_lookup(conn->transport);
	if (!priv)
		return;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		iscsi_cls_conn_printk(KERN_ERR, conn, "gracefully ignored "
				      "conn login (%d)\n", state);
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	ev->transport_handle = iscsi_handle(conn->transport);
	ev->type = ISCSI_KEVENT_CONN_LOGIN_STATE;
	ev->r.conn_login.state = state;
	ev->r.conn_login.cid = conn->cid;
	ev->r.conn_login.sid = iscsi_conn_get_sid(conn);
	iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_ATOMIC);

	iscsi_cls_conn_printk(KERN_INFO, conn, "detected conn login (%d)\n",
			      state);
}
EXPORT_SYMBOL_GPL(iscsi_conn_login_event);

void iscsi_post_host_event(uint32_t host_no, struct iscsi_transport *transport,
			   enum iscsi_host_event_code code, uint32_t data_size,
			   uint8_t *data)
{
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	int len = NLMSG_SPACE(sizeof(*ev) + data_size);

	skb = alloc_skb(len, GFP_NOIO);
	if (!skb) {
		printk(KERN_ERR "gracefully ignored host event (%d):%d OOM\n",
		       host_no, code);
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	ev->transport_handle = iscsi_handle(transport);
	ev->type = ISCSI_KEVENT_HOST_EVENT;
	ev->r.host_event.host_no = host_no;
	ev->r.host_event.code = code;
	ev->r.host_event.data_size = data_size;

	if (data_size)
		memcpy((char *)ev + sizeof(*ev), data, data_size);

	iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_NOIO);
}
EXPORT_SYMBOL_GPL(iscsi_post_host_event);

void iscsi_ping_comp_event(uint32_t host_no, struct iscsi_transport *transport,
			   uint32_t status, uint32_t pid, uint32_t data_size,
			   uint8_t *data)
{
	struct nlmsghdr *nlh;
	struct sk_buff *skb;
	struct iscsi_uevent *ev;
	int len = NLMSG_SPACE(sizeof(*ev) + data_size);

	skb = alloc_skb(len, GFP_NOIO);
	if (!skb) {
		printk(KERN_ERR "gracefully ignored ping comp: OOM\n");
		return;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	ev->transport_handle = iscsi_handle(transport);
	ev->type = ISCSI_KEVENT_PING_COMP;
	ev->r.ping_comp.host_no = host_no;
	ev->r.ping_comp.status = status;
	ev->r.ping_comp.pid = pid;
	ev->r.ping_comp.data_size = data_size;
	memcpy((char *)ev + sizeof(*ev), data, data_size);

	iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_NOIO);
}
EXPORT_SYMBOL_GPL(iscsi_ping_comp_event);

static int
iscsi_if_send_reply(uint32_t group, int seq, int type, int done, int multi,
		    void *payload, int size)
{
	struct sk_buff	*skb;
	struct nlmsghdr	*nlh;
	int len = NLMSG_SPACE(size);
	int flags = multi ? NLM_F_MULTI : 0;
	int t = done ? NLMSG_DONE : type;

	skb = alloc_skb(len, GFP_ATOMIC);
	if (!skb) {
		printk(KERN_ERR "Could not allocate skb to send reply.\n");
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, t, (len - sizeof(*nlh)), 0);
	nlh->nlmsg_flags = flags;
	memcpy(NLMSG_DATA(nlh), payload, size);
	return iscsi_multicast_skb(skb, group, GFP_ATOMIC);
}

static int
iscsi_if_get_stats(struct iscsi_transport *transport, struct nlmsghdr *nlh)
{
	struct iscsi_uevent *ev = NLMSG_DATA(nlh);
	struct iscsi_stats *stats;
	struct sk_buff *skbstat;
	struct iscsi_cls_conn *conn;
	struct nlmsghdr	*nlhstat;
	struct iscsi_uevent *evstat;
	struct iscsi_internal *priv;
	int len = NLMSG_SPACE(sizeof(*ev) +
			      sizeof(struct iscsi_stats) +
			      sizeof(struct iscsi_stats_custom) *
			      ISCSI_STATS_CUSTOM_MAX);
	int err = 0;

	priv = iscsi_if_transport_lookup(transport);
	if (!priv)
		return -EINVAL;

	conn = iscsi_conn_lookup(ev->u.get_stats.sid, ev->u.get_stats.cid);
	if (!conn)
		return -EEXIST;

	do {
		int actual_size;

		skbstat = alloc_skb(len, GFP_ATOMIC);
		if (!skbstat) {
			iscsi_cls_conn_printk(KERN_ERR, conn, "can not "
					      "deliver stats: OOM\n");
			return -ENOMEM;
		}

		nlhstat = __nlmsg_put(skbstat, 0, 0, 0,
				      (len - sizeof(*nlhstat)), 0);
		evstat = NLMSG_DATA(nlhstat);
		memset(evstat, 0, sizeof(*evstat));
		evstat->transport_handle = iscsi_handle(conn->transport);
		evstat->type = nlh->nlmsg_type;
		evstat->u.get_stats.cid =
			ev->u.get_stats.cid;
		evstat->u.get_stats.sid =
			ev->u.get_stats.sid;
		stats = (struct iscsi_stats *)
			((char*)evstat + sizeof(*evstat));
		memset(stats, 0, sizeof(*stats));

		transport->get_stats(conn, stats);
		actual_size = NLMSG_SPACE(sizeof(struct iscsi_uevent) +
					  sizeof(struct iscsi_stats) +
					  sizeof(struct iscsi_stats_custom) *
					  stats->custom_length);
		actual_size -= sizeof(*nlhstat);
		actual_size = NLMSG_LENGTH(actual_size);
		skb_trim(skbstat, NLMSG_ALIGN(actual_size));
		nlhstat->nlmsg_len = actual_size;

		err = iscsi_multicast_skb(skbstat, ISCSI_NL_GRP_ISCSID,
					  GFP_ATOMIC);
	} while (err < 0 && err != -ECONNREFUSED);

	return err;
}

/**
 * iscsi_session_event - send session destr. completion event
 * @session: iscsi class session
 * @event: type of event
 */
int iscsi_session_event(struct iscsi_cls_session *session,
			enum iscsi_uevent_e event)
{
	struct iscsi_internal *priv;
	struct Scsi_Host *shost;
	struct iscsi_uevent *ev;
	struct sk_buff  *skb;
	struct nlmsghdr *nlh;
	int rc, len = NLMSG_SPACE(sizeof(*ev));

	priv = iscsi_if_transport_lookup(session->transport);
	if (!priv)
		return -EINVAL;
	shost = iscsi_session_to_shost(session);

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "Cannot notify userspace of session "
					 "event %u\n", event);
		return -ENOMEM;
	}

	nlh = __nlmsg_put(skb, 0, 0, 0, (len - sizeof(*nlh)), 0);
	ev = NLMSG_DATA(nlh);
	ev->transport_handle = iscsi_handle(session->transport);

	ev->type = event;
	switch (event) {
	case ISCSI_KEVENT_DESTROY_SESSION:
		ev->r.d_session.host_no = shost->host_no;
		ev->r.d_session.sid = session->sid;
		break;
	case ISCSI_KEVENT_CREATE_SESSION:
		ev->r.c_session_ret.host_no = shost->host_no;
		ev->r.c_session_ret.sid = session->sid;
		break;
	case ISCSI_KEVENT_UNBIND_SESSION:
		ev->r.unbind_session.host_no = shost->host_no;
		ev->r.unbind_session.sid = session->sid;
		break;
	default:
		iscsi_cls_session_printk(KERN_ERR, session, "Invalid event "
					 "%u.\n", event);
		kfree_skb(skb);
		return -EINVAL;
	}

	/*
	 * this will occur if the daemon is not up, so we just warn
	 * the user and when the daemon is restarted it will handle it
	 */
	rc = iscsi_multicast_skb(skb, ISCSI_NL_GRP_ISCSID, GFP_KERNEL);
	if (rc == -ESRCH)
		iscsi_cls_session_printk(KERN_ERR, session,
					 "Cannot notify userspace of session "
					 "event %u. Check iscsi daemon\n",
					 event);

	ISCSI_DBG_TRANS_SESSION(session, "Completed handling event %d rc %d\n",
				event, rc);
	return rc;
}
EXPORT_SYMBOL_GPL(iscsi_session_event);

static int
iscsi_if_create_session(struct iscsi_internal *priv, struct iscsi_endpoint *ep,
			struct iscsi_uevent *ev, pid_t pid,
			uint32_t initial_cmdsn,	uint16_t cmds_max,
			uint16_t queue_depth)
{
	struct iscsi_transport *transport = priv->iscsi_transport;
	struct iscsi_cls_session *session;
	struct Scsi_Host *shost;

	session = transport->create_session(ep, cmds_max, queue_depth,
					    initial_cmdsn);
	if (!session)
		return -ENOMEM;

	session->creator = pid;
	shost = iscsi_session_to_shost(session);
	ev->r.c_session_ret.host_no = shost->host_no;
	ev->r.c_session_ret.sid = session->sid;
	ISCSI_DBG_TRANS_SESSION(session,
				"Completed creating transport session\n");
	return 0;
}

static int
iscsi_if_create_conn(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct iscsi_cls_conn *conn;
	struct iscsi_cls_session *session;

	session = iscsi_session_lookup(ev->u.c_conn.sid);
	if (!session) {
		printk(KERN_ERR "iscsi: invalid session %d.\n",
		       ev->u.c_conn.sid);
		return -EINVAL;
	}

	conn = transport->create_conn(session, ev->u.c_conn.cid);
	if (!conn) {
		iscsi_cls_session_printk(KERN_ERR, session,
					 "couldn't create a new connection.");
		return -ENOMEM;
	}

	ev->r.c_conn_ret.sid = session->sid;
	ev->r.c_conn_ret.cid = conn->cid;

	ISCSI_DBG_TRANS_CONN(conn, "Completed creating transport conn\n");
	return 0;
}

static int
iscsi_if_destroy_conn(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct iscsi_cls_conn *conn;

	conn = iscsi_conn_lookup(ev->u.d_conn.sid, ev->u.d_conn.cid);
	if (!conn)
		return -EINVAL;

	ISCSI_DBG_TRANS_CONN(conn, "Destroying transport conn\n");
	if (transport->destroy_conn)
		transport->destroy_conn(conn);

	return 0;
}

static int
iscsi_set_param(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	char *data = (char*)ev + sizeof(*ev);
	struct iscsi_cls_conn *conn;
	struct iscsi_cls_session *session;
	int err = 0, value = 0;

	session = iscsi_session_lookup(ev->u.set_param.sid);
	conn = iscsi_conn_lookup(ev->u.set_param.sid, ev->u.set_param.cid);
	if (!conn || !session)
		return -EINVAL;

	switch (ev->u.set_param.param) {
	case ISCSI_PARAM_SESS_RECOVERY_TMO:
		sscanf(data, "%d", &value);
		session->recovery_tmo = value;
		break;
	default:
		err = transport->set_param(conn, ev->u.set_param.param,
					   data, ev->u.set_param.len);
	}

	return err;
}

static int iscsi_if_ep_connect(struct iscsi_transport *transport,
			       struct iscsi_uevent *ev, int msg_type)
{
	struct iscsi_endpoint *ep;
	struct sockaddr *dst_addr;
	struct Scsi_Host *shost = NULL;
	int non_blocking, err = 0;

	if (!transport->ep_connect)
		return -EINVAL;

	if (msg_type == ISCSI_UEVENT_TRANSPORT_EP_CONNECT_THROUGH_HOST) {
		shost = scsi_host_lookup(ev->u.ep_connect_through_host.host_no);
		if (!shost) {
			printk(KERN_ERR "ep connect failed. Could not find "
			       "host no %u\n",
			       ev->u.ep_connect_through_host.host_no);
			return -ENODEV;
		}
		non_blocking = ev->u.ep_connect_through_host.non_blocking;
	} else
		non_blocking = ev->u.ep_connect.non_blocking;

	dst_addr = (struct sockaddr *)((char*)ev + sizeof(*ev));
	ep = transport->ep_connect(shost, dst_addr, non_blocking);
	if (IS_ERR(ep)) {
		err = PTR_ERR(ep);
		goto release_host;
	}

	ev->r.ep_connect_ret.handle = ep->id;
release_host:
	if (shost)
		scsi_host_put(shost);
	return err;
}

static int iscsi_if_ep_disconnect(struct iscsi_transport *transport,
				  u64 ep_handle)
{
	struct iscsi_cls_conn *conn;
	struct iscsi_endpoint *ep;

	if (!transport->ep_disconnect)
		return -EINVAL;

	ep = iscsi_lookup_endpoint(ep_handle);
	if (!ep)
		return -EINVAL;
	conn = ep->conn;
	if (conn) {
		mutex_lock(&conn->ep_mutex);
		conn->ep = NULL;
		mutex_unlock(&conn->ep_mutex);
	}

	transport->ep_disconnect(ep);
	return 0;
}

static int
iscsi_if_transport_ep(struct iscsi_transport *transport,
		      struct iscsi_uevent *ev, int msg_type)
{
	struct iscsi_endpoint *ep;
	int rc = 0;

	switch (msg_type) {
	case ISCSI_UEVENT_TRANSPORT_EP_CONNECT_THROUGH_HOST:
	case ISCSI_UEVENT_TRANSPORT_EP_CONNECT:
		rc = iscsi_if_ep_connect(transport, ev, msg_type);
		break;
	case ISCSI_UEVENT_TRANSPORT_EP_POLL:
		if (!transport->ep_poll)
			return -EINVAL;

		ep = iscsi_lookup_endpoint(ev->u.ep_poll.ep_handle);
		if (!ep)
			return -EINVAL;

		ev->r.retcode = transport->ep_poll(ep,
						   ev->u.ep_poll.timeout_ms);
		break;
	case ISCSI_UEVENT_TRANSPORT_EP_DISCONNECT:
		rc = iscsi_if_ep_disconnect(transport,
					    ev->u.ep_disconnect.ep_handle);
		break;
	}
	return rc;
}

static int
iscsi_tgt_dscvr(struct iscsi_transport *transport,
		struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct sockaddr *dst_addr;
	int err;

	if (!transport->tgt_dscvr)
		return -EINVAL;

	shost = scsi_host_lookup(ev->u.tgt_dscvr.host_no);
	if (!shost) {
		printk(KERN_ERR "target discovery could not find host no %u\n",
		       ev->u.tgt_dscvr.host_no);
		return -ENODEV;
	}


	dst_addr = (struct sockaddr *)((char*)ev + sizeof(*ev));
	err = transport->tgt_dscvr(shost, ev->u.tgt_dscvr.type,
				   ev->u.tgt_dscvr.enable, dst_addr);
	scsi_host_put(shost);
	return err;
}

static int
iscsi_set_host_param(struct iscsi_transport *transport,
		     struct iscsi_uevent *ev)
{
	char *data = (char*)ev + sizeof(*ev);
	struct Scsi_Host *shost;
	int err;

	if (!transport->set_host_param)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.set_host_param.host_no);
	if (!shost) {
		printk(KERN_ERR "set_host_param could not find host no %u\n",
		       ev->u.set_host_param.host_no);
		return -ENODEV;
	}

	err = transport->set_host_param(shost, ev->u.set_host_param.param,
					data, ev->u.set_host_param.len);
	scsi_host_put(shost);
	return err;
}

static int
iscsi_set_path(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct iscsi_path *params;
	int err;

	if (!transport->set_path)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.set_path.host_no);
	if (!shost) {
		printk(KERN_ERR "set path could not find host no %u\n",
		       ev->u.set_path.host_no);
		return -ENODEV;
	}

	params = (struct iscsi_path *)((char *)ev + sizeof(*ev));
	err = transport->set_path(shost, params);

	scsi_host_put(shost);
	return err;
}

static int
iscsi_set_iface_params(struct iscsi_transport *transport,
		       struct iscsi_uevent *ev, uint32_t len)
{
	char *data = (char *)ev + sizeof(*ev);
	struct Scsi_Host *shost;
	int err;

	if (!transport->set_iface_param)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.set_iface_params.host_no);
	if (!shost) {
		printk(KERN_ERR "set_iface_params could not find host no %u\n",
		       ev->u.set_iface_params.host_no);
		return -ENODEV;
	}

	err = transport->set_iface_param(shost, data, len);
	scsi_host_put(shost);
	return err;
}

static int
iscsi_send_ping(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	struct sockaddr *dst_addr;
	int err;

	if (!transport->send_ping)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.iscsi_ping.host_no);
	if (!shost) {
		printk(KERN_ERR "iscsi_ping could not find host no %u\n",
		       ev->u.iscsi_ping.host_no);
		return -ENODEV;
	}

	dst_addr = (struct sockaddr *)((char *)ev + sizeof(*ev));
	err = transport->send_ping(shost, ev->u.iscsi_ping.iface_num,
				   ev->u.iscsi_ping.iface_type,
				   ev->u.iscsi_ping.payload_size,
				   ev->u.iscsi_ping.pid,
				   dst_addr);
	scsi_host_put(shost);
	return err;
}

static int
iscsi_get_chap(struct iscsi_transport *transport, struct nlmsghdr *nlh)
{
	struct iscsi_uevent *ev = NLMSG_DATA(nlh);
	struct Scsi_Host *shost = NULL;
	struct iscsi_chap_rec *chap_rec;
	struct iscsi_internal *priv;
	struct sk_buff *skbchap;
	struct nlmsghdr *nlhchap;
	struct iscsi_uevent *evchap;
	uint32_t chap_buf_size;
	int len, err = 0;
	char *buf;

	if (!transport->get_chap)
		return -EINVAL;

	priv = iscsi_if_transport_lookup(transport);
	if (!priv)
		return -EINVAL;

	chap_buf_size = (ev->u.get_chap.num_entries * sizeof(*chap_rec));
	len = NLMSG_SPACE(sizeof(*ev) + chap_buf_size);

	shost = scsi_host_lookup(ev->u.get_chap.host_no);
	if (!shost) {
		printk(KERN_ERR "%s: failed. Cound not find host no %u\n",
		       __func__, ev->u.get_chap.host_no);
		return -ENODEV;
	}

	do {
		int actual_size;

		skbchap = alloc_skb(len, GFP_KERNEL);
		if (!skbchap) {
			printk(KERN_ERR "can not deliver chap: OOM\n");
			err = -ENOMEM;
			goto exit_get_chap;
		}

		nlhchap = __nlmsg_put(skbchap, 0, 0, 0,
				      (len - sizeof(*nlhchap)), 0);
		evchap = NLMSG_DATA(nlhchap);
		memset(evchap, 0, sizeof(*evchap));
		evchap->transport_handle = iscsi_handle(transport);
		evchap->type = nlh->nlmsg_type;
		evchap->u.get_chap.host_no = ev->u.get_chap.host_no;
		evchap->u.get_chap.chap_tbl_idx = ev->u.get_chap.chap_tbl_idx;
		evchap->u.get_chap.num_entries = ev->u.get_chap.num_entries;
		buf = (char *) ((char *)evchap + sizeof(*evchap));
		memset(buf, 0, chap_buf_size);

		err = transport->get_chap(shost, ev->u.get_chap.chap_tbl_idx,
				    &evchap->u.get_chap.num_entries, buf);

		actual_size = NLMSG_SPACE(sizeof(*ev) + chap_buf_size);
		skb_trim(skbchap, NLMSG_ALIGN(actual_size));
		nlhchap->nlmsg_len = actual_size;

		err = iscsi_multicast_skb(skbchap, ISCSI_NL_GRP_ISCSID,
					  GFP_KERNEL);
	} while (err < 0 && err != -ECONNREFUSED);

exit_get_chap:
	scsi_host_put(shost);
	return err;
}

static int iscsi_delete_chap(struct iscsi_transport *transport,
			     struct iscsi_uevent *ev)
{
	struct Scsi_Host *shost;
	int err = 0;

	if (!transport->delete_chap)
		return -ENOSYS;

	shost = scsi_host_lookup(ev->u.delete_chap.host_no);
	if (!shost) {
		printk(KERN_ERR "%s could not find host no %u\n",
		       __func__, ev->u.delete_chap.host_no);
		return -ENODEV;
	}

	err = transport->delete_chap(shost, ev->u.delete_chap.chap_tbl_idx);
	scsi_host_put(shost);
	return err;
}

static int
iscsi_if_recv_msg(struct sk_buff *skb, struct nlmsghdr *nlh, uint32_t *group)
{
	int err = 0;
	struct iscsi_uevent *ev = NLMSG_DATA(nlh);
	struct iscsi_transport *transport = NULL;
	struct iscsi_internal *priv;
	struct iscsi_cls_session *session;
	struct iscsi_cls_conn *conn;
	struct iscsi_endpoint *ep = NULL;

	if (nlh->nlmsg_type == ISCSI_UEVENT_PATH_UPDATE)
		*group = ISCSI_NL_GRP_UIP;
	else
		*group = ISCSI_NL_GRP_ISCSID;

	priv = iscsi_if_transport_lookup(iscsi_ptr(ev->transport_handle));
	if (!priv)
		return -EINVAL;
	transport = priv->iscsi_transport;

	if (!try_module_get(transport->owner))
		return -EINVAL;

	switch (nlh->nlmsg_type) {
	case ISCSI_UEVENT_CREATE_SESSION:
		err = iscsi_if_create_session(priv, ep, ev,
					      NETLINK_CB(skb).pid,
					      ev->u.c_session.initial_cmdsn,
					      ev->u.c_session.cmds_max,
					      ev->u.c_session.queue_depth);
		break;
	case ISCSI_UEVENT_CREATE_BOUND_SESSION:
		ep = iscsi_lookup_endpoint(ev->u.c_bound_session.ep_handle);
		if (!ep) {
			err = -EINVAL;
			break;
		}

		err = iscsi_if_create_session(priv, ep, ev,
					NETLINK_CB(skb).pid,
					ev->u.c_bound_session.initial_cmdsn,
					ev->u.c_bound_session.cmds_max,
					ev->u.c_bound_session.queue_depth);
		break;
	case ISCSI_UEVENT_DESTROY_SESSION:
		session = iscsi_session_lookup(ev->u.d_session.sid);
		if (session)
			transport->destroy_session(session);
		else
			err = -EINVAL;
		break;
	case ISCSI_UEVENT_UNBIND_SESSION:
		session = iscsi_session_lookup(ev->u.d_session.sid);
		if (session)
			scsi_queue_work(iscsi_session_to_shost(session),
					&session->unbind_work);
		else
			err = -EINVAL;
		break;
	case ISCSI_UEVENT_CREATE_CONN:
		err = iscsi_if_create_conn(transport, ev);
		break;
	case ISCSI_UEVENT_DESTROY_CONN:
		err = iscsi_if_destroy_conn(transport, ev);
		break;
	case ISCSI_UEVENT_BIND_CONN:
		session = iscsi_session_lookup(ev->u.b_conn.sid);
		conn = iscsi_conn_lookup(ev->u.b_conn.sid, ev->u.b_conn.cid);

		if (conn && conn->ep)
			iscsi_if_ep_disconnect(transport, conn->ep->id);

		if (!session || !conn) {
			err = -EINVAL;
			break;
		}

		ev->r.retcode =	transport->bind_conn(session, conn,
						ev->u.b_conn.transport_eph,
						ev->u.b_conn.is_leading);
		if (ev->r.retcode || !transport->ep_connect)
			break;

		ep = iscsi_lookup_endpoint(ev->u.b_conn.transport_eph);
		if (ep) {
			ep->conn = conn;

			mutex_lock(&conn->ep_mutex);
			conn->ep = ep;
			mutex_unlock(&conn->ep_mutex);
		} else
			iscsi_cls_conn_printk(KERN_ERR, conn,
					      "Could not set ep conn "
					      "binding\n");
		break;
	case ISCSI_UEVENT_SET_PARAM:
		err = iscsi_set_param(transport, ev);
		break;
	case ISCSI_UEVENT_START_CONN:
		conn = iscsi_conn_lookup(ev->u.start_conn.sid, ev->u.start_conn.cid);
		if (conn)
			ev->r.retcode = transport->start_conn(conn);
		else
			err = -EINVAL;
		break;
	case ISCSI_UEVENT_STOP_CONN:
		conn = iscsi_conn_lookup(ev->u.stop_conn.sid, ev->u.stop_conn.cid);
		if (conn)
			transport->stop_conn(conn, ev->u.stop_conn.flag);
		else
			err = -EINVAL;
		break;
	case ISCSI_UEVENT_SEND_PDU:
		conn = iscsi_conn_lookup(ev->u.send_pdu.sid, ev->u.send_pdu.cid);
		if (conn)
			ev->r.retcode =	transport->send_pdu(conn,
				(struct iscsi_hdr*)((char*)ev + sizeof(*ev)),
				(char*)ev + sizeof(*ev) + ev->u.send_pdu.hdr_size,
				ev->u.send_pdu.data_size);
		else
			err = -EINVAL;
		break;
	case ISCSI_UEVENT_GET_STATS:
		err = iscsi_if_get_stats(transport, nlh);
		break;
	case ISCSI_UEVENT_TRANSPORT_EP_CONNECT:
	case ISCSI_UEVENT_TRANSPORT_EP_POLL:
	case ISCSI_UEVENT_TRANSPORT_EP_DISCONNECT:
	case ISCSI_UEVENT_TRANSPORT_EP_CONNECT_THROUGH_HOST:
		err = iscsi_if_transport_ep(transport, ev, nlh->nlmsg_type);
		break;
	case ISCSI_UEVENT_TGT_DSCVR:
		err = iscsi_tgt_dscvr(transport, ev);
		break;
	case ISCSI_UEVENT_SET_HOST_PARAM:
		err = iscsi_set_host_param(transport, ev);
		break;
	case ISCSI_UEVENT_PATH_UPDATE:
		err = iscsi_set_path(transport, ev);
		break;
	case ISCSI_UEVENT_SET_IFACE_PARAMS:
		err = iscsi_set_iface_params(transport, ev,
					     nlmsg_attrlen(nlh, sizeof(*ev)));
		break;
	case ISCSI_UEVENT_PING:
		err = iscsi_send_ping(transport, ev);
		break;
	case ISCSI_UEVENT_GET_CHAP:
		err = iscsi_get_chap(transport, nlh);
		break;
	case ISCSI_UEVENT_DELETE_CHAP:
		err = iscsi_delete_chap(transport, ev);
		break;
	default:
		err = -ENOSYS;
		break;
	}

	module_put(transport->owner);
	return err;
}

/*
 * Get message from skb.  Each message is processed by iscsi_if_recv_msg.
 * Malformed skbs with wrong lengths or invalid creds are not processed.
 */
static void
iscsi_if_rx(struct sk_buff *skb)
{
	mutex_lock(&rx_queue_mutex);
	while (skb->len >= NLMSG_SPACE(0)) {
		int err;
		uint32_t rlen;
		struct nlmsghdr	*nlh;
		struct iscsi_uevent *ev;
		uint32_t group;

		nlh = nlmsg_hdr(skb);
		if (nlh->nlmsg_len < sizeof(*nlh) ||
		    skb->len < nlh->nlmsg_len) {
			break;
		}

		ev = NLMSG_DATA(nlh);
		rlen = NLMSG_ALIGN(nlh->nlmsg_len);
		if (rlen > skb->len)
			rlen = skb->len;

		err = iscsi_if_recv_msg(skb, nlh, &group);
		if (err) {
			ev->type = ISCSI_KEVENT_IF_ERROR;
			ev->iferror = err;
		}
		do {
			/*
			 * special case for GET_STATS:
			 * on success - sending reply and stats from
			 * inside of if_recv_msg(),
			 * on error - fall through.
			 */
			if (ev->type == ISCSI_UEVENT_GET_STATS && !err)
				break;
			if (ev->type == ISCSI_UEVENT_GET_CHAP && !err)
				break;
			err = iscsi_if_send_reply(group, nlh->nlmsg_seq,
				nlh->nlmsg_type, 0, 0, ev, sizeof(*ev));
		} while (err < 0 && err != -ECONNREFUSED && err != -ESRCH);
		skb_pull(skb, rlen);
	}
	mutex_unlock(&rx_queue_mutex);
}

#define ISCSI_CLASS_ATTR(_prefix,_name,_mode,_show,_store)		\
struct device_attribute dev_attr_##_prefix##_##_name =	\
	__ATTR(_name,_mode,_show,_store)

/*
 * iSCSI connection attrs
 */
#define iscsi_conn_attr_show(param)					\
static ssize_t								\
show_conn_param_##param(struct device *dev, 				\
			struct device_attribute *attr, char *buf)	\
{									\
	struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev->parent);	\
	struct iscsi_transport *t = conn->transport;			\
	return t->get_conn_param(conn, param, buf);			\
}

#define iscsi_conn_attr(field, param)					\
	iscsi_conn_attr_show(param)					\
static ISCSI_CLASS_ATTR(conn, field, S_IRUGO, show_conn_param_##param,	\
			NULL);

iscsi_conn_attr(max_recv_dlength, ISCSI_PARAM_MAX_RECV_DLENGTH);
iscsi_conn_attr(max_xmit_dlength, ISCSI_PARAM_MAX_XMIT_DLENGTH);
iscsi_conn_attr(header_digest, ISCSI_PARAM_HDRDGST_EN);
iscsi_conn_attr(data_digest, ISCSI_PARAM_DATADGST_EN);
iscsi_conn_attr(ifmarker, ISCSI_PARAM_IFMARKER_EN);
iscsi_conn_attr(ofmarker, ISCSI_PARAM_OFMARKER_EN);
iscsi_conn_attr(persistent_port, ISCSI_PARAM_PERSISTENT_PORT);
iscsi_conn_attr(exp_statsn, ISCSI_PARAM_EXP_STATSN);
iscsi_conn_attr(persistent_address, ISCSI_PARAM_PERSISTENT_ADDRESS);
iscsi_conn_attr(ping_tmo, ISCSI_PARAM_PING_TMO);
iscsi_conn_attr(recv_tmo, ISCSI_PARAM_RECV_TMO);

#define iscsi_conn_ep_attr_show(param)					\
static ssize_t show_conn_ep_param_##param(struct device *dev,		\
					  struct device_attribute *attr,\
					  char *buf)			\
{									\
	struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev->parent);	\
	struct iscsi_transport *t = conn->transport;			\
	struct iscsi_endpoint *ep;					\
	ssize_t rc;							\
									\
	/*								\
	 * Need to make sure ep_disconnect does not free the LLD's	\
	 * interconnect resources while we are trying to read them.	\
	 */								\
	mutex_lock(&conn->ep_mutex);					\
	ep = conn->ep;							\
	if (!ep && t->ep_connect) {					\
		mutex_unlock(&conn->ep_mutex);				\
		return -ENOTCONN;					\
	}								\
									\
	if (ep)								\
		rc = t->get_ep_param(ep, param, buf);			\
	else								\
		rc = t->get_conn_param(conn, param, buf);		\
	mutex_unlock(&conn->ep_mutex);					\
	return rc;							\
}

#define iscsi_conn_ep_attr(field, param)				\
	iscsi_conn_ep_attr_show(param)					\
static ISCSI_CLASS_ATTR(conn, field, S_IRUGO,				\
			show_conn_ep_param_##param, NULL);

iscsi_conn_ep_attr(address, ISCSI_PARAM_CONN_ADDRESS);
iscsi_conn_ep_attr(port, ISCSI_PARAM_CONN_PORT);

static struct attribute *iscsi_conn_attrs[] = {
	&dev_attr_conn_max_recv_dlength.attr,
	&dev_attr_conn_max_xmit_dlength.attr,
	&dev_attr_conn_header_digest.attr,
	&dev_attr_conn_data_digest.attr,
	&dev_attr_conn_ifmarker.attr,
	&dev_attr_conn_ofmarker.attr,
	&dev_attr_conn_address.attr,
	&dev_attr_conn_port.attr,
	&dev_attr_conn_exp_statsn.attr,
	&dev_attr_conn_persistent_address.attr,
	&dev_attr_conn_persistent_port.attr,
	&dev_attr_conn_ping_tmo.attr,
	&dev_attr_conn_recv_tmo.attr,
	NULL,
};

static umode_t iscsi_conn_attr_is_visible(struct kobject *kobj,
					 struct attribute *attr, int i)
{
	struct device *cdev = container_of(kobj, struct device, kobj);
	struct iscsi_cls_conn *conn = transport_class_to_conn(cdev);
	struct iscsi_transport *t = conn->transport;
	int param;

	if (attr == &dev_attr_conn_max_recv_dlength.attr)
		param = ISCSI_PARAM_MAX_RECV_DLENGTH;
	else if (attr == &dev_attr_conn_max_xmit_dlength.attr)
		param = ISCSI_PARAM_MAX_XMIT_DLENGTH;
	else if (attr == &dev_attr_conn_header_digest.attr)
		param = ISCSI_PARAM_HDRDGST_EN;
	else if (attr == &dev_attr_conn_data_digest.attr)
		param = ISCSI_PARAM_DATADGST_EN;
	else if (attr == &dev_attr_conn_ifmarker.attr)
		param = ISCSI_PARAM_IFMARKER_EN;
	else if (attr == &dev_attr_conn_ofmarker.attr)
		param = ISCSI_PARAM_OFMARKER_EN;
	else if (attr == &dev_attr_conn_address.attr)
		param = ISCSI_PARAM_CONN_ADDRESS;
	else if (attr == &dev_attr_conn_port.attr)
		param = ISCSI_PARAM_CONN_PORT;
	else if (attr == &dev_attr_conn_exp_statsn.attr)
		param = ISCSI_PARAM_EXP_STATSN;
	else if (attr == &dev_attr_conn_persistent_address.attr)
		param = ISCSI_PARAM_PERSISTENT_ADDRESS;
	else if (attr == &dev_attr_conn_persistent_port.attr)
		param = ISCSI_PARAM_PERSISTENT_PORT;
	else if (attr == &dev_attr_conn_ping_tmo.attr)
		param = ISCSI_PARAM_PING_TMO;
	else if (attr == &dev_attr_conn_recv_tmo.attr)
		param = ISCSI_PARAM_RECV_TMO;
	else {
		WARN_ONCE(1, "Invalid conn attr");
		return 0;
	}

	return t->attr_is_visible(ISCSI_PARAM, param);
}

static struct attribute_group iscsi_conn_group = {
	.attrs = iscsi_conn_attrs,
	.is_visible = iscsi_conn_attr_is_visible,
};

/*
 * iSCSI session attrs
 */
#define iscsi_session_attr_show(param, perm)				\
static ssize_t								\
show_session_param_##param(struct device *dev,				\
			   struct device_attribute *attr, char *buf)	\
{									\
	struct iscsi_cls_session *session = 				\
		iscsi_dev_to_session(dev->parent);			\
	struct iscsi_transport *t = session->transport;			\
									\
	if (perm && !capable(CAP_SYS_ADMIN))				\
		return -EACCES;						\
	return t->get_session_param(session, param, buf);		\
}

#define iscsi_session_attr(field, param, perm)				\
	iscsi_session_attr_show(param, perm)				\
static ISCSI_CLASS_ATTR(sess, field, S_IRUGO, show_session_param_##param, \
			NULL);
iscsi_session_attr(targetname, ISCSI_PARAM_TARGET_NAME, 0);
iscsi_session_attr(initial_r2t, ISCSI_PARAM_INITIAL_R2T_EN, 0);
iscsi_session_attr(max_outstanding_r2t, ISCSI_PARAM_MAX_R2T, 0);
iscsi_session_attr(immediate_data, ISCSI_PARAM_IMM_DATA_EN, 0);
iscsi_session_attr(first_burst_len, ISCSI_PARAM_FIRST_BURST, 0);
iscsi_session_attr(max_burst_len, ISCSI_PARAM_MAX_BURST, 0);
iscsi_session_attr(data_pdu_in_order, ISCSI_PARAM_PDU_INORDER_EN, 0);
iscsi_session_attr(data_seq_in_order, ISCSI_PARAM_DATASEQ_INORDER_EN, 0);
iscsi_session_attr(erl, ISCSI_PARAM_ERL, 0);
iscsi_session_attr(tpgt, ISCSI_PARAM_TPGT, 0);
iscsi_session_attr(username, ISCSI_PARAM_USERNAME, 1);
iscsi_session_attr(username_in, ISCSI_PARAM_USERNAME_IN, 1);
iscsi_session_attr(password, ISCSI_PARAM_PASSWORD, 1);
iscsi_session_attr(password_in, ISCSI_PARAM_PASSWORD_IN, 1);
iscsi_session_attr(chap_out_idx, ISCSI_PARAM_CHAP_OUT_IDX, 1);
iscsi_session_attr(chap_in_idx, ISCSI_PARAM_CHAP_IN_IDX, 1);
iscsi_session_attr(fast_abort, ISCSI_PARAM_FAST_ABORT, 0);
iscsi_session_attr(abort_tmo, ISCSI_PARAM_ABORT_TMO, 0);
iscsi_session_attr(lu_reset_tmo, ISCSI_PARAM_LU_RESET_TMO, 0);
iscsi_session_attr(tgt_reset_tmo, ISCSI_PARAM_TGT_RESET_TMO, 0);
iscsi_session_attr(ifacename, ISCSI_PARAM_IFACE_NAME, 0);
iscsi_session_attr(initiatorname, ISCSI_PARAM_INITIATOR_NAME, 0);
iscsi_session_attr(targetalias, ISCSI_PARAM_TARGET_ALIAS, 0);

static ssize_t
show_priv_session_state(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct iscsi_cls_session *session = iscsi_dev_to_session(dev->parent);
	return sprintf(buf, "%s\n", iscsi_session_state_name(session->state));
}
static ISCSI_CLASS_ATTR(priv_sess, state, S_IRUGO, show_priv_session_state,
			NULL);
static ssize_t
show_priv_session_creator(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct iscsi_cls_session *session = iscsi_dev_to_session(dev->parent);
	return sprintf(buf, "%d\n", session->creator);
}
static ISCSI_CLASS_ATTR(priv_sess, creator, S_IRUGO, show_priv_session_creator,
			NULL);

#define iscsi_priv_session_attr_show(field, format)			\
static ssize_t								\
show_priv_session_##field(struct device *dev, 				\
			  struct device_attribute *attr, char *buf)	\
{									\
	struct iscsi_cls_session *session = 				\
			iscsi_dev_to_session(dev->parent);		\
	if (session->field == -1)					\
		return sprintf(buf, "off\n");				\
	return sprintf(buf, format"\n", session->field);		\
}

#define iscsi_priv_session_attr_store(field)				\
static ssize_t								\
store_priv_session_##field(struct device *dev,				\
			   struct device_attribute *attr,		\
			   const char *buf, size_t count)		\
{									\
	int val;							\
	char *cp;							\
	struct iscsi_cls_session *session =				\
		iscsi_dev_to_session(dev->parent);			\
	if ((session->state == ISCSI_SESSION_FREE) ||			\
	    (session->state == ISCSI_SESSION_FAILED))			\
		return -EBUSY;						\
	if (strncmp(buf, "off", 3) == 0)				\
		session->field = -1;					\
	else {								\
		val = simple_strtoul(buf, &cp, 0);			\
		if (*cp != '\0' && *cp != '\n')				\
			return -EINVAL;					\
		session->field = val;					\
	}								\
	return count;							\
}

#define iscsi_priv_session_rw_attr(field, format)			\
	iscsi_priv_session_attr_show(field, format)			\
	iscsi_priv_session_attr_store(field)				\
static ISCSI_CLASS_ATTR(priv_sess, field, S_IRUGO | S_IWUSR,		\
			show_priv_session_##field,			\
			store_priv_session_##field)
iscsi_priv_session_rw_attr(recovery_tmo, "%d");

static struct attribute *iscsi_session_attrs[] = {
	&dev_attr_sess_initial_r2t.attr,
	&dev_attr_sess_max_outstanding_r2t.attr,
	&dev_attr_sess_immediate_data.attr,
	&dev_attr_sess_first_burst_len.attr,
	&dev_attr_sess_max_burst_len.attr,
	&dev_attr_sess_data_pdu_in_order.attr,
	&dev_attr_sess_data_seq_in_order.attr,
	&dev_attr_sess_erl.attr,
	&dev_attr_sess_targetname.attr,
	&dev_attr_sess_tpgt.attr,
	&dev_attr_sess_password.attr,
	&dev_attr_sess_password_in.attr,
	&dev_attr_sess_username.attr,
	&dev_attr_sess_username_in.attr,
	&dev_attr_sess_fast_abort.attr,
	&dev_attr_sess_abort_tmo.attr,
	&dev_attr_sess_lu_reset_tmo.attr,
	&dev_attr_sess_tgt_reset_tmo.attr,
	&dev_attr_sess_ifacename.attr,
	&dev_attr_sess_initiatorname.attr,
	&dev_attr_sess_targetalias.attr,
	&dev_attr_priv_sess_recovery_tmo.attr,
	&dev_attr_priv_sess_state.attr,
	&dev_attr_priv_sess_creator.attr,
	&dev_attr_sess_chap_out_idx.attr,
	&dev_attr_sess_chap_in_idx.attr,
	NULL,
};

static umode_t iscsi_session_attr_is_visible(struct kobject *kobj,
					    struct attribute *attr, int i)
{
	struct device *cdev = container_of(kobj, struct device, kobj);
	struct iscsi_cls_session *session = transport_class_to_session(cdev);
	struct iscsi_transport *t = session->transport;
	int param;

	if (attr == &dev_attr_sess_initial_r2t.attr)
		param = ISCSI_PARAM_INITIAL_R2T_EN;
	else if (attr == &dev_attr_sess_max_outstanding_r2t.attr)
		param = ISCSI_PARAM_MAX_R2T;
	else if (attr == &dev_attr_sess_immediate_data.attr)
		param = ISCSI_PARAM_IMM_DATA_EN;
	else if (attr == &dev_attr_sess_first_burst_len.attr)
		param = ISCSI_PARAM_FIRST_BURST;
	else if (attr == &dev_attr_sess_max_burst_len.attr)
		param = ISCSI_PARAM_MAX_BURST;
	else if (attr == &dev_attr_sess_data_pdu_in_order.attr)
		param = ISCSI_PARAM_PDU_INORDER_EN;
	else if (attr == &dev_attr_sess_data_seq_in_order.attr)
		param = ISCSI_PARAM_DATASEQ_INORDER_EN;
	else if (attr == &dev_attr_sess_erl.attr)
		param = ISCSI_PARAM_ERL;
	else if (attr == &dev_attr_sess_targetname.attr)
		param = ISCSI_PARAM_TARGET_NAME;
	else if (attr == &dev_attr_sess_tpgt.attr)
		param = ISCSI_PARAM_TPGT;
	else if (attr == &dev_attr_sess_chap_in_idx.attr)
		param = ISCSI_PARAM_CHAP_IN_IDX;
	else if (attr == &dev_attr_sess_chap_out_idx.attr)
		param = ISCSI_PARAM_CHAP_OUT_IDX;
	else if (attr == &dev_attr_sess_password.attr)
		param = ISCSI_PARAM_USERNAME;
	else if (attr == &dev_attr_sess_password_in.attr)
		param = ISCSI_PARAM_USERNAME_IN;
	else if (attr == &dev_attr_sess_username.attr)
		param = ISCSI_PARAM_PASSWORD;
	else if (attr == &dev_attr_sess_username_in.attr)
		param = ISCSI_PARAM_PASSWORD_IN;
	else if (attr == &dev_attr_sess_fast_abort.attr)
		param = ISCSI_PARAM_FAST_ABORT;
	else if (attr == &dev_attr_sess_abort_tmo.attr)
		param = ISCSI_PARAM_ABORT_TMO;
	else if (attr == &dev_attr_sess_lu_reset_tmo.attr)
		param = ISCSI_PARAM_LU_RESET_TMO;
	else if (attr == &dev_attr_sess_tgt_reset_tmo.attr)
		param = ISCSI_PARAM_TGT_RESET_TMO;
	else if (attr == &dev_attr_sess_ifacename.attr)
		param = ISCSI_PARAM_IFACE_NAME;
	else if (attr == &dev_attr_sess_initiatorname.attr)
		param = ISCSI_PARAM_INITIATOR_NAME;
	else if (attr == &dev_attr_sess_targetalias.attr)
		param = ISCSI_PARAM_TARGET_ALIAS;
	else if (attr == &dev_attr_priv_sess_recovery_tmo.attr)
		return S_IRUGO | S_IWUSR;
	else if (attr == &dev_attr_priv_sess_state.attr)
		return S_IRUGO;
	else if (attr == &dev_attr_priv_sess_creator.attr)
		return S_IRUGO;
	else {
		WARN_ONCE(1, "Invalid session attr");
		return 0;
	}

	return t->attr_is_visible(ISCSI_PARAM, param);
}

static struct attribute_group iscsi_session_group = {
	.attrs = iscsi_session_attrs,
	.is_visible = iscsi_session_attr_is_visible,
};

/*
 * iSCSI host attrs
 */
#define iscsi_host_attr_show(param)					\
static ssize_t								\
show_host_param_##param(struct device *dev, 				\
			struct device_attribute *attr, char *buf)	\
{									\
	struct Scsi_Host *shost = transport_class_to_shost(dev);	\
	struct iscsi_internal *priv = to_iscsi_internal(shost->transportt); \
	return priv->iscsi_transport->get_host_param(shost, param, buf); \
}

#define iscsi_host_attr(field, param)					\
	iscsi_host_attr_show(param)					\
static ISCSI_CLASS_ATTR(host, field, S_IRUGO, show_host_param_##param,	\
			NULL);

iscsi_host_attr(netdev, ISCSI_HOST_PARAM_NETDEV_NAME);
iscsi_host_attr(hwaddress, ISCSI_HOST_PARAM_HWADDRESS);
iscsi_host_attr(ipaddress, ISCSI_HOST_PARAM_IPADDRESS);
iscsi_host_attr(initiatorname, ISCSI_HOST_PARAM_INITIATOR_NAME);
iscsi_host_attr(port_state, ISCSI_HOST_PARAM_PORT_STATE);
iscsi_host_attr(port_speed, ISCSI_HOST_PARAM_PORT_SPEED);

static struct attribute *iscsi_host_attrs[] = {
	&dev_attr_host_netdev.attr,
	&dev_attr_host_hwaddress.attr,
	&dev_attr_host_ipaddress.attr,
	&dev_attr_host_initiatorname.attr,
	&dev_attr_host_port_state.attr,
	&dev_attr_host_port_speed.attr,
	NULL,
};

static umode_t iscsi_host_attr_is_visible(struct kobject *kobj,
					 struct attribute *attr, int i)
{
	struct device *cdev = container_of(kobj, struct device, kobj);
	struct Scsi_Host *shost = transport_class_to_shost(cdev);
	struct iscsi_internal *priv = to_iscsi_internal(shost->transportt);
	int param;

	if (attr == &dev_attr_host_netdev.attr)
		param = ISCSI_HOST_PARAM_NETDEV_NAME;
	else if (attr == &dev_attr_host_hwaddress.attr)
		param = ISCSI_HOST_PARAM_HWADDRESS;
	else if (attr == &dev_attr_host_ipaddress.attr)
		param = ISCSI_HOST_PARAM_IPADDRESS;
	else if (attr == &dev_attr_host_initiatorname.attr)
		param = ISCSI_HOST_PARAM_INITIATOR_NAME;
	else if (attr == &dev_attr_host_port_state.attr)
		param = ISCSI_HOST_PARAM_PORT_STATE;
	else if (attr == &dev_attr_host_port_speed.attr)
		param = ISCSI_HOST_PARAM_PORT_SPEED;
	else {
		WARN_ONCE(1, "Invalid host attr");
		return 0;
	}

	return priv->iscsi_transport->attr_is_visible(ISCSI_HOST_PARAM, param);
}

static struct attribute_group iscsi_host_group = {
	.attrs = iscsi_host_attrs,
	.is_visible = iscsi_host_attr_is_visible,
};

/* convert iscsi_port_speed values to ascii string name */
static const struct {
	enum iscsi_port_speed	value;
	char			*name;
} iscsi_port_speed_names[] = {
	{ISCSI_PORT_SPEED_UNKNOWN,	"Unknown" },
	{ISCSI_PORT_SPEED_10MBPS,	"10 Mbps" },
	{ISCSI_PORT_SPEED_100MBPS,	"100 Mbps" },
	{ISCSI_PORT_SPEED_1GBPS,	"1 Gbps" },
	{ISCSI_PORT_SPEED_10GBPS,	"10 Gbps" },
};

char *iscsi_get_port_speed_name(struct Scsi_Host *shost)
{
	int i;
	char *speed = "Unknown!";
	struct iscsi_cls_host *ihost = shost->shost_data;
	uint32_t port_speed = ihost->port_speed;

	for (i = 0; i < ARRAY_SIZE(iscsi_port_speed_names); i++) {
		if (iscsi_port_speed_names[i].value & port_speed) {
			speed = iscsi_port_speed_names[i].name;
			break;
		}
	}
	return speed;
}
EXPORT_SYMBOL_GPL(iscsi_get_port_speed_name);

/* convert iscsi_port_state values to ascii string name */
static const struct {
	enum iscsi_port_state	value;
	char			*name;
} iscsi_port_state_names[] = {
	{ISCSI_PORT_STATE_DOWN,		"LINK DOWN" },
	{ISCSI_PORT_STATE_UP,		"LINK UP" },
};

char *iscsi_get_port_state_name(struct Scsi_Host *shost)
{
	int i;
	char *state = "Unknown!";
	struct iscsi_cls_host *ihost = shost->shost_data;
	uint32_t port_state = ihost->port_state;

	for (i = 0; i < ARRAY_SIZE(iscsi_port_state_names); i++) {
		if (iscsi_port_state_names[i].value & port_state) {
			state = iscsi_port_state_names[i].name;
			break;
		}
	}
	return state;
}
EXPORT_SYMBOL_GPL(iscsi_get_port_state_name);

static int iscsi_session_match(struct attribute_container *cont,
			   struct device *dev)
{
	struct iscsi_cls_session *session;
	struct Scsi_Host *shost;
	struct iscsi_internal *priv;

	if (!iscsi_is_session_dev(dev))
		return 0;

	session = iscsi_dev_to_session(dev);
	shost = iscsi_session_to_shost(session);
	if (!shost->transportt)
		return 0;

	priv = to_iscsi_internal(shost->transportt);
	if (priv->session_cont.ac.class != &iscsi_session_class.class)
		return 0;

	return &priv->session_cont.ac == cont;
}

static int iscsi_conn_match(struct attribute_container *cont,
			   struct device *dev)
{
	struct iscsi_cls_session *session;
	struct iscsi_cls_conn *conn;
	struct Scsi_Host *shost;
	struct iscsi_internal *priv;

	if (!iscsi_is_conn_dev(dev))
		return 0;

	conn = iscsi_dev_to_conn(dev);
	session = iscsi_dev_to_session(conn->dev.parent);
	shost = iscsi_session_to_shost(session);

	if (!shost->transportt)
		return 0;

	priv = to_iscsi_internal(shost->transportt);
	if (priv->conn_cont.ac.class != &iscsi_connection_class.class)
		return 0;

	return &priv->conn_cont.ac == cont;
}

static int iscsi_host_match(struct attribute_container *cont,
			    struct device *dev)
{
	struct Scsi_Host *shost;
	struct iscsi_internal *priv;

	if (!scsi_is_host_device(dev))
		return 0;

	shost = dev_to_shost(dev);
	if (!shost->transportt  ||
	    shost->transportt->host_attrs.ac.class != &iscsi_host_class.class)
		return 0;

        priv = to_iscsi_internal(shost->transportt);
        return &priv->t.host_attrs.ac == cont;
}

struct scsi_transport_template *
iscsi_register_transport(struct iscsi_transport *tt)
{
	struct iscsi_internal *priv;
	unsigned long flags;
	int err;

	BUG_ON(!tt);

	priv = iscsi_if_transport_lookup(tt);
	if (priv)
		return NULL;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return NULL;
	INIT_LIST_HEAD(&priv->list);
	priv->iscsi_transport = tt;
	priv->t.user_scan = iscsi_user_scan;
	priv->t.create_work_queue = 1;

	priv->dev.class = &iscsi_transport_class;
	dev_set_name(&priv->dev, "%s", tt->name);
	err = device_register(&priv->dev);
	if (err)
		goto free_priv;

	err = sysfs_create_group(&priv->dev.kobj, &iscsi_transport_group);
	if (err)
		goto unregister_dev;

	/* host parameters */
	priv->t.host_attrs.ac.class = &iscsi_host_class.class;
	priv->t.host_attrs.ac.match = iscsi_host_match;
	priv->t.host_attrs.ac.grp = &iscsi_host_group;
	priv->t.host_size = sizeof(struct iscsi_cls_host);
	transport_container_register(&priv->t.host_attrs);

	/* connection parameters */
	priv->conn_cont.ac.class = &iscsi_connection_class.class;
	priv->conn_cont.ac.match = iscsi_conn_match;
	priv->conn_cont.ac.grp = &iscsi_conn_group;
	transport_container_register(&priv->conn_cont);

	/* session parameters */
	priv->session_cont.ac.class = &iscsi_session_class.class;
	priv->session_cont.ac.match = iscsi_session_match;
	priv->session_cont.ac.grp = &iscsi_session_group;
	transport_container_register(&priv->session_cont);

	spin_lock_irqsave(&iscsi_transport_lock, flags);
	list_add(&priv->list, &iscsi_transports);
	spin_unlock_irqrestore(&iscsi_transport_lock, flags);

	printk(KERN_NOTICE "iscsi: registered transport (%s)\n", tt->name);
	return &priv->t;

unregister_dev:
	device_unregister(&priv->dev);
	return NULL;
free_priv:
	kfree(priv);
	return NULL;
}
EXPORT_SYMBOL_GPL(iscsi_register_transport);

int iscsi_unregister_transport(struct iscsi_transport *tt)
{
	struct iscsi_internal *priv;
	unsigned long flags;

	BUG_ON(!tt);

	mutex_lock(&rx_queue_mutex);

	priv = iscsi_if_transport_lookup(tt);
	BUG_ON (!priv);

	spin_lock_irqsave(&iscsi_transport_lock, flags);
	list_del(&priv->list);
	spin_unlock_irqrestore(&iscsi_transport_lock, flags);

	transport_container_unregister(&priv->conn_cont);
	transport_container_unregister(&priv->session_cont);
	transport_container_unregister(&priv->t.host_attrs);

	sysfs_remove_group(&priv->dev.kobj, &iscsi_transport_group);
	device_unregister(&priv->dev);
	mutex_unlock(&rx_queue_mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(iscsi_unregister_transport);

static __init int iscsi_transport_init(void)
{
	int err;

	printk(KERN_INFO "Loading iSCSI transport class v%s.\n",
		ISCSI_TRANSPORT_VERSION);

	atomic_set(&iscsi_session_nr, 0);

	err = class_register(&iscsi_transport_class);
	if (err)
		return err;

	err = class_register(&iscsi_endpoint_class);
	if (err)
		goto unregister_transport_class;

	err = class_register(&iscsi_iface_class);
	if (err)
		goto unregister_endpoint_class;

	err = transport_class_register(&iscsi_host_class);
	if (err)
		goto unregister_iface_class;

	err = transport_class_register(&iscsi_connection_class);
	if (err)
		goto unregister_host_class;

	err = transport_class_register(&iscsi_session_class);
	if (err)
		goto unregister_conn_class;

	nls = netlink_kernel_create(&init_net, NETLINK_ISCSI, 1, iscsi_if_rx,
				    NULL, THIS_MODULE);
	if (!nls) {
		err = -ENOBUFS;
		goto unregister_session_class;
	}

	iscsi_eh_timer_workq = create_singlethread_workqueue("iscsi_eh");
	if (!iscsi_eh_timer_workq)
		goto release_nls;

	return 0;

release_nls:
	netlink_kernel_release(nls);
unregister_session_class:
	transport_class_unregister(&iscsi_session_class);
unregister_conn_class:
	transport_class_unregister(&iscsi_connection_class);
unregister_host_class:
	transport_class_unregister(&iscsi_host_class);
unregister_iface_class:
	class_unregister(&iscsi_iface_class);
unregister_endpoint_class:
	class_unregister(&iscsi_endpoint_class);
unregister_transport_class:
	class_unregister(&iscsi_transport_class);
	return err;
}

static void __exit iscsi_transport_exit(void)
{
	destroy_workqueue(iscsi_eh_timer_workq);
	netlink_kernel_release(nls);
	transport_class_unregister(&iscsi_connection_class);
	transport_class_unregister(&iscsi_session_class);
	transport_class_unregister(&iscsi_host_class);
	class_unregister(&iscsi_endpoint_class);
	class_unregister(&iscsi_iface_class);
	class_unregister(&iscsi_transport_class);
}

module_init(iscsi_transport_init);
module_exit(iscsi_transport_exit);

MODULE_AUTHOR("Mike Christie <michaelc@cs.wisc.edu>, "
	      "Dmitry Yusupov <dmitry_yus@yahoo.com>, "
	      "Alex Aizman <itn780@yahoo.com>");
MODULE_DESCRIPTION("iSCSI Transport Interface");
MODULE_LICENSE("GPL");
MODULE_VERSION(ISCSI_TRANSPORT_VERSION);
MODULE_ALIAS_NET_PF_PROTO(PF_NETLINK, NETLINK_ISCSI);
