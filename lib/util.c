/* ----------------------------------------------------------------------------
   libconfig - A library for processing structured configuration files
   Copyright (C) 2005-2026  Mark A Lindner

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

#ifdef HAVE_CONFIG_H
#include "ac_config.h"
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* for strtod_l() in glibc */
#endif

#include "util.h"
#include "wincompat.h"

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_XLOCALE_H) || defined(__APPLE__)
#include <xlocale.h>
#endif

#if defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS)

#define LIBCONFIG_HAVE_C_LOCALE

struct libconfig_locale
{
  _locale_t numeric;
};

#elif defined(__APPLE__) \
  || ((defined HAVE_NEWLOCALE) && (defined HAVE_FREELOCALE) \
      && (defined HAVE_STRTOD_L))

#define LIBCONFIG_HAVE_C_LOCALE

struct libconfig_locale
{
  locale_t numeric;
};

#endif

/* ------------------------------------------------------------------------- */

void libconfig_fatal_error_handler(const char *message)
{
  if(posix_write(STDERR_FILENO, (const void *)message, strlen(message))) {}
  abort();
}

/* ------------------------------------------------------------------------- */

static void (*__libconfig_fatal_error_func)(const char *) =
  libconfig_fatal_error_handler;

static const char *__libconfig_malloc_failure_message =
  "\alibconfig: memory allocation failure\n";

/* ------------------------------------------------------------------------- */

void libconfig_set_fatal_error_func(void (*func)(const char *))
{
  __libconfig_fatal_error_func = (func ? func
                                  : libconfig_fatal_error_handler);
}

/* ------------------------------------------------------------------------- */

void libconfig_fatal_error(const char *message)
{
  __libconfig_fatal_error_func(message);
}

/* ------------------------------------------------------------------------- */

void *libconfig_malloc(size_t size)
{
  void *ptr = malloc(size);
  if(!ptr)
    libconfig_fatal_error(__libconfig_malloc_failure_message);

  return(ptr);
}

/* ------------------------------------------------------------------------- */

void *libconfig_calloc(size_t nmemb, size_t size)
{
  void *ptr = calloc(nmemb, size);
  if(!ptr)
    libconfig_fatal_error(__libconfig_malloc_failure_message);

  return(ptr);
}

/* ------------------------------------------------------------------------- */

void *libconfig_realloc(void *ptr, size_t size)
{
  ptr = realloc(ptr, size);
  if(!ptr)
    libconfig_fatal_error(__libconfig_malloc_failure_message);

  return(ptr);
}

/* ------------------------------------------------------------------------- */

char *libconfig_strdup(const char *str)
{
  char *copy = NULL;

  if(str)
  {
    copy = strdup(str);
    if(!copy)
      libconfig_fatal_error(__libconfig_malloc_failure_message);
  }

  return(copy);
}

/* ------------------------------------------------------------------------- */

void *libconfig_reallocarray(void *ptr, size_t nmemb, size_t size)
{
  if(size && (nmemb > (SIZE_MAX / size)))
  {
    libconfig_fatal_error(__libconfig_malloc_failure_message);
    return(NULL);
  }

  return(libconfig_realloc(ptr, nmemb * size));
}

/* ------------------------------------------------------------------------- */

size_t libconfig_size_add(size_t a, size_t b)
{
  if(a > (SIZE_MAX - b))
  {
    libconfig_fatal_error(__libconfig_malloc_failure_message);
    return(SIZE_MAX); /* no allocation of this size can succeed */
  }

  return(a + b);
}

/* ------------------------------------------------------------------------- */

/* Returns 1 on success, 0 on failure. Sets is_long to 1 if value is a
   64-bit int, otherwise to 0.
*/

int libconfig_parse_integer(const char *s, int base, long long *val,
                            int *is_long)
{
  char *endptr;
  int errsave = errno;

  errno = 0;
  *val = strtoll(s, &endptr, base);

  /* Only unsuffixed non-decimal values use 32-bit signed interpretation. */
  if((base != 10) && (*endptr != 'L')
     && (*val > INT32_MAX) && (*val <= UINT32_MAX))
    *val = (long long)(int)*val;

  *is_long = ((*val < INT32_MIN) || (*val > INT32_MAX));

  /* Check for trailing L's. */
  while(!errno && *endptr == 'L')
  {
    *is_long = 1;
    ++endptr;
  }

  if(*endptr || errno)
  {
    errno = errsave;
    return(0);  /* parse error */
  }
  errno = errsave;

  return(1);
}

