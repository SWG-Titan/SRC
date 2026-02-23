-- Load loader and persister packages with scale support
--
-- Usage:
--   cd /home/swg/swg-main/src/game/server/database/packages
--   sqlplus username/password@database @load_scale_packages.sql
--
-- Or with full path:
--   sqlplus username/password@database @/home/swg/swg-main/src/game/server/database/packages/load_scale_packages.sql
--   (must run from packages dir for @ to find loader.plsqlh etc, or use full paths below)

set echo on
set feedback on
whenever sqlerror exit failure

-- Loader: spec first, then body
@loader.plsqlh
@loader.plsql

-- Persister: spec first, then body
@persister.plsqlh
@persister.plsql

exit
