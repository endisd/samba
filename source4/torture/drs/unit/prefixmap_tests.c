/*
   Unix SMB/CIFS implementation.

   DRSUAPI prefixMap unit tests

   Copyright (C) Kamen Mazdrashki <kamen.mazdrashki@postpath.com> 2009

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "includes.h"
#include "torture/smbtorture.h"
#include "torture/rpc/drsuapi.h"
#include "dsdb/samdb/samdb.h"


/**
 * Private data to be shared among all test in Test case
 */
struct drsut_prefixmap_data {
	struct dsdb_schema_prefixmap *pfm_new;
	struct dsdb_schema_prefixmap *pfm_full;
};

/**
 * Test-oid data structure
 */
struct drsut_pfm_oid_data {
	uint32_t	id;
	const char	*bin_oid;
	const char	*oid_prefix;
};

/**
 * Default prefixMap initialization data
 */
static const struct drsut_pfm_oid_data _prefixmap_test_new_data[] = {
	{.id=0x00000000, .bin_oid="5504",                 .oid_prefix="2.5.4"},
	{.id=0x00000001, .bin_oid="5506",                 .oid_prefix="2.5.6"},
	{.id=0x00000002, .bin_oid="2A864886F7140102",     .oid_prefix="1.2.840.113556.1.2"},
	{.id=0x00000003, .bin_oid="2A864886F7140103",     .oid_prefix="1.2.840.113556.1.3"},
	{.id=0x00000004, .bin_oid="6086480165020201",     .oid_prefix="2.16.840.1.101.2.2.1"},
	{.id=0x00000005, .bin_oid="6086480165020203",     .oid_prefix="2.16.840.1.101.2.2.3"},
	{.id=0x00000006, .bin_oid="6086480165020105",     .oid_prefix="2.16.840.1.101.2.1.5"},
	{.id=0x00000007, .bin_oid="6086480165020104",     .oid_prefix="2.16.840.1.101.2.1.4"},
	{.id=0x00000008, .bin_oid="5505",                 .oid_prefix="2.5.5"},
	{.id=0x00000009, .bin_oid="2A864886F7140104",     .oid_prefix="1.2.840.113556.1.4"},
	{.id=0x0000000A, .bin_oid="2A864886F7140105",     .oid_prefix="1.2.840.113556.1.5"},
	{.id=0x00000013, .bin_oid="0992268993F22C64",     .oid_prefix="0.9.2342.19200300.100"},
	{.id=0x00000014, .bin_oid="6086480186F84203",     .oid_prefix="2.16.840.1.113730.3"},
	{.id=0x00000015, .bin_oid="0992268993F22C6401",   .oid_prefix="0.9.2342.19200300.100.1"},
	{.id=0x00000016, .bin_oid="6086480186F8420301",   .oid_prefix="2.16.840.1.113730.3.1"},
	{.id=0x00000017, .bin_oid="2A864886F7140105B658", .oid_prefix="1.2.840.113556.1.5.7000"},
	{.id=0x00000018, .bin_oid="5515",                 .oid_prefix="2.5.21"},
	{.id=0x00000019, .bin_oid="5512",                 .oid_prefix="2.5.18"},
	{.id=0x0000001A, .bin_oid="5514",                 .oid_prefix="2.5.20"},
};

/**
 * Data to be used for creating full prefix map for testing
 */
