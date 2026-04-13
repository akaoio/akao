# SQLite WASM + OPFS Architecture

## Mục tiêu

Xây dựng một lớp persistence mạnh mẽ, hiệu năng cao cho browser dựa trên:
- **SQLite WASM** (`@sqlite.org/sqlite-wasm`) chạy hoàn toàn trong dedicated worker
- **OPFS** (Origin Private File System) làm backend lưu trữ thật
- **SQL.js** — class giao tiếp từ bất kỳ context nào (main thread, component, khác worker)
- **OPFS.js** — class độc lập encapsulate native OPFS API, phục vụ nhiều tác vụ (không chỉ SQLite)

Tương lai: `SQL` và `IDB` đều được điều phối qua `DB.js` — trở thành trung tâm dữ liệu hệ thống.

---

## Tại sao cần thiết kế như vậy?

### SQLite trong browser không thể chạy ở main thread
`@sqlite.org/sqlite-wasm` với OPFS VFS dùng `FileSystemSyncAccessHandle` — API **chỉ khả dụng** trong **dedicated worker**. Nếu chạy ở main thread sẽ block UI hoặc không khả dụng.

### Ghi OPFS liên tục sẽ crash
`FileSystemSyncAccessHandle.flush()` là thao tác nặng. SQLite nội bộ gọi flush mỗi khi commit. Nếu insert/update liên tục không có chiến lược buffering, worker sẽ bị treo hoặc crash tab.  
**Giải pháp**: WAL mode + periodic checkpoint theo chu kỳ thay vì flush ngay tức khắc.

### IDB inconsistency
IndexedDB không có query language, không có transaction phức tạp, không JOIN. Với dữ liệu có cấu trúc quan hệ (orders, items, inventory), SQLite vượt trội hơn.

---

## Phân tích IDB — Pattern tham khảo

IDB được xây dựng theo các pattern sau mà SQL học theo:

| Pattern | IDB | SQL |
|---|---|---|
| Constructor + `this.ready` Promise | ✅ | ✅ |
| Internal `$method` cho low-level ops | `$get`, `$put`, `$del` | `$call` |
| Fluent Chain builder | `idb.get("key").put(val)` | N/A |
| Reactive callbacks map | `callbacks = new Map()` | N/A |
| Browser/Node dual implementation | `BROWSER` / `NODE` | chỉ browser/worker |
| Thread dispatch | không, local IDB | worker via `Threads` |

---

## Kiến trúc tổng thể

```
┌─────────────────────────────────────────────────┐
│  Main Thread / Component / Other Worker          │
│                                                  │
│  import SQL from "/core/SQL.js"                  │
│  const db = new SQL({ name: "app" })             │
│  await db.exec("CREATE TABLE ...")               │
│  const rows = await db.all("SELECT * FROM ...")  │
└──────────────────┬──────────────────────────────┘
                   │ postMessage({ queue, method, params })
                   │ via Threads.queue()
                   ▼
┌─────────────────────────────────────────────────┐
│  Dedicated Worker: core/threads/sql.js           │
│  (dùng Thread base class, giống wave/qr)         │
│                                                  │
│  ┌─────────────────────────────────────────┐    │
│  │  @sqlite.org/sqlite-wasm (WASM)         │    │
│  │  lazily loaded từ /core/SQL/sqlite3.js  │    │
│  │  sqlite3.oo1.OpfsDb (OPFS VFS)          │    │
│  │                                         │    │
│  │  WAL mode + wal_autocheckpoint = 0      │    │
│  │  setInterval checkpoint(PASSIVE)        │    │
│  └────────────────────┬────────────────────┘    │
│                        │ OPFS Sync Access Handle │
└────────────────────────┼────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────┐
│  OPFS — Origin Private File System              │
│  /<name>.db, /<name>.db-wal, /<name>.db-shm     │
└─────────────────────────────────────────────────┘
```

---

## Module 1: `OPFS.js` — Standalone OPFS Wrapper

**Vị trí**: `src/core/OPFS.js` + `src/core/OPFS/`

