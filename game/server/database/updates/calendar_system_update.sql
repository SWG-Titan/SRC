-- ======================================================================
-- Calendar System Database Update Script
-- Copyright 2026 Titan Project
--
-- Run this script on your SWG Oracle database to add calendar support.
-- This script is safe to run multiple times - it checks for existing objects.
-- ======================================================================

-- Calendar Events table
DECLARE
  cnt NUMBER;
BEGIN
  SELECT COUNT(*) INTO cnt FROM user_tables WHERE table_name = 'CALENDAR_EVENTS';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE '
      CREATE TABLE calendar_events
      (
        event_id VARCHAR2(64) NOT NULL,
        title VARCHAR2(256) NOT NULL,
        description VARCHAR2(2000),
        event_type INT NOT NULL,
        event_year INT NOT NULL,
        event_month INT NOT NULL,
        event_day INT NOT NULL,
        event_hour INT NOT NULL,
        event_minute INT NOT NULL,
        duration INT NOT NULL,
        guild_id INT DEFAULT 0,
        city_id INT DEFAULT 0,
        server_event_key VARCHAR2(64),
        recurring CHAR(1) DEFAULT ''N'',
        recurrence_type INT DEFAULT 0,
        broadcast_start CHAR(1) DEFAULT ''N'',
        active CHAR(1) DEFAULT ''N'',
        creator_id NUMBER(20),
        created_date DATE DEFAULT SYSDATE,
        PRIMARY KEY (event_id)
      )';
    DBMS_OUTPUT.PUT_LINE('Created table: CALENDAR_EVENTS');
  ELSE
    DBMS_OUTPUT.PUT_LINE('Table CALENDAR_EVENTS already exists');
  END IF;
END;
/

-- Calendar Events indexes
DECLARE
  cnt NUMBER;
BEGIN
  SELECT COUNT(*) INTO cnt FROM user_indexes WHERE index_name = 'CAL_EVT_TYPE_IDX';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE 'CREATE INDEX cal_evt_type_idx ON calendar_events (event_type)';
    DBMS_OUTPUT.PUT_LINE('Created index: CAL_EVT_TYPE_IDX');
  END IF;

  SELECT COUNT(*) INTO cnt FROM user_indexes WHERE index_name = 'CAL_EVT_DATE_IDX';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE 'CREATE INDEX cal_evt_date_idx ON calendar_events (event_year, event_month, event_day)';
    DBMS_OUTPUT.PUT_LINE('Created index: CAL_EVT_DATE_IDX');
  END IF;

  SELECT COUNT(*) INTO cnt FROM user_indexes WHERE index_name = 'CAL_EVT_GUILD_IDX';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE 'CREATE INDEX cal_evt_guild_idx ON calendar_events (guild_id)';
    DBMS_OUTPUT.PUT_LINE('Created index: CAL_EVT_GUILD_IDX');
  END IF;

  SELECT COUNT(*) INTO cnt FROM user_indexes WHERE index_name = 'CAL_EVT_CITY_IDX';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE 'CREATE INDEX cal_evt_city_idx ON calendar_events (city_id)';
    DBMS_OUTPUT.PUT_LINE('Created index: CAL_EVT_CITY_IDX');
  END IF;

  SELECT COUNT(*) INTO cnt FROM user_indexes WHERE index_name = 'CAL_EVT_ACTIVE_IDX';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE 'CREATE INDEX cal_evt_active_idx ON calendar_events (active)';
    DBMS_OUTPUT.PUT_LINE('Created index: CAL_EVT_ACTIVE_IDX');
  END IF;
END;
/

-- Grant permissions on calendar_events
BEGIN
  EXECUTE IMMEDIATE 'GRANT SELECT ON calendar_events TO PUBLIC';
  DBMS_OUTPUT.PUT_LINE('Granted SELECT on CALENDAR_EVENTS to PUBLIC');
EXCEPTION
  WHEN OTHERS THEN NULL;
END;
/

-- Calendar Settings table
DECLARE
  cnt NUMBER;
BEGIN
  SELECT COUNT(*) INTO cnt FROM user_tables WHERE table_name = 'CALENDAR_SETTINGS';
  IF (cnt = 0) THEN
    EXECUTE IMMEDIATE '
      CREATE TABLE calendar_settings
      (
        setting_id INT NOT NULL,
        bg_texture VARCHAR2(256),
        src_x INT DEFAULT 0,
        src_y INT DEFAULT 0,
        src_w INT DEFAULT 512,
        src_h INT DEFAULT 512,
        last_modified DATE DEFAULT SYSDATE,
        modified_by NUMBER(20),
        PRIMARY KEY (setting_id)
      )';
    DBMS_OUTPUT.PUT_LINE('Created table: CALENDAR_SETTINGS');

    -- Insert default settings row
    EXECUTE IMMEDIATE 'INSERT INTO calendar_settings (setting_id, bg_texture, src_x, src_y, src_w, src_h) VALUES (1, ''ui_calendar_bg.dds'', 0, 0, 512, 512)';
    COMMIT;
    DBMS_OUTPUT.PUT_LINE('Inserted default calendar settings');
  ELSE
    DBMS_OUTPUT.PUT_LINE('Table CALENDAR_SETTINGS already exists');
  END IF;
END;
/

-- Grant permissions on calendar_settings
BEGIN
  EXECUTE IMMEDIATE 'GRANT SELECT ON calendar_settings TO PUBLIC';
  DBMS_OUTPUT.PUT_LINE('Granted SELECT on CALENDAR_SETTINGS to PUBLIC');
EXCEPTION
  WHEN OTHERS THEN NULL;
END;
/

-- Update version number (if version_number table exists)
DECLARE
  cnt NUMBER;
BEGIN
  SELECT COUNT(*) INTO cnt FROM user_tables WHERE table_name = 'VERSION_NUMBER';
  IF (cnt > 0) THEN
    UPDATE version_number SET version_number = GREATEST(version_number, 272), min_version_number = GREATEST(min_version_number, 272);
    COMMIT;
    DBMS_OUTPUT.PUT_LINE('Updated VERSION_NUMBER to 272');
  END IF;
END;
/

-- ======================================================================
-- Update loader package header
-- ======================================================================
-- Note: The loader and persister packages need to be recompiled after
-- applying the schema changes. This is done by running the package
-- build scripts or by executing:
--   @loader.plsqlh
--   @loader.plsql
--   @persister.plsqlh
--   @persister.plsql
-- ======================================================================

DBMS_OUTPUT.PUT_LINE('');
DBMS_OUTPUT.PUT_LINE('======================================================================');
DBMS_OUTPUT.PUT_LINE('Calendar system database schema update complete!');
DBMS_OUTPUT.PUT_LINE('');
DBMS_OUTPUT.PUT_LINE('Tables created:');
DBMS_OUTPUT.PUT_LINE('  - CALENDAR_EVENTS');
DBMS_OUTPUT.PUT_LINE('  - CALENDAR_SETTINGS');
DBMS_OUTPUT.PUT_LINE('');
DBMS_OUTPUT.PUT_LINE('Next steps:');
DBMS_OUTPUT.PUT_LINE('  1. Recompile the loader and persister packages');
DBMS_OUTPUT.PUT_LINE('  2. Restart the game server');
DBMS_OUTPUT.PUT_LINE('======================================================================');
/

