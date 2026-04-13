# Pen DSL - Developer Reference

> **Source of truth**: `gun/lib/pen.js` (compiler + bridge), `pen/src/pen.zig` (VM), `pen/src/wasm.zig` (WASM boundary)

## Overview

Pen là WASM-based predicate engine validate **ai được phép ghi** vào Gun souls. Nó compile validation logic thành bytecode, encode thành base62, dùng chuỗi đó làm soul name (`$abc...`). Điều này ngăn spam và ghi trái phép mà không cần hệ thống permission tập trung.

**Pen KHÔNG phải query language.** Nó là validation engine chạy SAU khi ai đó cố gắng ghi, kiểm tra: signature, proof-of-work, temporal windows, và custom logic.

---

## Architecture (3 Layers)

### Layer 0: pen.wasm (WASM VM)

Compiled từ Zig (`pen/src/pen.zig`). Pure computation — không có I/O, không có system time. Nhận bytecode + registers từ host, trả về boolean.

Memory layout (shared 64KB buffer):
```
[0..3]     u32 LE  = bytecode length
[4..N+3]   bytecode bytes
[N+4..N+7] u32 LE  = register count
[N+8..]    register wire encoding
```

Return codes từ `exp.run()`:
- `1` = true (pass)
- `0` = false (fail)
- `-1` = runtime error
- `-2` = bad version byte
- `-3` = max recursion depth exceeded

### Layer 1: lib/pen.js (Compiler + Bridge)

JavaScript layer trong Gun (`gun/lib/pen.js`) làm 4 việc:
1. Load `pen.wasm` (Node.js: từ file; Browser: fetch)
2. Expose `pen.run(bytecode, regs)` — write registers vào shared buffer, call `exp.run()`
3. Expose `pen.bc.*` — bytecode builder helpers
4. Hook vào Gun SEA pipeline: detect `$`-soul → gọi `penStage()`

### Layer 2: Application Code

```javascript
// SEA.pen() là SYNCHRONOUS — không cần await
const soul = SEA.pen({
  key: SEA.candle({ back: 100, fwd: 2 }),
  sign: true
})
// soul = "$abc123..." — STRING, không phải object

gun.get(soul).get(key).put(data, null, { authenticator: myPair })
// Chỉ thành công nếu key trong candle window + có authenticator hợp lệ
```

---

## Registers

Khi Gun intercept write vào `$soul`, `penStage()` populate 6 registers:

```javascript
// lib/pen.js — penStage()
var sec = SEA.check.$sea(ctx.msg, ctx.at.user || '', null)
var regs = [
  ctx.key,           // R[0] = key đang ghi
  ctx.val,           // R[1] = value (JSON string)
  soul,              // R[2] = soul name (bắt đầu '$')
  ctx.state || 0,    // R[3] = Gun HAM state timestamp
  Date.now(),        // R[4] = Unix ms hiện tại (injected by JS, NOT by WASM)
  sec.upub || ''     // R[5] = pub key từ opts.authenticator (không phải gun.user())
]
pen.run(bytecode, regs)
```

**R[5] nguồn**: Lấy từ `msg._.sea.authenticator` (truyền qua `put(data, cb, { authenticator: pair })`). Không cần `gun.user()` auth session — chỉ cần pass authenticator vào opts.

---

## ISA v1 Opcode Reference

Nguồn: `pen/src/pen.zig`. Version byte `0x01` bắt đầu mọi program (added bởi `bc.prog()`).

### Constants & Literals

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x00` | `NULL` | null value |
| `0x01` | `TRUE` | boolean true |
| `0x02` | `FALSE` | boolean false |
| `0x03 [len] [bytes]` | `STR` | string literal (1-byte length + UTF-8, max 255 bytes) |
| `0x04 [uleb128]` | `UINT` | unsigned int (ULEB128 variable length) |
| `0x07 [sleb128]` | `INT` | signed int (SLEB128 variable length) |
| `0x08 [8bytes]` | `F64` | float64 (big-endian 8 bytes) |
| `0x23` | `PASS` | always true |
| `0x24` | `FAIL` | always false |
| `0xE0..0xEF` | `intn(0–15)` | inline small integers 0–15 |

### Registers & Locals

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x10 [n]` | `REG(n)` | register R[n] nếu n<128; local[n-128] nếu n≥128 |
| `0xF0` | `r0` | shorthand R[0] = key |
| `0xF1` | `r1` | shorthand R[1] = val |
| `0xF2` | `r2` | shorthand R[2] = soul |
| `0xF3` | `r3` | shorthand R[3] = state |
| `0xF4` | `r4` | shorthand R[4] = Date.now() |
| `0xF5` | `r5` | shorthand R[5] = upub |
| `0xF8..0xFB` | `local(0–3)` | local variables 0–3 (shorthands) |

