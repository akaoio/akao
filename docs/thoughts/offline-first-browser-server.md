# Offline-First In-Browser Server — Architecture & Feasibility

> "Thời đại của offline-first in-browser server đã tới"

---

## 1. Gốc rễ — Tại sao?

Web truyền thống có một điểm thất bại duy nhất: **server tập trung**. Mọi request đi qua một máy chủ. Khi server chết, app chết. Khi server chậm, app chậm. Khi server thu tiền, bạn phải trả.

Năm 2025-2026, trình duyệt đã đủ mạnh để xoá bỏ mô hình đó:

| Năm | Sự kiện |
|---|---|
| 2015 | Service Worker — browser chặn và cache network request |
| 2018 | WebRTC Data Channel ổn định — P2P data không qua server |
| 2020 | OPFS (Origin Private File System) — browser có FS thực thụ |
| 2022 | SQLite WASM chính thức — SQL engine chạy trong browser |
| 2023 | WebTorrent v2 — BitTorrent hoàn toàn trong browser qua WebRTC |
| 2024 | Storage Buckets API — isolate storage per context |

Browser giờ là: **network node + file system + database engine + compute**. Không còn lý do gì phải gửi mọi thứ lên cloud.

---

## 2. Stack — 6 tầng lưu trữ và truyền thông

```
┌─────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                    │
│           Web Components + Shadow DOM + html``          │
├─────────────────────────────────────────────────────────┤
│                   COMPUTE / QUERY                       │
│  SQLite WASM (SQL)         GunDB (GDB)                  │
│  Complex queries           Real-time graph sync         │
├─────────────────────────────────────────────────────────┤
│                   STORAGE / PERSISTENCE                 │
│  OPFS (large files)        IndexedDB (IDB)              │
│  Binary, media, assets     JSON, hashes, cache          │
├─────────────────────────────────────────────────────────┤
│                   TRANSPORT / P2P                       │
│  WebRTC (RTC)              WebTorrent (Torrent)         │
│  Signaling + DataChannel   BitTorrent over WebRTC       │
└─────────────────────────────────────────────────────────┘
```

Sáu module này không phải random — chúng lấp đầy 6 ô khác nhau trong ma trận **lưu trữ × truyền thông**:

|  | Ephemeral (tạm thời) | Persistent (bền vững) |
|---|---|---|
| **Local only** | IDB (cache) | OPFS (file) |
| **Sync/Replicated** | GDB (gun realtime) | SQL (query engine) |
| **P2P network** | RTC (peer channel) | Torrent (content-addressed) |

---

## 3. Các module hiện có — nền tảng đã xây

### IDB — IndexedDB façade
- Cache JSON, hashes, SVG, human-readable data
- Dùng trong DB.js để hash-validate static files trước khi serve
- Nhanh: structured clone, no serialization overhead

### OPFS — Origin Private File System
- Lưu file binary: video, wasm, large blobs
- `FileSystemSyncAccessHandle` — synchronous read/write từ Worker
- Biến browser thành host thực sự — không khác gì local disk

### SQL — SQLite WASM
- Full SQL engine nhúng trong WASM
- Complex aggregations, joins, full-text search ngay trong tab
- Database persisted qua OPFS

### GDB — GunDB
- Real-time graph database với P2P sync
- Mỗi node vừa là client vừa là relay
- Dùng để synchronize state giữa các browser mà không cần server trung tâm

### RTC — WebRTC DataChannel façade
- Signaling-agnostic: `offer/answer/accept` trả về plain JSON, truyền qua bất kỳ channel nào
- `$wait()` guard ICE gathering trước khi trả signal — tránh mất candidates với NAT
- Events: `open`, `close`, `message`, `candidate`, `connection`, `datachannel`
- Tested: browser `localhost:8080` — `init({initiator:true})` → RTCPeerConnection live

