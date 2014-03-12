/*
 * Copyright 2008+ Evgeniy Polyakov <zbr@ioremap.net>
 *
 * This file is part of Elliptics.
 *
 * Elliptics is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Elliptics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Elliptics.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __DNET_PACKET_H
#define __DNET_PACKET_H

#ifndef __KERNEL__
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include <string.h>

#include <elliptics/typedefs.h>
#include <elliptics/core.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

enum dnet_commands {
	DNET_CMD_LOOKUP = 1,			/* Lookup address by ID and per-object info: size, permissions and so on*/
	DNET_CMD_REVERSE_LOOKUP,		/* Lookup ID by address */
	DNET_CMD_JOIN,				/* Join the network - force remote nodes to update
						 * their route tables to include given node with given
						 * address
						 */
	DNET_CMD_WRITE,
	DNET_CMD_READ,				/* IO commands. They have to follow by the
						 * IO attribute which will have offset and size
						 * parameters.
						 */
	DNET_CMD_LIST_DEPRECATED,	/* List all objects for given node ID. Deperacted and forbidden */
	DNET_CMD_EXEC,				/* Execute given command on the remote node */
	DNET_CMD_ROUTE_LIST,			/* Receive route table from given node */
	DNET_CMD_STAT,				/* Gather remote VM, LA and FS statistics */
	DNET_CMD_NOTIFY,			/* Notify when object in question was modified */
	DNET_CMD_DEL,				/* Remove given object from the storage */
	DNET_CMD_STAT_COUNT,			/* Gather remote per-cmd statistics */
	DNET_CMD_STATUS,			/* Change elliptics node status */
	DNET_CMD_READ_RANGE,			/* Read range of objects */
	DNET_CMD_DEL_RANGE,			/* Remove range of objects */
	DNET_CMD_AUTH,				/* Authentification cookie check */
	DNET_CMD_BULK_READ,			/* Read a number of ids at one time */
	DNET_CMD_DEFRAG,			/* Start defragmentation process if backend supports it */
	DNET_CMD_ITERATOR,			/* Start/stop/pause/status for server-side iterator */
	DNET_CMD_INDEXES_UPDATE,		/* Update secondary indexes for id */
	DNET_CMD_INDEXES_INTERNAL,		/* Update identificators table for certain secondary index. Internal usage only */
	DNET_CMD_INDEXES_FIND,		/* Find all objects by indexes */
	DNET_CMD_MONITOR_STAT,		/* Gather monitor json statistics */
	DNET_CMD_UNKNOWN,			/* This slot is allocated for statistics gathered for unknown commands */
	__DNET_CMD_MAX,
};

enum dnet_counters {
	DNET_CNTR_LA1 = __DNET_CMD_MAX*2,	/* Load average for 1 min */
	DNET_CNTR_LA5,				/* Load average for 5 min */
	DNET_CNTR_LA15,				/* Load average for 15 min */
	DNET_CNTR_BSIZE,			/* Block size */
	DNET_CNTR_FRSIZE,			/* Fragment size */
	DNET_CNTR_BLOCKS,			/* Filesystem size in frsize units */
	DNET_CNTR_BFREE,			/* # free blocks */
	DNET_CNTR_BAVAIL,			/* # free blocks for non-root */
	DNET_CNTR_FILES,			/* # inodes */
	DNET_CNTR_FFREE,			/* # free inodes */
	DNET_CNTR_FAVAIL,			/* # free inodes for non-root */
	DNET_CNTR_FSID,				/* File system ID */
	DNET_CNTR_VM_ACTIVE,			/* Active memory */
	DNET_CNTR_VM_INACTIVE,			/* Inactive memory */
	DNET_CNTR_VM_TOTAL,			/* Total memory */
	DNET_CNTR_VM_FREE,			/* Free memory */
	DNET_CNTR_VM_CACHED,			/* Used for cache */
	DNET_CNTR_VM_BUFFERS,			/* Used for buffers */
	DNET_CNTR_NODE_FILES,			/* # Number of available objects in the backend */
	DNET_CNTR_NODE_FILES_REMOVED,		/* Number of removed objects, but yet not cleaned, like marked as removed in eblob backend */
	DNET_CNTR_RESERVED2,			/* Reserved for future statistics */
	DNET_CNTR_RESERVED3,			/* Reserved for future statistics */
	DNET_CNTR_RESERVED4,			/* Reserved for future statistics */
	DNET_CNTR_RESERVED5,			/* Reserved for future statistics */
	DNET_CNTR_RESERVED6,			/* Reserved for future statistics */
	DNET_CNTR_RESERVED7,			/* Reserved for future statistics */
	DNET_CNTR_UNKNOWN,			/* This slot is allocated for statistics gathered for unknown counters */
	__DNET_CNTR_MAX,
};

