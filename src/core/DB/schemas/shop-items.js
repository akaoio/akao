export const SCHEMA_SHOP_ITEMS = `
    CREATE TABLE IF NOT EXISTS shop_items (
        id        TEXT NOT NULL,
        locale    TEXT NOT NULL,
        name      TEXT,
        price     REAL,
        currency  TEXT,
        category  TEXT,
        data      TEXT,
        path      TEXT,
        synced_at INTEGER DEFAULT (unixepoch()),
        PRIMARY KEY (id, locale)
    )
`