**Mục đích**: Class độc lập encapsulate native OPFS async API thành các method dễ dùng. Không phụ thuộc vào SQLite hay SQL layer. Chỉ dùng async OPFS API → hoạt động ở main thread lẫn worker.

### API Design

```javascript
class OPFS {
    constructor({ root = "" } = {})
    // root: subdirectory prefix, ví dụ "blobs" → tất cả paths bắt đầu từ /blobs/

    // --- File Operations ---
    read(path)              // → Promise<ArrayBuffer>
    write(path, data)       // → Promise<void>   data: string | ArrayBuffer | Blob
    del(path)               // → Promise<void>   recursive — file hoặc directory
    move(src, dst)          // → Promise<void>   rename / relocate

    // --- Directory Operations ---
    mkdir(path)             // → Promise<FileSystemDirectoryHandle>
    list(path)              // → Promise<string[]>  names trong directory
    exist(path)             // → Promise<boolean>

    // --- Internal ---
    _path(path)             // prepend this.root → full path array
    $root()                 // → Promise<FileSystemDirectoryHandle>  OPFS origin (cached, module-level)
    $dir(path, create)      // → Promise<FileSystemDirectoryHandle>  navigate từ $root()
    $handle(path, create)   // → Promise<FileSystemFileHandle>
}
```

### Path convention

`_path()` là người duy nhất xử lý `this.root` — prepend vào path của user trước khi truyền vào `$dir`/`$handle`. `$root()` luôn trả về OPFS origin thuần túy.

```javascript
const opfs = new OPFS({ root: "blobs" })
await opfs.write(["users", "avatar-123.jpg"], buffer)
// _path(["users", "avatar-123.jpg"]) → ["blobs", "users", "avatar-123.jpg"]
// $root() → /  ($origin)
// $dir(["blobs", "users"]) → /blobs/users/
// write → /blobs/users/avatar-123.jpg
```

### Folder structure

```
src/core/OPFS/
├── root.js      $root() — module-level cached OPFS origin handle
├── dir.js       $dir(path, create) — navigate directory tree từ $root()
├── handle.js    $handle(path, create) — get/create file handle
├── read.js      read(path) → ArrayBuffer
├── write.js     write(path, data) → void
├── del.js       del(path) → void  (recursive)
├── move.js      move(src, dst) → void  (FileSystemFileHandle.move API)
├── mkdir.js     mkdir(path) → FileSystemDirectoryHandle
├── list.js      list(path) → string[]
└── exist.js     exist(path) → boolean
```

---

## Module 2: `SQL.js` — SQLite Client Class

**Vị trí**: `src/core/SQL.js` + `src/core/SQL/`

**Mục đích**: Class gọi được từ mọi context. Không chứa SQLite WASM — chỉ giao tiếp với `sql` worker qua `Threads`. Mọi method trả về Promise.

### Constructor

```javascript
class SQL {
    constructor({ name = "system" } = {}) {
        this.name = name
        // Gửi "open" ngay trong constructor, this.ready resolve khi worker xác nhận DB đã mở.
        // Mọi method public đều await this.ready trước khi dispatch.
        this.ready = this.$call("open", { name })
    }
}
```

### API Design (1-word methods)

```javascript
exec(sql, params)    // → Promise<Row[]>                    SELECT trả rows, DML trả []
run(sql, params)     // → Promise<{ changes, lastId }>      INSERT / UPDATE / DELETE
get(sql, params)     // → Promise<Row | null>               SELECT row đầu tiên (hoặc null)
all(sql, params)     // → Promise<Row[]>                    SELECT tất cả rows
batch(queries)       // → Promise<any[][]>                  queries: [{ sql, params }, ...]
$call(method, params)// → Promise<any>   internal dispatch qua Threads, timeout 30s
```

`params` là array positional: `["value1", 42]`.

### Folder structure

```
src/core/SQL/
├── call.js      $call(method, params) — Threads dispatch, 30s timeout
├── exec.js      exec(sql, params)
├── run.js       run(sql, params)
├── get.js       get(sql, params)
├── all.js       all(sql, params)
├── batch.js     batch(queries)
└── sqlite3.js   (build output) — copy từ @sqlite.org/sqlite-wasm/dist/index.mjs
```

