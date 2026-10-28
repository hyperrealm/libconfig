/*
 * Regression test for the stack-buffer-overflow in ls-config's read_config()
 * (contrib/ls-config/src/ls-config.c).
 *
 * read_config() formats float values with sprintf(buffer, "%f", ...) into a
 * 256-byte stack buffer. "%f" never switches to exponent notation, so a value
 * like 1e300 in the configuration file prints as 301 digits plus ".000000"
 * and runs past the end of the buffer (the widest double, -DBL_MAX, needs 318
 * bytes). The same code handles a scalar float, float elements of an array
 * and float elements of a list.
 *
 * This test pulls in the real ls-config.c (renaming its main() out of the
 * way), writes a configuration file with a large float in each of those
 * three places and reads each one back through read_config(), the same way
 *
 *   ls-config -q -v -g scalar -f float.cfg
 *
 * would. Built with -fsanitize=address, the unpatched code aborts inside
 * sprintf(); with the fix it prints the full values and returns cleanly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define main ls_config_main
#include "ls-config.c"
#undef main

static const char *config_text =
  "scalar = 1e300;\n"
  "array = [ 1.5, -1.7976931348623157e308 ];\n"
  "list = ( 2.5, 1e300 );\n";

int main(void) {
  char path[] = "/tmp/ls-config-float-XXXXXX";
  char *names[] = {"scalar", "array", "list"};
  struct flags optflags = {0};
  FILE *fp;
  int fd, i, rc = 0;

  fd = mkstemp(path);
  if(fd < 0 || (fp = fdopen(fd, "w")) == NULL) {
    perror("mkstemp");
    return 1;
  }
  fputs(config_text, fp);
  fclose(fp);

  optflags.quiet = 1;
  optflags.values = 1;

  for(i = 0; i < (int)(sizeof(names) / sizeof(names[0])); i++) {
    if(read_config(path, names[i], optflags) != 0) {
      fprintf(stderr, "FAIL: read_config() could not read %s\n", names[i]);
      rc = 1;
    }
  }

  unlink(path);

  if(rc == 0)
    fprintf(stderr, "PASS: ls-config printed large float values without "
                    "overflowing its reading buffer\n");

  return rc;
}
