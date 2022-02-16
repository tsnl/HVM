// NOTE: the below header was copied from the compiler for the V programming language
//       on 9 Feb, 2022 at ~11:15p PT (UTC-8:00)
// Updates upstream must be manually integrated.
// The devs are extremely grateful to the authors of this header, who kindly licensed it
// under the MIT license.
// Permalink: https://github.com/vlang/v/blob/e93a52a267b7649d92da13aa4b035cbaf4079eee/thirdparty/stdatomic/nix/atomic.h

/*
    Compability header for stdatomic.h that works for all compilers supported
    by V. For TCC libatomic from the operating system is used
*/
#ifndef __ATOMIC_H
#   define __ATOMIC_H

#ifndef __cplusplus
//  If C just use stdatomic.h
#   ifndef __TINYC__
#       include <stdatomic.h>
#   endif
#else
// CPP wrapper for atomic operations that are compatible with C
// Normally redirects to a different file.
// #error "this header is not compatible with C++"
#endif

#ifdef __TINYC__

// (for HVM)
// Additional definitions for atomic_bool, atomic_flag
// No native support for _Atomic(). Place object in structure to prevent
// most forms of direct non-atomic access.
#define _Atomic(T) T volatile
typedef _Atomic(_Bool) atomic_bool;
typedef _Atomic(char) atomic_char;
typedef _Atomic(signed char) atomic_schar;
typedef _Atomic(unsigned char) atomic_uchar;
typedef _Atomic(short) atomic_short;
typedef _Atomic(unsigned short) atomic_ushort;
typedef _Atomic(int) atomic_int;
typedef _Atomic(unsigned int) atomic_uint;
typedef _Atomic(long) atomic_long;
typedef _Atomic(unsigned long) atomic_ulong;
typedef _Atomic(long long) atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(uintptr_t) atomic_uintptr_t;

// use functions for 64, 32 and 8 bit from libatomic directly
// since tcc is not capible to use "generic" C functions
// there is no header file for libatomic so we provide function declarations here

extern unsigned long long __atomic_load_8(unsigned long long* x, int mo);
extern void __atomic_store_8(unsigned long long* x, unsigned long long y, int mo);
extern _Bool __atomic_compare_exchange_8(unsigned long long* x, unsigned long long* expected, unsigned long long y, int mo, int mo2);
extern _Bool __atomic_compare_exchange_8(unsigned long long* x, unsigned long long* expected, unsigned long long y, int mo, int mo2);
extern unsigned long long __atomic_exchange_8(unsigned long long* x, unsigned long long y, int mo);
extern unsigned long long __atomic_fetch_add_8(unsigned long long* x, unsigned long long y, int mo);
extern unsigned long long __atomic_fetch_sub_8(unsigned long long* x, unsigned long long y, int mo);
extern unsigned long long __atomic_fetch_and_8(unsigned long long* x, unsigned long long y, int mo);
extern unsigned long long __atomic_fetch_or_8(unsigned long long* x, unsigned long long y, int mo);
extern unsigned long long __atomic_fetch_xor_8(unsigned long long* x, unsigned long long y, int mo);

extern unsigned int __atomic_load_4(unsigned int* x, int mo);
extern void __atomic_store_4(unsigned int* x, unsigned int y, int mo);
extern _Bool __atomic_compare_exchange_4(unsigned int* x, unsigned int* expected, unsigned int y, int mo, int mo2);
extern _Bool __atomic_compare_exchange_4(unsigned int* x, unsigned int* expected, unsigned int y, int mo, int mo2);
extern unsigned int __atomic_exchange_4(unsigned int* x, unsigned int y, int mo);
extern unsigned int __atomic_fetch_add_4(unsigned int* x, unsigned int y, int mo);
extern unsigned int __atomic_fetch_sub_4(unsigned int* x, unsigned int y, int mo);
extern unsigned int __atomic_fetch_and_4(unsigned int* x, unsigned int y, int mo);
extern unsigned int __atomic_fetch_or_4(unsigned int* x, unsigned int y, int mo);
extern unsigned int __atomic_fetch_xor_4(unsigned int* x, unsigned int y, int mo);