### call.js — Thread dispatch

```javascript
import { threads } from "/core/Threads.js"

export function $call(method, params) {
    return new Promise((resolve, reject) => {
        let settled = false
        const timer = setTimeout(() => {
            if (settled) return
            settled = true
            reject(new Error(`SQL worker timeout: ${method}`))
        }, 30000)

        threads.queue({
            thread: "sql",
            method,
            params: { db: this.name, ...params },   // merge db name vào params
            callback: (response, error) => {
                if (settled) return
                settled = true
                clearTimeout(timer)
                if (error) reject(new Error(error?.message || String(error)))
                else resolve(response)
            }
        })
    })
}
```

---

## Module 3: `core/threads/sql.js` — SQLite Worker

**Vị trí**: `src/core/threads/sql.js`

**Mục đích**: Dedicated worker chạy SQLite WASM với OPFS backend. Theo cùng pattern `Thread` base class như `wave`, `qr`.

### Bootstrap flow (Thread pattern)

```
1. new Thread()  →  xử lý onmessage tự động qua Thread.process()
2. thread.init() gọi ensureSQLite() — lazy load WASM một lần
3. Mọi method (open, exec, run, ...) được gọi qua Thread.process()
4. Thread tự dispatch queue → callback
```

Không cần `self.onmessage` thủ công — `Thread` base class lo hết.

### SQLite init (lazy)

```javascript
async function ensureSQLite() {
    if (sqlite3) return sqlite3
    const { default: sqlite3InitModule } = await import("/core/SQL/sqlite3.js")
    sqlite3 = await sqlite3InitModule({
        locateFile: (file) => `/core/SQL/${file}`   // resolve sqlite3.wasm
    })
    return sqlite3
}
```

`sqlite3.js` và `sqlite3.wasm` được copy vào `build/core/SQL/` bởi `builder/core.js`.

### Database management

Worker quản lý nhiều DB instance (mỗi `new SQL({ name })` → 1 DB riêng):

```javascript
const dbs     = new Map()   // name → sqlite3.oo1.OpfsDb instance
const pending = new Map()   // name → pending write count
const intervals = new Map() // name → setInterval id
```

### open (WAL config)

```javascript
thread.open = async function ({ db: name }) {
    await ensureSQLite()
    if (dbs.has(name)) return { ok: true }

    const d = new sqlite3.oo1.OpfsDb(`/${name}.db`)
    d.exec("PRAGMA journal_mode = WAL")
    d.exec("PRAGMA synchronous = NORMAL")
    d.exec("PRAGMA wal_autocheckpoint = 0")   // tắt auto-checkpoint, tự quản

    dbs.set(name, d)
    pending.set(name, 0)

    const id = setInterval(() => {
        if (!pending.get(name)) return
        d.exec("PRAGMA wal_checkpoint(PASSIVE)")
        pending.set(name, 0)
    }, FLUSH_INTERVAL)
    intervals.set(name, id)

    return { ok: true }
}
```

### Chiến lược ghi OPFS (không crash)

```
                  Mỗi write (run / batch)
                         │
                         ▼
               trackWrite(name):
               pending[name]++
               ≥ CHECKPOINT_THRESHOLD?
               └─ yes → wal_checkpoint(PASSIVE) ngay
                         │
              [setInterval mỗi FLUSH_INTERVAL ms]
               pending[name] > 0?
               └─ yes → wal_checkpoint(PASSIVE)
               pending[name] = 0
```

```javascript
const FLUSH_INTERVAL       = 2000   // ms
const CHECKPOINT_THRESHOLD = 200    // writes tối đa trước khi force flush
```

- `synchronous = NORMAL` → SQLite không fsync sau mỗi commit, chỉ khi WAL đầy
- `wal_autocheckpoint = 0` → tắt auto-checkpoint của SQLite, worker tự điều khiển
- `close()` → `wal_checkpoint(FULL)` + `db.close()` + clear interval

### Handlers

