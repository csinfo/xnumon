/*-
 * xnumon - monitor macOS for malicious activity
 * https://www.roe.ch/xnumon
 *
 * Copyright (c) 2017-2018, Daniel Roethlisberger <daniel@roe.ch>.
 * All rights reserved.
 *
 * Licensed under the Open Software License version 3.0.
 */

#ifndef PROCMON_H
#define PROCMON_H

#include "ipaddr.h"
#include "auevent.h"
#include "config.h"
#include "sys.h"
#include "tommylist.h"
#include "codesign.h"
#include "log.h"
#include "logevt.h"
#include "attrib.h"

#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>

typedef struct {
	uint32_t procs;
	uint32_t images;
	uint64_t eimisseds;
	uint64_t cwdmisseds;
	uint64_t ooms;
	uint32_t kqsize;
	uint64_t kqlookups;
	uint64_t kqnotfounds;
	uint64_t kqtimeouts;
	uint64_t kqskips;
} procmon_stat_t;

void procmon_fork(struct timespec *, audit_proc_t *, pid_t) NONNULL(1,2);
void procmon_spawn(struct timespec *, audit_proc_t *, pid_t,
                   char *, audit_attr_t *, char **) NONNULL(1,2);
void procmon_exec(struct timespec *, audit_proc_t *,
                  char *, audit_attr_t *, char **) NONNULL(1,2);
void procmon_exit(pid_t);
void procmon_wait4(pid_t);
void procmon_chdir(pid_t, char *) NONNULL(2);

void procmon_kern_preexec(struct timespec *, pid_t, const char *) NONNULL(1,3);

void procmon_preloadpid(pid_t);

int procmon_init(config_t *) WUNRES NONNULL(1);
void procmon_fini(void);
void procmon_stats(procmon_stat_t *) NONNULL(1);
bool procmon_kpriority(void) WUNRES;
uint32_t procmon_images(void) WUNRES;
const char * procmon_getcwd(pid_t) WUNRES;

/*
 * image_exec_t is both the data structure containing a snapshot of an
 * executable image (processes keep track of their exec image), and is reused
 * as an exec log event in order to avoid allocating and copying all this
 * data doubly.  This reuse accounts for at least some of the complexity in
 * its implementation.
 */
typedef struct image_exec {
	logevt_header_t hdr;

	unsigned long flags;
#define EIFLAG_PIDLOOKUP    0x0001UL  /* image created from pid lookup */
#define EIFLAG_NOPATH       0x0002UL  /* external fetching failed, no path */
#define EIFLAG_STAT         0x0004UL  /* set if stat is fully set */
#define EIFLAG_ATTR         0x0008UL  /* set if stat was populated from attr */
#define EIFLAG_HASHES       0x0010UL  /* set if hashes are available */
#define EIFLAG_SHEBANG      0x0020UL  /* set if file contains #! (is script) */
#define EIFLAG_DONE         0x0040UL  /* set if processing is complete */
#define EIFLAG_ENOMEM       0x0080UL  /* set if parts missing due to ENOMEM */
#define EIFLAG_NOLOG        0x0100UL  /* do not submit this for logging */

	/* open/analysis/close state */
	int fd;

	/* exec data */
	pid_t pid;
	struct timespec fork_tv;
	char **argv; /* free */
	char *path; /* free */
	char *cwd; /* free */
	audit_proc_t subject;

	/* stat attrs if EIFLAG_STAT or EIFLAG_ATTR is set */
	stat_attr_t stat;

	/* hashes if EIFLAG_HASHES is set */
	hashes_t hashes;

	/* codesign results, or NULL */
	codesign_t *codesign;

	/* for interpreters, ptr to script file */
	struct image_exec *script;
	/* origin image */
	struct image_exec *prev;

	/* kext queue ttl */
	size_t kqttl;
#define MAXKQTTL 16     /* maximum out-of-order window and water level up to
                           which the kextctl file descriptor will be drained
                           with priority versus the auditpipe descriptor */

	size_t refs;
	pthread_mutex_t refsmutex;
} image_exec_t;

image_exec_t * image_exec_by_pid(pid_t) MALLOC;
void image_exec_free(image_exec_t *) NONNULL(1);

#endif