enum dnet_monitor_categories {
	DNET_MONITOR_ALL = 0,		/* Category for requesting all available statistics */
	DNET_MONITOR_CACHE,		/* Category for cache statistics */
	DNET_MONITOR_IO,		/* Category for IO queue statistics */
	DNET_MONITOR_COMMANDS,		/* Category for commands statistics */
	DNET_MONITOR_IO_HISTOGRAMS,	/* Category for IO hisograms statistics */
	DNET_MONITOR_BACKEND,		/* Category for backend statistics */
	__DNET_MONITOR_MAX		/* Paranoidal check */
};

/*
 * Transaction ID direction bit.
 * When set, data is a reply for the given transaction.
 */
#define DNET_TRANS_REPLY		0x8000000000000000ULL

/*
 * Command flags.
 */

/*
 * When set, node will generate a reply when transaction
 * is completed and put completion status into cmd.status
 * field.
 */
#define DNET_FLAGS_NEED_ACK		(1<<0)

/* There will be more commands with the same parameters (transaction number and id) */
#define DNET_FLAGS_MORE			(1<<1)

/* Transaction is about to be destroyed */
#define DNET_FLAGS_DESTROY		(1<<2)

/* Do not forward requst to antoher node even if given ID does not belong to our range */
#define DNET_FLAGS_DIRECT		(1<<3)

/* Do not locks operations - must be set for script callers or recursive operations */
#define DNET_FLAGS_NOLOCK		(1<<4)

/* Only valid flag for LOOKUP command - when set, return checksum in file_info structure */
#define DNET_FLAGS_CHECKSUM		(1<<5)

/* Currently only valid flag for LOOKUP command - when set, don't check fileinfo in cache */
#define DNET_FLAGS_NOCACHE		(1<<6)

struct dnet_id {
	uint8_t			id[DNET_ID_SIZE];
	uint32_t		group_id;
	uint32_t		trace_id;
} __attribute__ ((packed));

struct dnet_raw_id {
	uint8_t			id[DNET_ID_SIZE];
} __attribute__ ((packed));

static inline void dnet_convert_raw_id(struct dnet_raw_id *id __attribute__ ((unused)))
{
}

static inline void dnet_setup_id(struct dnet_id *id, unsigned int group_id, const unsigned char *raw)
{
	memcpy(id->id, raw, DNET_ID_SIZE);
	id->group_id = group_id;
}

struct dnet_cmd
{
	struct dnet_id		id;
	int			status;
	int			cmd;
	uint64_t		flags;
	uint64_t		trans;
	uint64_t		size;
	uint8_t			data[0];
} __attribute__ ((packed));

static inline void dnet_convert_id(struct dnet_id *id)
{
	id->group_id = dnet_bswap32(id->group_id);
	id->trace_id = dnet_bswap32(id->trace_id);
}

static inline void dnet_convert_cmd(struct dnet_cmd *cmd)
{
	dnet_convert_id(&cmd->id);
	cmd->flags = dnet_bswap64(cmd->flags);
	cmd->cmd = dnet_bswap32(cmd->cmd);
	cmd->status = dnet_bswap32(cmd->status);
	cmd->size = dnet_bswap64(cmd->size);
	cmd->trans = dnet_bswap64(cmd->trans);
}

/*
 * cmd flags which are not 'common' to all commands
 * they occupy higher 32 bits
 */

/* drop notifiction */
#define DNET_ATTR_DROP_NOTIFICATION		(1ULL<<32)

/* What type of counters to fetch */
#define DNET_ATTR_CNTR_GLOBAL			(1ULL<<32)

/* Bulk request for checking files */
#define DNET_ATTR_BULK_CHECK			(1ULL<<32)

/*
 * ascending sort data before returning range request to user
 */
#define DNET_ATTR_SORT				(1ULL<<35)

#define DNET_ADDR_SIZE		28

struct dnet_addr
{
	uint8_t			addr[DNET_ADDR_SIZE];
	uint16_t		addr_len;
	uint16_t		family;
} __attribute__ ((packed));

