create database star_data
    with owner postgres;

create table public.simbad
(
    id            bigserial
        primary key,
    index         bigint                     not null,
    g_source_id   bigint,
    ra            double precision           not null,
    dec           double precision           not null,
    glat          double precision,
    glon          double precision,
    x             double precision,
    y             double precision,
    z             double precision,
    parallax      double precision           not null,
    distance_ly   double precision default 0 not null,
    teff          integer,
    fe_h          double precision,
    luminosity    double precision,
    radius        double precision,
    type          text,
    name1         text,
    name2         text,
    name3         text,
    name4         text,
    name5         text,
    name6         text,
    name7         text,
    name8         text,
    name9         text,
    name10         text,
    spectral_type text,
    geom          geometry,
    g_mag         double precision,
    b             double precision,
    v             double precision,
    object_type   integer
);

alter table public.simbad
    owner to postgres;

create index "simbad_lower(name)_index"
    on public.simbad (lower(name1));

create index simbad_distance_ly_index
    on public.simbad (distance_ly);

create index simbad_name1_index
    on public.simbad (name1);

create index simbad_g_source_id_index
    on public.simbad (g_source_id);

create index simbad_geom_idx
    on public.simbad using gist (geom public.gist_geometry_ops_nd);

create unique index simbad_index_uindex
    on public.simbad (index);

create index simbad_teff_index
    on public.simbad (teff);

create index simbad_object_type_index
    on public.simbad (object_type);

create trigger a_ly_default_simbad
    before insert or update
    on public.simbad
    for each row
    when (new.distance_ly IS NOT NULL AND new.parallax IS NOT NULL)
execute procedure public.trg_ly_default();

create trigger c_cartesian_insert
    before insert
    on public.simbad
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

create trigger c_cartesian_update
    before update
    on public.simbad
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

create trigger b_gcoord_insert
    before insert
    on public.simbad
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

create trigger b_gcoord_update
    before update
    on public.simbad
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

create table public.export_overrides
(
    index             bigint not null
        constraint namefixes_pk
            primary key,
    new_name6         text,
    new_name7         text,
    new_name8         text,
    new_spectral_type text
);

alter table public.export_overrides
    owner to postgres;

create table public.gaia
(
    id               bigserial
        primary key,
    index            bigint             not null,
    source_id        bigint,
    ra               double precision   not null,
    dec              double precision   not null,
    glon             double precision,
    glat             double precision,
    parallax         double precision   not null,
    luminosity       double precision,
    radius           double precision,
    non_single_star  smallint,
    teff_source      smallint default 1 not null,
    teff_gspphot     real,
    logg_gspphot     real,
    mh_gspphot       real,
    distance_ly      real     default 0 not null,
    ruwe             real               not null,
    ebpminrp_gspphot real,
    phot_g_mean_mag  real,
    phot_bp_mean_mag real,
    phot_rp_mean_mag real,
    bp_rp            real,
    x                double precision,
    y                double precision,
    z                double precision,
    geom             geometry
)
    with (autovacuum_enabled = on, fillfactor = 70);

alter table public.gaia
    owner to postgres;

create unique index gaia_index_idx
    on public.gaia (index);

create index gaia_source_id_idx
    on public.gaia (source_id);

create index gaia_distance_ly_idx
    on public.gaia (distance_ly);

create index gaia_geom_idx
    on public.gaia using gist (geom public.gist_geometry_ops_nd);

create trigger c_cartesian_update
    before update
    on public.gaia
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

create trigger c_cartesian_insert
    before insert
    on public.gaia
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

create trigger b_gcoord_insert
    before insert
    on public.gaia
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

create trigger b_gcoord_update
    before update
    on public.gaia
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

create trigger a_ly_default
    before insert or update
    on public.gaia
    for each row
    when (new.parallax IS NOT NULL)
execute procedure public.trg_ly_default();