### Logic

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x20 [n] [e...]` | `AND(n, exprs)` | short-circuit AND, n sub-expressions |
| `0x21 [n] [e...]` | `OR(n, exprs)` | short-circuit OR, n sub-expressions |
| `0x22 [e]` | `NOT(e)` | logical negation |

### Comparison

| Opcode | Assembly |
|--------|----------|
| `0x30` | `EQ(a, b)` — strings: deep equal; numbers: numeric |
| `0x31` | `NE(a, b)` |
| `0x32` | `LT(a, b)` — strings: lexicographic; numbers: numeric |
| `0x33` | `GT(a, b)` |
| `0x34` | `LTE(a, b)` |
| `0x35` | `GTE(a, b)` |

### Arithmetic

| Opcode | Assembly | Ghi chú |
|--------|----------|---------|
| `0x40` | `ADD(a, b)` | int+int=int; else float |
| `0x41` | `SUB(a, b)` | |
| `0x42` | `MUL(a, b)` | |
| `0x43` | `DIVU(a, b)` | floor division (integer) |
| `0x44` | `MOD(a, b)` | |
| `0x45` | `DIVF(a, b)` | float division |
| `0x46` | `ABS(a)` | absolute value |
| `0x47` | `NEG(a)` | negation |

### String Operations

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x50` | `LEN(a)` | string length → int |
| `0x51` | `SLICE(str, start, end)` | substring [start, end) |
| `0x52 [sep] [idx]` | `SEG(str, sep_byte, idx)` | split str by sep char, get segment idx → string |
| `0x53` | `TONUM(a)` | parse string → float64 |
| `0x54` | `TOSTR(a)` | value → string |
| `0x55` | `CONCAT(a, b)` | concatenate strings |
| `0x56` | `PRE(a, b)` | a.startsWith(b) |
| `0x57` | `SUF(a, b)` | a.endsWith(b) |
| `0x58` | `INCLUDES(a, b)` | a.includes(b) |
| `0x59` | `REGEX(a, b)` | **always false** — not implemented in core |
| `0x5A` | `UPPER(a)` | uppercase |
| `0x5B` | `LOWER(a)` | lowercase |

### Type Checks

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x60` | `ISS(a)` | is string |
| `0x61` | `ISN(a)` | is number (int hoặc float) |
| `0x62` | `ISX(a)` | is null |
| `0x63` | `ISB(a)` | is boolean |
| `0x64 [mn] [mx]` | `LNG(a, min, max)` | string length in [mn, mx]; **mn và mx mỗi cái là 1 byte (0–255)** |

### Control Flow

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x70 [slot]` | `LET(slot, def, body)` | bind local[slot] = def; evaluate body |
| `0x71` | `IF(cond, then, else)` | lazy — chỉ evaluate nhánh được chọn |

