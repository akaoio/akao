export const SCHEMA_GAME_ITEMS = `
    CREATE TABLE IF NOT EXISTS game_items (
        id        TEXT NOT NULL,
        game_id   TEXT NOT NULL,
        locale    TEXT NOT NULL,
        name      TEXT,
        rarity    TEXT,
        type      TEXT,
        value     REAL,
        icon      TEXT,
        data      TEXT,
        path      TEXT,
        synced_at INTEGER DEFAULT (unixepoch()),
        PRIMARY KEY (id, game_id, locale)
    );
    CREATE INDEX IF NOT EXISTS idx_game_items_filter ON game_items(game_id, locale, type, rarity);
    CREATE INDEX IF NOT EXISTS idx_game_items_name   ON game_items(game_id, locale, name);
`
