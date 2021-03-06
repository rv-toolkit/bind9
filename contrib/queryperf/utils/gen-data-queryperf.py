#!/usr/bin/python

#
# Contributed by Stephane Bortzmeyer <bortzmeyer@nic.fr>
#
# "A small tool which may be useful with contrib/queryperf. This script
#  can generate files of queries, both with random names (to test the
#  behaviour with NXdomain) and with domains from a real zone file."
#

import getopt
import random
import re
import string
import sys

maxsize = 10
tld = 'org'
num = 4
percent_random = 0.3
gen = None
zone_file = None
domains = {}
domain_ns = r'^([a-z0-9-\.]+)((\s+\d+)?(\s+IN)?|(\s+IN)(\s+\d+)?)\s+NS'
domain_ns_re = re.compile(domain_ns, re.IGNORECASE)


def remove_tld(label, tld):
    if label.endswith('.' + tld + '.'):
        return label[0:-(1 + len(tld) + 1)]
    else:
        return label


# characters allowed in a label
LDH = string.digits + string.ascii_lowercase + "-"


def gen_random_label():
    """
    Generate a random DNS label.

    Note that this may generate labels that start/end with '-'.
    """
    label = ""
    for _ in range(gen.randint(1, maxsize)):
        label = label + gen.choice(LDH)
    return label


def make_domain(label):
    return "www." + label + "." + tld + "     A"


def usage():
    sys.stdout.write("Usage: " + sys.argv[0] + " [-n number] " +
                     "[-p percent-random] [-t TLD]\n")
    sys.stdout.write("       [-m MAXSIZE] [-f zone-file]\n")


try:
    optlist, args = getopt.getopt(sys.argv[1:], "hp:f:n:t:m:",
                                  ["help", "percentrandom=", "zonefile=",
                                   "number=", "tld=",
                                   "maxsize="])
    for option, value in optlist:
        if option == "--help" or option == "-h":
            usage()
            sys.exit(0)
        elif option == "--number" or option == "-n":
            num = int(value)
        elif option == "--maxsize" or option == "-m":
            maxsize = int(value)
        elif option == "--percentrandom" or option == "-p":
            percent_random = float(value)
        elif option == "--tld" or option == "-t":
            tld = str(value)
        elif option == "--zonefile" or option == "-f":
            zone_file = str(value)
        else:
            getopt.error("Unknown option " + option)
except getopt.error as reason:
    sys.stderr.write(sys.argv[0] + ": " + str(reason) + "\n")
    usage()
    sys.exit(1)

if len(args) > 0:
    usage()
    sys.exit(1)

gen = random.Random()
if zone_file:
    file = open(zone_file)
    line = file.readline()
    while line:
        domain_line = domain_ns_re.match(line)
        if domain_line:
            print(domain_line.group(1))
            domain = remove_tld(domain_line.group(1), tld)
            domains[domain] = 1
        line = file.readline()
    file.close()
if zone_file:
    if not domains:
        sys.stderr.write("No domains found in '%s'\n" % zone_file)
        sys.exit(1)
    domain_names = list(domains.keys())
for i in range(num):
    if zone_file:
        if gen.random() < percent_random:
            sys.stdout.write(make_domain(gen_random_label()))
        else:
            sys.stdout.write(make_domain(gen.choice(domain_names)))
    else:
        sys.stdout.write(make_domain(gen_random_label()))
    sys.stdout.write("\n")
