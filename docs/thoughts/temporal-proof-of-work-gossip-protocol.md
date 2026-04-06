# Temporal PoW Gossip Protocol

> **Status**: Implemented via Pen DSL's `SEA.candle()` helper
> 
> **Related**: See [`pen-dsl-guide.md`](./pen-dsl-guide.md) for Pen implementation details

---

## Lịch sử & vấn đề

### 1. Clock Drift trong Hệ Phân Tán

Một vấn đề lớn trong hệ dữ liệu phi tập trung là thời gian, timestamp. Trong hệ phân tán, thời gian UTC timestamp giữa các máy là không đồng nhất, có thể bị drift tăng dần, tích lũy drift theo thời gian.

**Timeline**:
- **1960-70s**: UTC ra đời, đồng thuận epoch 0 là `00:00:00 UTC on January 1, 1970` → Unix timestamp
- **1985**: NTP (Network Time Protocol) ra đời giúp đồng bộ hóa timestamp toàn cầu
- **Hiện tại**: Ngay cả với NTP, sự đồng bộ vẫn không 100% do network latency → lệch tới vài trăm ms giữa các máy

### 2. Zombie Data Problem

Trong một hệ dữ liệu phi tập trung, rất khó để xóa một dữ liệu:
- Trong Gun: `put(null)` chỉ "quên" key, không thực sự xóa khỏi đĩa cứng và mạng
- Nếu xóa local: dữ liệu từ peer khác sẽ tràn sang và lấp vào đĩa (zombie data)
- Cách **SAI**: duy trì danh sách các dữ liệu cần xóa
  - Sai vì: bản thân danh sách xóa cũng là dữ liệu → chỉ chuyển mềm sang cứng

### 3. Epoch Problem trong Blockchain

Mọi người cần một nguồn sự thật chung về thời gian để biết ghi dữ liệu vào đâu, tìm thấy nhau trong mạng.

**Blockchains (ETH/BSC)**: Dùng đồng hồ epoch dựa vào block number
- ❌ Phải duy trì chuỗi block numbers → truy ngược từng block để tìm mốc thời gian
- ❌ Block hash không chứa giá trị thời gian
- ❌ Mỗi peer phải gọi RPC để truy chuỗi khối → không offline-first, ngày càng đắt đỏ

---

## Giải pháp: Candle-Based Temporal Windows

### 1. Candle Number (Thay vì Raw Timestamp)

**Ý tưởng**: Chia thời gian thành các "nến" (candles) rời rạc, bắt đầu từ UTC epoch 0.

```javascript
// Candle size: 5 phút = 300,000ms
const candle = Math.floor(Date.now() / 300000)

// Ví dụ:
// 2026-04-05 14:00:00 → candle = 5777152
// 2026-04-05 14:05:00 → candle = 5777153
// 2026-04-05 14:10:00 → candle = 5777154
```

**Đặc điểm**:
- ✅ **Deterministic**: Mọi máy tính được candle giống nhau (toán học thuần túy, không cần RPC)
- ✅ **Compact**: 7 chữ số vs 13 chữ số timestamp (tiết kiệm 6 chars trong key)
- ✅ **Predictable**: Mọi giá trị nến quá khứ/tương lai đều biết trước, không cần smart contract
- ✅ **Offline-first**: Tính candle không cần mạng

### 2. Sliding Time Window (Epoch Lỏng)

Mỗi peer chỉ duy trì và chấp nhận dữ liệu trong một **khoảng nến** (time window):

```
Window = (current - back) → (current + fwd)
```

**Ví dụ**: `back=100, fwd=2` với 5-phút candles:
```
Current candle: 5777250
Window: 5777150 → 5777252
        │         │
        │         └─ +2 candles (10 phút tương lai, cho phép clock skew)
        └─────────── -100 candles (500 phút = 8.3 giờ quá khứ)
```

**Cơ chế**:
- ✅ **Fair play writes**: Ghi dữ liệu trong window → toàn mạng chấp nhận
- ❌ **Unfair writes**: Ghi ngoài window → mọi người reject (không nhìn thấy)
- 🔄 **Sliding**: Window tự động di chuyển theo thời gian (không cần hardcode start/end)

### 3. Time Source (Đa Nguồn Thời Gian)

**Pen không biết thời gian** - Pen WASM là pure computation VM, không có I/O.

