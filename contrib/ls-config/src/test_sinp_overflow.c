/*
 * Regression test for the heap-buffer-overflow in ls-config's command line
 * parsing (reported against contrib/ls-config/src/ls-config.c).
 *
 * ls-config copies each of its -s/-g/-d/-p/-f (and the matching long-option)
 * arguments into a fixed 256-byte scratch buffer called "sinp" using
 * sscanf(optarg, "%s", sinp) / sscanf(optarg, "%[^\n]s", sinp). Neither
 * conversion had a field width, so an argument longer than the buffer wrote
 * straight past the end of the heap allocation. A "-s" (or "--set") value of
 * a few hundred bytes was enough to trigger it.
 *
 * This test pulls in the real ls-config.c (renaming its main() out of the
 * way) and drives it with an oversized --set argument, the same way a
 * shell invocation like
 *
 *   ls-config --set "$(python3 -c 'print("A"*500)')" -f /dev/null
 *
 * would. Built with -fsanitize=address, the unpatched code aborts inside
 * sscanf() before ever reaching the end of main(); with the fix (a
 * "%255s" / "%255[^\n]s" field width matching the sinp buffer) it just runs
 * ls-config's normal argument handling and exits cleanly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main ls_config_main
#include "ls-config.c"
#undef main

static void report_pass(void) {
  fprintf(stderr, "PASS: ls-config handled the oversized argument without "
                   "overflowing sinp\n");
}

int main(void) {
  char long_value[600];
  memset(long_value, 'A', sizeof(long_value) - 1);
  long_value[sizeof(long_value) - 1] = '\0';

  char *argv[] = {"ls-config", "--set", long_value, "-f", "/dev/null"};
  int argc = (int)(sizeof(argv) / sizeof(argv[0]));

  /* ls_config_main() always ends by calling exit(), so if we get there at
     all (rather than being aborted by the sanitizer midway through option
     parsing) the buffer wasn't overrun. atexit() lets us say so on the way
     out. */
  atexit(report_pass);

  ls_config_main(argc, argv);

  /* unreachable: ls_config_main() always exits */
  return 0;
}
