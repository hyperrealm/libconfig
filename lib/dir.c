/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2015  Mark A Lindner

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

#include "dir.h"

#ifdef WIN32
#include <windows.h>
#else // !WIN32
#include <dirent.h>
#include <stdlib.h>
#endif // WIN32

/* ------------------------------------------------------------------------- */

struct dir_info
{
#ifdef WIN32
  HANDLE dir;
  WIN32_FIND_DATA dir_entry;
  BOOL has_more_files;
#else // !WIN32
  DIR *dir;
#endif // WIN32
};

/* ------------------------------------------------------------------------- */

void *dir_open(const char *path)
{
  struct dir_info *dir_info =
    (struct dir_info *)calloc(1, sizeof(struct dir_info));

#ifdef WIN32

  dir_info->dir = FindFirstFileA(path, &(dir_info->dir_entry));
  if(dir_info->dir != INVALID_HANDLE_VALUE)
    dir_info->has_more_files = TRUE;
  else
  {
    free(dir_info);
    dir_info = NULL;
  }

#else // !WIN32

  dir_info->dir = opendir(path);
  if(!dir_info->dir)
  {
    free(dir_info);
    dir_info = NULL;
  }

#endif // WIN32

  return(dir_info);
}

/* ------------------------------------------------------------------------- */

const char *dir_next_ordinary_file(void *dirp)
{
  struct dir_info *dir_info = (struct dir_info *)dirp;

#ifdef WIN32

  if(!dir_info->has_more_files)
    return(NULL);

  while((dir_info->dir_entry.dwFileAttributes & FILE_ATTRIBUTE_NORMAL)
        != FILE_ATTRIBUTE_NORMAL)
  {
    if(!FindNextFile(dir_info->dir, &(dir_info->dir_entry)))
    {
      dir_info->has_more_files = FALSE;
      return(NULL);
    }
  }

  return(dir_info->dir_entry.cFileName);

#else // !WIN32

  // TODO

#endif // WIN32
}

/* ------------------------------------------------------------------------- */

void dir_close(void *dirp)
{
  struct dir_info *dir_info = (struct dir_info *)dirp;

#ifdef WIN32

  FindClose(dir_info->dir);

#else // !WIN32

  closedir(dir_info->dir);

#endif // WIN32

  free(dir_info);
}

/* ------------------------------------------------------------------------- */