```
thread.open(  { db: name }                 ) → { ok: true }
thread.exec(  { db, sql, params }          ) → Row[]
thread.run(   { db, sql, params }          ) → { changes, lastId }
thread.get(   { db, sql, params }          ) → Row | null
thread.all(   { db, sql, params }          ) → Row[]
thread.batch( { db, queries }              ) → any[][]   (trong transaction)
thread.close( { db: name }                 ) → { ok: true }
```

---

## Build — sqlite-wasm assets

`builder/core.js` copy 2 files vào `build/core/SQL/`:

```javascript
// node_modules/@sqlite.org/sqlite-wasm/dist/index.mjs → build/core/SQL/sqlite3.js
// node_modules/@sqlite.org/sqlite-wasm/dist/sqlite3.wasm → build/core/SQL/sqlite3.wasm
```

- `index.mjs` chứa `sqlite3InitModule` bundled sẵn (default export)
- `sqlite3.wasm` là binary WebAssembly, được `locateFile` resolve đúng vị trí

---

## Launcher.js — Đăng ký thread

```javascript
threads.register("sql", { worker: true, type: "module" })
```

---

## Tương lai: DB.js trở thành Dispatcher

Hiện tại `DB.js` chỉ là hash-validated static file loader. Sau khi SQL và IDB ổn định:

```
DB.js (tương lai)
├── DB.get(path)    → hash-cached static files (hiện tại)
├── DB.sql(name)    → trả SQL instance theo name (lazy singleton)
├── DB.idb(name)    → trả IDB instance theo name (lazy singleton)
└── DB.path(id)     → path utility (hiện tại)
```

---

## Ví dụ sử dụng

### SQL — Component / Main thread

```javascript
import SQL from "/core/SQL.js"

const db = new SQL({ name: "app" })
await db.ready   // chờ worker mở DB xong

await db.exec(`
    CREATE TABLE IF NOT EXISTS orders (
        id     INTEGER PRIMARY KEY AUTOINCREMENT,
        userId TEXT NOT NULL,
        total  REAL NOT NULL,
        status TEXT DEFAULT 'pending',
        at     INTEGER DEFAULT (unixepoch())
    )
`)

const { lastId } = await db.run(
    "INSERT INTO orders (userId, total) VALUES (?, ?)",
    ["user-abc", 99.50]
)
const order   = await db.get("SELECT * FROM orders WHERE id = ?", [lastId])
const pending = await db.all("SELECT * FROM orders WHERE status = ?", ["pending"])

await db.batch([
    { sql: "UPDATE orders SET status = ? WHERE id = ?", params: ["paid", 1] },
    { sql: "INSERT INTO ledger (orderId, amount) VALUES (?, ?)", params: [1, 99.50] }
])
```

### OPFS standalone

```javascript
import OPFS from "/core/OPFS.js"

const fs = new OPFS({ root: "blobs" })

await fs.write(["images", "avatar-123.jpg"], arrayBuffer)
const buffer = await fs.read(["images", "avatar-123.jpg"])
const exists = await fs.exist(["images", "avatar-123.jpg"])  // true
const files  = await fs.list(["images"])                     // ["avatar-123.jpg"]
await fs.del(["images", "avatar-123.jpg"])
```

---

## Checklist triển khai

- [x] `src/core/OPFS/root.js` — module-level cached OPFS origin (không navigate subdirectory)
- [x] `src/core/OPFS/dir.js`
- [x] `src/core/OPFS/handle.js`
- [x] `src/core/OPFS/read.js` → ArrayBuffer
- [x] `src/core/OPFS/write.js`
- [x] `src/core/OPFS/del.js` → recursive
- [x] `src/core/OPFS/move.js`
- [x] `src/core/OPFS/mkdir.js`
- [x] `src/core/OPFS/list.js`
- [x] `src/core/OPFS/exist.js`
- [x] `src/core/OPFS.js` — `_path()` là nơi duy nhất xử lý `this.root` prefix
- [x] `src/core/SQL/call.js` — timeout 30s, merge `{ db: this.name, ...params }`
- [x] `src/core/SQL/exec.js`
- [x] `src/core/SQL/run.js`
- [x] `src/core/SQL/get.js`
- [x] `src/core/SQL/all.js`
- [x] `src/core/SQL/batch.js`
- [x] `src/core/SQL.js` — `this.ready = this.$call("open", { name })`
- [x] `src/core/threads/sql.js` — Thread pattern, lazy WASM, WAL + periodic checkpoint
- [x] `src/core/Launcher.js` — `threads.register("sql", ...)`
- [x] `builder/core.js` — copy `index.mjs` → `sqlite3.js` + `sqlite3.wasm`