static inline void dnet_convert_addr(struct dnet_addr *addr)
{
	addr->addr_len = dnet_bswap16(addr->addr_len);
	addr->family = dnet_bswap16(addr->family);
}

struct dnet_addr_container {
	int			addr_num;
	struct dnet_addr	addrs[0];
} __attribute__ ((packed));

static inline void dnet_convert_addr_container(struct dnet_addr_container *cnt)
{
	int i;

	for (i = 0; i < cnt->addr_num; ++i)
		dnet_convert_addr(&cnt->addrs[i]);

	cnt->addr_num = dnet_bswap32(cnt->addr_num);
}

struct dnet_addr_cmd
{
	struct dnet_cmd			cmd;
	struct dnet_addr_container	cnt;
} __attribute__ ((packed));

static inline void dnet_convert_addr_cmd(struct dnet_addr_cmd *acmd)
{
	dnet_convert_addr_container(&acmd->cnt);
	dnet_convert_cmd(&acmd->cmd);
}

static inline int dnet_addr_equal(struct dnet_addr *a1, struct dnet_addr *a2)
{
	if (a1->family != a2->family)
		return 0;
	if (a1->addr_len != a2->addr_len)
		return 0;
	if (memcmp(a1->addr, a2->addr, a1->addr_len))
		return 0;

	return 1;
}

struct dnet_list
{
	struct dnet_id		id;
	uint32_t		size;
	uint8_t			data[0];
} __attribute__ ((packed));

static inline void dnet_convert_list(struct dnet_list *l)
{
	dnet_convert_id(&l->id);
	l->size = dnet_bswap32(l->size);
}

/* Internal flag, used when we want to skip data sending */
#define DNET_IO_FLAGS_SKIP_SENDING	(1<<0)

/* Append given data at the end of the object */
#define DNET_IO_FLAGS_APPEND		(1<<1)

#define DNET_IO_FLAGS_COMPRESS		(1<<2)

/* eblob prepare/commit phase */
#define DNET_IO_FLAGS_PREPARE		(1<<4)
#define DNET_IO_FLAGS_COMMIT		(1<<5)

/* Object has to be removed during check procedure */
#define DNET_IO_FLAGS_REMOVED		(1<<6)

/* Overwrite data */
#define DNET_IO_FLAGS_OVERWRITE		(1<<7)

/* Do not checksum data */
#define DNET_IO_FLAGS_NOCSUM		(1<<8)

/*
 * this flag is used when we want backend not to perform any additional actions
 * except than write data at given offset. This is no-op in filesystem backend,
 * but eblob should disable prepare/commit operations.
 */
#define DNET_IO_FLAGS_PLAIN_WRITE	(1<<9)

/* Do not really send data in range request.
 * Send only statistics instead.
 *
 * -- we do not care if it matches above DNET_IO_FLAGS_PLAIN_WRITE,
 *  since using plain write and nodata (read) is useless anyway
 */
#define DNET_IO_FLAGS_NODATA		(1<<9)

/*
 * Cache flags
 * DNET_IO_FLAGS_CACHE says we should first check cache: read/write or delete
 * DNET_IO_FLAGS_CACHE_ONLY means we do not want to sink to disk, just return whatever cache processing returned (even error)
 *
 * Please note, that READ command always goes into the cache, and if cache read succeeds, we return cached data
 * without going down to disk
 *
 * When DNET_IO_FLAGS_CACHE_REMOVE_FROM_DISK is set and object is being removed from cache, then remove object from disk too.
 */
#define DNET_IO_FLAGS_CACHE		(1<<10)
#define DNET_IO_FLAGS_CACHE_ONLY	(1<<11)
#define DNET_IO_FLAGS_CACHE_REMOVE_FROM_DISK	(1<<12)

/*
 * DNET_IO_FLAGS_COMPARE_AND_SWAP
 *
 * Abort write if checksum of data being overwritten don't match
 * checksum in dnet_io_attr.parent
 *
 * Succeeds if there is no record with given key
 */
#define DNET_IO_FLAGS_COMPARE_AND_SWAP (1<<13)

/*
 * DNET_IO_FLAGS_CHECKSUM
 *
 * Want data checksum on read
 * Checksum is returned in io->parent
 */
#define DNET_IO_FLAGS_CHECKSUM		(1<<14)

