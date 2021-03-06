/**
 * Copyright 2013 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include"config.h"
#include<stdexcept>
#include"gtest/gtest.h"

#include"test_util.h"

static void
reset_getopt()
{
#if HAVE_DECL_OPTRESET
  optreset = 1;
#endif
  optind = 1;
}

extern int wrapped_main(int, char**);

TEST(Usage, NoOpts)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    NULL,
  };
  EXPECT_EQ(1, wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv));
  EXPECT_TRUE(s.stdout().find("\nUsage: ") != std::string::npos);
  EXPECT_EQ("stpm-sign: Need to specify keyfile and data file\n", s.stderr());
  EXPECT_EQ("", s.stdlog());
}

TEST(Sign, NoDataFile)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-k",
    (char*)"",
    NULL,
  };
  EXPECT_EQ(1, wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv));
  EXPECT_TRUE(s.stdout().find("\nUsage: ") != std::string::npos);
  EXPECT_EQ("stpm-sign: Need to specify keyfile and data file\n", s.stderr());
  EXPECT_EQ("", s.stdlog());
}

TEST(Sign, NoKeyFile)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-f",
    (char*)"",
    NULL,
  };
  EXPECT_EQ(1, wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv));
  EXPECT_TRUE(s.stdout().find("\nUsage: ") != std::string::npos);
  EXPECT_EQ("stpm-sign: Need to specify keyfile and data file\n", s.stderr());
  EXPECT_EQ("", s.stdlog());
}

TEST(Usage, HelpOpts)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-h",
    NULL,
  };
  EXPECT_EQ(0, wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv));
  EXPECT_TRUE(s.stdout().find("\nUsage: ") != std::string::npos);
  EXPECT_EQ("", s.stderr());
  EXPECT_EQ("", s.stdlog());
}

TEST(Sign, BadKeyfileName)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-k",
    (char*)"/non/existing/file/here/3ht.sn,hsn",
    (char*)"-f",
    (char*)"/dev/null",
    NULL,
  };
  EXPECT_THROW(wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv),
               std::runtime_error);
  EXPECT_EQ("", s.stdout());
  EXPECT_EQ("", s.stderr());
  EXPECT_EQ("", s.stdlog());
}

TEST(Sign, BadDatafileName)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-k",
    (char*)"testdata/correct.key",
    (char*)"-f",
    (char*)"/non/existing/file/here/3ht.sn,hsn",
    NULL,
  };
  EXPECT_THROW(wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv),
               std::runtime_error);
  EXPECT_EQ("", s.stdlog());
  EXPECT_EQ("", s.stdout());
  EXPECT_EQ("", s.stderr());
  EXPECT_EQ("", s.stdlog());
}

TEST(Sign, BadKeyfile)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-k",
    (char*)"testdata/broken.key",
    (char*)"-f",
    (char*)"/dev/null",
    NULL,
  };
  bool threw = false;
  try {
    wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv);
  } catch (const std::runtime_error& e) {
    EXPECT_EQ("Keyfile format error(line 2: typo 010001)",
              std::string(e.what()));
    EXPECT_EQ("", s.stdlog());
    EXPECT_EQ("", s.stdout());
    EXPECT_EQ("", s.stderr());
    threw = true;
  }
  EXPECT_TRUE(threw);
}

TEST(Sign, OK)
{
  CaptureStreams s;
  reset_getopt();
  char *argv[] = {
    (char*)"sign",
    (char*)"-k",
    (char*)"testdata/correct.key",
    (char*)"-f",
    (char*)"/dev/null",
    NULL,
  };
  EXPECT_EQ(0, wrapped_main(sizeof(argv)/sizeof(void*) - 1, argv));
  EXPECT_EQ("Loaded key: mod=010203,exp=010001,blob=010203040506\n--- Signature ---\n12345678\n", s.stdout());
  EXPECT_EQ("", s.stderr());
  EXPECT_EQ("", s.stdlog());
}
/* ---- Emacs Variables ----
 * Local Variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