### Macros

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0x80 [reg] [sep] [idx]` | `SEGR(reg, sep, idx)` | shorthand SEG: split R[reg] by sep → segment idx (string) |
| `0x81 [reg] [sep] [idx]` | `SEGRN(reg, sep, idx)` | như SEGR nhưng parse số → TONUM(SEG(...)) |

### Policy Opcodes (appended AFTER expression tree)

Policy bytes được append VÀO SAU cây expression khi compile. WASM VM **không** thực thi chúng — chúng được đọc bởi `scanpolicy()` ở JS bridge layer.

| Opcode | Assembly | Mô tả |
|--------|----------|--------|
| `0xC0` | `sign` | Yêu cầu valid authenticator hoặc valid signature trong val |
| `0xC1 [len] [bytes]` | `cert` | Yêu cầu val chứa cert JSON được ký bởi cert issuer pub |
| `0xC3` | `open` | Không cần auth, forward trực tiếp |
| `0xC4 [field] [diff]` | `pow` | SHA-256 hex của R[field] phải bắt đầu bằng `diff` ký tự '0' |

### Constraints & Limits

| Limit | Value | Ghi chú |
|-------|-------|---------|
| `MAX_STR` | 128 bytes | Strings truncated tới 128 bytes trong WASM |
| `MAX_LOCALS` | 32 | Local slots 0–31; accessed via `reg(128+n)` |
| `MAX_REGS` | 64 | R[0]–R[63] |
| `MAX_DEPTH` | 32 | Expression recursion depth |
| `LNG` min/max | 0–255 | Single byte each |
| `STR` literal | 0–255 bytes | 1-byte length prefix |
| `REGEX` | always false | Not implemented in core |

---

## Write Flow

```javascript
// 1. Trader viết data
gun.get("$abc...").get("5777152:tea:buy:x3f7").put(order, null, { authenticator: myPair })

// 2. Gun pipeline intercepts write
// 3. penStage() detect soul[0] === '$'
// 4. Decode bytecode: pen.unpack(soul.slice(1))
// 5. scanpolicy() → tìm policy bytes sau expression tree
// 6. Populate registers (JS injects):
var regs = [ctx.key, ctx.val, soul, ctx.state||0, Date.now(), sec.upub||'']

// 7. pen.run(bytecode, regs) → true/false
// 8. Nếu false → reject("PEN: predicate failed")
// 9. Nếu true → check pow (nếu có) → applypolicy()
```

**WASM không biết thời gian** — `R[4] = Date.now()` được inject bởi JS trước khi gọi WASM.

### Policy Enforcement sau predicate pass

1. **pow (0xC4)**: `SEA.work(regs[field], null, cb, {name:'SHA-256',encode:'hex'})` → check prefix
2. **cert (0xC1)**: Verify `val['+']` được ký bởi cert issuer
3. **sign (0xC0)**: Có `authenticator` → ký fresh; không có → verify signature trong `val['*']`
4. **open (0xC3)** hoặc không có policy: Forward trực tiếp

### Candle Calculation Flow

```javascript
// 1. Gun intercepts write
gun.get("$abc...").get("5777152_item_buy_nonce").put(data)

// 2. lib/pen.js injects timestamp
const now = Date.now()  // e.g., 1733145600000 (Unix ms)

// 3. Pen bytecode extracts candle from key
// Bytecode: LET(0, DIVU(R[4], 300000))  ← current = floor(now / 300000)
const current_candle = Math.floor(now / 300000)  // e.g., 5777152

// 4. Compare with candle from key segment 0
const key_candle = parseInt(key.split('_')[0])  // 5777152

// 5. Validate window
if (key_candle >= current_candle - 100 &&   // Not too old (±8h)
    key_candle <= current_candle + 2) {     // Not too future (±10min)
  return true  // ACCEPT
} else {
  return false // REJECT
}
```

---

## SEA.pen(spec) — Compiler API

**Synchronous**. Trả về soul string `'$...'` trực tiếp.

```javascript
const soul = SEA.pen(spec)
// Returns: "$abc123..." — string
// NOT async, NOT object
```

### spec fields

```javascript
SEA.pen({
  // Predicate fields (compile thành expression tree):
  key:   expr,    // validate R[0] = key
  val:   expr,    // validate R[1] = value
  soul:  expr,    // validate R[2] = soul string
  state: expr,    // validate R[3] = HAM timestamp

  // Policy fields (appended AFTER expression tree):
  sign: true,             // 0xC0 — require authenticator
  cert: "ownerPubKey",    // 0xC1 — cert-gated writes
  open: true,             // 0xC3 — no auth required
  pow: {
    field: 0,             // R[field] hash (0=key, 1=val, ...)
    difficulty: 3,        // SHA-256 hex phải bắt đầu bằng 3 ký tự '0'
  },
})
```

Nếu nhiều predicate fields được chỉ định → AND lại. Nếu không có → `PASS` (always true).

### expr format

`expr` truyền vào `spec.key`, `spec.val`, `spec.soul`, `spec.state`:

```javascript
// String literal → EQ(field, "value")
key: "exact-key"