/*
 * When set, write will not return file info, but only plain ack
 * Flag value is the same as DNET_IO_FLAGS_CHECKSUM, since logically
 * both flags can not be set in one command (either read or write)
 */
#define DNET_IO_FLAGS_WRITE_NO_FILE_INFO	(1<<14)

/*
 * DNET_INDEXES_FLAGS_INTERSECT
 *
 * Return only objects which have all of the indexes.
 *
 * This flag is for DNET_CMD_INDEXES_FIND request only.
 */
#define DNET_INDEXES_FLAGS_INTERSECT		(1<<0)

/*
 * DNET_INDEXES_FLAGS_UNITE
 *
 * Return all objects which have at least one of the indexes.
 *
 * This flag is for DNET_CMD_INDEXES_FIND request only.
 */
#define DNET_INDEXES_FLAGS_UNITE		(1<<1)

/*
 * DNET_INDEXES_FLAGS_UPDATE_ONLY
 *
 * Not replace list of the indexes by new one. Add indexes which
 * don't exist and add not present one.
 *
 * This flag is for DNET_CMD_INDEXES_UPDATE request only.
 */
#define DNET_INDEXES_FLAGS_UPDATE_ONLY		(1<<2)

/*
 * DNET_INDEXES_FLAGS_MORE
 *
 * Used for bulk find requests. If this flag is set this request is
 * not the last. Next request is placed right after it in this cmd.
 *
 * This flag is for DNET_CMD_INDEXES_FIND request only.
 */
#define DNET_INDEXES_FLAGS_MORE			(1<<3)

/*
 * DNET_INDEXES_FLAGS_REMOVE_ONLY
 *
 * Remove all requested indexes from the object list.
 *
 * This flag is for DNET_CMD_INDEXES_UPDATE request only.
 */
#define DNET_INDEXES_FLAGS_REMOVE_ONLY		(1<<4)


/*
 * DNET_INDEXES_FLAGS_INTERNAL_INSERT
 *
 * Add object to the index's list.
 *
 * This flag is for DNET_CMD_INDEXES_INTERNAL request only.
 */
#define DNET_INDEXES_FLAGS_INTERNAL_INSERT	(1<<0)

/*
 * DNET_INDEXES_FLAGS_INTERNAL_REMOVE
 *
 * Remove object from the index's list.
 *
 * This flag is for DNET_CMD_INDEXES_INTERNAL request only.
 */
#define DNET_INDEXES_FLAGS_INTERNAL_REMOVE	(1<<1)

/*
 * DNET_INDEXES_FLAGS_INTERNAL_REMOVE_ALL
 *
 * Remove index from the storage.
 *
 * This flag is for DNET_CMD_INDEXES_INTERNAL request only.
 */
#define DNET_INDEXES_FLAGS_INTERNAL_REMOVE_ALL	(1<<2)

/*
 * DNET_INDEXES_FLAGS_INTERNAL_REMOVE_FROM_OBJECTS
 *
 * Send requests to remove index from object's list in storages.
 * Use it in addition to DNET_INDEXES_FLAGS_INTERNAL_REMOVE_ALL.
 *
 * This flag is for DNET_CMD_INDEXES_INTERNAL request only.
 */
#define DNET_INDEXES_FLAGS_INTERNAL_REMOVE_FROM_OBJECTS	(1<<3)

/*
 * DNET_INDEXES_FLAGS_INTERNAL_REMOVE_FROM_STORAGE
 *
 * Also remove object from disk.
 * Use it in addition to DNET_INDEXES_FLAGS_INTERNAL_REMOVE_ALL
 *
 * This flag is for DNET_CMD_INDEXES_INTERNAL request only.
 */
#define DNET_INDEXES_FLAGS_INTERNAL_REMOVE_FROM_STORAGE	(1<<4)
/*
 * This index is capped collection, so it may need to remove some
 * object from it.
 *
 * This flag is for DNET_CMD_INDEXES_INTERNAL request only.
 */
#define DNET_INDEXES_FLAGS_INTERNAL_CAPPED_COLLECTION	(1<<5)


struct dnet_time {
	uint64_t		tsec, tnsec;
};

/*
 * Returns true if t1 is before than t2.
 */