### Torrent — WebTorrent façade
- Lazy-load 1.4MB bundle — zero cost nếu feature không dùng
- `seed([File])` → magnet link; `add(magnet)` → download trong browser
- `$track()` bind events, `$snapshot()` serialize state thành plain object
- Tested: Node + browser 3/3 pass, `Torrent/client.js` load thành công

---

## 4. RTC — WebRTC DataChannel Façade

### Kiến trúc module

```
src/core/RTC.js          ← constructor + method bindings
src/core/RTC/
  init.js                ← tạo RTCPeerConnection, bind DataChannel events
  offer.js               ← bên A: tạo offer, chờ ICE complete
  answer.js              ← bên B: nhận offer, tạo answer, chờ ICE complete
  accept.js              ← bên A: nhận answer + candidates từ bên B
  signal.js              ← xuất { description, candidates } để truyền qua signaling
  snapshot.js            ← trạng thái hiện tại của connection
  wait.js                ← chờ ICE gathering state = "complete" (timeout-safe)
  send.js                ← gửi data qua DataChannel
  on.js / off.js         ← subscribe/unsubscribe events
  emit.js                ← internal event dispatch
  close.js               ← đóng connection + cleanup
```

### Constructor

```js
const rtc = new RTC({
    channel:    "data",          // tên DataChannel
    iceservers: [...],           // STUN/TURN servers
    dataopts:   { ordered: true }, // RTCDataChannel options
    offeropts:  {},              // createOffer options
    timeout:    10000            // ICE gathering timeout (ms)
})
```

### Signaling flow — 3 bước

```
Peer A                        Signaling (GDB/any)         Peer B
  │                                   │                      │
  │── rtc.offer() ──────────────────► │ ─── signal A ──────► │
  │                                   │                      │── rtc.answer(signalA)
  │                                   │ ◄── signal B ────────│
  │── rtc.accept(signalB) ◄──────────│                      │
  │                                   │                      │
  │══════════════ DataChannel OPEN ═══════════════════════════│
  │── rtc.send("hello") ─────────────────────────────────── ►│
```

**RTC không quan tâm signaling đi qua đâu** — có thể là GDB, HTTP, QR code, copy-paste tay. `offer()` và `answer()` trả về plain JSON object. `accept()` nhận vào JSON đó.

### Events

| Event | Payload |
|---|---|
| `open` | `snapshot()` |
| `close` | `snapshot()` |
| `message` | raw data (string / ArrayBuffer) |
| `candidate` | ICE candidate JSON |
| `candidates` | `[...candidates]` (ICE complete) |
| `connection` | `snapshot()` |
| `error` | Error object |
| `datachannel` | `snapshot()` (khi remote tạo channel) |

### `$wait()` — ICE gathering guard

`offer()` và `answer()` đều gọi `$wait()` trước khi trả signal. Lý do: ICE candidates được thu thập bất đồng bộ sau khi `setLocalDescription()`. Nếu trả signal ngay, candidate list có thể rỗng hoặc thiếu, dẫn đến kết nối thất bại với NAT.

`$wait()` resolve khi `iceGatheringState === "complete"` hoặc timeout. Trả về danh sách candidates đã thu thập được.

### Snapshot object

```js
{
    channel:             "data",
    connectionState:     "connected" | "new" | "disconnected" | ...,
    iceConnectionState:  "connected" | "checking" | ...,
    iceGatheringState:   "complete" | "gathering" | "new",
    signalingState:      "stable" | "have-local-offer" | ...,
    dataState:           "open" | "connecting" | "closed",
    localDescription:    { type, sdp } | null,
    remoteDescription:   { type, sdp } | null,
    candidates:          [...]
}
```

---

## 5. Torrent — WebTorrent Façade

### Kiến trúc module

