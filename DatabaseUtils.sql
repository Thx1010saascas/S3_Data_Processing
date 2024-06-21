------------------------------------
-- Fast get-count for Gaia
------------------------------------
SELECT (CASE WHEN c.reltuples < 0 THEN NULL WHEN c.relpages = 0 THEN float8 '0' ELSE c.reltuples / c.relpages END * (pg_catalog.pg_relation_size(c.oid) / pg_catalog.current_setting('block_size')::int))::bigint FROM pg_catalog.pg_class c WHERE  c.oid = 'gaia'::regclass;

------------------------------------
-- Fast get-count for Simbad
------------------------------------
SELECT (CASE WHEN c.reltuples < 0 THEN NULL WHEN c.relpages = 0 THEN float8 '0' ELSE c.reltuples / c.relpages END * (pg_catalog.pg_relation_size(c.oid) / pg_catalog.current_setting('block_size')::int))::bigint FROM pg_catalog.pg_class c WHERE  c.oid = 'simbad'::regclass;

------------------------------------
-- Truncate the Gaia table
------------------------------------
truncate table gaia;
ALTER SEQUENCE gaia_id_seq RESTART WITH 1;
vacuum full gaia;

------------------------------------
-- Truncate the Simbad table
------------------------------------
truncate table simbad;
ALTER SEQUENCE simbad_id_seq RESTART WITH 1;
INSERT INTO public.simbad (index, ra, dec, glat, glon, parallax, teff, luminosity, radius, spectral_type, name1, name2, type, object_type)
VALUES (0, 8.5269444, -52.234384377289665, 0.0172222, 101.89844377219657, 206611570.248, 5772, 1, 1, 'G2V', 'Sol', 'Sun', '*', 1);
vacuum full simbad;