static inline int dnet_time_before(struct dnet_time *t1, struct dnet_time *t2)
{
	if ((long)(t1->tsec - t2->tsec) < 0)
		return 1;

	if ((long)(t2->tsec - t1->tsec) < 0)
		return 0;

	return ((long)(t1->tnsec - t2->tnsec) < 0);
}
#define dnet_time_after(t2, t1) 	dnet_time_before(t1, t2)

struct dnet_io_attr
{
	uint8_t			parent[DNET_ID_SIZE];
	uint8_t			id[DNET_ID_SIZE];

	/*
	 * used in range request as start and number for LIMIT(start, num)
	 *
	 * write prepare request uses @num is used as a placeholder
	 * for number of bytes to reserve on disk
	 *
	 * @start is used in cache writes: it is treated as object lifetime in seconds, if zero, object is never removed.
	 * When object's lifetime is over, it is removed from cache, but not from disk.
	 */
	uint64_t		start, num;

	/*
	 * @timestamp and @user_flags is used in extended headers in metadata writes.
	 */
	struct dnet_time	timestamp;
	uint64_t		user_flags;

	/*
	 * Total size of the object being read.
	 * Particulary useful when client asks for part of the object (by specifying size in read request).
	 */
	uint64_t		total_size;

	uint64_t		reserved1;
	uint32_t		reserved2;

	uint32_t		flags;
	uint64_t		offset;
	uint64_t		size;
} __attribute__ ((packed));

static inline void dnet_convert_io_attr(struct dnet_io_attr *a)
{
	a->start = dnet_bswap64(a->start);
	a->num = dnet_bswap64(a->num);

	a->flags = dnet_bswap32(a->flags);
	a->offset = dnet_bswap64(a->offset);
	a->size = dnet_bswap64(a->size);
}

struct dnet_history_entry
{
	uint8_t			id[DNET_ID_SIZE];
	uint32_t		flags;
	uint64_t		reserved;
	uint64_t		tsec, tnsec;
	uint64_t		offset;
	uint64_t		size;
} __attribute__ ((packed));

/*
 * Helper structure and set of functions to map history file and perform basic checks.
 */
struct dnet_history_map
{
	struct dnet_history_entry	*ent;
	long				num;
	ssize_t				size;
	int				fd;
};

static inline void dnet_convert_history_entry(struct dnet_history_entry *a)
{
	a->flags = dnet_bswap32(a->flags);
	a->offset = dnet_bswap64(a->offset);
	a->size = dnet_bswap64(a->size);
	a->tsec = dnet_bswap64(a->tsec);
	a->tnsec = dnet_bswap64(a->tnsec);
}

static inline void dnet_setup_history_entry(struct dnet_history_entry *e,
		unsigned char *id, uint64_t size, uint64_t offset,
		struct timespec *ts, uint32_t flags)
{
	if (!ts) {
		struct timeval tv;

		gettimeofday(&tv, NULL);

		e->tsec = tv.tv_sec;
		e->tnsec = tv.tv_usec * 1000;
	} else {
		e->tsec = ts->tv_sec;
		e->tnsec = ts->tv_nsec;
	}

	memcpy(e->id, id, DNET_ID_SIZE);

	e->size = size;
	e->offset = offset;
	e->flags = flags;
	e->reserved = 0;

	dnet_convert_history_entry(e);
}

struct dnet_stat
{
	/* Load average from the target system multiplied by 100 */
	uint16_t		la[3];

	uint16_t		namemax;	/* maximum filename length */

	uint64_t		bsize;		/* Block size */
	uint64_t		frsize;		/* Fragment size */
	uint64_t		blocks;		/* Filesystem size in frsize units */
	uint64_t		bfree;		/* # free blocks */
	uint64_t		bavail;		/* # free blocks for non-root */
	uint64_t		files;		/* # inodes */
	uint64_t		ffree;		/* # free inodes */
	uint64_t		favail;		/* # free inodes for non-root */
	uint64_t		fsid;		/* file system ID */
	uint64_t		flag;		/* mount flags */

	/*
	 * VM counters in KB (1024) units.
	 * On FreeBSD vm_buffers is used for wire counter.
	 */
	uint64_t		vm_active;
	uint64_t		vm_inactive;
	uint64_t		vm_total;
	uint64_t		vm_free;
	uint64_t		vm_cached;
	uint64_t		vm_buffers;

	/*
	 * Per node IO statistics will live here.
	 * Reserved for future use.
	 */
	uint64_t		node_files;
	uint64_t		node_files_removed;
	uint64_t		reserved[30];
};