static const struct drsut_pfm_oid_data _prefixmap_full_map_data[] = {
	{.id=0x00000000, .bin_oid="0x5504",                     .oid_prefix="2.5.4"},
	{.id=0x00000001, .bin_oid="0x5506",                     .oid_prefix="2.5.6"},
	{.id=0x00000002, .bin_oid="0x2A864886F7140102",         .oid_prefix="1.2.840.113556.1.2"},
	{.id=0x00000003, .bin_oid="0x2A864886F7140103",         .oid_prefix="1.2.840.113556.1.3"},
	{.id=0x00000004, .bin_oid="0x6086480165020201",         .oid_prefix="2.16.840.1.101.2.2.1"},
	{.id=0x00000005, .bin_oid="0x6086480165020203",         .oid_prefix="2.16.840.1.101.2.2.3"},
	{.id=0x00000006, .bin_oid="0x6086480165020105",         .oid_prefix="2.16.840.1.101.2.1.5"},
	{.id=0x00000007, .bin_oid="0x6086480165020104",         .oid_prefix="2.16.840.1.101.2.1.4"},
	{.id=0x00000008, .bin_oid="0x5505",                     .oid_prefix="2.5.5"},
	{.id=0x00000009, .bin_oid="0x2A864886F7140104",         .oid_prefix="1.2.840.113556.1.4"},
	{.id=0x0000000a, .bin_oid="0x2A864886F7140105",         .oid_prefix="1.2.840.113556.1.5"},
	{.id=0x00000013, .bin_oid="0x0992268993F22C64",         .oid_prefix="0.9.2342.19200300.100"},
	{.id=0x00000014, .bin_oid="0x6086480186F84203",         .oid_prefix="2.16.840.1.113730.3"},
	{.id=0x00000015, .bin_oid="0x0992268993F22C6401",       .oid_prefix="0.9.2342.19200300.100.1"},
	{.id=0x00000016, .bin_oid="0x6086480186F8420301",       .oid_prefix="2.16.840.1.113730.3.1"},
	{.id=0x00000017, .bin_oid="0x2A864886F7140105B658",     .oid_prefix="1.2.840.113556.1.5.7000"},
	{.id=0x00000018, .bin_oid="0x5515",                     .oid_prefix="2.5.21"},
	{.id=0x00000019, .bin_oid="0x5512",                     .oid_prefix="2.5.18"},
	{.id=0x0000001a, .bin_oid="0x5514",                     .oid_prefix="2.5.20"},
	{.id=0x0000000b, .bin_oid="0x2A864886F71401048204",     .oid_prefix="1.2.840.113556.1.4.260"},
	{.id=0x0000000c, .bin_oid="0x2A864886F714010538",       .oid_prefix="1.2.840.113556.1.5.56"},
	{.id=0x0000000d, .bin_oid="0x2A864886F71401048206",     .oid_prefix="1.2.840.113556.1.4.262"},
	{.id=0x0000000e, .bin_oid="0x2A864886F714010539",       .oid_prefix="1.2.840.113556.1.5.57"},
	{.id=0x0000000f, .bin_oid="0x2A864886F71401048207",     .oid_prefix="1.2.840.113556.1.4.263"},
	{.id=0x00000010, .bin_oid="0x2A864886F71401053A",       .oid_prefix="1.2.840.113556.1.5.58"},
	{.id=0x00000011, .bin_oid="0x2A864886F714010549",       .oid_prefix="1.2.840.113556.1.5.73"},
	{.id=0x00000012, .bin_oid="0x2A864886F71401048231",     .oid_prefix="1.2.840.113556.1.4.305"},
	{.id=0x0000001b, .bin_oid="0x2B060104018B3A6577",       .oid_prefix="1.3.6.1.4.1.1466.101.119"},
	{.id=0x0000001c, .bin_oid="0x6086480186F8420302",       .oid_prefix="2.16.840.1.113730.3.2"},
	{.id=0x0000001d, .bin_oid="0x2B06010401817A01",         .oid_prefix="1.3.6.1.4.1.250.1"},
	{.id=0x0000001e, .bin_oid="0x2A864886F70D0109",         .oid_prefix="1.2.840.113549.1.9"},
	{.id=0x0000001f, .bin_oid="0x0992268993F22C6404",       .oid_prefix="0.9.2342.19200300.100.4"},
	{.id=0x00000020, .bin_oid="0x2A864886F714010617",       .oid_prefix="1.2.840.113556.1.6.23"},
	{.id=0x00000021, .bin_oid="0x2A864886F71401061201",     .oid_prefix="1.2.840.113556.1.6.18.1"},
	{.id=0x00000022, .bin_oid="0x2A864886F71401061202",     .oid_prefix="1.2.840.113556.1.6.18.2"},
	{.id=0x00000023, .bin_oid="0x2A864886F71401060D03",     .oid_prefix="1.2.840.113556.1.6.13.3"},
	{.id=0x00000024, .bin_oid="0x2A864886F71401060D04",     .oid_prefix="1.2.840.113556.1.6.13.4"},
	{.id=0x00000025, .bin_oid="0x2B0601010101",             .oid_prefix="1.3.6.1.1.1.1"},
	{.id=0x00000026, .bin_oid="0x2B0601010102",             .oid_prefix="1.3.6.1.1.1.2"},
	{.id=0x000003ed, .bin_oid="0x2A864886F7140104B65866",   .oid_prefix="1.2.840.113556.1.4.7000.102"},
	{.id=0x00000428, .bin_oid="0x2A864886F7140105B6583E",   .oid_prefix="1.2.840.113556.1.5.7000.62"},
	{.id=0x0000044c, .bin_oid="0x2A864886F7140104B6586683", .oid_prefix="1.2.840.113556.1.4.7000.102:0x83"},
	{.id=0x0000044f, .bin_oid="0x2A864886F7140104B6586681", .oid_prefix="1.2.840.113556.1.4.7000.102:0x81"},
	{.id=0x0000047d, .bin_oid="0x2A864886F7140105B6583E81", .oid_prefix="1.2.840.113556.1.5.7000.62:0x81"},
	{.id=0x00000561, .bin_oid="0x2A864886F7140105B6583E83", .oid_prefix="1.2.840.113556.1.5.7000.62:0x83"},
	{.id=0x000007d1, .bin_oid="0x2A864886F71401061401",     .oid_prefix="1.2.840.113556.1.6.20.1"},
	{.id=0x000007e1, .bin_oid="0x2A864886F71401061402",     .oid_prefix="1.2.840.113556.1.6.20.2"},
	{.id=0x00001b86, .bin_oid="0x2A817A",                   .oid_prefix="1.2.250"},
	{.id=0x00001c78, .bin_oid="0x2A817A81",                 .oid_prefix="1.2.250:0x81"},
	{.id=0x00001c7b, .bin_oid="0x2A817A8180",               .oid_prefix="1.2.250:0x8180"},
};