/* ------------------------------------------------------------------------- */

struct libconfig_locale *libconfig_locale_create(void)
{
#ifdef LIBCONFIG_HAVE_C_LOCALE

  struct libconfig_locale *loc = __new(struct libconfig_locale);

#if defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS)
  loc->numeric = _create_locale(LC_NUMERIC, "C");
#else
  loc->numeric = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
#endif

  /* The "C" locale always exists, so this can only fail for lack of
     memory. */
  if(!loc->numeric)
    libconfig_fatal_error(__libconfig_malloc_failure_message);

  return(loc);

#else

  return(NULL);

#endif
}

/* ------------------------------------------------------------------------- */

void libconfig_locale_destroy(struct libconfig_locale *loc)
{
#ifdef LIBCONFIG_HAVE_C_LOCALE

  if(!loc)
    return;

#if defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS)
  _free_locale(loc->numeric);
#else
  freelocale(loc->numeric);
#endif

  __delete(loc);

#endif
}

/* ------------------------------------------------------------------------- */

double libconfig_parse_double(const char *s,
                              const struct libconfig_locale *loc)
{
#ifdef LIBCONFIG_HAVE_C_LOCALE
  return(strtod_l(s, NULL, loc->numeric));
#else
  return(strtod(s, NULL));
#endif
}

/* ------------------------------------------------------------------------- */

void libconfig_format_double(double val, int precision, int sci_ok, char *buf,
                             size_t buflen, const struct libconfig_locale *loc)
{
  const char *fmt = sci_ok ? "%.*g" : "%.*f";
  char *p, *q;

#if defined(LIBCONFIG_WINDOWS_OS) && !defined(LIBCONFIG_MINGW_OS)

  /* Unlike snprintf(), this doesn't terminate a truncated string. */
  _snprintf_l(buf, buflen - 3, fmt, loc->numeric, precision, val);
  buf[buflen - 4] = '\0';

#elif defined(__APPLE__)

  snprintf_l(buf, buflen - 3, loc->numeric, fmt, precision, val);

#else

  /* No snprintf_l(), so replace the current locale's radix character. */
  snprintf(buf, buflen - 3, fmt, precision, val);

  {
    const char *radix = localeconv()->decimal_point;

    if(radix && *radix && strcmp(radix, ".") && (p = strstr(buf, radix)))
    {
      size_t len = strlen(radix);

      *p = '.';
      if(len > 1)
        memmove(p + 1, p + len, strlen(p + len) + 1);
    }
  }

#endif

  /* Check for exponent. */
  p = strchr(buf, 'e');
  if(p) return;

  /* Check for decimal point. */
  p = strchr(buf, '.');
  if(!p)
  {
    /* No decimal point. Add trailing ".0". */
    strcat(buf, ".0");
  }
  else
  {
    /* Remove any excess trailing 0's after decimal point. */
    for(q = buf + strlen(buf) - 1; q > p + 1; --q)
    {
      if(*q == '0')
        *q = '\0';
      else
        break;
    }
  }
}

/* ------------------------------------------------------------------------- */

/* buf must be at least 65 bytes. Return value is pointer to most significant
   nonzero bit, or pointer to the least significant zero bit if value is 0. */

char *libconfig_format_bin(int64_t val, char *buf)
{
  static const int num_bits = sizeof(val) * BITS_IN_BYTE;
  char *p = buf + num_bits;
  char *first_bit = NULL;

  *p = '\0';

  for(int i = 0; i < num_bits; ++i)
  {
    int x = val & 1;
    val >>= 1;
    *(--p) = '0' + x;
    if(!first_bit || x) first_bit = p;
  }

  return(first_bit);
}

/* ------------------------------------------------------------------------- */