// Equality / inequality:
key: { eq: "x" }               // field === "x"
key: { ne: "x" }               // field !== "x"

// String predicates:
key: { pre: "order_" }         // field.startsWith("order_")
key: { suf: "_v2" }            // field.endsWith("_v2")
key: { inc: "item" }           // field.includes("item")

// Numeric comparison (scalar — so với field):
state: { lt: 9999 }
state: { gt: 0 }
state: { lte: 9999 }
state: { gte: 1 }

// Numeric comparison (array — so với arbitrary values):
key: { lt:  [{ reg: 4 }, 9999999] }
key: { gte: [{ reg: 129 }, { sub: [{ reg: 128 }, 10] }] }

// Logical:
key: { and: [expr1, expr2] }
key: { or:  [expr1, expr2] }
key: { not: expr }

// Type checks:
val: { type: 'string' }        // ISS
val: { type: 'number' }        // ISN
val: { type: 'null' }          // ISX
val: { type: 'bool' }          // ISB

// String length:
key: { length: [3, 64] }       // 3 ≤ len(field) ≤ 64 (max 255 each)

// Segment (split string by sep, get segment at idx):
key: { seg: { sep: ':', idx: 0 } }
key: { seg: { sep: ':', idx: 1, of: { reg: 0 } } }
key: { seg: { sep: ':', idx: 0, match: { gte: 100 } } }
key: { seg: { sep: ':', idx: 1, match: { length: [1, 64] } } }

// LET — bind local variable (slot 0–31):
key: { let: { bind: 0, def: { divu: [{ reg: 4 }, 300000] }, body: exprUsingLocal128 } }

// IF — conditional lazy evaluation:
key: { if: { cond: expr, then: expr, else: expr } }

// Arithmetic (array of 2 compileVal values):
{ add:  [{ reg: 4 }, 1000] }
{ sub:  [{ reg: 128 }, 5] }
{ divu: [{ reg: 4 }, 300000] }
{ mod:  [{ reg: 0 }, 100] }
{ mul:  [{ reg: 3 }, 2] }

// Register / local reference:
{ reg: 0 }    // R[0] = key
{ reg: 4 }    // R[4] = Date.now()
{ reg: 128 }  // local[0] (bind: 0)
{ reg: 129 }  // local[1] (bind: 1)

// Coercion:
{ tonum: { reg: 0 } }
{ tostr: { reg: 3 } }
```

**Lưu ý**:
- `seg` luôn trả về **string** — dùng `{ tonum: ... }` để convert sang số trước khi so sánh
- `length: [min, max]` — min và max mỗi cái tối đa 255 (1 byte)

---

## SEA.candle(opts) — Temporal Window Helper

**Synchronous**. Trả về một **expr object** (dùng làm giá trị cho `spec.key`).

```javascript
const expr = SEA.candle(opts)
// Returns: { let: { bind: 0, def: ..., body: { let: { bind: 1, ... } } } }

// Dùng trực tiếp trong spec.key:
const soul = SEA.pen({ key: SEA.candle({ back: 100, fwd: 2 }), sign: true })
```

### opts

```javascript
SEA.candle({
  seg:  0,        // index segment trong key chứa candle number (default 0)
  sep:  '_',      // separator chia key thành segments (default '_')
  size: 300000,   // candle size tính bằng ms (default 300000 = 5 phút)
  back: 100,      // cho phép tối đa back candles trong quá khứ (default 100)
  fwd:  2,        // cho phép tối đa fwd candles trong tương lai (default 2)
})
```

### Compiled bytecode (equivalent logic)

```javascript
// SEA.candle({ seg: 0, sep: ':', size: 300000, back: 100, fwd: 2 }) compiles to:
LET(local[0] = floor(R[4] / 300000))       // current candle
  LET(local[1] = tonum(seg(R[0], ':', 0))) // candle from key's segment 0
    AND(
      local[1] >= local[0] - 100,           // không quá cũ
      local[1] <= local[0] + 2              // không quá xa tương lai
    )
```

**Candle number** = `floor(Date.now() / size)`. Key phải chứa candle number tại segment `seg`:

```
key = "5777152_alice_buy_x3f7"
       ^^^^^^^
       floor(Date.now() / 300000) ≈ candle number hiện tại
