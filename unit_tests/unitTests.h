#pragma once

#include "/workspaces/cse3310_s004_group_18/unit_tests/munit/munit.h"
#include "/workspaces/cse3310_s004_group_18/include/copyExtfat.h"
#include "/workspaces/cse3310_s004_group_18/include/directoryExtfat.h"
#include "/workspaces/cse3310_s004_group_18/include/extfat.h"
#include "/workspaces/cse3310_s004_group_18/include/parseArgs.h"
#include "/workspaces/cse3310_s004_group_18/include/routines.h"
#include "/workspaces/cse3310_s004_group_18/include/util.h"

MunitResult test_writeByteInFile(const MunitParameter params[], void* fixture);
MunitResult test_printName(const MunitParameter params[], void* user_data);
MunitResult test_parseArgs(const MunitParameter params[], void* user_data);
