/*
 * Copyright (C) 1999  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef DNS_SECPROTO_H
#define DNS_SECPROTO_H 1

#include <isc/lang.h>

#include <dns/types.h>

ISC_LANG_BEGINDECLS

isc_result_t dns_secproto_fromtext(dns_secproto_t *secprotop,
				   isc_textregion_t *source);
/*
 * Convert the text 'source' refers to into a DNSSEC security protocol value.
 * The text may contain either a mnemonic protocol name or a decimal protocol
 * number.
 *
 * Requires:
 *	'secprotop' is a valid pointer.
 *
 *	'source' is a valid text region.
 *
 * Returns:
 *	DNS_R_SUCCESS			on success
 *	DNS_R_UNKNOWN			mnemonic type is unknown
 *	DNS_R_RANGE			numeric type is out of range
 */

isc_result_t
dns_secproto_totext(dns_secproto_t secproto, isc_buffer_t *target);
/*
 * Put a textual representation of the DNSSEC security protocol 'secproto'
 * into 'target'.
 *
 * Requires:
 *	'secproto' is a valid secproto.
 *
 *	'target' is a valid text buffer.
 *
 * Ensures:
 *	If the result is success:
 *		The used space in 'target' is updated.
 *
 * Returns:
 *	DNS_R_SUCCESS			on success
 *	DNS_R_NOSPACE			target buffer is too small
 */

ISC_LANG_ENDDECLS

#endif /* DNS_SECPROTO_H */