## Mục tiêu

Xây dựng một lớp persistence mạnh mẽ, hiệu năng cao cho browser dựa trên:
- **SQLite WASM** (`@sqlite.org/sqlite-wasm`) chạy hoàn toàn trong dedicated worker
- **OPFS** (Origin Private File System) làm backend lưu trữ thật
- **SQL.js** — class giao tiếp từ bất kỳ context nào (main thread, component, khác worker)
- **OPFS.js** — class độc lập encapsulate native OPFS API, phục vụ nhiều tác vụ (không chỉ SQLite)

Tương lai: `SQL` và `IDB` đều được điều phối qua `DB.js` — trở thành trung tâm dữ liệu hệ thống.

---

## Tại sao cần thiết kế như vậy?

### SQLite trong browser không thể chạy ở main thread
`@sqlite.org/sqlite-wasm` với OPFS VFS dùng `FileSystemSyncAccessHandle` — API **chỉ khả dụng** trong **dedicated worker**. Nếu chạy ở main thread sẽ block UI hoặc không khả dụng.

### Ghi OPFS liên tục sẽ crash
`FileSystemSyncAccessHandle.flush()` là thao tác nặng. SQLite nội bộ gọi flush mỗi khi commit. Nếu insert/update liên tục không có chiến lược buffering, worker sẽ bị treo hoặc crash tab.  
**Giải pháp**: WAL mode + periodic checkpoint theo chu kỳ thay vì flush ngay tức khắc.

### IDB inconsistency
IndexedDB không có query language, không có transaction phức tạp, không JOIN. Với dữ liệu có cấu trúc quan hệ (orders, items, inventory), SQLite vượt trội hơn.

---

## Phân tích IDB — Pattern tham khảo

IDB được xây dựng theo các pattern sau mà SQL phải học theo:

| Pattern | IDB | SQL tương đương |
|---|---|---|
| Constructor + `this.ready` Promise | ✅ | ✅ cần có |
| Internal `$method` cho low-level ops | `$get`, `$put`, `$del` | `$call`, `$exec` |
| Fluent Chain builder | `idb.get("key").put(val)` | N/A (SQL khác) |
| Reactive callbacks map | `callbacks = new Map()` | N/A (SQL không reactive) |
| Browser/Node dual implementation | `BROWSER` / `NODE` | chỉ browser/worker |
| Thread dispatch through execute() | không, local IDB | worker via Threads |

---

## Kiến trúc tổng thể

```
┌─────────────────────────────────────────────────┐
│  Main Thread / Component / Other Worker          │
│                                                  │
│  import SQL from "/core/SQL.js"                  │
│  const db = new SQL({ name: "app" })             │
│  await db.exec("CREATE TABLE ...")               │
│  const rows = await db.all("SELECT * FROM ...")  │
└──────────────────┬──────────────────────────────┘
                   │ postMessage({ queue, method, params })
                   │ via Threads.queue()
                   ▼
┌─────────────────────────────────────────────────┐
│  Dedicated Worker: core/threads/sql.js           │
│                                                  │
│  ┌─────────────────────────────────────────┐    │
│  │  @sqlite.org/sqlite-wasm (WASM)         │    │
│  │  sqlite3.oo1.OpfsDb (OPFS VFS)          │    │
│  │                                         │    │
│  │  WAL mode + periodic checkpoint        │    │
│  │  (flush theo chu kỳ, không flush liên tục) │  │
│  └────────────────────┬────────────────────┘    │
│                        │ OPFS Sync Access Handle │
└────────────────────────┼────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────┐
│  OPFS — Origin Private File System              │
│  /app.db, /app.db-wal, /app.db-shm              │
└─────────────────────────────────────────────────┘
```