```

Window với `back=100`, `fwd=2`, `size=300000`:
- **back**: 100 × 5 phút = 500 phút ≈ 8.3 giờ quá khứ
- **fwd**: 2 × 5 phút = 10 phút tương lai (clock skew tolerance)

See also: [`docs/thoughts/temporal-proof-of-work-gossip-protocol.md`](../thoughts/temporal-proof-of-work-gossip-protocol.md) for original candle concept.

---

## Common Patterns

### Pattern 1: Signed write — bất kỳ ai có authenticator

```javascript
const soul = SEA.pen({ sign: true })

gun.get(soul).put(data, null, { authenticator: myPair })
```

### Pattern 2: Key prefix bắt buộc + signed

```javascript
const soul = SEA.pen({
  key: { pre: 'order_' },
  sign: true
})

gun.get(soul).get('order_123').put(data, null, { authenticator: myPair })
```

### Pattern 3: Candle window (anti-replay) + signed

```javascript
const soul = SEA.pen({
  key: SEA.candle({ back: 100, fwd: 2 }),
  sign: true
})

// Khi ghi, key phải bắt đầu bằng candle number hiện tại:
const candle = Math.floor(Date.now() / 300000)
gun.get(soul).get(`${candle}_${orderId}`).put(data, null, { authenticator: myPair })
```

### Pattern 4: Key segments validation + candle + signed + PoW (P2P orders)

```javascript
// Soul cho order window của item "organic-green-tea", side "buy", candle 5777152
// Key format: <timestamp>:<pub>:<nonce>
const soul = SEA.pen({
  key: { and: [
    {
      let: {
        bind: 0,
        def: { divu: [{ tonum: { seg: { sep: ':', idx: 0, of: { reg: 0 } } } }, 300000] },
        body: { and: [
          { gte: [{ reg: 128 }, 5777152] },
          { lte: [{ reg: 128 }, 5777152] }
        ]}
      }
    },
    { eq: [
      { seg: { sep: ':', idx: 1, of: { reg: 0 } } },
      { reg: 5 }
    ]},
    { seg: { sep: ':', idx: 2, of: { reg: 0 }, match: { length: [1, 64] } } },
  ]},
  val: { type: 'string' },
  sign: true,
  pow: { field: 0, difficulty: 3 },
  params: { baseId: 'organic-green-tea', side: 'buy', candle: 5777152 }
})
```

### Pattern 5: Platform-controlled writes (cert-gated)

```javascript
// Chỉ holders của cert do platformPub ký mới được write
const soul = SEA.pen({
  key: { length: [1, 128] },
  cert: platformPub
})
```

---

## P2P Order Book — Ví dụ Hoàn Chỉnh

### createOrderSoul

```javascript
// src/core/OrderPen.js
import SEA from "/core/SEA.js"

// SEA.pen() là synchronous — không cần await, không cần async
function createOrderSoul(baseId, side, candle) {
  return SEA.pen({
    key: { and: [
      {
        let: {
          bind: 0,
          def: { divu: [{ tonum: { seg: { sep: ':', idx: 0, of: { reg: 0 } } } }, 300000] },
          body: { and: [
            { gte: [{ reg: 128 }, candle] },
            { lte: [{ reg: 128 }, candle] }
          ]}
        }
      },
      { eq: [
        { seg: { sep: ':', idx: 1, of: { reg: 0 } } },
        { reg: 5 }
      ]},
      { seg: { sep: ':', idx: 2, of: { reg: 0 }, match: { length: [1, 64] } } },
    ]},
    sign: true,
    pow: { field: 0, difficulty: 3 },
    params: { baseId, side, candle }
  })
}
```

### createOrder

```javascript
// src/UI/routes/order/logic.js
import gun from "/core/Gun.js"
import { createOrderSoul } from "/core/OrderPen.js"
import { Access } from "/core/Access.js"

