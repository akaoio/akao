# Indexing & Discovery Architecture Guide

**Version** 1.0 · **Status** Production · **Date** 2026-04-07

> Comprehensive guide to the 4-layer data architecture powering decentralized P2P trading: Gun (sync) → Pen (validation) → SQL (query) → IDB (cache)

---

## Table of Contents

1. [Overview](#overview)
2. [Architecture Layers](#architecture-layers)
3. [Order Book Discovery](#order-book-discovery)
4. [SQLite Query Layer](#sqlite-query-layer)
5. [IndexedDB Cache Layer](#indexeddb-cache-layer)
6. [Implementation Examples](#implementation-examples)
7. [Performance Considerations](#performance-considerations)

---

## Overview

### Design Philosophy

**Decentralized-first, Local-first, Offline-capable**

- **No central database** - Gun provides P2P sync across peers
- **Client-side validation** - Pen DSL enforces schema before data enters network
- **Advanced local queries** - SQLite enables complex filtering/sorting without server
- **Instant UX** - IndexedDB cache serves data while sync happens in background

### The 4-Layer Stack

```
┌─────────────────────────────────────────────────────────┐
│ Layer 4: UI Components                                  │
│ (React-like components, template rendering)             │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ Layer 3: IDB (IndexedDB Cache)                          │
│ - Instant reads from local cache                        │
│ - Reactive subscriptions (.on())                        │
│ - Key-value storage for simple queries                  │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ Layer 2: SQL (SQLite WASM + OPFS)                       │
│ - Complex relational queries (JOIN, GROUP BY, ORDER BY) │
│ - Runs in dedicated Web Worker                          │
│ - Persistent storage via OPFS                           │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│ Layer 1: Gun + Pen (P2P Sync + Validation)              │
│ - Real-time data sync across peers                      │
│ - Temporal validation (candle-based expiry)             │
│ - Proof-of-work anti-spam                               │
│ - Schema enforcement at write time                      │
└─────────────────────────────────────────────────────────┘
```

---

## Architecture Layers

### Layer 1: Gun + Pen (P2P Sync + Validation)

**Purpose**: Decentralized data sync with client-side validation

**Technology**: 
- [GunDB](https://gun.eco) - Distributed graph database
- Pen DSL - Schema validation language

**Key Features**:
- Real-time sync across all connected peers
- No central server required
- Cryptographic signatures for authenticity
- Temporal validation prevents stale data
- Proof-of-work prevents spam

**Use Cases**:
- Order book publishing
- Real-time order updates
- User profile sync
- Chat/messaging

**Code Pattern**:

```javascript
import gun from "/core/Gun.js"
import SEA from "/core/SEA.js"

// Define validation schema
const orderSoul = SEA.pen({
    key: { and: [
        SEA.candle({ seg: 0, sep: ':', size: 300000, back: 100, fwd: 2 }),
        { seg: { sep: ':', idx: 1, match: { eq: itemSlug } } },
        { seg: { sep: ':', idx: 2, match: { or: [{ eq: 'buy' }, { eq: 'sell' }] } } }
    ]},
    sign: true,
    pow: { field: 0, difficulty: 2 }
})

// Write data (validated at Gun peer level)
const key = `${candle}:${item}:${type}:${nonce}`
gun.get(orderSoul).get(key).put(orderData, ack => {
    if (ack.err) console.error('Validation failed:', ack.err)
})

// Subscribe to updates (real-time)
gun.get(orderSoul).map().on((order, key) => {
    console.log('New order:', key, order)
})
```

---

### Layer 2: SQL (SQLite WASM + OPFS)

**Purpose**: Complex queries on local data

**Technology**:
- [@sqlite.org/sqlite-wasm](https://www.npmjs.com/package/@sqlite.org/sqlite-wasm) - SQLite compiled to WebAssembly
- OPFS (Origin Private File System) - Persistent file storage API
- Dedicated Web Worker - Runs SQLite without blocking UI

**Key Features**:
- Full SQL support (JOIN, GROUP BY, aggregate functions)
- ACID transactions
- WAL mode for better concurrency
- Persistent across sessions
- ~100x faster than IDB for complex queries

**Architecture**:

```
Main Thread                  Worker Thread                OPFS
-----------                  -------------                ----
SQL.all()  ─────postMessage────→ sqlite3.exec() ─────→  app.db
           ←────postMessage───── return rows            app.db-wal
                                                         app.db-shm
```

**Code Pattern**:

```javascript
import SQL from "/core/SQL.js"

// Initialize (lazy-loads worker + WASM)
const db = new SQL({ name: "shop" })
await db.ready

// Create tables
await db.exec(`
    CREATE TABLE IF NOT EXISTS orders (
        id TEXT PRIMARY KEY,
        item TEXT,
        type TEXT,
        price REAL,
        quantity INTEGER,
        trader TEXT,
        created INTEGER
    )
`)

// Complex queries
const cheapOrders = await db.all(`
    SELECT * FROM orders 
    WHERE type = 'sell' AND price < ?
    ORDER BY price ASC
    LIMIT 10
`, [100])

// Joins across tables
const orderWithItem = await db.all(`
    SELECT o.*, i.name, i.rarity 
    FROM orders o
    JOIN items i ON o.item = i.id
    WHERE o.trader != ?
`, [myPubKey])
```

**When to Use SQL vs IDB**:
- ✅ SQL: Filtering, sorting, joining, aggregation
- ❌ SQL: Simple key-value reads (overkill, use IDB)

---

### Layer 3: IDB (IndexedDB Cache)

**Purpose**: Fast key-value cache with reactive subscriptions

**Technology**:
- Native IndexedDB API
- Custom reactive wrapper (`/core/IDB.js`)

**Key Features**:
- Instant reads (no worker overhead)
- Reactive `.on()` subscriptions
- Automatic sync with Gun
- Fluent API: `IDB.get(path).put(value)`

**Code Pattern**:

```javascript
import { Indexes } from "/core/Stores.js"

// Write to cache
await Indexes.Statics.get(["items", itemId]).put(itemData)

// Read from cache (instant)
const item = await Indexes.Statics.get(["items", itemId]).once()

// Reactive subscription
Indexes.Statics.get(["items", itemId]).on(({ value }) => {
    console.log('Item updated:', value)
})
```

**Stores Available**:
- `Indexes.Hashes` - File hash cache (for DB.get validation)
- `Indexes.Statics` - Static file cache
- `Indexes.Auth` - Authentication state
- `Indexes.Lives` - Real-time data (orders, trades)
- `Indexes.Cart` - Shopping cart state

---

### Layer 4: DB (Unified Data Access)

**Purpose**: Smart routing between HTTP, IDB, SQL, Gun

**Pattern**:

```javascript
import DB from "/core/DB.js"

// DB.get() automatically:
// 1. Checks IDB cache
// 2. Validates hash
// 3. Fetches from HTTP if stale
// 4. Syncs to SQL in background
const data = await DB.get(["statics", "items", itemId, "meta.json"])

// SQL queries (when you need complex logic)
const results = await DB.query(`
    SELECT * FROM orders 
    WHERE price BETWEEN ? AND ?
`, [min, max])
```

---

## Order Book Discovery

### Temporal Key Format

**Key Structure**: `<candle>:<item_slug>:<type>:<nonce>`

**Examples**:
```
5820000:penitent-greaves:buy:a3f7b2    # Buy order
5820001:penitent-greaves:sell:x8k2m1   # Sell order
5820002:diablo-4-gold:buy:m9n3k5       # Another buy order
```

**Candle Calculation**:
```javascript
const CANDLE_SIZE = 300000 // 5 minutes in ms
const candle = Math.floor(Date.now() / CANDLE_SIZE)
```

**Benefits**:
- Auto-expiry: Orders outside candle window are rejected
- Chronological ordering: Lower candle = older order
- Efficient range queries: `5820000:*` to `5820100:*`

---

### Creating an Order

**Step 1: Define Pen Schema** (`src/core/OrderPen.js`)

```javascript
import SEA from "/core/SEA.js"

export function createOrderSoul() {
    return SEA.pen({
        key: { and: [
            // Temporal validation: ±8 hours past, ±10 min future
            SEA.candle({ 
                seg: 0,           // Candle is first segment
                sep: ':',         // Separator
                size: 300000,     // 5-minute candles
                back: 100,        // 100 candles back (~8 hours)
                fwd: 2            // 2 candles forward (~10 min)
            }),
            
            // Item slug validation
            { seg: { 
                sep: ':', 
                idx: 1, 
                of: { reg: 0 }, 
                match: { length: [1, 128] } 
            }},
            
            // Type validation: buy or sell only
            { seg: { 
                sep: ':', 
                idx: 2, 
                of: { reg: 0 },
                match: { or: [{ eq: 'buy' }, { eq: 'sell' }] }
            }}
        ]},
        val: { type: 'string' },  // JSON-encoded order metadata
        sign: true,               // Require cryptographic signature
        pow: {                    // Proof-of-work anti-spam
            field: 0,             // Apply PoW to key (not value)
            difficulty: 2         // Require 2 leading zero bits
        }
    })
}
```

**Step 2: Publish Order** (`src/UI/routes/order/logic.js`)

```javascript
import gun from "/core/Gun.js"
import { createOrderSoul } from "/core/OrderPen.js"
import { Access } from "/core/Access.js"

export async function createOrder({ item, type, quantity, price, currency, chain }) {
    const soul = createOrderSoul()
    const candle = Math.floor(Date.now() / 300000)
    
    // Generate random nonce (for PoW mining)
    let nonce = Math.random().toString(36).slice(2, 8)
    
    // Key format: <candle>:<item>:<type>:<nonce>
    const key = `${candle}:${item}:${type}:${nonce}`
    
    // Order metadata (stored as value, not in key)
    const orderData = JSON.stringify({
        orderId: sha256(`${Access.states.pub}:${item}:${Date.now()}`),
        type,
        item,
        quantity,
        price,
        currency,
        chain,
        trader: Access.states.pub,
        created: Date.now(),
        status: 'open'
    })
    
    // Publish to Gun (Pen validates key + PoW)
    return new Promise((resolve, reject) => {
        gun.get(soul).get(key).put(orderData, ack => {
            if (ack.err) reject(new Error(ack.err))
            else resolve({ key, orderId: JSON.parse(orderData).orderId })
        }, { authenticator: Access.states.pair })
    })
}
```

---

### Discovering Orders

**Pattern 1: Real-time Subscription** (Gun `.map()`)

```javascript
export function subscribeOrders(itemSlug, type, callback) {
    const soul = createOrderSoul()
    const candle = Math.floor(Date.now() / 300000)
    
    // Subscribe to current + recent candles
    for (let i = 0; i < 10; i++) {
        const c = candle - i
        const prefix = `${c}:${itemSlug}:${type}:`
        
        gun.get(soul).map({
            '.': { '*': prefix }  // Wildcard match: c:item:type:*
        }).on((orderJson, key) => {
            if (!orderJson) return
            
            try {
                const order = JSON.parse(orderJson)
                callback({ ...order, key })
            } catch (e) {
                console.error('Invalid order JSON:', key)
            }
        })
    }
}

// Usage
subscribeOrders('penitent-greaves', 'buy', (order) => {
    console.log('New buy order:', order.price, order.quantity)
})
```

**Pattern 2: SQL Query** (Local filtering after sync)

```javascript
import DB from "/core/DB.js"

export async function findCheapestOrders(itemSlug, maxPrice, limit = 10) {
    return DB.query(`
        SELECT * FROM orders
        WHERE item = ? 
          AND type = 'sell'
          AND price <= ?
          AND status = 'open'
        ORDER BY price ASC, created ASC
        LIMIT ?
    `, [itemSlug, maxPrice, limit])
}

// Usage
const deals = await findCheapestOrders('penitent-greaves', 100, 5)
```

**Pattern 3: IDB Cache** (Instant reads for known orders)

```javascript
import { Indexes } from "/core/Stores.js"

// Cache order when discovered
gun.get(soul).map().on((orderJson, key) => {
    const order = JSON.parse(orderJson)
    Indexes.Lives.get(["orders", order.orderId]).put(order)
})

// Read from cache (instant, no Gun query)
const order = await Indexes.Lives.get(["orders", orderId]).once()
```

---

## SQLite Query Layer

### Worker Architecture

**File Structure**:
```
src/core/
├── SQL.js                          # Main thread interface
├── SQL/
│   ├── sqlite3.js                  # WASM loader
│   ├── sqlite3.wasm                # SQLite binary
│   └── sqlite3-opfs-async-proxy.js # OPFS helper
└── threads/
    └── sql.js                       # Dedicated worker
```

**Communication Flow**:

```javascript
// Main Thread (SQL.js)
class SQL {
    constructor({ name }) {
        this.name = name
        this.ready = Threads.queue("sql", { 
            method: "init", 
            params: [name] 
        })
    }
    
    async exec(sql) {
        return Threads.queue("sql", { 
            method: "exec", 
            params: [sql] 
        })
    }
    
    async all(sql, params = []) {
        return Threads.queue("sql", { 
            method: "all", 
            params: [sql, params] 
        })
    }
}

// Worker Thread (threads/sql.js)
import Thread from "/core/Thread.js"
import sqlite3InitModule from "/core/SQL/sqlite3.js"

const thread = new Thread()
let db = null

thread.init = async function() {
    const sqlite3 = await sqlite3InitModule()
    db = new sqlite3.oo1.OpfsDb("/shop.db")
    
    // Enable WAL mode for better concurrency
    db.exec("PRAGMA journal_mode=WAL")
    db.exec("PRAGMA wal_autocheckpoint=0")
    
    // Periodic checkpoint (don't block writes)
    setInterval(() => {
        db.exec("PRAGMA wal_checkpoint(PASSIVE)")
    }, 60000) // Every minute
}

thread.exec = async function({ sql }) {
    db.exec(sql)
    return { success: true }
}

thread.all = async function({ sql, params }) {
    const stmt = db.prepare(sql)
    stmt.bind(params)
    
    const rows = []
    while (stmt.step()) {
        rows.push(stmt.getAsObject())
    }
    stmt.finalize()
    
    return rows
}
```

---

### Schema Design

**Orders Table**:

```sql
CREATE TABLE IF NOT EXISTS orders (
    orderId TEXT PRIMARY KEY,
    type TEXT NOT NULL,           -- 'buy' or 'sell'
    item TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    price REAL NOT NULL,
    currency TEXT NOT NULL,
    chain INTEGER NOT NULL,
    trader TEXT NOT NULL,         -- SEA pub key
    created INTEGER NOT NULL,     -- Unix timestamp ms
    status TEXT NOT NULL,         -- 'open', 'matched', 'completed', 'cancelled'
    candle INTEGER NOT NULL,      -- For efficient pruning
    
    -- Indexes for common queries
    INDEX idx_item_type ON orders(item, type),
    INDEX idx_price ON orders(price),
    INDEX idx_candle ON orders(candle)
)
```

**Trades Table**:

```sql
CREATE TABLE IF NOT EXISTS trades (
    tradeId TEXT PRIMARY KEY,
    orderId TEXT NOT NULL,
    maker TEXT NOT NULL,
    taker TEXT NOT NULL,
    item TEXT NOT NULL,
    quantity INTEGER NOT NULL,
    price REAL NOT NULL,
    currency TEXT NOT NULL,
    chain INTEGER NOT NULL,
    status TEXT NOT NULL,
    created INTEGER NOT NULL,
    completed INTEGER,
    
    FOREIGN KEY (orderId) REFERENCES orders(orderId)
)
```

---

### Sync Strategy

**Gun → SQL Pipeline**:

```javascript
// src/core/DB/transformer.js
export function transform(path, data) {
    // Route Gun data to SQL tables
    if (path[0] === 'orders') {
        return {
            schema: `CREATE TABLE IF NOT EXISTS orders (...)`,
            upsert: `
                INSERT INTO orders VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ON CONFLICT(orderId) DO UPDATE SET
                    status = excluded.status,
                    price = excluded.price
            `,
            values: [
                data.orderId,
                data.type,
                data.item,
                data.quantity,
                data.price,
                data.currency,
                data.chain,
                data.trader,
                data.created,
                data.status
            ]
        }
    }
}

// src/core/DB.js
export class DB {
    static $syncToSQL(path, data) {
        const op = transform(path, data)
        if (!op) return
        
        DB.sql().then(db => {
            const ready = DB._schemas.has(op.schema)
                ? Promise.resolve()
                : db.exec(op.schema).then(() => DB._schemas.add(op.schema))
            
            return ready.then(() => db.run(op.upsert, op.values))
        }).catch(err => console.warn('[DB.$syncToSQL]', path, err))
    }
}
```

---

## IndexedDB Cache Layer

### IDB Store Design

**Hierarchy**:

```
Indexes (root)
├── Hashes (hash validation)
│   └── ["statics", "items", "abc", "meta.json"] → "sha256hash..."
├── Statics (static file cache)
│   └── ["statics", "items", "abc", "meta.json"] → { name: "...", ... }
├── Lives (real-time data)
│   ├── ["orders", orderId] → { price: 100, ... }
│   └── ["trades", tradeId] → { status: "completed", ... }
├── Auth (authentication)
│   └── ["user", pubKey] → { avatar: 3, ... }
└── Cart (shopping cart)
    └── [itemId] → { quantity: 2 }
```

### Reactive Pattern

**Implementation** (`src/core/IDB.js`):

```javascript
export class IDB {
    constructor(name) {
        this.name = name
        this.db = null
        this.ready = this._open()
        this.callbacks = new Map() // path → Set(callbacks)
    }
    
    get(path) {
        const self = this
        const key = JSON.stringify(path)
        
        return {
            // Write + notify subscribers
            async put(value) {
                await self.ready
                await self.$put(key, value)
                self.$notify(key, value)
            },
            
            // Read once
            async once() {
                await self.ready
                return self.$get(key)
            },
            
            // Subscribe to changes
            on(callback) {
                if (!self.callbacks.has(key)) {
                    self.callbacks.set(key, new Set())
                }
                self.callbacks.get(key).add(callback)
                
                // Return unsubscribe function
                return () => {
                    self.callbacks.get(key)?.delete(callback)
                }
            }
        }
    }
    
    $notify(key, value) {
        this.callbacks.get(key)?.forEach(cb => {
            cb({ value, key: JSON.parse(key) })
        })
    }
}
```

**Usage**:

```javascript
import { Indexes } from "/core/Stores.js"

// Component lifecycle
export class CART extends HTMLElement {
    constructor() {
        super()
        this.subscriptions = []
    }
    
    connectedCallback() {
        // Subscribe to cart changes
        const unsub = Indexes.Cart.get([itemId]).on(({ value }) => {
            this.render({ quantity: value?.quantity || 0 })
        })
        this.subscriptions.push(unsub)
    }
    
    disconnectedCallback() {
        // Clean up subscriptions
        this.subscriptions.forEach(fn => fn())
    }
    
    async addToCart() {
        const current = await Indexes.Cart.get([itemId]).once()
        const quantity = (current?.quantity || 0) + 1
        
        // Update triggers .on() callbacks automatically
        await Indexes.Cart.get([itemId]).put({ 
            itemId, 
            quantity,
            added: Date.now()
        })
    }
}
```

---

## Implementation Examples

### Complete Order Flow

**1. Maker Creates Buy Order**:

```javascript
// UI/routes/order/create.js
import { createOrder } from "./logic.js"
import { Access } from "/core/Access.js"

async function handleCreateOrder(form) {
    const { item, quantity, price, currency, chain } = form
    
    // Step 1: Create Fund Proof wallet (FP)
    const secret_M = await SEA.secret(Platform.pub, Access.states.pair)
    const root_M = HDNodeWallet.fromSeed(sha256(secret_M))
    const orderId = sha256(`${Access.states.pub}:${item}:${Date.now()}`)
    const index_order = parseInt(sha256(orderId).slice(0,8), 16) & 0x7fffffff
    const FP = root_M.deriveChild(index_order)
    
    // Step 2: Deposit to FP
    const totalAmount = price * quantity + platformFee + commission
    await wallet.sendTransaction({ 
        to: FP.address, 
        value: ethers.parseUnits(totalAmount.toString(), 6) 
    })
    
    // Step 3: Publish order to Gun (includes FP address for verification)
    const { key } = await createOrder({
        item,
        type: 'buy',
        quantity,
        price,
        currency,
        chain,
        fundProof: FP.address  // Taker verifies this on-chain
    })
    
    console.log('Order created:', key)
    alert('Order published! Waiting for taker...')
}
```

**2. Taker Discovers and Accepts**:

```javascript
// UI/routes/order/discover.js
import { subscribeOrders } from "./logic.js"
import { Chain } from "/core/Stores.js"

// Subscribe to buy orders for this item
subscribeOrders(itemSlug, 'buy', async (order) => {
    // Verify maker has funds (on-chain check)
    const chain = Chain.get(order.chain)
    const balance = await chain.provider.getBalance(order.fundProof)
    const required = ethers.parseUnits(
        (order.price * order.quantity + 10).toString(), 
        6
    )
    
    if (balance < required) {
        console.warn('Insufficient funds in FP:', order.orderId)
        return // Skip this order
    }
    
    // Display order in UI
    renderOrder(order)
})

async function handleAcceptOrder(order) {
    // Create trade request
    const tradeId = sha256(`${order.orderId}:${Access.states.pub}:${Date.now()}`)
    
    gun.get('trades').get(tradeId).put({
        tradeId,
        orderId: order.orderId,
        maker: order.trader,
        taker: Access.states.pub,
        status: 'matched',
        created: Date.now()
    }, ack => {
        if (ack.err) console.error('Trade creation failed:', ack.err)
        else alert('Trade matched! Waiting for maker deposit to escrow...')
    })
}
```

**3. Real-time Trade Monitoring**:

```javascript
// UI/routes/trade/monitor.js
import { Indexes } from "/core/Stores.js"

export class TRADE_MONITOR extends HTMLElement {
    connectedCallback() {
        const tradeId = this.getAttribute('trade-id')
        
        // Subscribe to trade status changes
        this.unsub = Indexes.Lives.get(['trades', tradeId]).on(({ value: trade }) => {
            this.render(trade)
            
            // Update UI based on status
            switch (trade.status) {
                case 'matched':
                    this.showStatus('⏳ Waiting for deposit to escrow...')
                    break
                case 'deposited':
                    this.showStatus('✅ Funds locked. Deliver item in-game.')
                    break
                case 'delivered':
                    this.showStatus('📦 Item delivered. Confirm receipt?')
                    break
                case 'completed':
                    this.showStatus('🎉 Trade completed!')
                    break
            }
        })
    }
    
    disconnectedCallback() {
        this.unsub?.()
    }
}
```

---

### SQL Analytics Queries

**Order Book Analytics**:

```javascript
// Get price spread for an item
const spread = await DB.query(`
    SELECT 
        (SELECT MIN(price) FROM orders WHERE item = ? AND type = 'sell' AND status = 'open') as ask,
        (SELECT MAX(price) FROM orders WHERE item = ? AND type = 'buy' AND status = 'open') as bid
`, [itemId, itemId])

console.log('Spread:', spread[0].ask - spread[0].bid)

// Get order book depth (aggregated by price level)
const depth = await DB.query(`
    SELECT 
        type,
        price,
        SUM(quantity) as total_quantity,
        COUNT(*) as order_count
    FROM orders
    WHERE item = ? AND status = 'open'
    GROUP BY type, price
    ORDER BY type, price
`, [itemId])

// Top traders by volume
const topTraders = await DB.query(`
    SELECT 
        trader,
        COUNT(*) as trade_count,
        SUM(price * quantity) as total_volume
    FROM trades
    WHERE status = 'completed'
        AND created > ?
    GROUP BY trader
    ORDER BY total_volume DESC
    LIMIT 10
`, [Date.now() - 7 * 86400000]) // Last 7 days
```

---

## Performance Considerations

### Query Optimization

**❌ Bad: Scan entire Gun graph**
```javascript
gun.get('orders').map().on((order) => {
    if (order.price < 100 && order.type === 'sell') {
        // This iterates ALL orders in memory!
    }
})
```

**✅ Good: SQL indexed query**
```javascript
const cheap = await DB.query(`
    SELECT * FROM orders 
    WHERE type = 'sell' AND price < 100
    ORDER BY price ASC
`)
```

---

### Candle Pruning

**Auto-delete stale orders**:

```javascript
// Run daily cleanup
setInterval(async () => {
    const cutoff = Math.floor(Date.now() / 300000) - 1000 // 1000 candles ago
    
    await DB.exec(`
        DELETE FROM orders 
        WHERE candle < ${cutoff}
    `)
    
    console.log('Pruned old orders')
}, 86400000) // 24 hours
```

---

### IDB Cache Strategy

**Cache hot data, not everything**:

```javascript
// ❌ Bad: Cache every order
gun.get(soul).map().on((order, key) => {
    Indexes.Lives.get(['orders', key]).put(order) // Bloats cache
})

// ✅ Good: Cache only user's orders
gun.get(soul).map().on((order, key) => {
    if (order.trader === myPubKey) {
        Indexes.Lives.get(['orders', key]).put(order)
    }
})
```

---

### Worker Thread Limits

**Don't spawn too many workers**:

```javascript
// ❌ Bad: New worker per query
const db1 = new SQL({ name: 'db1' })
const db2 = new SQL({ name: 'db2' })
// Uses 2 workers!

// ✅ Good: Reuse single worker
const db = new SQL({ name: 'shop' })
// All queries use same worker
```

---

## Summary

### When to Use Each Layer

| Task | Layer | Why |
|------|-------|-----|
| Publish order | Gun + Pen | P2P sync, no server |
| Real-time discovery | Gun `.map()` | Live subscriptions |
| Filter by price | SQL | Indexed queries |
| Get known order | IDB | Instant cache read |
| Analytics/reporting | SQL | Complex aggregations |
| User preferences | IDB | Simple key-value |

### Data Flow Example

```
1. Maker creates order
   → Gun.put() validates with Pen
   → Broadcasts to peers
   
2. Taker's peer receives
   → Gun.on() callback fires
   → Syncs to IDB cache
   → Syncs to SQL table
   
3. Taker queries
   → SQL: "price < 100" filter
   → Returns filtered results
   → UI renders from SQL rows
   
4. Taker accepts
   → Gun.put() creates trade
   → Real-time sync to Maker
   → Both see status update instantly
```

---

## References

- [GunDB Documentation](https://gun.eco/docs/)
- [Pen DSL Guide](./pen-dsl-guide.md)
- [SQLite WASM Architecture](./sqlite-wasm-opfs-worker.md)
- [P2P Trading White Paper](./white-paper.md)
- [Temporal PoW Gossip Protocol](./temporal-proof-of-work-gossip-protocol.md)

---

**Last Updated**: 2026-04-07
