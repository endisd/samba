#!/bin/sh

if [ -n "$TEST_DATA_PREFIX" ]; then
	LDB_URL="$TEST_DATA_PREFIX/tdbtest.ldb"
else
	LDB_URL="tdbtest.ldb"
fi
export LDB_URL

PATH=bin:$PATH
export PATH

rm -f $LDB_URL*
LDB_MODULES_PATH=`dirname $0`/../../../bin/modules/testldb
echo $LDB_MODULES_PATH

echo "LDB_URL: $LDB_URL"
cat <<EOF | $VALGRIND ldbadd || exit 1
dn: @MODULES
@LIST: sample
EOF

cat <<EOF | $VALGRIND ldbadd || exit 1
dn: dc=bar
dc: bar
someThing: someThingElse
EOF

$VALGRIND ldbsearch "(touchedBy=sample)" | grep "touchedBy: sample" || exit 1
# This action are expected to fails because the sample module return an error when presented the relax control

cat <<EOF | $VALGRIND ldbadd --controls "relax:0" && exit 1
dn: dc=foobar
dc: foobar
someThing: someThingElse
EOF

cat <<EOF | $VALGRIND ldbmodify --controls "relax:0" && exit 1
dn: dc=bar
changetype: replace
replace someThing
someThing: someThingElseBetter
EOF


set