static inline void dnet_convert_stat(struct dnet_stat *st)
{
	int i;

	for (i=0; i<3; ++i)
		st->la[i] = dnet_bswap16(st->la[i]);

	st->bsize = dnet_bswap64(st->bsize);
	st->frsize = dnet_bswap64(st->frsize);
	st->blocks = dnet_bswap64(st->blocks);
	st->bfree = dnet_bswap64(st->bfree);
	st->bavail = dnet_bswap64(st->bavail);
	st->files = dnet_bswap64(st->files);
	st->ffree = dnet_bswap64(st->ffree);
	st->favail = dnet_bswap64(st->favail);
	st->fsid = dnet_bswap64(st->fsid);
	st->namemax = dnet_bswap16(st->namemax);

	st->vm_active = dnet_bswap64(st->vm_active);
	st->vm_inactive = dnet_bswap64(st->vm_inactive);
	st->vm_total = dnet_bswap64(st->vm_total);
	st->vm_free = dnet_bswap64(st->vm_free);
	st->vm_buffers = dnet_bswap64(st->vm_buffers);
	st->vm_cached = dnet_bswap64(st->vm_cached);
}

struct dnet_io_notification
{
	struct dnet_addr		addr;
	struct dnet_io_attr		io;
};

static inline void dnet_convert_io_notification(struct dnet_io_notification *n)
{
	dnet_convert_addr(&n->addr);
	dnet_convert_io_attr(&n->io);
}

struct dnet_stat_count
{
	uint64_t			count;
	uint64_t			err;
};

static inline void dnet_convert_stat_count(struct dnet_stat_count *st, int num)
{
	int i;

	for (i=0; i<num; ++i) {
		st[i].count = dnet_bswap64(st[i].count);
		st[i].err = dnet_bswap64(st[i].err);
	}
}

struct dnet_addr_stat
{
	struct dnet_addr		addr;
	int				num;
	int				cmd_num;
	struct dnet_stat_count		count[0];
} __attribute__ ((packed));

static inline void dnet_convert_addr_stat(struct dnet_addr_stat *st, int num)
{
	st->addr.addr_len = dnet_bswap32(st->addr.addr_len);
	st->num = dnet_bswap32(st->num);
	if (!num)
		num = st->num;
	st->cmd_num = dnet_bswap32(st->cmd_num);

	dnet_convert_stat_count(st->count, num);
}

static inline void dnet_stat_inc(struct dnet_stat_count *st, int cmd, int err)
{
	if (cmd >= __DNET_CMD_MAX)
		cmd = DNET_CMD_UNKNOWN;

	if (!err)
		st[cmd].count++;
	else
		st[cmd].err++;
}

static inline void dnet_convert_time(struct dnet_time *tm)
{
	tm->tsec = dnet_bswap64(tm->tsec);
	tm->tnsec = dnet_bswap64(tm->tnsec);
}

static inline void dnet_current_time(struct dnet_time *t)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	t->tsec = tv.tv_sec;
	t->tnsec = tv.tv_usec * 1000;
}

static inline void dnet_empty_time(struct dnet_time *t)
{
	t->tsec = -1;
	t->tnsec = -1;
}

static inline int dnet_time_is_empty(struct dnet_time *t)
{
	static const uint64_t empty = -1;
	if (t->tsec == empty && t->tnsec == empty)
		return 1;
	return 0;
}

struct dnet_file_info {
	int			flen;		/* filename length, which goes after this structure */
	unsigned char		checksum[DNET_CSUM_SIZE];

	uint64_t		size;
	uint64_t		offset;		/* offset within eblob */

	struct dnet_time	mtime;
};

static inline void dnet_convert_file_info(struct dnet_file_info *info)
{
	info->flen = dnet_bswap32(info->flen);

	info->size = dnet_bswap64(info->size);
	info->offset = dnet_bswap64(info->offset);

	dnet_convert_time(&info->mtime);
}

static inline void dnet_info_from_stat(struct dnet_file_info *info, struct stat *st)
{
	info->size = st->st_size;
	info->offset = 0;

	info->mtime.tsec = st->st_mtime;
	info->mtime.tnsec = 0;
}

/* Elliptics node status - if set, status will be changed */
#define DNET_ATTR_STATUS_CHANGE		(1<<0)

/* Elliptics node should exit */
#define DNET_STATUS_EXIT		(1<<0)

