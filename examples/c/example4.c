/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2018  Mark A Lindner

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

#include <dirent.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <libconfig.h>

/* This example reads the configuration file 'example.cfg' and displays
 * some of its contents.
 */

static const char **include_func(config_t *config,
                                 const char *include_dir,
                                 const char *path,
                                 const char **error)
{
  char *p;
  DIR *dp;
  struct dirent *dir_entry;
  struct stat stat_buf;
  char include_path[PATH_MAX + 1];
  size_t include_path_len = 0;
  char file_path[PATH_MAX + 1];
  char **result = NULL;
  char **result_next = result;
  int result_count = 0;
  int result_capacity = 0;

  *include_path = 0;

  if(*path != '/')
  {
    if(include_dir)
    {
      strcat(include_path, include_dir);
      include_path_len += strlen(include_dir);
    }
  }

  p = strrchr(path, '/');
  if(p > path)
  {
    int len = p - path;

    if((include_path_len > 0)
       && (*(include_path + include_path_len - 1) != '/'))
    {
      strcat(include_path, "/");
      ++include_path_len;
    }

    strncat(include_path, path, len);
    include_path_len += len;
  }

  if(include_path_len == 0)
  {
    strcpy(include_path, ".");
    include_path_len = 1;
  }

  dp = opendir(include_path);
  if(dp)
  {
    while((dir_entry = readdir(dp)) != NULL)
    {
      snprintf(file_path, PATH_MAX, "%s/%s", include_path, dir_entry->d_name);
      if(lstat(file_path, &stat_buf) != 0) continue;
      if(!S_ISREG(stat_buf.st_mode)) continue;
      if(fnmatch(path, file_path, FNM_PATHNAME) != 0) continue;

      if(result_count == result_capacity)
      {
        result_capacity += 16;
        result = (char **)realloc(result, (result_capacity + 1) * sizeof(char *));
        result_next = result + result_count;
      }

      *result_next = strdup(file_path);
      ++result_next;
      ++result_count;

      printf("file: %s\n", file_path);
    }
    closedir(dp);
  }

  *result_next = NULL;

  return((const char **)result);
}

int main(int argc, char **argv)
{
  config_t cfg;

  config_init(&cfg);
  config_set_include_func(&cfg, include_func);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, "example4.cfg"))
  {
    fprintf(stderr, "%s:%d - %s\n", config_error_file(&cfg),
            config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }

  config_write(&cfg, stdout);

  return(EXIT_SUCCESS);
}