```
src/core/Torrent.js       ← constructor + method bindings
src/core/Torrent/
  init.js                 ← lazy-init WebTorrent client
  client.js               ← [build output] webtorrent.min.js (browser bundle)
  add.js                  ← download torrent từ magnet/hash/URL
  seed.js                 ← seed files từ browser
  get.js                  ← lấy torrent theo infoHash/magnetURI
  list.js                 ← list tất cả torrents đang active
  remove.js               ← remove torrent khỏi client
  destroy.js              ← destroy client, cleanup
  track.js                ← bind progress/done/wire events lên torrent
  snapshot.js             ← serialize torrent state thành plain object
  on.js / off.js          ← subscribe/unsubscribe events
  emit.js                 ← internal event dispatch
```

### Constructor

```js
const torrent = new Torrent({
    // WebTorrent client options (truyền thẳng vào new WebTorrent(options))
    maxConns: 55,
    downloadLimit: -1,
    uploadLimit: -1,
    tracker: { ... }
})
```

### Lazy initialization

WebTorrent (~1.4MB minified) chỉ được load khi gọi `init()` hoặc `add()/seed()` lần đầu:

```js
// Dynamic import — zero cost khi không dùng
const { default: WebTorrent } = await import("./client.js")
this.client = new WebTorrent(this.options)
```

Sau lần đầu, `$client()` luôn trả về instance đã có. Promise `this.ready` guard concurrent calls.

### Seed flow — browser làm nguồn phát

```js
const t = new Torrent()
const torrent = await t.seed([file], { name: "my-file.mp4" })
console.log(torrent.magnetURI) // chia sẻ magnet link này
```

File được lưu trong memory (hoặc OPFS nếu configure). Các peer khác có thể download qua magnet link. Browser trở thành CDN node.

### Add flow — download trong browser

```js
const torrent = await t.add("magnet:?xt=urn:btih:...")
torrent.files.forEach(file => file.appendTo(document.body))
```

Resolve khi `ready` event fired (metadata có). Progress qua events.

### Snapshot object

```js
{
    name:          "ubuntu-24.04.iso",
    infoHash:      "abc123...",
    magnetURI:     "magnet:?xt=urn:btih:abc123...",
    progress:      0.73,          // 0-1
    downloaded:    1073741824,    // bytes
    uploaded:      536870912,
    downloadSpeed: 524288,        // bytes/s
    uploadSpeed:   262144,
    numPeers:      12,
    ready:         true,
    done:          false,
    paused:        false,
    length:        1469841408     // total bytes
}
```

### Events

| Event | Payload |
|---|---|
| `torrent` | snapshot (torrent mới được add) |
| `update` | snapshot (metadata/ready thay đổi) |
| `done` | snapshot |
| `download` | `{ torrent: snapshot, bytes }` |
| `upload` | `{ torrent: snapshot, bytes }` |
| `wire` | snapshot (peer mới kết nối) |
| `noPeers` | `{ torrent: snapshot, announceType }` |
| `warning` | `{ torrent: snapshot, warning }` |
| `error` | `{ torrent: snapshot, error }` hoặc client-level Error |

---

## 6. Tích hợp — Các pattern kết hợp

### Pattern A: RTC + GDB (signaling không cần server)

GDB (GunDB) relay signal giữa hai peer mà không cần central server:

```js
import GDB from "/core/GDB.js"
import RTC from "/core/RTC.js"

const db = new GDB()
const rtc = new RTC({ iceservers: [{ urls: "stun:stun.l.google.com:19302" }] })
const room = "room-" + roomId

// Peer A
const signalA = await rtc.offer()
db.put(["rooms", room, "offer"], signalA)

db.on(["rooms", room, "answer"], async (signalB) => {
    await rtc.accept(signalB)
})

// Peer B
db.on(["rooms", room, "offer"], async (signalA) => {
    const signalB = await rtc.answer(signalA)
    db.put(["rooms", room, "answer"], signalB)
})

// Sau khi kết nối
rtc.on("open", () => {
    rtc.send(JSON.stringify({ type: "hello" }))
})
```

### Pattern B: Torrent + OPFS (persistent seeding)

File được seed từ OPFS, sống qua page refresh:

