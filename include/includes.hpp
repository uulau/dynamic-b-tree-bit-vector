// Copyright (c) 2017, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#pragma once

#include "stdint.h"
#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
#include <map>
#include <cmath>
#include <algorithm>

#define WORD_SIZE 64;

using namespace std;

typedef unsigned char uchar;
typedef uint64_t ulint;

typedef unsigned char symbol;
typedef pair<uint64_t, uint64_t> range_t;