/**
 * OID-to-ATTID mappings to be used for testing
 */
static const struct {
	const char 	*oid;
	uint32_t 	id;
	uint32_t 	attid;
} _prefixmap_test_data[] = {
	{.oid="2.5.4.0", 		.id=0x00000000, .attid=0x000000},
	{.oid="2.5.4.42", 		.id=0x00000000, .attid=0x00002a},
	{.oid="1.2.840.113556.1.2.1", 	.id=0x00000002, .attid=0x020001},
	{.oid="1.2.840.113556.1.2.13", 	.id=0x00000002, .attid=0x02000d},
	{.oid="1.2.840.113556.1.2.281", .id=0x00000002, .attid=0x020119},
	{.oid="1.2.840.113556.1.4.125", .id=0x00000009, .attid=0x09007d},
	{.oid="1.2.840.113556.1.4.146", .id=0x00000009, .attid=0x090092},
	{.oid="1.2.250.1", 	 	.id=0x00001b86, .attid=0x1b860001},
	{.oid="1.2.250.130", 	 	.id=0x00001b86, .attid=0x1b860082},
	{.oid="1.2.250.16386", 	 	.id=0x00001c78, .attid=0x1c788002},
	{.oid="1.2.250.2097154", 	.id=0x00001c7b, .attid=0x1c7b8002},
};


/**
 * Creates dsdb_schema_prefixmap based on predefined data
 *
 */
static WERROR _drsut_prefixmap_new(const struct drsut_pfm_oid_data *_pfm_init_data, uint32_t count,
				   TALLOC_CTX *mem_ctx, struct dsdb_schema_prefixmap **_pfm)
{
	uint32_t i;
	struct dsdb_schema_prefixmap *pfm;

	pfm = talloc(mem_ctx, struct dsdb_schema_prefixmap);
	W_ERROR_HAVE_NO_MEMORY(pfm);

	pfm->length = count;
	pfm->prefixes = talloc_array(pfm, struct dsdb_schema_prefixmap_oid, pfm->length);
	if (!pfm->prefixes) {
		talloc_free(pfm);
		return WERR_NOMEM;
	}

	for (i = 0; i < pfm->length; i++) {
		pfm->prefixes[i].id = _pfm_init_data[i].id;
		pfm->prefixes[i].bin_oid = strhex_to_data_blob(pfm, _pfm_init_data[i].bin_oid);
		if (!pfm->prefixes[i].bin_oid.data) {
			talloc_free(pfm);
			return WERR_NOMEM;
		}
	}

	*_pfm = pfm;

	return WERR_OK;
}


/*
 * Tests dsdb_schema_pfm_new()
 */
