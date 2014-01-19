#pragma once

#include "PegSolitaire_export.h"

// TODO enhance the EXPORT_TESTS functionality to define EXPORT_TESTS
//#ifdef EXPORT_TESTS
#  define PEGSOLITAIRE_TEST_EXPORT PEGSOLITAIRE_EXPORT
//#else
//#  define PEGSOLITAIRE_TEST_EXPORT
//#endif

// TODO make portable!
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)