extern unsigned short __atomic_load_2(unsigned short* x, int mo);
extern void __atomic_store_2(unsigned short* x, unsigned short y, int mo);
extern _Bool __atomic_compare_exchange_2(unsigned short* x, unsigned short* expected, unsigned short y, int mo, int mo2);
extern _Bool __atomic_compare_exchange_2(unsigned short* x, unsigned short* expected, unsigned short y, int mo, int mo2);
extern unsigned short __atomic_exchange_2(unsigned short* x, unsigned short y, int mo);
extern unsigned short __atomic_fetch_add_2(unsigned short* x, unsigned short y, int mo);
extern unsigned short __atomic_fetch_sub_2(unsigned short* x, unsigned short y, int mo);
extern unsigned short __atomic_fetch_and_2(unsigned short* x, unsigned short y, int mo);
extern unsigned short __atomic_fetch_or_2(unsigned short* x, unsigned short y, int mo);
extern unsigned short __atomic_fetch_xor_2(unsigned short* x, unsigned short y, int mo);

extern unsigned char __atomic_load_1(unsigned char* x, int mo);
extern void __atomic_store_1(unsigned char* x, unsigned char y, int mo);
extern _Bool __atomic_compare_exchange_1(unsigned char* x, unsigned char* expected, unsigned char y, int mo, int mo2);
extern _Bool __atomic_compare_exchange_1(unsigned char* x, unsigned char* expected, unsigned char y, int mo, int mo2);
extern unsigned char __atomic_exchange_1(unsigned char* x, unsigned char y, int mo);
extern unsigned char __atomic_fetch_add_1(unsigned char* x, unsigned char y, int mo);
extern unsigned char __atomic_fetch_sub_1(unsigned char* x, unsigned char y, int mo);
extern unsigned char __atomic_fetch_and_1(unsigned char* x, unsigned char y, int mo);
extern unsigned char __atomic_fetch_or_1(unsigned char* x, unsigned char y, int mo);
extern unsigned char __atomic_fetch_xor_1(unsigned char* x, unsigned char y, int mo);

// The default functions should work with pointers so we have to decide based on pointer size
#if UINTPTR_MAX == 0xFFFFFFFF

#define atomic_load_explicit __atomic_load_4
#define atomic_store_explicit __atomic_store_4
#define atomic_compare_exchange_weak_explicit __atomic_compare_exchange_4
#define atomic_compare_exchange_strong_explicit __atomic_compare_exchange_4
#define atomic_exchange_explicit __atomic_exchange_4
#define atomic_fetch_add_explicit __atomic_fetch_add_4
#define atomic_fetch_sub_explicit __atomic_sub_fetch_4

#else

#define atomic_load_explicit __atomic_load_8
#define atomic_store_explicit __atomic_store_8
#define atomic_compare_exchange_weak_explicit __atomic_compare_exchange_8
#define atomic_compare_exchange_strong_explicit __atomic_compare_exchange_8
#define atomic_exchange_explicit __atomic_exchange_8
#define atomic_fetch_add_explicit __atomic_fetch_add_8
#define atomic_fetch_sub_explicit __atomic_sub_fetch_8

#endif

// memory order policies - we use "sequentially consistent" by default

#define memory_order int
#define memory_order_relaxed 0
#define memory_order_consume 1
#define memory_order_acquire 2
#define memory_order_release 3
#define memory_order_acq_rel 4
#define memory_order_seq_cst 5