---

## Module 1: `OPFS.js` — Standalone OPFS Wrapper

**Vị trí**: `src/core/OPFS.js` + `src/core/OPFS/`

**Mục đích**: Class độc lập encapsulate native OPFS browser API thành các method dễ dùng. Không phụ thuộc vào SQLite hay SQL layer. Có thể dùng cho bất kỳ tác vụ nào cần file storage.

**Lưu ý**: OPFS async API hoạt động ở main thread và worker. OPFS sync API (`createSyncAccessHandle`) chỉ hoạt động trong dedicated worker — OPFS.js chỉ dùng async API để đảm bảo tính portable.

### API Design

```javascript
class OPFS {
    constructor({ root = "" } = {})
    // root: subdirectory prefix (e.g., "db", "blobs")

    // --- File Operations ---
    read(path)              // → Promise<string | ArrayBuffer>
    write(path, data)       // → Promise<void>   data: string | ArrayBuffer | Blob
    del(path)               // → Promise<void>   works for file or empty dir
    move(src, dst)          // → Promise<void>   rename / relocate

    // --- Directory Operations ---  
    mkdir(path)             // → Promise<FileSystemDirectoryHandle>
    list(path)              // → Promise<string[]>  names in directory
    exist(path)             // → Promise<boolean>

    // --- Internal ---
    $root()                 // → Promise<FileSystemDirectoryHandle>
    $dir(path, create)      // → Promise<FileSystemDirectoryHandle>  navigate + optionally create
    $handle(path, create)   // → Promise<FileSystemFileHandle>
}
```

### Folder structure

```
src/core/OPFS/
├── read.js      read(path) → string | ArrayBuffer
├── write.js     write(path, data) → void
├── del.js       del(path) → void
├── move.js      move(src, dst) → void
├── mkdir.js     mkdir(path) → FileSystemDirectoryHandle
├── list.js      list(path) → string[]
├── exist.js     exist(path) → boolean
├── root.js      $root() — get root directory handle (cached)
├── dir.js       $dir(path, create) — navigate directory tree
└── handle.js    $handle(path, create) — get/create file handle
```

### Path convention
Path là `string[]` array giống chuẩn của project:
```javascript
const opfs = new OPFS({ root: "blobs" })
await opfs.write(["users", "avatar-123.jpg"], buffer)
// → OPFS root / blobs / users / avatar-123.jpg
```

---

## Module 2: `SQL.js` — SQLite Client Class

**Vị trí**: `src/core/SQL.js` + `src/core/SQL/`

**Mục đích**: Class gọi được từ mọi context. Không chứa SQLite WASM — chỉ giao tiếp với `sql` worker qua `Threads`. Trả về Promise từ mọi operation.

### Constructor

```javascript
class SQL {
    constructor({ name = "system" } = {}) {
        this.name = name
        this.ready = // Promise resolved khi worker confirm ready
    }
}
```

`this.ready` resolved sau khi gửi `{ method: "open", params: { name } }` tới worker và nhận OK.

### API Design (1-word methods)

```javascript
// DDL + DML + Query — chạy bất kỳ SQL
exec(sql, params)    // → Promise<Row[]>   SELECT trả rows, DML trả []

// Tối ưu hóa semantics
run(sql, params)     // → Promise<{ changes: number, lastId: number }>  cho INSERT/UPDATE/DELETE
get(sql, params)     // → Promise<Row | undefined>   SELECT trả 1 row đầu tiên
all(sql, params)     // → Promise<Row[]>              SELECT trả tất cả rows

// Transaction
batch(queries)       // → Promise<any[]>  queries: [{ sql, params }]

// Internal
$call(method, params)// → Promise<any>   core dispatch qua Threads
```

`params` luôn là array: `["value1", 42]` — positional binding.

### Folder structure

