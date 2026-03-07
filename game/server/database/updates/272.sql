-- Calendar Events table
declare
  cnt number;
begin
  select count(*) into cnt from user_tables where table_name = 'CALENDAR_EVENTS';
  if (cnt = 0) then
    execute immediate '
      create table calendar_events
      (
        event_id varchar2(64) not null,
        title varchar2(256) not null,
        description varchar2(2000),
        event_type int not null,
        event_year int not null,
        event_month int not null,
        event_day int not null,
        event_hour int not null,
        event_minute int not null,
        duration int not null,
        guild_id int default 0,
        city_id int default 0,
        server_event_key varchar2(64),
        recurring char(1) default ''N'',
        recurrence_type int default 0,
        broadcast_start char(1) default ''N'',
        active char(1) default ''N'',
        creator_id number(20),
        created_date date default sysdate,
        primary key (event_id)
      )';
    execute immediate 'create index cal_evt_type_idx on calendar_events (event_type)';
    execute immediate 'create index cal_evt_date_idx on calendar_events (event_year, event_month, event_day)';
    execute immediate 'create index cal_evt_guild_idx on calendar_events (guild_id)';
    execute immediate 'create index cal_evt_city_idx on calendar_events (city_id)';
    execute immediate 'create index cal_evt_active_idx on calendar_events (active)';
    execute immediate 'grant select on calendar_events to public';
  end if;
end;
/

-- Calendar Settings table
declare
  cnt number;
begin
  select count(*) into cnt from user_tables where table_name = 'CALENDAR_SETTINGS';
  if (cnt = 0) then
    execute immediate '
      create table calendar_settings
      (
        setting_id int not null,
        bg_texture varchar2(256),
        src_x int default 0,
        src_y int default 0,
        src_w int default 512,
        src_h int default 512,
        last_modified date default sysdate,
        modified_by number(20),
        primary key (setting_id)
      )';
    execute immediate 'grant select on calendar_settings to public';
    execute immediate 'insert into calendar_settings (setting_id, bg_texture, src_x, src_y, src_w, src_h) values (1, ''ui_calendar_bg.dds'', 0, 0, 512, 512)';
    commit;
  end if;
end;
/

update version_number set version_number=272, min_version_number=272;