**Gun injects timestamp vào R[4]**:
```javascript
// lib/pen.js - penStage() function
var regs = [
  ctx.key,                    // R[0]
  ctx.val,                    // R[1]
  soul,                       // R[2]
  ctx.state || 0,            // R[3] - HAM timestamp
  Date.now(),                // R[4] ← INJECTED BY GUN.JS
  sec.upub || ''            // R[5] ← from opts.authenticator (SEA.check.$sea)
]
pen.run(bytecode, regs)  // Pass to WASM
```

**Date.now() nguồn gốc**:

| Environment | Source | Accuracy | Drift Risk |
|-------------|--------|----------|------------|
| **Browser** | `window.performance.now()` | ~1ms | Low (synced to OS) |
| **Node.js** | `process.hrtime()` → `Date.now()` | ~1ms | Medium (OS clock) |
| **System** | NTP servers (`pool.ntp.org`) | ±100ms | Low (auto-sync) |
| **Blockchain** | Latest block timestamp | ±1-15s | Very low (consensus) |

**Cải thiện độ chính xác** (future work):

```javascript
// Tham khảo nhiều nguồn thời gian
async function getConsensusTime() {
  const sources = await Promise.all([
    fetch('https://worldtimeapi.org/api/ip').then(r => r.json()),
    getLatestBlockTime('ethereum'),
    getLatestBlockTime('polygon'),
    getNTPTime('pool.ntp.org')
  ])
  
  // Median time từ các nguồn (loại bỏ outliers)
  const times = sources.map(s => s.timestamp).sort()
  return times[Math.floor(times.length / 2)]
}

// Inject consensus time thay vì Date.now()
var regs = [ctx.key, ctx.val, soul, ctx.state, 
            await getConsensusTime(),  // Thay vì Date.now()
            sec.upub]       // from opts.authenticator
```

**Current implementation**: Gun dùng `Date.now()` trực tiếp (đơn giản, đủ cho ±10 phút clock skew tolerance).

---

## Nâng cao: PoW Anti-Spam

### Kết hợp với Proof-of-Work

Để giảm thiểu spam, yêu cầu **mine nhẹ** khi ghi dữ liệu:

```javascript
// Mỗi write phải:
// 1. Ký (signature)
// 2. Mine nonce để hash có prefix "000..." (PoW difficulty)
// 3. Candle number trong window

// Độ dài prefix được quy định trong soul/key
const difficulty = 3  // Cần 3 chữ số 0 → ~8 hash attempts
```

**Cơ chế reject**:
- ❌ Thiếu signature → reject
- ❌ PoW không đủ → reject
- ❌ Candle ngoài window → reject

### Auto-Expiry (Tự Động Xóa)

**Khi nến già vượt window**:
- 🗑️ Không ai bảo ai nhưng mọi người tự biết phải xóa khỏi đĩa cứng
- 📡 Không cần lan truyền tin tức
- 📋 Không cần duy trì danh sách xóa
- ♻️ Zombie data tự nhiên bị loại bỏ

**Ví dụ**:
```
Current: 5777250
Window: 5777150 → 5777252

Data với candle 5777149:
- Đã ra ngoài window (< 5777150)
- Mọi peer tự động xóa local
- Nếu peer nào broadcast lại → mọi người ignore
```

---

## Implementation: Pen DSL

### SEA.candle() Helper

Candle validation được implement trong Pen DSL (WASM bytecode):

```javascript
import SEA from "/core/SEA.js"

// Tạo soul với temporal validation (synchronous)
const soul = SEA.pen({
  key: SEA.candle({
    seg: 0,        // Candle ở segment đầu tiên của key
    sep: "_",      // Phân tách bởi "_"
    size: 300000,  // 5 phút
    back: 100,     // -500 phút (-8.3 giờ)
    fwd: 2         // +10 phút
  }),
  sign: true,                          // Yêu cầu authenticator
  pow: { field: 0, difficulty: 3 }     // PoW trên key, difficulty = 3
})

// Soul = "$abc..." (base62-encoded bytecode)
gun.get(soul).get(key).put(data, null, { authenticator: myPair })
```

**Compile thành bytecode**:
```
LET local[0] = floor(Date.now() / 300000)      ← current candle
  LET local[1] = extract_number(key, seg=0)    ← candle from key
    AND(
      local[1] >= local[0] - 100,              ← không quá cũ
      local[1] <= local[0] + 2                 ← không quá tương lai
    )
```

### Key Format với Candle

```
<candle>_<entity>_<type>_<nonce>
```

**Ví dụ P2P Order**:
```
5777152_organic-green-tea_buy_a3f7b2
│       │                  │   │
│       │                  │   └─ Random nonce (uniqueness)
│       │                  └───── Order type
│       └──────────────────────── Item slug  
└──────────────────────────────── Candle number (MUST BE FIRST)
```