/* Ellipitcs node goes ro/rw */
#define DNET_STATUS_RO			(1<<1)

struct dnet_node_status {
	int nflags;
	int status_flags;  /* DNET_STATUS_EXIT, DNET_STATUS_RO should be specified here */
	uint32_t log_level;
};

static inline void dnet_convert_node_status(struct dnet_node_status *st)
{
	st->nflags = dnet_bswap32(st->nflags);
	st->status_flags = dnet_bswap32(st->status_flags);
	st->log_level = dnet_bswap32(st->log_level);
}

#define DNET_AUTH_COOKIE_SIZE	32

struct dnet_auth {
	char			cookie[DNET_AUTH_COOKIE_SIZE];
	uint64_t		flags;
	uint64_t		unused[3];
};

static inline void dnet_convert_auth(struct dnet_auth *a)
{
	a->flags = dnet_bswap64(a->flags);
}

/*!
 * Flag used by dnet_ext_list_extract() to indicate that we need to free old
 * data pointer on dnet_ext_list_destroy()
 */
enum dnet_ext_free_data {
	DNET_EXT_DONT_FREE_ON_DESTROY,
	DNET_EXT_FREE_ON_DESTROY
};

/*!
 * Versions ov extension headers
 */
enum dnet_ext_versions {
	DNET_EXT_VERSION_FIRST,
	DNET_EXT_VERSION_V1,
	DNET_EXT_VERSION_LAST,
};

/*! In-memory extension header */
struct dnet_ext;

/*! On-disk extension list header */
struct dnet_ext_list_hdr {
	uint8_t			version;	/* Extension header version */
	uint8_t			__pad1[3];	/* For future use (should be NULLed) */
	uint32_t		size;		/* Size of all extensions */
	struct dnet_time	timestamp;	/* Time stamp of record */
	uint64_t		flags;		/* Custom flags for this record */
	uint64_t		__pad2[2];	/* For future use (should be NULLed) */
} __attribute__ ((packed));

/*! In-memory extension conatiner */
struct dnet_ext_list {
	uint8_t			version;	/* Extension header version */
	uint32_t		size;		/* Total size of extensions */
	uint64_t		flags;		/* Custom flags for this record */
	struct dnet_time	timestamp;	/* TS of header */
	struct dnet_ext		**exts;		/* Array of pointers to extensions */
	void			*data;		/* Pointer to original data before extraction */
};

/*! Types of extensions */
enum {
	DNET_EXTENSION_FIRST,		/* Assert */
	/* DNET_EXTENSION_USER_DATA, */
	DNET_EXTENSION_LAST		/* Assert */
};

/*
 * When set server-side iterator works with data as well as index/metadata,
 * otherwise only index/metadata is stored/sent to back client/disk
 */
#define DNET_IFLAGS_DATA		(1<<0)
/* When set key range is used */
#define DNET_IFLAGS_KEY_RANGE		(1<<1)
/* When set timestamp range is used */
#define DNET_IFLAGS_TS_RANGE		(1<<2)
/* Sanity */
#define DNET_IFLAGS_ALL			(DNET_IFLAGS_DATA	\
		| DNET_IFLAGS_KEY_RANGE | DNET_IFLAGS_TS_RANGE)

enum dnet_iterator_types {
	DNET_ITYPE_FIRST,		/* Sanity */
	DNET_ITYPE_DISK,		/*
					 * Iterator saves data chunks
					 * (index/metadata + (optionally) data)
					 * locally on server to $root/iter/$id
					 * instead of sending chunks to client
					 */
	DNET_ITYPE_NETWORK,		/* iterator sends data chunks to client */
	DNET_ITYPE_LAST,		/* Sanity */
};

/* Actions that can be passed to iterator */
enum dnet_iterator_action {
	DNET_ITERATOR_ACTION_FIRST,	/* Sanity */
	DNET_ITERATOR_ACTION_START,	/* Start iterator */
	DNET_ITERATOR_ACTION_PAUSE,	/* Pause iterator */
	DNET_ITERATOR_ACTION_CONTINUE,	/* Continue previously paused iterator */
	DNET_ITERATOR_ACTION_CANCEL,	/* Cancel running or paused iterator */
	DNET_ITERATOR_ACTION_LAST,	/* Sanity */
};

struct dnet_iterator_range
{
	struct dnet_raw_id	key_begin;	/* Start key */
	struct dnet_raw_id	key_end;	/* End key */
} __attribute__ ((packed));

