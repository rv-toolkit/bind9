
/*
 * Copyright (C) 1998, 1999  Internet Software Consortium.
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

#include	<config.h>

#include	<ctype.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	<isc/mem.h>
#include	<isc/buffer.h>
#include	<isc/error.h>

#include	<dns/master.h>
#include	<dns/name.h>
#include	<dns/rdataset.h>
#include	<dns/result.h>
#include	<dns/types.h>
#include	<tests/t_api.h>

#define	BUFLEN		255
#define	BIGBUFLEN	(64 * 1024)

static dns_result_t	t1_commit_callback(dns_rdatacallbacks_t *callbacks,
			   dns_name_t *owner, dns_rdataset_t *dataset);
static void		t1(void);

isc_mem_t	*T1_mctx;
char		*Tokens[T_MAXTOKS + 1];

static dns_result_t
t1_commit_callback(dns_rdatacallbacks_t *callbacks, dns_name_t *owner,
	      dns_rdataset_t *dataset) {

	char buf[BIGBUFLEN];
	isc_buffer_t target;
	dns_result_t result;
	
	callbacks = callbacks;	/*unused*/

	isc_buffer_init(&target, buf, BIGBUFLEN, ISC_BUFFERTYPE_TEXT);
	result = dns_rdataset_totext(dataset, owner, ISC_FALSE, ISC_FALSE,
				     &target);
	if (result != DNS_R_SUCCESS)
		t_info("dns_rdataset_totext: %s\n", dns_result_totext(result));

	return(result);
}

static int
test_master(char *testfile, char *origin, dns_result_t exp_result) {
	int			result;
	int			len;
	isc_result_t		isc_result;
	dns_result_t		dns_result;
	dns_name_t		dns_origin;
	isc_buffer_t		source;
	isc_buffer_t		target;
	unsigned char		name_buf[BUFLEN];
	int			soacount;
	int			nscount;
	dns_rdatacallbacks_t	callbacks;

	result = T_UNRESOLVED;
	if (T1_mctx == NULL)
		isc_result = isc_mem_create(0, 0, &T1_mctx);
	else
		isc_result = ISC_R_SUCCESS;
	if (isc_result == ISC_R_SUCCESS) {
		len = strlen(origin);
		isc_buffer_init(&source, origin, len,
				ISC_BUFFERTYPE_TEXT);
		isc_buffer_add(&source, len);
		isc_buffer_setactive(&source, len);
		isc_buffer_init(&target, name_buf, BUFLEN, ISC_BUFFERTYPE_BINARY);
		dns_name_init(&dns_origin, NULL);
		dns_result = dns_name_fromtext(&dns_origin, &source, dns_rootname,
					   ISC_FALSE, &target);
		if (dns_result == DNS_R_SUCCESS) {
			dns_rdatacallbacks_init(&callbacks);
			callbacks.commit = t1_commit_callback;
			
			dns_result = dns_master_load(	testfile,
							&dns_origin,
							&dns_origin,
							1,
							ISC_FALSE,
						 	&soacount,
							&nscount,
						 	&callbacks,
							T1_mctx);

			if (dns_result == exp_result)
				result = T_PASS;
			else {
				t_info("dns_master_load: got %s, expected %s\n",
					dns_result_totext(dns_result),
					dns_result_totext(exp_result));
				result = T_FAIL;
			}
		}
		else {
			t_info("dns_name_fromtext failed %s\n",
					dns_result_totext(dns_result));
		}
	}
	else {
		t_info("isc_mem_create failed %d\n", isc_result);
	}
	return(result);
}

static int
test_master_x(char *filename) {

	FILE		*fp;
	char		*p;
	int		line;
	int		cnt;
	int		result;

	result = T_UNRESOLVED;

	fp = fopen(filename, "r");
	if (fp != NULL) {
		line = 0;
		while ((p = t_fgetbs(fp)) != NULL) {

			++line;

			/* skip comment lines */
			if ((isspace(*p)) || (*p == '#'))
				continue;

			/* name of data file, origin, expected result */
			cnt = t_bustline(p, Tokens);
			if (cnt == 3) {
				result = test_master(
						Tokens[0],
						Tokens[1],
						t_dns_result_fromtext(Tokens[2]));
			}
			else {
				t_info("bad format in %s at line %d\n",
						filename, line);
			}

			(void) free(p);
		}
		(void) fclose(fp);
	}
	else {
		t_info("Missing datafile %s\n", filename);
	}
	return(result);
}

static char *a1 =	"dns_master_load loads a valid master file and "
			"returns DNS_R_SUCCESS";
static void
t1() {
	int	result;
	t_assert("dns_master_load", 1, T_REQUIRED, a1);
	result = test_master_x("dns_master_load_1_data");
	t_result(result);
}

static char *a2 =	"dns_master_load returns DNS_R_UNEXPECTEDEND when the "
			"masterfile input ends unexpectedly";
static void
t2() {
	int	result;
	t_assert("dns_master_load", 2, T_REQUIRED, a2);
	result = test_master_x("dns_master_load_2_data");
	t_result(result);
}

static char *a3 =	"dns_master_load returns DNS_R_NOOWNER when the "
			"an ownername is not specified";

static void
t3() {
	int	result;
	t_assert("dns_master_load", 3, T_REQUIRED, a3);
	result = test_master_x("dns_master_load_3_data");
	t_result(result);
}

static char *a4 =	"dns_master_load returns DNS_R_NOTTL when the "
			"the ttl is not specified";

static void
t4() {
	int	result;
	t_assert("dns_master_load", 4, T_REQUIRED, a4);
	result = test_master_x("dns_master_load_4_data");
	t_result(result);
}

static char *a5 =	"dns_master_load returns DNS_R_BADCLASS when the "
			"the record class did not match the zone class";

static void
t5() {
	int	result;

	t_assert("dns_master_load", 5, T_REQUIRED, a5);
	result = test_master_x("dns_master_load_5_data");

	t_result(result);
}

testspec_t	T_testlist[] = {
	{	t1,	"DNS_R_SUCCESS"		},
	{	t2,	"DNS_R_UNEXPECTEDEND"	},
	{	t3,	"DNS_NOOWNER"		},
	{	t4,	"DNS_NOTTL"		},
	{	t5,	"DNS_BADCLASS"		},
	{	NULL,	NULL			}
};