async function createorder({ baseId, side, baseQuantity, quoteQuantity }) {
  const candle = Math.floor(Date.now() / 300000)
  const soul = createOrderSoul(baseId, side, candle)
  const nonce = String.random(6)             // gun/src/shim.js

  // Key format trong soul window: <timestamp>:<pub>:<nonce>
  const key = `${Date.now()}:${Access.states.pub}:${nonce}`

  gun.get(soul).get(key).put({
    maker: {
      pub: Access.states.pub,
      epub: Access.states.epub,
      xpub: Access.states.xpub
    },
    side,
    base: { type: 'item', id: baseId, quantity: baseQuantity },
    quote: { type: 'crypto', quantity: quoteQuantity, contract: 'USDT', chain: 1 },
    created: Date.now(),
    status: 'open'
  }, ack => {
    if (ack.err) console.error('Order rejected:', ack.err)
    else console.log('Order created:', key)
  }, { authenticator: Access.states.pair })
}
```

### subscribeorders

```javascript
async function subscribeorders(baseId, callback) {
  const candle = Math.floor(Date.now() / 300000)

  // Scan current + previous candle souls
  for (const c of [candle - 1, candle]) {
    gun.get(createOrderSoul(baseId, "buy", c)).map().on((order, orderId) => {
      if (!order || !order.side) return
      callback({ ...order, id: orderId })
    })
  }
}
```

### Key Format

```
soul params: { baseId, side, candle }
key:         <timestamp>:<pub>:<nonce>

Ví dụ:
- params: { baseId: "organic-green-tea", side: "buy", candle: 5777152 }
- key:    1744440123456:maker_full_pub:a3f7b2

└─ params: baseId + side + candle        — compile-time soul identity
   seg 0: 1744440123456                  — timestamp ms, floor(/300000) must equal candle
   seg 1: maker_full_pub                 — full writer pub, matched against R[5]
   seg 2: a3f7b2                         — nonce random, iterated cho PoW
```

**Vì sao timestamp ở đầu**:
- Pen validation lấy `seg: 0` rồi derive `candle = floor(timestamp / 300000)`
- soul đã cố định đúng market window bằng `params`
- key vẫn giữ thứ tự thời gian tự nhiên trong cùng candle

---

## Debugging

### Inspect soul bytecode

```javascript
// Soul là base62-encoded bytecode
const soul = SEA.pen({ key: { pre: 'x_' }, sign: true })
console.log(soul)  // "$abc123..."

// Unpack bytecode để inspect:
import pen from "/gun/lib/pen.js"
const bc = pen.unpack(soul.slice(1))
console.log(bc)    // Uint8Array
```

### Test validation thủ công

```javascript
import pen from "/gun/lib/pen.js"

const soul = SEA.pen({ key: { pre: 'order_' }, sign: true })
const bytecode = pen.unpack(soul.slice(1))

pen.ready.then(() => {
  const ok = pen.run(bytecode, [
    'order_123',   // R[0] = key
    '{"test":1}',  // R[1] = val
    soul,          // R[2] = soul
    0,             // R[3] = state
    Date.now(),    // R[4] = now
    'myPubKey'     // R[5] = upub
  ])
  console.log('Validation:', ok)  // true/false
})
```

### Common Errors

| Error | Nguyên nhân |
|-------|-------------|
| `PEN: predicate failed` | Key/val không match expression trong soul |
| `PEN: valid signature required` | sign policy nhưng không có authenticator và không có `val['*']` |
| `PEN: PoW insufficient` | SHA-256(R[field]) không bắt đầu đủ '0' |
| `PEN: cert required` | cert policy nhưng val không có `{ '+', '*' }` |

---

## Candle Window Configurations

```javascript
// Tight (market orders: ±15 phút)
SEA.candle({ size: 300000, back: 3,   fwd: 3 })

// Medium (limit orders: ±4 giờ)
SEA.candle({ size: 300000, back: 48,  fwd: 2 })

// Loose (GTC orders: ±8 giờ) — DEFAULT
SEA.candle({ size: 300000, back: 100, fwd: 2 })

// Ultra loose (platform: ±30 ngày)
SEA.candle({ size: 3600000, back: 720, fwd: 10 })
```

---

## References

- **VM source**: `pen/src/pen.zig`
- **WASM interface**: `pen/src/wasm.zig`
- **Bridge + compiler**: `gun/lib/pen.js`
- **Test suite**: `pen/test/test.js`
- **Temporal concept**: [`docs/thoughts/temporal-proof-of-work-gossip-protocol.md`](./temporal-proof-of-work-gossip-protocol.md)
- **P2P trading protocol**: [`docs/thoughts/white-paper.md`](./white-paper.md)
