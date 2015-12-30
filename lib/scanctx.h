/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2014  Mark A Lindner

   This file is part of libconfig.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, see
   <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------------
*/

#ifndef __libconfig_scanctx_h
#define __libconfig_scanctx_h

#include "libconfig.h"
#include "strbuf.h"

#include <stdio.h>
#include <sys/types.h>

#define MAX_INCLUDE_DEPTH 10

struct scan_context
{
  config_t *config;
  const char *top_filename;
  const char *files[MAX_INCLUDE_DEPTH];
  void *buffers[MAX_INCLUDE_DEPTH];
  FILE *streams[MAX_INCLUDE_DEPTH];
  strbuf_t string;
  const char **filenames;
  unsigned int num_filenames;
  int depth;
  void **dentries;		/* dirent** */
  const char *basedir;	/* basedir for @include_dir */
  unsigned de_max, de_cur;	/* counters into dirent* array */
};

struct dirent;	/* forward decl */

extern void scanctx_init(struct scan_context *ctx, const char *top_filename);
extern const char **scanctx_cleanup(struct scan_context *ctx,
                                    unsigned int *num_filenames);

extern const char *scanctx_getpath(struct scan_context *ctx);
extern const char *scanctx_filename(struct scan_context *ctx, const char *dirname, const char *filename);

extern const char* scanctx_dirnext(struct scan_context* ctx);
extern int   scanctx_dirscan(struct scan_context* ctx, const char* dirname,
				int (*filter)(const struct dirent *),
				int (*compar)(const struct dirent **, const struct dirent **));
extern void  scanctx_dirend(struct scan_context* ctx);
extern int   scanctx_inloop(const struct scan_context* ctx);

extern FILE *scanctx_push_include(struct scan_context *ctx, void *prev_buffer,
                                  const char *file, const char **error);
extern void *scanctx_pop_include(struct scan_context *ctx);

#define scanctx_append_string(C, S)             \
  strbuf_append(&((C)->string), (S))

extern char *scanctx_take_string(struct scan_context *ctx);

extern const char *scanctx_current_filename(struct scan_context *ctx);

#endif /* __libconfig_scanctx_h */