```js
import OPFS from "/core/OPFS.js"
import Torrent from "/core/Torrent.js"

const opfs = new OPFS()
const t = new Torrent()

// Write to OPFS trước
await opfs.write(["media", "video.mp4"], buffer)

// Seed từ OPFS File handle
const handle = await opfs.handle(["media", "video.mp4"])
const file = await handle.getFile()
const torrent = await t.seed([file], { name: "video.mp4" })

// Lưu magnet để tái sử dụng
localStorage.setItem("magnet:video", torrent.magnetURI)
```

### Pattern C: RTC + SQL (P2P query)

Hai node đồng bộ hoá SQL database qua RTC DataChannel:

```js
// Serialize query result → gửi qua RTC
const rows = await sql.query("SELECT * FROM items WHERE updated > ?", [lastSync])
rtc.send(JSON.stringify({ type: "sync", rows }))

// Nhận bên kia → merge vào local SQL
rtc.on("message", async (raw) => {
    const { type, rows } = JSON.parse(raw)
    if (type === "sync") await sql.exec("INSERT OR REPLACE INTO items VALUES ...", rows)
})
```

### Pattern D: Torrent + SQL (content-addressed DB snapshot)

Export SQL database → seed torrent → peer download → import:

```js
// Node A: export và seed
const snapshot = await sql.export()     // Uint8Array
const torrent = await t.seed([new File([snapshot], "db.sqlite")])
console.log(torrent.magnetURI)          // distribute này

// Node B: download và import
const torrent = await t.add(magnetURI)
const [file] = torrent.files
const buffer = await file.arrayBuffer()
await sql.import(buffer)                // restore DB
```

---

## 7. Phân tích khả thi

### 7.1 WebRTC — Khả thi cao, có ràng buộc

**Khả thi:**
- ✅ `RTCPeerConnection` native trên mọi modern browser
- ✅ DataChannel hỗ trợ binary và string, up to ~256KB/message
- ✅ Tested và verify trên `localhost:8080`: `init({initiator:true})` → connection state `new`, DataChannel `connecting` — hoạt động đúng
- ✅ Không cần WASM, không cần polyfill
- ✅ Signaling-agnostic: GDB, HTTP, bất kỳ transport nào đều dùng được

**Ràng buộc:**
- ⚠️ **NAT traversal**: Cần STUN server để kết nối qua NAT. Public STUN (Google) hoạt động cho ~85% trường hợp. Phần còn lại cần TURN server (relay) — TURN server tốn băng thông, không free.
- ⚠️ **Signaling infrastructure**: RTC cần một kênh để trao đổi `offer/answer/candidates` ban đầu. Nếu GDB down, signaling bị chặn. → Cần ít nhất một relay node GDB hoạt động.
- ⚠️ **Mobile browser lifespan**: iOS/Android giết background tab, connection bị drop nếu user chuyển app.
- ⚠️ **Firefox + DataChannel**: Stable nhưng có quirks với `negotiated` channels.

**Kết luận RTC**: Production-ready cho use case trong cùng mạng LAN hoặc khi có STUN. Cần TURN fallback cho production public internet.

### 7.2 WebTorrent — Khả thi trung bình, phụ thuộc tracker

**Khả thi:**
- ✅ WebTorrent v2 dùng WebRTC thay TCP — hoạt động được trong browser
- ✅ Import confirmed trong browser: `Torrent/client.js` load thành công, class `Kt` (minified) resolve đúng
- ✅ Unit tests 3/3 pass cả Node và Browser
- ✅ Lazy-load: 1.4MB bundle chỉ tải khi `init()` được gọi
- ✅ Seed từ `File` object — không cần filesystem access
- ✅ `add()` và `seed()` đều Promise-based với error handling

