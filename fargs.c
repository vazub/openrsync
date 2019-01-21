/*	$Id$ */
/*
 * Copyright (c) 2019 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
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
#include <sys/stat.h>

#include <assert.h>
#include <stdlib.h>

#include "extern.h"

char **
fargs_cmdline(struct sess *sess, const struct fargs *f)
{
	char	**args;
	size_t	  i = 0, j, argsz = 0;

	assert(NULL != f);
	assert(f->sourcesz > 0);

	/* Be explicit with array size. */

	argsz += 1; 	/* dot separator */
	argsz += 1; 	/* sink file */
	argsz += 5; 	/* per-mode maximum */
	argsz += 10;	/* shared args */
	argsz += 1;	/* NULL pointer */
	argsz += f->sourcesz;

	args = calloc(argsz, sizeof(char *));
	if (NULL == args) {
		ERR(sess, "calloc");
		return NULL;
	}

	if (FARGS_RECEIVER == f->mode || FARGS_SENDER == f->mode) {
		assert(NULL != f->host);
		args[i++] = "ssh";
		args[i++] = f->host;
		args[i++] = "rsync";
		args[i++] = "--server";
		if (FARGS_RECEIVER == f->mode)
			args[i++] = "--sender";
	} else {
		args[i++] = "rsync";
		args[i++] = "--server";
	}

	/* Shared arguments. */

	if (sess->opts->verbose > 3)
		args[i++] = "-v";
	if (sess->opts->verbose > 2)
		args[i++] = "-v";
	if (sess->opts->verbose > 1)
		args[i++] = "-v";
	if (sess->opts->verbose > 0)
		args[i++] = "-v";
	if (sess->opts->dry_run)
		args[i++] = "-n";
	if (sess->opts->preserve_times)
		args[i++] = "-t";
	if (sess->opts->preserve_perms)
		args[i++] = "-p";
	if (sess->opts->recursive)
		args[i++] = "-r";

	/*
	 * This is for reference implementation rsync servers.
	 * We want to use an old protocol, but the old implementation of
	 * MD4 was also broken in a way we don't want to support.
	 * Force it to use MD5, even though we'll stipulate that we use
	 * the old protocol.
	 */

	args[i++] = "--checksum-choice";
	args[i++] = "md5";

	/* Terminate with a full-stop for reasons unknown. */

	args[i++] = ".";

	if (FARGS_RECEIVER == f->mode) {
		for (j = 0; j < f->sourcesz; j++)
			args[i++] = f->sources[j];
	} else
		args[i++] = f->sink;

	args[i] = NULL;
	return args;
}

