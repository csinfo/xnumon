/*-
 * xnumon - monitor macOS for malicious activity
 * https://www.roe.ch/xnumon
 *
 * Copyright (c) 2017-2018, Daniel Roethlisberger <daniel@roe.ch>.
 * All rights reserved.
 *
 * Licensed under the Open Software License version 3.0.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "hashes.h"
#include "strset.h"
#include "attrib.h"

#include <stddef.h>

typedef struct {
	char *path;
	char *id;

	bool launchd_mode;      /* only settable via command line */

	size_t limit_nofile;

	int kextlevel;
#define KEXTLEVEL_NONE 0
#define KEXTLEVEL_OPEN 1
#define KEXTLEVEL_HASH 2
#define KEXTLEVEL_CSIG 3
	int hflags;
	/* HASH_* see hashes.h */
	bool codesign;

	size_t ancestors;       /* 0 unlimited, > 0 limited */

	int logdst;
	int logfmt;
	int logoneline;         /* compact one-line log format */
	char *logfile;

	strset_t suppress_image_exec_by_ident;
	strset_t suppress_image_exec_by_path;
	strset_t suppress_process_access_by_ident;
	strset_t suppress_process_access_by_path;
} config_t;

config_t * config_new(const char *) MALLOC;
void config_free(config_t *) NONNULL(1);

int config_str(config_t *, const char *, const char *) NONNULL(1,2,3) WUNRES;

int config_kextlevel(config_t *, const char *) NONNULL(1,2);
const char * config_kextlevel_s(config_t *) NONNULL(1);

#endif