static inline uintptr_t atomic_load(uintptr_t* x) {
	return atomic_load_explicit(x, memory_order_seq_cst);
}
static inline void atomic_store(uintptr_t* x, uintptr_t y) {
	atomic_store_explicit(x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak(uintptr_t* x, uintptr_t* expected, uintptr_t y) {
	return (int)atomic_compare_exchange_weak_explicit(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong(uintptr_t* x, uintptr_t* expected, uintptr_t y) {
	return (int)atomic_compare_exchange_strong_explicit(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline uintptr_t atomic_exchange(uintptr_t* x, uintptr_t y) {
	return atomic_exchange_explicit(x, y, memory_order_seq_cst);
}
static inline uintptr_t atomic_fetch_add(uintptr_t* x, uintptr_t y) {
	return atomic_fetch_add_explicit(x, y, memory_order_seq_cst);
}
static inline uintptr_t atomic_fetch_sub(uintptr_t* x, uintptr_t y) {
	return atomic_fetch_sub_explicit(x, y, memory_order_seq_cst);
}
static inline uintptr_t atomic_fetch_and(uintptr_t* x, uintptr_t y) {
	return atomic_fetch_and_explicit(x, y, memory_order_seq_cst);
}
static inline uintptr_t atomic_fetch_or(uintptr_t* x, uintptr_t y) {
	return atomic_fetch_or_explicit(x, y, memory_order_seq_cst);
}
static inline uintptr_t atomic_fetch_xor(uintptr_t* x, uintptr_t y) {
	return atomic_fetch_xor_explicit(x, y, memory_order_seq_cst);
}

#define atomic_load_ptr atomic_load
#define atomic_store_ptr atomic_store
#define atomic_compare_exchange_weak_ptr atomic_compare_exchange_weak
#define atomic_compare_exchange_strong_ptr atomic_compare_exchange_strong
#define atomic_exchange_ptr atomic_exchange
#define atomic_fetch_add_ptr atomic_fetch_add
#define atomic_fetch_sub_ptr atomic_fetch_sub
#define atomic_fetch_and_ptr atomic_fetch_and
#define atomic_fetch_or_ptr atomic_fetch_or
#define atomic_fetch_xor_ptr atomic_fetch_xor

// specialized versions for 64 bit

static inline unsigned long long atomic_load_u64(unsigned long long* x) {
	return __atomic_load_8(x, memory_order_seq_cst);
}
static inline void atomic_store_u64(unsigned long long* x, unsigned long long y) {
	__atomic_store_8(x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_u64(unsigned long long* x, unsigned long long* expected, unsigned long long y) {
	return (int)__atomic_compare_exchange_8(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_u64(unsigned long long* x, unsigned long long* expected, unsigned long long y) {
	return (int)__atomic_compare_exchange_8(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned long long atomic_exchange_u64(unsigned long long* x, unsigned long long y) {
	return __atomic_exchange_8(x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_add_u64(unsigned long long* x, unsigned long long y) {
	return __atomic_fetch_add_8(x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_sub_u64(unsigned long long* x, unsigned long long y) {
	return __atomic_fetch_sub_8(x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_and_u64(unsigned long long* x, unsigned long long y) {
	return __atomic_fetch_and_8(x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_or_u64(unsigned long long* x, unsigned long long y) {
	return __atomic_fetch_or_8(x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_xor_u64(unsigned long long* x, unsigned long long y) {
	return __atomic_fetch_xor_8(x, y, memory_order_seq_cst);
}

static inline unsigned atomic_load_u32(unsigned* x) {
	return __atomic_load_4(x, memory_order_seq_cst);
}
static inline void atomic_store_u32(unsigned* x, unsigned y) {
	__atomic_store_4(x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_u32(unsigned* x, unsigned* expected, unsigned y) {
	return (int)__atomic_compare_exchange_4(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_u32(unsigned* x, unsigned* expected, unsigned y) {
	return (int)__atomic_compare_exchange_4(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned atomic_exchange_u32(unsigned* x, unsigned y) {
	return __atomic_exchange_4(x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_add_u32(unsigned* x, unsigned y) {
	return __atomic_fetch_add_4(x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_sub_u32(unsigned* x, unsigned y) {
	return __atomic_fetch_sub_4(x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_and_u32(unsigned* x, unsigned y) {
	return __atomic_fetch_and_4(x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_or_u32(unsigned* x, unsigned y) {
	return __atomic_fetch_or_4(x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_xor_u32(unsigned* x, unsigned y) {
	return __atomic_fetch_xor_4(x, y, memory_order_seq_cst);
}

static inline unsigned short atomic_load_u16(unsigned short* x) {
	return __atomic_load_2(x, memory_order_seq_cst);
}
static inline void atomic_store_u16(unsigned short* x, unsigned short y) {
	__atomic_store_2(x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_u16(unsigned short* x, unsigned short* expected, unsigned short y) {
	return (int)__atomic_compare_exchange_2(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_u16(unsigned short* x, unsigned short* expected, unsigned short y) {
	return (int)__atomic_compare_exchange_2(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned short atomic_exchange_u16(unsigned short* x, unsigned short y) {
	return __atomic_exchange_2(x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_add_u16(unsigned short* x, unsigned short y) {
	return __atomic_fetch_add_2(x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_sub_u16(unsigned short* x, unsigned short y) {
	return __atomic_fetch_sub_2(x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_and_u16(unsigned short* x, unsigned short y) {
	return __atomic_fetch_and_2(x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_or_u16(unsigned short* x, unsigned short y) {
	return __atomic_fetch_or_2(x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_xor_u16(unsigned short* x, unsigned short y) {
	return __atomic_fetch_xor_2(x, y, memory_order_seq_cst);
}

static inline unsigned char atomic_load_byte(unsigned char* x) {
	return __atomic_load_1(x, memory_order_seq_cst);
}
static inline void atomic_store_byte(unsigned char* x, unsigned char y) {
	__atomic_store_1(x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_byte(unsigned char* x, unsigned char* expected, unsigned char y) {
	return __atomic_compare_exchange_1(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_byte(unsigned char* x, unsigned char* expected, unsigned char y) {
	return __atomic_compare_exchange_1(x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned char atomic_exchange_byte(unsigned char* x, unsigned char y) {
	return __atomic_exchange_1(x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_add_byte(unsigned char* x, unsigned char y) {
	return __atomic_fetch_add_1(x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_sub_byte(unsigned char* x, unsigned char y) {
	return __atomic_fetch_sub_1(x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_and_byte(unsigned char* x, unsigned char y) {
	return __atomic_fetch_and_1(x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_or_byte(unsigned char* x, unsigned char y) {
	return __atomic_fetch_or_1(x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_xor_byte(unsigned char* x, unsigned char y) {
	return __atomic_fetch_xor_1(x, y, memory_order_seq_cst);
}

/* (from atomic_cpp.h)
 * 7.17.8 Atomic flag type and operations.
 *
 * XXX: Assume atomic_bool can be used as an atomic_flag. Is there some
 * kind of compiler built-in type we could use?
 */
typedef struct { atomic_bool __flag; } atomic_flag;
#ifdef __ATOMICS_AVOID_DOT_INIT
#	define ATOMIC_VAR_INIT(value) {value}
#else
#	define ATOMIC_VAR_INIT(value) {.__val = (value)}
#endif
#define ATOMIC_FLAG_INIT {ATOMIC_VAR_INIT(false)}

static inline _Bool
atomic_flag_test_and_set_explicit(volatile atomic_flag *__object, memory_order __order) {
    return (atomic_exchange_explicit(&__object->__flag, 1, __order));
}
static inline void
atomic_flag_clear_explicit(volatile atomic_flag *__object, memory_order __order) {
    atomic_store_explicit(&__object->__flag, 0, __order);
}
static inline _Bool
atomic_flag_test_and_set(volatile atomic_flag *__object) {
    return (atomic_flag_test_and_set_explicit(__object, memory_order_seq_cst));
}
static inline void
atomic_flag_clear(volatile atomic_flag *__object) {
    atomic_flag_clear_explicit(__object, memory_order_seq_cst);
}

#else

// (non-TCC)

// Since V might be confused with "generic" C functions either we provide special versions
// for gcc/clang, too
static inline unsigned long long atomic_load_u64(unsigned long long* x) {
	return atomic_load_explicit((_Atomic unsigned long long*)x, memory_order_seq_cst);
}
static inline void atomic_store_u64(unsigned long long* x, unsigned long long y) {
	atomic_store_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_u64(unsigned long long* x, unsigned long long* expected, unsigned long long y) {
	return (int)atomic_compare_exchange_weak_explicit((_Atomic unsigned long long*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_u64(unsigned long long* x, unsigned long long* expected, unsigned long long y) {
	return (int)atomic_compare_exchange_strong_explicit((_Atomic unsigned long long*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned long long atomic_exchange_u64(unsigned long long* x, unsigned long long y) {
	return atomic_exchange_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_add_u64(unsigned long long* x, unsigned long long y) {
	return atomic_fetch_add_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_sub_u64(unsigned long long* x, unsigned long long y) {
	return atomic_fetch_sub_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_and_u64(unsigned long long* x, unsigned long long y) {
	return atomic_fetch_and_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_or_u64(unsigned long long* x, unsigned long long y) {
	return atomic_fetch_or_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}
static inline unsigned long long atomic_fetch_xor_u64(unsigned long long* x, unsigned long long y) {
	return atomic_fetch_xor_explicit((_Atomic unsigned long long*)x, y, memory_order_seq_cst);
}


static inline void* atomic_load_ptr(void** x) {
	return atomic_load_explicit((_Atomic uintptr_t*)x, memory_order_seq_cst);
}
static inline void atomic_store_ptr(void** x, void* y) {
	atomic_store_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_ptr(void** x, void** expected, void* y) {
	return (int)atomic_compare_exchange_weak_explicit((_Atomic uintptr_t*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_ptr(void** x, void** expected, void* y) {
	return (int)atomic_compare_exchange_strong_explicit((_Atomic uintptr_t*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline void* atomic_exchange_ptr(void** x, void* y) {
	return atomic_exchange_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}
static inline void* atomic_fetch_add_ptr(void** x, void* y) {
	return atomic_fetch_add_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}
static inline void* atomic_fetch_sub_ptr(void** x, void* y) {
	return atomic_fetch_sub_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}
static inline void* atomic_fetch_and_ptr(void** x, void* y) {
	return atomic_fetch_and_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}
static inline void* atomic_fetch_or_ptr(void** x, void* y) {
	return atomic_fetch_or_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}
static inline void* atomic_fetch_xor_ptr(void** x, void* y) {
	return atomic_fetch_xor_explicit((_Atomic uintptr_t*)x, y, memory_order_seq_cst);
}


static inline unsigned atomic_load_u32(unsigned* x) {
	return atomic_load_explicit((_Atomic unsigned*)x, memory_order_seq_cst);
}
static inline void atomic_store_u32(unsigned* x, unsigned y) {
	atomic_store_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_u32(unsigned* x, unsigned* expected, unsigned y) {
	return (int)atomic_compare_exchange_weak_explicit((_Atomic unsigned*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_u32(unsigned* x, unsigned* expected, unsigned y) {
	return (int)atomic_compare_exchange_strong_explicit((_Atomic unsigned*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned atomic_exchange_u32(unsigned* x, unsigned y) {
	return atomic_exchange_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_add_u32(unsigned* x, unsigned y) {
	return atomic_fetch_add_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_sub_u32(unsigned* x, unsigned y) {
	return atomic_fetch_sub_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_and_u32(unsigned* x, unsigned y) {
	return atomic_fetch_and_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_or_u32(unsigned* x, unsigned y) {
	return atomic_fetch_or_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}
static inline unsigned atomic_fetch_xor_u32(unsigned* x, unsigned y) {
	return atomic_fetch_xor_explicit((_Atomic unsigned*)x, y, memory_order_seq_cst);
}

static inline unsigned short atomic_load_u16(unsigned short* x) {
	return atomic_load_explicit((_Atomic unsigned short*)x, memory_order_seq_cst);
}
static inline void atomic_store_u16(unsigned short* x, unsigned short y) {
	atomic_store_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_u16(unsigned short* x, unsigned short* expected, unsigned short y) {
	return (int)atomic_compare_exchange_weak_explicit((_Atomic unsigned short*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_u16(unsigned short* x, unsigned short* expected, unsigned short y) {
	return (int)atomic_compare_exchange_strong_explicit((_Atomic unsigned short*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned short atomic_exchange_u16(unsigned short* x, unsigned short y) {
	return atomic_exchange_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_add_u16(unsigned short* x, unsigned short y) {
	return atomic_fetch_add_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_sub_u16(unsigned short* x, unsigned short y) {
	return atomic_fetch_sub_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_and_u16(unsigned short* x, unsigned short y) {
	return atomic_fetch_and_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_or_u16(unsigned short* x, unsigned short y) {
	return atomic_fetch_or_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}
static inline unsigned short atomic_fetch_xor_u16(unsigned short* x, unsigned short y) {
	return atomic_fetch_xor_explicit((_Atomic unsigned short*)x, y, memory_order_seq_cst);
}

static inline unsigned char atomic_load_byte(unsigned char* x) {
	return atomic_load_explicit((_Atomic unsigned char*)x, memory_order_seq_cst);
}
static inline void atomic_store_byte(unsigned char* x, unsigned char y) {
	atomic_store_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_weak_byte(unsigned char* x, unsigned char* expected, unsigned char y) {
	return (int)atomic_compare_exchange_weak_explicit((_Atomic unsigned char*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline int atomic_compare_exchange_strong_byte(unsigned char* x, unsigned char* expected, unsigned char y) {
	return (int)atomic_compare_exchange_strong_explicit((_Atomic unsigned char*)x, expected, y, memory_order_seq_cst, memory_order_seq_cst);
}
static inline unsigned char atomic_exchange_byte(unsigned char* x, unsigned char y) {
	return atomic_exchange_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_add_byte(unsigned char* x, unsigned char y) {
	return atomic_fetch_add_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_sub_byte(unsigned char* x, unsigned char y) {
	return atomic_fetch_sub_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_and_byte(unsigned char* x, unsigned char y) {
	return atomic_fetch_and_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_or_byte(unsigned char* x, unsigned char y) {
	return atomic_fetch_or_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}
static inline unsigned char atomic_fetch_xor_byte(unsigned char* x, unsigned char y) {
	return atomic_fetch_xor_explicit((_Atomic unsigned char*)x, y, memory_order_seq_cst);
}

#endif
#endif