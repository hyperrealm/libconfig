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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#include <libconfig.h>
#include <tinytest.h>

/* ------------------------------------------------------------------------- */

static void parse_and_compare(const char *input_file, const char *output_file)
{
  config_t cfg;
  int ok;

  config_init(&cfg);
  config_set_include_dir(&cfg, "./testdata");

  ok = config_read_file(&cfg, input_file);
  if(!ok)
  {
    printf("error: %s:%d\n", config_error_text(&cfg),
           config_error_line(&cfg));
  }
  TT_ASSERT_TRUE(ok);

  remove("temp.cfg");
  TT_ASSERT_TRUE(config_write_file(&cfg, "temp.cfg"));

  TT_ASSERT_TXTFILE_EQ("temp.cfg", output_file);
  remove("temp.cfg");

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

static void parse_file_and_compare_error(const char *input_file,
                                         const char *parse_error)
{
  config_t cfg;
  char actual_error[128];
  char expected_error[128];

  config_init(&cfg);
  TT_ASSERT_FALSE(config_read_file(&cfg, input_file));

  snprintf(expected_error, sizeof(expected_error), "%s:%s",
           input_file, parse_error);

  snprintf(actual_error, sizeof(actual_error), "%s:%d %s\n",
           config_error_file(&cfg), config_error_line(&cfg),
           config_error_text(&cfg));

  config_destroy(&cfg);

  TT_ASSERT_STR_EQ(actual_error, expected_error);
}

/* ------------------------------------------------------------------------- */

static void parse_string_and_compare_error(const char *input_text,
                                           const char *parse_error)
{
  config_t cfg;
  char actual_error[128];
  char expected_error[128];

  config_init(&cfg);
  TT_ASSERT_FALSE(config_read_string(&cfg, input_text));

  snprintf(expected_error, sizeof(expected_error), "(null):%s", parse_error);

  snprintf(actual_error, sizeof(actual_error), "%s:%d %s\n",
           config_error_file(&cfg), config_error_line(&cfg),
           config_error_text(&cfg));

  config_destroy(&cfg);

  TT_ASSERT_STR_EQ(actual_error, expected_error);
}

/* ------------------------------------------------------------------------- */

static const char *read_file_to_string(const char *file)
{
  struct stat stbuf;
  FILE *fp;
  int size;
  char *buf;
  size_t r;

  TT_ASSERT_INT_EQ(0, stat(file, &stbuf));

  size = stbuf.st_size;
  buf = (char *)malloc(size + 1);
  TT_ASSERT_PTR_NOTNULL(buf);

  fp = fopen(file, "rt");
  TT_ASSERT_PTR_NOTNULL(fp);

  r = fread(buf, 1, size, fp);
  fclose(fp);

  TT_ASSERT_INT_EQ(size, r);

  *(buf + size) = 0;
  return(buf);
}

/* ------------------------------------------------------------------------- */

TT_TEST(ParsingAndFormatting)
{
  int i;

  for(i = 0;; ++i)
  {
    char input_file[128], output_file[128];
    sprintf(input_file, "testdata/input_%d.cfg", i);
    sprintf(output_file, "testdata/output_%d.cfg", i);
    printf("parsing %s\n", input_file);

    if(!tt_file_exists(input_file) || !tt_file_exists(output_file))
      break;

    parse_and_compare(input_file, output_file);
  }
}

/* ------------------------------------------------------------------------- */

TT_TEST(ParseInvalidFiles)
{
  int i;

  for(i = 0;; ++i)
  {
    char input_file[128], error_file[128];
    char error_text[128];
    FILE *fp;

    sprintf(input_file, "testdata/bad_input_%d.cfg", i);
    sprintf(error_file, "testdata/parse_error_%d.txt", i);

    if(!tt_file_exists(input_file) || !tt_file_exists(error_file))
      break;

    fp = fopen(error_file, "rt");
    TT_ASSERT_PTR_NOTNULL(fp);
    TT_ASSERT_PTR_NOTNULL(fgets(error_text, sizeof(error_text), fp));
    fclose(fp);

    parse_file_and_compare_error(input_file, error_text);
  }
}

/* ------------------------------------------------------------------------- */

TT_TEST(ParseInvalidStrings)
{
  int i;

  for(i = 0;; ++i)
  {
    char input_file[128], error_file[128];
    const char *input_text;
    char error_text[128];
    FILE *fp;

    sprintf(input_file, "testdata/bad_input_%d.cfg", i);
    sprintf(error_file, "testdata/parse_error_%d.txt", i);

    if(!tt_file_exists(input_file) || !tt_file_exists(error_file))
      break;

    input_text = read_file_to_string(input_file);

    fp = fopen(error_file, "rt");
    TT_ASSERT_PTR_NOTNULL(fp);
    TT_ASSERT_PTR_NOTNULL(fgets(error_text, sizeof(error_text), fp));
    fclose(fp);

    parse_string_and_compare_error(input_text, error_text);

    free((void *)input_text);
  }
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt1)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival;
  long long llval;

  buf = "someint=5;";

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT_EQ(ival, 5);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT_EQ(llval, 5);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt2)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival = 123;
  long long llval;

  buf = "someint=8589934592;"; /* 2^33 */

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  /* Should fail because value was parsed as an int64. */
  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_FALSE(rc);
  TT_ASSERT_INT_EQ(ival, 123);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT64_EQ(llval, 8589934592LL);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt3)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival = 123;
  long long llval;

  buf = "someint=-8589934592;"; /* -2^33 */

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  /* Should fail because value was parsed as an int64. */
  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_FALSE(rc);
  TT_ASSERT_INT_EQ(ival, 123);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT64_EQ(llval, -8589934592LL);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt4)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival = 123;
  long long llval;

  buf = "someint=2147483647;";  /* 2^31-1 */

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT_EQ(ival, 2147483647);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT64_EQ(llval, 2147483647LL);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt5)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival = 123;
  long long llval;

  buf = "someint=2147483648;"; /* 2^31 */

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  /* Should fail because value was parsed as an int64. */
  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_FALSE(rc);
  TT_ASSERT_INT_EQ(ival, 123);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT64_EQ(llval, 2147483648LL);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt6)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival;
  long long llval;

  buf = "someint=-2147483648;"; /* -2^31 */

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT_EQ(ival, -2147483648LL);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT64_EQ(llval, -2147483648LL);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(BigInt7)
{
  char *buf;
  config_t cfg;
  int rc;
  int ival = 123;
  long long llval;

  buf = "someint=-2147483649;"; /* -2^31-1 */

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  /* Should fail because value was parsed as an int64. */
  rc = config_lookup_int(&cfg, "someint", &ival);
  TT_ASSERT_FALSE(rc);
  TT_ASSERT_INT_EQ(ival, 123);

  rc = config_lookup_int64(&cfg, "someint", &llval);
  TT_ASSERT_TRUE(rc);
  TT_ASSERT_INT64_EQ(llval, -2147483649LL);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(RemoveSetting)
{
  char *buf;
  config_t cfg;
  int rc;
  config_setting_t* rootSetting;

  buf = "a:{b:3;c:4;}";

  config_init(&cfg);
  rc = config_read_string(&cfg, buf);
  TT_ASSERT_TRUE(rc);

  rootSetting = config_root_setting(&cfg);
  rc = config_setting_remove(rootSetting, "a.c");
  TT_ASSERT_TRUE(rc);

  /* a and a.b are found */
  rootSetting = config_lookup(&cfg, "a");
  TT_EXPECT_PTR_NOTNULL(rootSetting);
  rootSetting = config_lookup(&cfg, "a.b");
  TT_EXPECT_PTR_NOTNULL(rootSetting);
  rootSetting = config_lookup(&cfg, "a.c");
  TT_EXPECT_PTR_NULL(rootSetting);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(EscapedStrings)
{
  config_t cfg;
  int ok;
  const char *str;

  config_init(&cfg);
  config_set_include_dir(&cfg, "./testdata");

  ok = config_read_file(&cfg, "testdata/strings.cfg");
  if(!ok)
  {
    printf("error: %s:%d\n", config_error_text(&cfg),
           config_error_line(&cfg));
  }
  TT_ASSERT_TRUE(ok);

  ok = config_lookup_string(&cfg, "escape_seqs.str", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc", str);

  ok = config_lookup_string(&cfg, "escape_seqs.newline", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc\ndef\n", str);

  ok = config_lookup_string(&cfg, "escape_seqs.cr", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc\rdef\r", str);

  ok = config_lookup_string(&cfg, "escape_seqs.tab", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc\tdef\t", str);

  ok = config_lookup_string(&cfg, "escape_seqs.feed", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc\fdef\f", str);

  ok = config_lookup_string(&cfg, "escape_seqs.backslash", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc\\def\\", str);

  ok = config_lookup_string(&cfg, "escape_seqs.dquote", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("abc\"def\"", str);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

TT_TEST(OverrideSetting)
{
  config_t cfg;
  int ok;
  int ival;
  const char *str;

  config_init(&cfg);
  config_set_options(&cfg, CONFIG_OPTION_ALLOW_OVERRIDES);
  config_set_include_dir(&cfg, "./testdata");

  ok = config_read_file(&cfg, "testdata/override_setting.cfg");
  if(!ok)
  {
    printf("error: %s:%d\n", config_error_text(&cfg),
           config_error_line(&cfg));
  }
  TT_ASSERT_TRUE(ok);

  ok = config_lookup_string(&cfg, "group.message", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("overridden", str);

  ok = config_lookup_string(&cfg, "group.inner.name", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("overridden", str);

  ok = config_lookup_string(&cfg, "group.inner.other", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("other", str);

  ok = config_lookup_string(&cfg, "group.inner.none", &str);
  TT_ASSERT_FALSE(ok);

  ok = config_lookup_string(&cfg, "group.inner.other", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("other", str);

  ok = config_lookup_string(&cfg, "string", &str);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_STR_EQ("overridden", str);

  ok = config_lookup_int(&cfg, "int", &ival);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_INT_EQ(ival, 2);

  ok = config_lookup_int(&cfg, "group.array.[0]", &ival);
  TT_ASSERT_TRUE(ok);
  TT_ASSERT_INT_EQ(ival, 3);

  ok = config_lookup_int(&cfg, "group.array.[1]", &ival);
  TT_ASSERT_FALSE(ok);

  config_destroy(&cfg);
}

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
  int failures;

  TT_SUITE_START(LibConfigTests);
  TT_SUITE_TEST(LibConfigTests, ParsingAndFormatting);
  TT_SUITE_TEST(LibConfigTests, ParseInvalidFiles);
  TT_SUITE_TEST(LibConfigTests, ParseInvalidStrings);
  TT_SUITE_TEST(LibConfigTests, BigInt1);
  TT_SUITE_TEST(LibConfigTests, BigInt2);
  TT_SUITE_TEST(LibConfigTests, BigInt3);
  TT_SUITE_TEST(LibConfigTests, BigInt4);
  TT_SUITE_TEST(LibConfigTests, BigInt5);
  TT_SUITE_TEST(LibConfigTests, BigInt6);
  TT_SUITE_TEST(LibConfigTests, BigInt7);
  TT_SUITE_TEST(LibConfigTests, RemoveSetting);
  TT_SUITE_TEST(LibConfigTests, EscapedStrings);
  TT_SUITE_TEST(LibConfigTests, OverrideSetting);
  TT_SUITE_RUN(LibConfigTests);
  failures = TT_SUITE_NUM_FAILURES(LibConfigTests);
  TT_SUITE_END(LibConfigTests);

  if (failures)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
