/*
 * Copyright (C) Internet Systems Consortium, Inc. ("ISC")
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * See the COPYRIGHT file distributed with this work for additional
 * information regarding copyright ownership.
 */

#pragma once

#include <inttypes.h>

#include <isc/assertions.h>
#include <isc/atomic.h>
#include <isc/error.h>
#include <isc/lang.h>
#include <isc/mutex.h>
#include <isc/platform.h>
#include <isc/types.h>

/*! \file isc/refcount.h
 * \brief Implements a locked reference counter.
 *
 * These functions may actually be
 * implemented using macros, and implementations of these macros are below.
 * The isc_refcount_t type should not be accessed directly, as its contents
 * depend on the implementation.
 */

ISC_LANG_BEGINDECLS

/*
 * Function prototypes
 */

/*
 * isc_result_t
 * isc_refcount_init(isc_refcount_t *ref, unsigned int n);
 *
 * Initialize the reference counter.  There will be 'n' initial references.
 *
 * Requires:
 *	ref != NULL
 */

/*
 * void
 * isc_refcount_destroy(isc_refcount_t *ref);
 *
 * Destroys a reference counter.
 *
 * Requires:
 *	ref != NULL
 *	The number of references is 0.
 */

/*
 * void
 * isc_refcount_increment(isc_refcount_t *ref, unsigned int *targetp);
 * isc_refcount_increment0(isc_refcount_t *ref, unsigned int *targetp);
 *
 * Increments the reference count, returning the new value in targetp if it's
 * not NULL.  The reference counter typically begins with the initial counter
 * of 1, and will be destroyed once the counter reaches 0.  Thus,
 * isc_refcount_increment() additionally requires the previous counter be
 * larger than 0 so that an error which violates the usage can be easily
 * caught.  isc_refcount_increment0() does not have this restriction.
 *
 * Requires:
 *	ref != NULL.
 */

/*
 * void
 * isc_refcount_decrement(isc_refcount_t *ref, unsigned int *targetp);
 *
 * Decrements the reference count,  returning the new value in targetp if it's
 * not NULL.
 *
 * Requires:
 *	ref != NULL.
 */


/*
 * Sample implementations
 */
#ifdef ISC_PLATFORM_USETHREADS

typedef struct isc_refcount {
	atomic_int_fast32_t refs;
} isc_refcount_t;

#define isc_refcount_init(rp, n)				\
	atomic_init(&(rp)->refs, n)

#define isc_refcount_current(rp)				\
	atomic_load_explicit(&(rp)->refs, memory_order_relaxed)

#define isc_refcount_destroy(rp)				\
	ISC_REQUIRE(isc_refcount_current(rp) == 0)

#define isc_refcount_increment0(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int32_t prev;					\
		prev = atomic_fetch_add_explicit		\
			(&(rp)->refs, 1, memory_order_relaxed);	\
		if (_tmp != NULL)				\
			*_tmp = prev + 1;			\
	} while (0)

#define isc_refcount_increment(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int32_t prev;					\
		prev = atomic_fetch_add_explicit		\
			(&(rp)->refs, 1, memory_order_relaxed);	\
		ISC_REQUIRE(prev > 0);				\
		if (_tmp != NULL)				\
			*_tmp = prev + 1;			\
	} while (0)

#define isc_refcount_decrement(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int32_t prev;					\
		prev = atomic_fetch_sub_explicit		\
			(&(rp)->refs, 1, memory_order_relaxed); \
		ISC_REQUIRE(prev > 0);				\
		if (_tmp != NULL)				\
			*_tmp = prev - 1;			\
	} while (0)

#else /* ISC_PLATFORM_USETHREADS */

typedef struct isc_refcount {
	int refs;
} isc_refcount_t;

#define isc_refcount_init(rp, n)				\
	(&(rp)->refs = n)

#define isc_refcount_destroy(rp) ISC_REQUIRE((rp)->refs == 0)
#define isc_refcount_current(rp) ((unsigned int)((rp)->refs))

#define isc_refcount_increment0(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int _n = ++(rp)->refs;				\
		if (_tmp != NULL)				\
			*_tmp = _n;				\
	} while (0)

#define isc_refcount_increment(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int _n;						\
		ISC_REQUIRE((rp)->refs > 0);			\
		_n = ++(rp)->refs;				\
		if (_tmp != NULL)				\
			*_tmp = _n;				\
	} while (0)

#define isc_refcount_decrement(rp, tp)				\
	do {							\
		unsigned int *_tmp = (unsigned int *)(tp);	\
		int _n;						\
		ISC_REQUIRE((rp)->refs > 0);			\
		_n = --(rp)->refs;				\
		if (_tmp != NULL)				\
			*_tmp = _n;				\
	} while (0)

#endif /* ISC_PLATFORM_USETHREADS */

ISC_LANG_ENDDECLS
