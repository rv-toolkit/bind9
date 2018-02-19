/*
 * Copyright (C) 2004-2007, 2009, 2013-2018  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2000, 2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: hmacmd5.c,v 1.16 2009/02/06 23:47:42 tbox Exp $ */

/*! \file
 * This code implements the HMAC-MD5 keyed hash algorithm
 * described in RFC2104.
 */

#include "config.h"

#include <isc/assertions.h>
#include <isc/hmacmd5.h>
#include <isc/md5.h>
#include <isc/platform.h>
#include <isc/safe.h>
#include <isc/string.h>
#include <isc/types.h>
#include <isc/util.h>

#ifdef ISC_PLATFORM_OPENSSLHASH
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
#define HMAC_CTX_new() &(ctx->_ctx), HMAC_CTX_init(&(ctx->_ctx))
#define HMAC_CTX_free(ptr) HMAC_CTX_cleanup(ptr)
#endif

void
isc_hmacmd5_init(isc_hmacmd5_t *ctx, const unsigned char *key,
		 unsigned int len)
{
	ctx->ctx = HMAC_CTX_new();
	RUNTIME_CHECK(ctx->ctx != NULL);
	RUNTIME_CHECK(HMAC_Init_ex(ctx->ctx, (const void *) key,
				   (int) len, EVP_md5(), NULL) == 1);
}

void
isc_hmacmd5_invalidate(isc_hmacmd5_t *ctx) {
	if (ctx->ctx == NULL)
		return;
	HMAC_CTX_free(ctx->ctx);
	ctx->ctx = NULL;
}

void
isc_hmacmd5_update(isc_hmacmd5_t *ctx, const unsigned char *buf,
		   unsigned int len)
{
	RUNTIME_CHECK(HMAC_Update(ctx->ctx, buf, (int) len) == 1);
}

void
isc_hmacmd5_sign(isc_hmacmd5_t *ctx, unsigned char *digest) {
	RUNTIME_CHECK(HMAC_Final(ctx->ctx, digest, NULL) == 1);
	HMAC_CTX_free(ctx->ctx);
	ctx->ctx = NULL;
}

#else

#define PADLEN 64
#define IPAD 0x36
#define OPAD 0x5C

/*!
 * Start HMAC-MD5 process.  Initialize an md5 context and digest the key.
 */
void
isc_hmacmd5_init(isc_hmacmd5_t *ctx, const unsigned char *key,
		 unsigned int len)
{
	unsigned char ipad[PADLEN];
	int i;

	memset(ctx->key, 0, sizeof(ctx->key));
	if (len > sizeof(ctx->key)) {
		isc_md5_t md5ctx;
		isc_md5_init(&md5ctx);
		isc_md5_update(&md5ctx, key, len);
		isc_md5_final(&md5ctx, ctx->key);
	} else
		memmove(ctx->key, key, len);

	isc_md5_init(&ctx->md5ctx);
	memset(ipad, IPAD, sizeof(ipad));
	for (i = 0; i < PADLEN; i++)
		ipad[i] ^= ctx->key[i];
	isc_md5_update(&ctx->md5ctx, ipad, sizeof(ipad));
}

void
isc_hmacmd5_invalidate(isc_hmacmd5_t *ctx) {
	isc_md5_invalidate(&ctx->md5ctx);
	isc_safe_memwipe(ctx->key, sizeof(ctx->key));
}

/*!
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void
isc_hmacmd5_update(isc_hmacmd5_t *ctx, const unsigned char *buf,
		   unsigned int len)
{
	isc_md5_update(&ctx->md5ctx, buf, len);
}

/*!
 * Compute signature - finalize MD5 operation and reapply MD5.
 */
void
isc_hmacmd5_sign(isc_hmacmd5_t *ctx, unsigned char *digest) {
	unsigned char opad[PADLEN];
	int i;

	isc_md5_final(&ctx->md5ctx, digest);

	memset(opad, OPAD, sizeof(opad));
	for (i = 0; i < PADLEN; i++)
		opad[i] ^= ctx->key[i];

	isc_md5_init(&ctx->md5ctx);
	isc_md5_update(&ctx->md5ctx, opad, sizeof(opad));
	isc_md5_update(&ctx->md5ctx, digest, ISC_MD5_DIGESTLENGTH);
	isc_md5_final(&ctx->md5ctx, digest);
	isc_hmacmd5_invalidate(ctx);
}
#endif /* !ISC_PLATFORM_OPENSSLHASH */

/*!
 * Verify signature - finalize MD5 operation and reapply MD5, then
 * compare to the supplied digest.
 */
isc_boolean_t
isc_hmacmd5_verify(isc_hmacmd5_t *ctx, unsigned char *digest) {
	return (isc_hmacmd5_verify2(ctx, digest, ISC_MD5_DIGESTLENGTH));
}

isc_boolean_t
isc_hmacmd5_verify2(isc_hmacmd5_t *ctx, unsigned char *digest, size_t len) {
	unsigned char newdigest[ISC_MD5_DIGESTLENGTH];

	REQUIRE(len <= ISC_MD5_DIGESTLENGTH);
	isc_hmacmd5_sign(ctx, newdigest);
	return (isc_safe_memequal(digest, newdigest, len));
}