static bool torture_drs_unit_pfm_new(struct torture_context *tctx, struct drsut_prefixmap_data *priv)
{
	int i;
	WERROR werr;
	TALLOC_CTX *mem_ctx;
	struct dsdb_schema_prefixmap *pfm = NULL;

	mem_ctx = talloc_new(priv);

	/* create new prefix map */
	werr = dsdb_schema_pfm_new(mem_ctx, &pfm);
	torture_assert_werr_ok(tctx, werr, "dsdb_schema_pfm_new() failed!");
	torture_assert(tctx, pfm != NULL, "NULL prefixMap created!");
	torture_assert(tctx, pfm->length > 0, "Empty prefixMap created!");
	torture_assert(tctx, pfm->prefixes != NULL, "No prefixes for newly created prefixMap!");

	/* compare newly created prefixMap with template one */
	torture_assert(tctx, priv->pfm_new->length == pfm->length,
			"Newly created prefixMap differs in length with expected one");
	for (i = 0; i < pfm->length; i++) {
		struct dsdb_schema_prefixmap_oid *pfm_oid = &pfm->prefixes[i];
		struct dsdb_schema_prefixmap_oid *pfm_oid_exp = &priv->pfm_new->prefixes[i];

		torture_assert(tctx, pfm_oid->id == pfm_oid_exp->id,
				talloc_asprintf(mem_ctx, "Different IDs for index=%d", i));
		torture_assert_data_blob_equal(tctx, pfm_oid->bin_oid, pfm_oid_exp->bin_oid,
						talloc_asprintf(mem_ctx, "Different bin_oid for index=%d", i));
	}

	talloc_free(mem_ctx);

	return true;
}

/**
 * Tests dsdb_schema_pfm_make_attid() using full prefixMap.
 * In this test we know exactly which ATTID and prefixMap->ID
 * should be returned, i.e. no prefixMap entries should be added.
 */
static bool torture_drs_unit_pfm_make_attid_full_map(struct torture_context *tctx, struct drsut_prefixmap_data *priv)
{
	WERROR werr;
	uint32_t i, count;
	uint32_t attid;
	char *err_msg;

	count = ARRAY_SIZE(_prefixmap_test_data);
	for (i = 0; i < count; i++) {
		werr = dsdb_schema_pfm_make_attid(priv->pfm_full, _prefixmap_test_data[i].oid, &attid);
		/* prepare error message */
		err_msg = talloc_asprintf(priv, "dsdb_schema_pfm_make_attid() failed with %s",
						_prefixmap_test_data[i].oid);
		torture_assert(tctx, err_msg, "Unexpected: Have no memory!");
		/* verify result and returned ATTID */
		torture_assert_werr_ok(tctx, werr, err_msg);
		torture_assert_int_equal(tctx, attid, _prefixmap_test_data[i].attid, err_msg);
		/* reclaim memory for prepared error message */
		talloc_free(err_msg);
	}

	return true;
}

/**
 * Tests dsdb_schema_pfm_make_attid() using initially small prefixMap.
 * In this test we don't know exactly which ATTID and prefixMap->ID
 * should be returned, but we can verify lo-word of ATTID.
 * This test verifies implementation branch when a new
 * prefix should be added into prefixMap.
 */
static bool torture_drs_unit_pfm_make_attid_small_map(struct torture_context *tctx, struct drsut_prefixmap_data *priv)
{
	WERROR werr;
	uint32_t i, j;
	uint32_t idx;
	uint32_t attid, attid_2;
	char *err_msg;
	struct dsdb_schema_prefixmap *pfm = NULL;
	TALLOC_CTX *mem_ctx;

	mem_ctx = talloc_new(priv);

	/* create new prefix map */
	werr = dsdb_schema_pfm_new(mem_ctx, &pfm);
	torture_assert_werr_ok(tctx, werr, "dsdb_schema_pfm_new() failed!");
	torture_assert(tctx, pfm != NULL, "NULL prefixMap created!");
	torture_assert(tctx, pfm->length > 0, "Empty prefixMap created!");
	torture_assert(tctx, pfm->prefixes != NULL, "No prefixes for newly created prefixMap!");

	/* make some ATTIDs and check result */
	for (i = 0; i < ARRAY_SIZE(_prefixmap_test_data); i++) {
		werr = dsdb_schema_pfm_make_attid(pfm, _prefixmap_test_data[i].oid, &attid);

		/* prepare error message */
		err_msg = talloc_asprintf(mem_ctx, "dsdb_schema_pfm_make_attid() failed with %s",
						_prefixmap_test_data[i].oid);
		torture_assert(tctx, err_msg, "Unexpected: Have no memory!");

		/* verify result and returned ATTID */
		torture_assert_werr_ok(tctx, werr, err_msg);
		/* verify ATTID lo-word */
		torture_assert_int_equal(tctx, attid & 0xFFFF, _prefixmap_test_data[i].attid & 0xFFFF, err_msg);

		/* try again, this time verify for whole ATTID */
		werr = dsdb_schema_pfm_make_attid(pfm, _prefixmap_test_data[i].oid, &attid_2);
		torture_assert_werr_ok(tctx, werr, err_msg);
		torture_assert_int_equal(tctx, attid_2, attid, err_msg);

		/* reclaim memory for prepared error message */
		talloc_free(err_msg);

		/* check there is such an index in modified prefixMap */
		idx = (attid >> 16);
		for (j = 0; j < pfm->length; j++) {
			if (pfm->prefixes[j].id == idx)
				break;
		}
		if (j >= pfm->length) {
			torture_result(tctx, TORTURE_FAIL, __location__": No prefix for ATTID=0x%08X", attid);
			return false;
		}

	}

	talloc_free(mem_ctx);

	return true;
}

