#pragma once

using u8    = unsigned char;
using i8    = signed   char;

using u16   = unsigned short;
using i16   = signed   short;

using u32   = unsigned int;
using i32   = signed   int;

using u64   = unsigned long;
using i64   = signed   long;

using u128  = __uint128_t;
using i128  = __int128_t;

using f32   = float;
using f64   = double;
using f128  = long double;


static_assert(sizeof(u8) == 1,      "u8 type not 8 bit long!");
static_assert(sizeof(i8) == 1,      "i8 type not 8 bit long!");

static_assert(sizeof(u16) == 2,     "u16 type not 16 bit long!");
static_assert(sizeof(i16) == 2,     "i16 type not 16 bit long!");

static_assert(sizeof(u32) == 4,     "u32 type not 32 bit long!");
static_assert(sizeof(i32) == 4,     "i32 type not 32 bit long!");

static_assert(sizeof(u64) == 8,     "u64 type not 64 bit long!");
static_assert(sizeof(i64) == 8,     "i64 type not 64 bit long!");

static_assert(sizeof(u128) == 16,   "u128 type not 128 bit long!");
static_assert(sizeof(i128) == 16,   "i128 type not 128 bit long!");

static_assert(sizeof(f32) == 4,     "f32 type not 32 bit long!");
static_assert(sizeof(f64) == 8,     "f64 type not 64 bit long!");
static_assert(sizeof(f128) == 16,   "f128 type not 128 bit long!");