**Why candle first?**
- ✅ LEX query efficiency (Gun `.get({ '>': '5777150_', '<': '5777252_~' })`)
- ✅ Time-based pruning (old candles auto-filtered)
- ✅ Discovery optimization (query by time range)

### Discovery Query

```javascript
// Subscribe to orders trong window hiện tại
const current = Math.floor(Date.now() / 300000)

gun.get(orderSoul).get({
  '>': `${current}:organic-green-tea:buy:`,
  '<': `${current}:organic-green-tea:buy:~`
}).on((order, key) => {
  console.log("Order trong candle hiện tại:", order)
})

// Subscribe to future orders (realtime)
gun.get(orderSoul).get({
  '>': `${current}:`
}).on((order, key) => {
  console.log("New order:", order)
})
```

---

## Flexible Window Configurations

### Use Case Based Windows

| Use Case | Size | Back | Fwd | Window Duration |
|----------|------|------|-----|-----------------|
| **Market orders** (nhanh) | 60s | 5 | 1 | ±5-10 phút |
| **Limit orders** (trung bình) | 5min | 48 | 2 | ±4 giờ |
| **GTC orders** (dài hạn) | 5min | 100 | 2 | ~8 giờ |
| **Escrow contracts** | 1h | 720 | 10 | ±30 ngày |
| **Chat messages** | 5min | 288 | 2 | ±24 giờ |

### Clock Skew Tolerance

**Problem**: Peers có thể có đồng hồ sai lệch ±5 phút

**Solution**: `fwd` parameter cho phép writes "hơi tương lai"

```javascript
// Peer A (đồng hồ nhanh +5 phút):
const candle_A = Math.floor(Date.now() / 300000)  // 5777253

// Peer B (đồng hồ chuẩn):
const candle_B = Math.floor(Date.now() / 300000)  // 5777252

// Validation trên Peer B:
// 5777253 <= 5777252 + 2 ✓ → ACCEPT (trong fwd window)

// Peer C (đồng hồ nhanh +15 phút):
const candle_C = 5777255
// 5777255 <= 5777254 ✗ → REJECT (quá xa tương lai)
```

---

## So Sánh với Các Giải Pháp Khác

### vs Blockchain Epoch

| Feature | Blockchain Epoch | Candle Epoch |
|---------|-----------------|--------------|
| Tính toán | Cần RPC, trace blocks | Pure math, offline |
| Predictability | Phải query | Mọi giá trị biết trước |
| Overhead | Gas fees, network calls | Zero (local compute) |
| Offline-first | ❌ (cần RPC) | ✅ (deterministic) |
| Cleanup | Manual | Auto (window sliding) |

### vs Fixed Timestamps

| Feature | Fixed Timestamps | Sliding Candle Window |
|---------|-----------------|----------------------|
| Hardcode | Phải biết start/end | Không cần (relative) |
| Flexibility | Rigid (1 time range) | Flexible (sliding) |
| Key length | 13 digits | 7 digits |
| Expiry | Manual cleanup | Auto-expiry |
| Discovery | Full scan | Range query (LEX) |

---

## Best Practices

### 1. Chọn Candle Size

```javascript
// High-frequency (sub-minute precision)
size: 60000     // 1 phút → nhiều candles, precision cao

// P2P trading (recommended)
size: 300000    // 5 phút → cân bằng precision/grouping

// Long-term contracts
size: 3600000   // 1 giờ → ít candles, compact keys

// Daily settlements
size: 86400000  // 1 ngày → extreme compact
```

### 2. Chọn Window Size

```javascript
// Tight (market orders, chat)
back: 5-10, fwd: 1-2     // Minutes to hours

// Medium (limit orders, sessions)
back: 48-100, fwd: 2     // Hours to half-day

// Loose (GTC, escrow)
back: 288-720, fwd: 10   // Days to weeks
```

### 3. Key Structure Convention

```
<candle>_<primary>_<secondary>_<random>
│        │         │           │
│        │         │           └─ Uniqueness (nonce)
│        │         └───────────── Filters (type, etc)
│        └─────────────────────── Main entity
└──────────────────────────────── Time (ALWAYS FIRST)
```

---

## References

- **Pen DSL Implementation**: [`pen-dsl-guide.md`](./pen-dsl-guide.md)
- **P2P Trading Use Case**: [`white-paper.md`](./white-paper.md)
- **Gun Source**: `node_modules/@akaoio/gun/docs/pen.md`
- **SEA.candle() Source**: `node_modules/@akaoio/gun/lib/pen.js`