/**
 * Tests dsdb_schema_pfm_oid_from_attid() using full prefixMap.
 */
static bool torture_drs_unit_pfm_oid_from_attid(struct torture_context *tctx, struct drsut_prefixmap_data *priv)
{
	WERROR werr;
	uint32_t i, count;
	char *err_msg;
	const char *oid;

	count = ARRAY_SIZE(_prefixmap_test_data);
	for (i = 0; i < count; i++) {
		oid = NULL;
		werr = dsdb_schema_pfm_oid_from_attid(priv->pfm_full, _prefixmap_test_data[i].attid,
						      priv, &oid);
		/* prepare error message */
		err_msg = talloc_asprintf(priv, "dsdb_schema_pfm_oid_from_attid() failed with 0x%08X",
						_prefixmap_test_data[i].attid);
		torture_assert(tctx, err_msg, "Unexpected: Have no memory!");
		/* verify result and returned ATTID */
		torture_assert_werr_ok(tctx, werr, err_msg);
		torture_assert(tctx, oid, "dsdb_schema_pfm_oid_from_attid() returned NULL OID!!!");
		torture_assert_str_equal(tctx, oid, _prefixmap_test_data[i].oid, err_msg);
		/* reclaim memory for prepared error message */
		talloc_free(err_msg);
		/* free memory for OID */
		talloc_free(discard_const(oid));
	}

	return true;
}


/*
 * Setup/Teardown for test case
 */
static bool torture_drs_unit_prefixmap_setup(struct torture_context *tctx, struct drsut_prefixmap_data **priv)
{
	WERROR werr;

	*priv = talloc_zero(tctx, struct drsut_prefixmap_data);
	torture_assert(tctx, *priv != NULL, "Not enough memory");

	werr = _drsut_prefixmap_new(_prefixmap_test_new_data, ARRAY_SIZE(_prefixmap_test_new_data), tctx, &(*priv)->pfm_new);
	torture_assert_werr_ok(tctx, werr, "failed to create pfm_new");

	werr = _drsut_prefixmap_new(_prefixmap_full_map_data, ARRAY_SIZE(_prefixmap_full_map_data), tctx, &(*priv)->pfm_full);
	torture_assert_werr_ok(tctx, werr, "failed to create pfm_test");

	return true;
}

static bool torture_drs_unit_prefixmap_teardown(struct torture_context *tctx, struct drsut_prefixmap_data *priv)
{
	talloc_free(priv);

	return true;
}

/**
 * Test case initialization for
 * DRS-UNIT.prefixMap
 */
struct torture_tcase * torture_drs_unit_prefixmap(struct torture_suite *suite)
{
	typedef bool (*pfn_setup)(struct torture_context *, void **);
	typedef bool (*pfn_teardown)(struct torture_context *, void *);
	typedef bool (*pfn_run)(struct torture_context *, void *);

	struct torture_tcase * tc = torture_suite_add_tcase(suite, "prefixMap");

	torture_tcase_set_fixture(tc,
				  (pfn_setup)torture_drs_unit_prefixmap_setup,
				  (pfn_teardown)torture_drs_unit_prefixmap_teardown);

	tc->description = talloc_strdup(tc, "Unit tests for DRSUAPI::prefixMap implementation");

	torture_tcase_add_simple_test(tc, "new", (pfn_run)torture_drs_unit_pfm_new);

	torture_tcase_add_simple_test(tc, "make_attid_full_map", (pfn_run)torture_drs_unit_pfm_make_attid_full_map);
	torture_tcase_add_simple_test(tc, "make_attid_small_map", (pfn_run)torture_drs_unit_pfm_make_attid_small_map);
	torture_tcase_add_simple_test(tc, "oid_from_attid_full_map", (pfn_run)torture_drs_unit_pfm_oid_from_attid);

	return tc;
}