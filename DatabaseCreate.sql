--create database star_data    with owner postgres;

--create extension postgis;

drop table if exists public.simbad;

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
    name          text,
    name_wolf     text,
    name_ross     text,
    name_s        text,
    name_ss       text,
    name_vs       text,
    name_hip      text,
    name_hd       text,
    name_gj       text,
    name_wise     text,
    name_2mass    text,
    name_gaia     text,
    name13        text,
    name14        text,
    name15        text,
    spectral_type text,
    geom          geometry,
    g_mag         double precision,
    b             double precision,
    v             double precision,
    object_type   integer
);

alter table public.simbad
    owner to postgres;

create index if not exists "simbad_lower(name)_index" on public.simbad (lower(name));
create index if not exists simbad_distance_ly_index on public.simbad (distance_ly);
create index if not exists simbad_name_index on public.simbad (name);
create index if not exists simbad_g_source_id_index on public.simbad (g_source_id);
create index if not exists simbad_geom_idx on public.simbad using gist (geom public.gist_geometry_ops_nd);
create unique index if not exists simbad_index_uindex on public.simbad (index);
create index if not exists simbad_teff_index on public.simbad (teff);
create index if not exists simbad_object_type_index on public.simbad (object_type);

drop function if exists public.trg_cartesian_update();
create function public.trg_cartesian_update() returns trigger
    language plpgsql
as
$$
DECLARE
    lat float8;
    lon float8;
BEGIN
    lat := Radians(NEW.glat);
    lon := Radians(NEW.glon);

    NEW.x := (NEW.distance_ly * Cos(lon) * Cos(lat));
    NEW.y := (NEW.distance_ly * Cos(lon) * Sin(lat));
    NEW.z := (NEW.distance_ly * Sin(lon));

    -- Only be Sag A* which is 0,0,0.
    if NEW.distance_ly > 0 THEN
        NEW.x := NEW.x - 26669.9;
        NEW.y := NEW.y - -55.8;
        NEW.z := NEW.z;
    end if;

    RETURN NEW;
END
$$;

alter function public.trg_cartesian_update() owner to postgres;

drop function if exists public.trg_geom_default();
create function public.trg_geom_default() returns trigger
    language plpgsql
as
$$
BEGIN
    NEW.geom := ST_PointZ(NEW.x, NEW.y, NEW.z);

    RETURN NEW;
END
$$;

alter function public.trg_geom_default() owner to postgres;

drop function if exists public.trg_ly_default();
create function public.trg_ly_default() returns trigger
    language plpgsql
as
$$
BEGIN
    if NEW.parallax = 0 THEN
        NEW.distance_ly = 0;
    else
        NEW.distance_ly := (1000.0 / NEW.parallax * 3.26156378);
    end if;

    RETURN NEW;
END
$$;

alter function public.trg_ly_default() owner to postgres;

drop trigger if exists a_ly_default_simbad on public.simbad;
create trigger a_ly_default_simbad
    before insert or update
    on public.simbad
    for each row
    when (new.distance_ly IS NOT NULL AND new.parallax IS NOT NULL)
execute procedure public.trg_ly_default();

drop trigger if exists c_cartesian_insert on public.simbad;
create trigger c_cartesian_insert
    before insert
    on public.simbad
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

drop trigger if exists c_cartesian_update on public .simbad;
create trigger c_cartesian_update
    before update
    on public.simbad
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

drop trigger if exists b_gcoord_insert on public.simbad;
create trigger b_gcoord_insert
    before insert
    on public.simbad
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

drop trigger if exists b_gcoord_update on public.simbad;
create trigger b_gcoord_update
    before update
    on public.simbad
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

drop table if exists public.export_overrides;
create table public.export_overrides
(
    index             bigint not null
        constraint namefixes_pk
            primary key,
    new_name        text,
    new_name_s        text,
    new_name_ss       text,
    new_name_vs       text,
    new_spectral_type text
);

alter table public.export_overrides
    owner to postgres;

drop table if exists public.gaia;
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

alter table public.gaia  owner to postgres;
create unique index gaia_index_idx on public.gaia (index);
create index gaia_source_id_idx on public.gaia (source_id);
create index gaia_distance_ly_idx on public.gaia (distance_ly);
create index gaia_geom_idx on public.gaia using gist (geom public.gist_geometry_ops_nd);

drop trigger if exists c_cartesian_update on public.gaia;
create trigger c_cartesian_update
    before update
    on public.gaia
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

drop trigger if exists c_cartesian_insert on public.gaia;
create trigger c_cartesian_insert
    before insert
    on public.gaia
    for each row
    when (new.x IS NOT NULL AND new.y IS NOT NULL AND new.z IS NOT NULL)
execute procedure public.trg_geom_default();

drop trigger if exists b_gcoord_insert on public.gaia;
create trigger b_gcoord_insert
    before insert
    on public.gaia
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

drop trigger if exists b_gcoord_update on public.gaia;
create trigger b_gcoord_update
    before update
    on public.gaia
    for each row
    when (new.glat IS NOT NULL AND new.glon IS NOT NULL AND new.distance_ly IS NOT NULL)
execute procedure public.trg_cartesian_update();

drop trigger if exists a_ly_default on public.gaia;
create trigger a_ly_default
    before insert or update
    on public.gaia
    for each row
    when (new.parallax IS NOT NULL)
execute procedure public.trg_ly_default();