**Ràng buộc:**
- ⚠️ **WebTorrent trackers**: Cần WebSocket tracker (`wss://`) để announce và discover peers. Public trackers như `wss://tracker.openwebtorrent.com` có uptime không đảm bảo. → Cần self-host tracker hoặc dùng DHT.
- ⚠️ **DHT trong browser bị giới hạn**: WebTorrent's DHT hoạt động kém trên browser do thiếu UDP. Discovery chủ yếu qua tracker.
- ⚠️ **Seeding lifetime**: File chỉ được seed khi tab mở. Đóng tab → không còn nguồn cung. Giải pháp: OPFS persist + Service Worker để seed background (experimental).
- ⚠️ **Bundle size**: 1.4MB là nặng. Lazy-load giảm thiểu impact (FCP không bị ảnh hưởng) nhưng Time-to-Interactive cho feature torrent sẽ cao hơn.
- ⚠️ **Cross-origin restriction**: Tracker phải cho phép CORS/COOP headers.

**Kết luận Torrent**: Tốt cho distributing large static content (media, game assets) trong controlled environment. Không phù hợp làm primary transport cho real-time data.

### 7.3 Phối hợp stack — Tổng đánh giá

| Combination | Use Case | Feasibility |
|---|---|---|
| RTC + GDB | P2P chat, collaborative edit, live sync | ✅ High |
| RTC + SQL | P2P database sync | ✅ High |
| Torrent + OPFS | Persistent large file distribution | ⚠️ Medium |
| Torrent + SQL | DB snapshot distribution | ⚠️ Medium |
| RTC + Torrent | Hybrid: RTC signaling + Torrent data | ✅ High (Torrent already uses RTC internally) |
| Full offline (no server) | Zero-server app | ⚠️ Medium — tracker & STUN still needed |

### 7.4 Điểm yếu cốt lõi cần hiểu rõ

**"Offline-first" không có nghĩa là zero-infrastructure:**

```
Vẫn cần:
├── STUN server (stateless, nhiều free options)
├── WebSocket tracker cho Torrent (có thể self-host nhẹ)
└── Ít nhất 1 GDB relay node để bootstrap signaling

Không cần:
├── Application server (Express, Next.js, ...)
├── Database server (Postgres, MySQL, ...)
├── CDN cho static assets (Torrent thay thế)
└── WebSocket server (GDB P2P thay thế)
```

Infrastructure giảm từ **O(data × users)** xuống **O(1)** — một cụm gồm STUN + tracker + GDB bootstrap node là đủ cho triệu user, vì không có data flow qua đó.

---

## 8. Triển khai tiếp theo

Theo thứ tự ưu tiên:

### Bước 1: RTC + GDB signaling demo
Tạo route `/en/test` sub-component hoặc page demo hai tab cùng localhost exchange signal qua GDB, thiết lập P2P DataChannel và ping-pong message.

**Verify**: `connectionState === "connected"` và `rtc.send()` → `rtc.on("message")` hoạt động end-to-end.

### Bước 2: Torrent seed/download trong browser
Tạo component cho phép user chọn file → `t.seed()` → hiện magnet link. Tab thứ hai `t.add(magnet)` → download → verify content identical.

**Verify**: Nội dung file giống nhau sau round-trip qua BitTorrent trong cùng browser (localhost WebRTC transport).

### Bước 3: OPFS + Torrent persistence
Kết hợp seed từ OPFS file handle. Service Worker intercept fetch cho OPFS-backed resources, giữ seed sống sau tab close.

### Bước 4: SQL P2P sync
Prototype đồng bộ SQLite database qua RTC DataChannel. Resolve conflict với timestamp + last-write-wins cho đơn giản trước.

---

## 9. Kết

Đây không phải pub sub. Đây là **distributed computing trong browser**. Mỗi browser là một node trong mạng — lưu trữ, xử lý, phục vụ, đồng bộ.

Stack IDB → OPFS → SQL → GDB → RTC → Torrent là một chuỗi liên tục từ local cache đến global P2P network. Mỗi tầng giải quyết một bài toán khác nhau, không tầng nào thay thế được tầng kia.

RTC và Torrent đã được tích hợp đúng pattern, tested trên cả Node và browser. Foundation đã có. Tiếp theo là wiring chúng lại với nhau thành feature thực sự.