```
src/core/SQL/
├── exec.js      public exec(sql, params)
├── run.js       public run(sql, params)
├── get.js       public get(sql, params)
├── all.js       public all(sql, params)
├── batch.js     public batch(queries)
└── call.js      internal $call(method, params) → Threads dispatch
```

### call.js — Thread dispatch (như IDB's execute.js)

```javascript
// $call wraps threads.queue() + Promise
export function $call(method, params) {
    return new Promise((resolve, reject) => {
        threads.queue({
            thread: "sql",
            method,
            params: { db: this.name, ...params },
            callback: (response, error) => {
                if (error) reject(new Error(error.message))
                else resolve(response)
            }
        })
    })
}
```

---

## Module 3: `core/threads/sql.js` — SQLite Worker

**Vị trí**: `src/core/threads/sql.js`

**Mục đích**: Dedicated worker, chạy SQLite WASM với OPFS backend. Nhận messages, thực thi SQL, trả kết quả.

### Bootstrap flow

```
1. import sqlite3InitModule from "@sqlite.org/sqlite-wasm"
2. const sqlite3 = await sqlite3InitModule()
3. SQLite ready
4. postMessage({ queue: "__ready__", response: true })  → SQL.js resolves ready
5. Listen onmessage({ queue, method, params })
6. Route to handler
7. postMessage({ queue, response | error })
```

### Database management

Worker quản lý nhiều DB instance (mỗi `new SQL({ name })` tạo 1 DB riêng):
```javascript
const dbs = new Map()  // name → sqlite3.oo1.OpfsDb instance

function open({ name }) {
    if (dbs.has(name)) return
    const db = new sqlite3.oo1.OpfsDb(`/${name}.db`)
    db.exec("PRAGMA journal_mode = WAL")
    db.exec("PRAGMA synchronous = NORMAL")
    db.exec("PRAGMA wal_autocheckpoint = 1000")
    dbs.set(name, db)
}
```

### Message handlers

```javascript
// handlers map: method → function({ db, sql, params, queries, ... })
const handlers = {
    open:  ({ name })              → open DB, configure WAL
    exec:  ({ db, sql, params })   → db.exec(), return rows
    run:   ({ db, sql, params })   → db.exec(), return { changes, lastId }
    get:   ({ db, sql, params })   → exec + return rows[0]
    all:   ({ db, sql, params })   → exec + return rows
    batch: ({ db, queries })       → db.transaction(), return results[]
    close: ({ name })              → db.close(), dbs.delete(name)
}
```

### Periodic flush — Chiến lược ghi OPFS

Vấn đề cốt lõi: `FileSystemSyncAccessHandle.flush()` tốn kém. SQLite-WASM gọi flush sau mỗi transaction commit trong WAL mode khi `synchronous = FULL`. 

Giải pháp tầng 2 — **defer checkpoint**:

```
                  Mỗi write operation
                         │
                         ▼
               writeCount++, lastWrite = now
                         │
                         ▼
              [setInterval mỗi FLUSH_INTERVAL ms]
                         │
                ┌────────┴────────┐
                │  có gì để flush? │
                └────────┬────────┘
                         │ writeCount > 0
                         ▼
          db.exec("PRAGMA wal_checkpoint(PASSIVE)")
          writeCount = 0
```

Cấu hình:
```javascript
const FLUSH_INTERVAL = 2000   // ms — flush sau tối đa 2 giây
const CHECKPOINT_THRESHOLD = 200  // flush sớm nếu > 200 writes pending
```

Logic:
- `PRAGMA synchronous = NORMAL` → SQLite chỉ flush khi WAL đầy, không flush mỗi commit
- Worker dùng `setInterval` mỗi `FLUSH_INTERVAL` ms → `PRAGMA wal_checkpoint(PASSIVE)`
- Nếu `writeCount > CHECKPOINT_THRESHOLD` → checkpoint ngay, không chờ interval
- Khi `close()` → `PRAGMA wal_checkpoint(FULL)` để đảm bảo không mất dữ liệu

### onmessage handler pattern