/*
 * Iteration request
 */
struct dnet_iterator_request
{
	uint64_t			id;		/* Iterator ID, for pause/cont/cancel */
	uint32_t			action;		/* Action: start/pause/cont, XXX: enum */
	uint64_t			range_num;	/* Number of ranges for iterating */
	struct dnet_time		time_begin;	/* Start time */
	struct dnet_time		time_end;	/* End time */
	uint32_t			itype;		/* Callback to use: Net/File, XXX: enum */
	uint64_t			flags;		/* DNET_IFLAGS_* */
	uint64_t			reserved[5];
} __attribute__ ((packed));

static inline void dnet_convert_iterator_request(struct dnet_iterator_request *r)
{
	r->flags = dnet_bswap64(r->flags);
	r->id = dnet_bswap64(r->id);
	r->itype = dnet_bswap32(r->itype);
	r->action = dnet_bswap32(r->action);
	r->range_num = dnet_bswap64(r->range_num);
	dnet_convert_time(&r->time_begin);
	dnet_convert_time(&r->time_end);
}

/*
 * Iterator response
 * TODO: Maybe it's better to include whole ehdr in response
 */
struct dnet_iterator_response
{
	uint64_t			id;		/* Iterator ID, for pause/cont/cancel */
	struct dnet_raw_id		key;		/* Response key */
	int				status;		/* Response status */
	struct dnet_time		timestamp;	/* Timestamp from extended header */
	uint64_t			user_flags;	/* User flags set in extended header */
	uint64_t			size;
	uint64_t			reserved[4];
} __attribute__ ((packed));

static inline void dnet_convert_iterator_response(struct dnet_iterator_response *r)
{
	r->status = dnet_bswap32(r->status);
	r->user_flags = dnet_bswap32(r->user_flags);
	dnet_convert_time(&r->timestamp);
}

/*
 * Indexes request entry
 */
struct dnet_indexes_request_entry
{
	struct dnet_raw_id		id;		/* Index ID */
	uint64_t			flags;		/* Index flags */
	uint32_t			limit;		/* Capped collection limit */
	uint32_t			reserved1;
	uint64_t			reserved2;
	uint64_t			size;		/* Data size */
	char				data[0];	/* Index-specific data */
} __attribute__ ((packed));

/*
 * Indexes request
 */
struct dnet_indexes_request
{
	struct dnet_id			id;		/* Index ID with properly set group_id */
	uint32_t			flags;
	uint32_t			shard_id;
	uint32_t			shard_count;
	uint64_t			reserved[5];
	uint64_t			entries_count;	/* Count of indexes */
	struct dnet_indexes_request_entry	entries[0];	/* List of indexes to set */
} __attribute__ ((packed));

/*
 * Indexes reply entry
 */
struct dnet_indexes_reply_entry
{
	struct dnet_raw_id		id;		/* Index ID */
	int				status;		/* Index change status */
	uint64_t			reserved[2];
} __attribute__ ((packed));

/*
 * Indexes reply
 */
struct dnet_indexes_reply
{
	uint64_t			reserved[5];
	uint64_t			entries_count;	/* Count of results */
	struct dnet_indexes_reply_entry	entries[0];	/* List of entries update results */
} __attribute__ ((packed));

/*
 * Defragmentation control structure
 */

/* when status flag is set, do not start defrag, just return its status */
#define DNET_DEFRAG_FLAGS_STATUS	(1<<0)

struct dnet_defrag_ctl {
	uint64_t	flags;
	int		status;		/* positive number means defragmentation is in progress, negative - some error (errno) */
	int		pad;
	uint64_t	total;
	uint64_t	res[3];
};

static inline void dnet_convert_defrag_ctl(struct dnet_defrag_ctl *ctl)
{
	ctl->flags = dnet_bswap64(ctl->flags);
	ctl->status = dnet_bswap32(ctl->status);
	ctl->total = dnet_bswap64(ctl->total);
}

struct dnet_monitor_stat_request {
	int		category;
	int		reserved_int; // reserved for packing
	uint64_t	reserved[4];
} __attribute__ ((packed));

static inline void dnet_convert_monitor_stat_request(struct dnet_monitor_stat_request *r)
{
	r->category = dnet_bswap32(r->category);
}

#ifdef __cplusplus
}
#endif

#endif /* __DNET_PACKET_H */
