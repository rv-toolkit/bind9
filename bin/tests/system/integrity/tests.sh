#!/bin/sh
#
# Copyright (C) 2017, 2018  Internet Systems Consortium, Inc. ("ISC")
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

SYSTEMTESTTOP=..
. $SYSTEMTESTTOP/conf.sh

status=0
n=1

echo "I:check that 'check-integrity yes; check-mx-cname fail;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 mx mx-cname-fail > dig.out.test$n || ret=1
grep "status: SERVFAIL," dig.out.test$n > /dev/null || ret=1
grep "zone mx-cname-fail/IN: mx-cname-fail/MX 'cname.mx-cname-fail' is a CNAME (illegal)" ns1/named.run > /dev/null || ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity yes; check-mx-cname warn;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 mx mx-cname-warn > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone mx-cname-warn/IN: mx-cname-warn/MX 'cname.mx-cname-warn' is a CNAME (illegal)" ns1/named.run > /dev/null || ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity yes; check-mx-cname ignore;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 mx mx-cname-ignore > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone mx-cname-ignore/IN: mx-cname-ignore/MX 'cname.mx-cname-ignore' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity no; check-mx-cname fail;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 mx no-mx-cname-fail > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone no-mx-cname-fail/IN: no-mx-cname-fail/MX 'cname.no-mx-cname-fail' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity no; check-mx-cname warn;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 mx no-mx-cname-warn > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone no-mx-cname-warn/IN: no-mx-cname-warn/MX 'cname.no-mx-cname-warn' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity no; check-mx-cname ignore;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 mx no-mx-cname-ignore > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone no-mx-cname-ignore/IN: no-mx-cname-ignore/MX 'cname.no-mx-cname-ignore' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity yes; check-srv-cname fail;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 srv srv-cname-fail > dig.out.test$n || ret=1
grep "status: SERVFAIL," dig.out.test$n > /dev/null || ret=1
grep "zone srv-cname-fail/IN: srv-cname-fail/SRV 'cname.srv-cname-fail' is a CNAME (illegal)" ns1/named.run > /dev/null || ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity yes; check-srv-cname warn;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 srv srv-cname-warn > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone srv-cname-warn/IN: srv-cname-warn/SRV 'cname.srv-cname-warn' is a CNAME (illegal)" ns1/named.run > /dev/null || ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity yes; check-srv-cname ignore;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 srv srv-cname-ignore > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone srv-cname-ignore/IN: srv-cname-ignore/SRV 'cname.srv-cname-ignore' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity no; check-srv-cname fail;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 srv no-srv-cname-fail > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone no-srv-cname-fail/IN: no-srv-cname-fail/SRV 'cname.no-srv-cname-fail' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity no; check-srv-cname warn;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 srv no-srv-cname-warn > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone no-srv-cname-warn/IN: no-srv-cname-warn/SRV 'cname.no-srv-cname-warn' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:check that 'check-integrity no; check-srv-cname ignore;' works ($n)"
ret=0
$DIG -p 5300 @10.53.0.1 srv no-srv-cname-ignore > dig.out.test$n || ret=1
grep "status: NOERROR," dig.out.test$n > /dev/null || ret=1
grep "zone no-srv-cname-ignore/IN: no-srv-cname-ignore/SRV 'cname.no-srv-cname-ignore' is a CNAME (illegal)" ns1/named.run > /dev/null && ret=1
n=`expr $n + 1`
if [ $ret != 0 ]; then echo "I:failed"; fi
status=`expr $status + $ret`

echo "I:exit status: $status"
test $status -eq 0 || exit 1