```javascript
// Giống pattern của các worker khác trong project (wave, qr, update)
self.onmessage = async ({ data }) => {
    const { queue, method, params } = data
    if (!method || !queue) return
    try {
        const handler = handlers[method]
        if (!handler) throw new Error(`Unknown method: ${method}`)
        const response = await handler(params)
        self.postMessage({ queue, response })
    } catch (err) {
        self.postMessage({ queue, error: { message: err.message } })
    }
}
```

---

## Launcher.js — Đăng ký thread

Thêm `sql` thread vào `Launcher.js`:

```javascript
threads.register("sql", { worker: true, type: "module" })
```

---

## Tương lai: DB.js trở thành Dispatcher

Hiện tại `DB.js` chỉ là hash-validated static file loader. Sau khi SQL và IDB ổn định:

```
DB.js (tương lai)
├── DB.get(path)          → hash-cached static files (hiện tại)
├── DB.sql(name)          → trả SQL instance theo name (lazy init)
├── DB.idb(name)          → trả IDB instance theo name (lazy init)
└── DB.path(id)           → path utility (hiện tại)
```

`DB` trở thành singleton factory — không cần `import SQL from "/core/SQL.js"` riêng, chỉ cần `DB.sql("app")`.

---

## Ví dụ sử dụng

### Component / Main thread

```javascript
import SQL from "/core/SQL.js"

const db = new SQL({ name: "app" })
await db.ready

// DDL
await db.exec(`
    CREATE TABLE IF NOT EXISTS orders (
        id     INTEGER PRIMARY KEY AUTOINCREMENT,
        userId TEXT NOT NULL,
        total  REAL NOT NULL,
        status TEXT DEFAULT 'pending',
        at     INTEGER DEFAULT (unixepoch())
    )
`)

// Insert
const { lastId } = await db.run(
    "INSERT INTO orders (userId, total) VALUES (?, ?)",
    ["user-abc", 99.50]
)

// Query single row
const order = await db.get("SELECT * FROM orders WHERE id = ?", [lastId])

// Query all
const pending = await db.all("SELECT * FROM orders WHERE status = ?", ["pending"])

// Transaction
await db.batch([
    { sql: "UPDATE orders SET status = ? WHERE id = ?", params: ["paid", 1] },
    { sql: "INSERT INTO ledger (orderId, amount) VALUES (?, ?)", params: [1, 99.50] }
])
```

### OPFS standalone (không liên quan SQL)

```javascript
import OPFS from "/core/OPFS.js"

const fs = new OPFS({ root: "blobs" })

// Write
await fs.write(["images", "avatar-123.jpg"], arrayBuffer)

// Read
const buffer = await fs.read(["images", "avatar-123.jpg"])

// Check + list
const exists = await fs.exist(["images", "avatar-123.jpg"])  // true
const files = await fs.list(["images"])  // ["avatar-123.jpg"]

// Delete
await fs.del(["images", "avatar-123.jpg"])
```

---

## Checklist triển khai

- [ ] `src/core/OPFS/root.js` — cache root handle
- [ ] `src/core/OPFS/dir.js` — navigate/create directory tree
- [ ] `src/core/OPFS/handle.js` — get/create file handle
- [ ] `src/core/OPFS/read.js`
- [ ] `src/core/OPFS/write.js`
- [ ] `src/core/OPFS/del.js`
- [ ] `src/core/OPFS/move.js`
- [ ] `src/core/OPFS/mkdir.js`
- [ ] `src/core/OPFS/list.js`
- [ ] `src/core/OPFS/exist.js`
- [ ] `src/core/OPFS.js` — assemble class
- [ ] `src/core/SQL/call.js` — Threads dispatch
- [ ] `src/core/SQL/exec.js`
- [ ] `src/core/SQL/run.js`
- [ ] `src/core/SQL/get.js`
- [ ] `src/core/SQL/all.js`
- [ ] `src/core/SQL/batch.js`
- [ ] `src/core/SQL.js` — assemble class
- [ ] `src/core/threads/sql.js` — SQLite worker + WAL periodic flush
- [ ] `src/core/Launcher.js` — register "sql" thread
- [ ] `npm run build:core` sau khi hoàn thành
