# Pen DSL - Complete Guide for Gun Validation

## Overview

Pen is a WASM-based predicate language that validates **who can write** to Gun souls. It compiles validation logic into ~26KB bytecode, encodes it to ~77 base62 characters, and uses that as the soul name (`$abc...`). This prevents spam and unauthorized writes without requiring centralized permission systems.

**Key insight**: Pen is NOT a query language. It's a validation engine that runs AFTER someone attempts to write, checking: signature, proof-of-work, temporal windows, and custom logic.

### Why Pen Exists

Gun is a decentralized graph database. Without validation:
- Anyone can overwrite `gun.get("orders")` causing chaos
- No spam protection
- No access control
- No schema enforcement

Pen solves this by making the **soul itself encode the validation rules**. Only writes that pass the Pen bytecode execute successfully.

---

## Architecture (3 Layers)

### Tầng 0: pen.wasm (WASM VM)
- 26KB Zig-compiled WASM runtime
- Executes bytecode against host-provided registers
- Returns boolean (pass/fail)
- No I/O, pure computation

### Tầng 1: lib/pen.js (Compiler + Bridge)
- JavaScript compiler: source → bytecode
- Gun pipeline integration (`Gun.chain.pen()`)
- SEA integration (`SEA.pen()`, `SEA.candle()`)
- Register population from Gun graph data

### Tầng 2: Application Code
```javascript
// Define validation logic
const policy = await SEA.pen({
  expression: ["AND", 
    ["SGN", ["R", 5]], // Must be signed by R[5] (pub)
    ["POW", 3]         // Must have proof-of-work >= 3
  ]
})

// Use as soul
gun.get(policy.soul).put({ data: "value" }, ack => {
  // Only succeeds if signature + PoW valid
})
```

---

## ISA v1 Bytecode Format

### Expression Tree Structure
Pen bytecode is a **post-order traversal** of an expression tree:

```
Expression: ["AND", ["EQ", ["R", 0], "item:123"], ["SGN", ["R", 5]]]

Tree:
       AND
      /   \
    EQ    SGN
   /  \     \
  R0  "123" R5

Bytecode (pseudo): PUSH_R0 PUSH_STR("123") EQ PUSH_R5 SGN AND
```

### Opcodes Categories

#### 1. Constants & Literals
- `0x00` - Boolean false
- `0x01` - Boolean true
- `0x02` - Null
- `0x10-0x17` - Small integers 0-7
- `0x18` - Varint (1-9 bytes, LEB128)
- `0x20` - String literal (length + UTF-8)

#### 2. Registers
- `0x30` - Generic register access `["R", N]`
- `0xF0-0xFF` - Shorthand for R[0-15] (optimization)

**Standard Register Convention**:
```javascript
R[0]  = key     // The key being written
R[1]  = val     // The value being written  
R[2]  = soul    // The soul being written to
R[3]  = state   // Gun metadata (timestamp)
R[4]  = Date.now()  // Current Unix ms
R[5]  = pub     // Public key (from signature)
R[6]  = candle_back   // Temporal window start
R[7]  = candle_fwd    // Temporal window end
R[8+] = Custom  // App-defined
```

#### 3. Logic Operations
- `0x40` - `NOT` - Logical negation
- `0x41` - `AND` - Logical AND (short-circuit)
- `0x42` - `OR` - Logical OR (short-circuit)

#### 4. Comparison
- `0x50` - `EQ` - Equality (deep compare)
- `0x51` - `NE` - Not equal
- `0x52` - `LT` - Less than
- `0x53` - `LE` - Less or equal
- `0x54` - `GT` - Greater than
- `0x55` - `GE` - Greater or equal

#### 5. Arithmetic
- `0x60` - `ADD` - Addition
- `0x61` - `SUB` - Subtraction
- `0x62` - `MUL` - Multiplication
- `0x63` - `DIV` - Division
- `0x64` - `MOD` - Modulo

#### 6. String Operations
- `0x70` - `CAT` - Concatenation
- `0x71` - `LEN` - String length
- `0x72` - `SUB` - Substring
- `0x73` - `IDX` - Index of substring
- `0x74` - `SPL` - Split string
- `0x75` - `LOW` - Lowercase
- `0x76` - `UPP` - Uppercase

#### 7. Advanced
- `0x80` - `SEGR` - Segment register (slice R[N:M])
- `0x81` - `SEGR2` - 2D segment (matrix access)
- `0x90` - `LET` - Local variable (R[128-255])
- `0x91` - `GET` - Read local variable

#### 8. Policy Opcodes (Appended AFTER expression)
- `0xC0` - `SGN` - Verify signature by public key
- `0xC1` - `CRT` - Create-only (no updates)
- `0xC3` - `NOA` - No append (disable Gun's `gun.set()` append)
- `0xC4` - `POW` - Proof-of-work difficulty

---

## Gun-Pen Bridge: How Validation Works

### Write Flow

```javascript
// 1. User writes data
gun.get("$abc...soul").put({ key: "value" }, ack => { ... })

// 2. Gun pipeline intercepts write (check stage)
// 3. Detects soul starts with "$" → triggers penStage()
// 4. penStage() decodes soul bytecode: pen.unpack(soul.slice(1))
// 5. Populates registers (HOST INJECTION):
const registers = [
  ctx.key,                              // R[0] = "key"
  ctx.val,                              // R[1] = JSON.stringify(value)
  soul,                                 // R[2] = "$abc...soul"
  ctx.state || 0,                       // R[3] = HAM state timestamp
  Date.now(),                           // R[4] = CURRENT TIME (injected by Gun)
  ctx.at.user.is.pub || ''              // R[5] = writer's public key
]

// 6. Executes bytecode in WASM
pen.ready.then(() => {
  const ok = pen.run(bytecode, registers)  // true/false
  if (ok) next()      // Continue Gun pipeline
  else reject('PEN validation failed')
})
```

**Key point**: `R[4] = Date.now()` is **injected by Gun's JavaScript layer** (`lib/pen.js`), NOT by Pen WASM.

### Pen Core Doesn't Know Time

**Pen WASM** (`lib/pen.wasm`):
- ❌ No access to system time
- ❌ No `Date.now()` call inside WASM
- ❌ No network access for NTP
- ✅ Only receives `registers[]` array from host

**Gun Bridge** (`lib/pen.js`):
- ✅ Calls `Date.now()` in JavaScript
- ✅ Injects timestamp into R[4] register
- ✅ Passes registers to Pen WASM
- ✅ Pen bytecode uses `DIVU(R[4], 300000)` to calculate candle

**Architecture**:
```
┌─────────────────────────────────────────────┐
│  JavaScript Host (lib/pen.js)               │
│  - Calls Date.now() ← BROWSER/NODE API      │
│  - Injects into R[4]                        │
│  - ctx.state (HAM timestamp) → R[3]         │
│  - ctx.at.user.is.pub → R[5]                │
└──────────────┬──────────────────────────────┘
               │ registers = [key, val, soul, state, NOW, pub]
               ▼
┌─────────────────────────────────────────────┐
│  Pen WASM (lib/pen.wasm)                    │
│  - Pure computation (no I/O)                │
│  - Receives R[0-5] from host                │
│  - Executes: DIVU(R[4], 300000)             │
│  - Returns: boolean (pass/fail)             │
└─────────────────────────────────────────────┘
```

### Where Date.now() Comes From

**In Browser**:
```javascript
// lib/pen.js line ~598:
var regs = [
  ctx.key, 
  ctx.val, 
  soul,
  ctx.state || 0, 
  Date.now(),        // ← window.performance.now() or new Date()
  ctx.at.user.is.pub || ''
];
```

**In Node.js**:
```javascript
Date.now()  // ← process.hrtime() wrapped by V8
```

**Time Sources**:
1. **Browser**: `window.performance.now()` (high-res timer since page load)
2. **Node.js**: System clock via `clock_gettime()` syscall
3. **Both**: Can be affected by:
   - System clock changes (NTP sync)
   - User manual clock adjustment
   - VM time drift

### Candle Calculation Flow

```javascript
// 1. Gun intercepts write
gun.get("$abc...").get("5777152_item_buy_nonce").put(data)

// 2. lib/pen.js injects timestamp
const now = Date.now()  // e.g., 1733145600000 (Unix ms)
const registers = [key, val, soul, state, now, pub]

// 3. Pen bytecode extracts candle from key
// Bytecode: LET(0, DIVU(R[4], 300000))  ← current = floor(now / 300000)
const current_candle = Math.floor(now / 300000)  // 5777152

// 4. Compare with candle in key (segment 0)
const key_candle = parseInt(key.split('_')[0])  // 5777152

// 5. Validate window
if (key_candle >= current_candle - 100 &&   // Not too old
    key_candle <= current_candle + 2) {     // Not too future
  return true  // ACCEPT
} else {
  return false // REJECT
}
```

### SEA.pen() API

```javascript
const policy = await SEA.pen({
  // Expression tree
  expression: ["AND", 
    ["EQ", ["R", 0], "allowed_key"],
    ["SGN", ["R", 5]]
  ],
  
  // Additional registers (optional)
  registers: {
    8: "custom_value",
    9: 12345
  },
  
  // Temporal candle (optional)
  candle: { back: 100, fwd: 2 } // ±100 candles, +2 future
})

// Returns:
{
  soul: "$7f3a9b...",     // base62-encoded bytecode
  bytecode: Uint8Array,   // Raw WASM bytecode
  policy: "...",          // base62 string
  registers: { 6: ..., 7: ... } // Candle times if provided
}
```

### SEA.candle() Helper

Creates **flexible temporal validation window** (epoch lỏng).

**Candle number** = `Math.floor(Date.now() / size_ms)` - chia thời gian thành các "nến" rời rạc.

```javascript
const candle = SEA.candle({ 
  seg: 0,        // Key segment index (split by sep)
  sep: ":",      // Separator character (colon for consistency)
  size: 300000,  // Candle size in ms (default: 5 minutes)
  back: 100,     // Accept 100 candles in past (default)
  fwd: 2         // Accept 2 candles in future (default)
})

// With 5-min candles:
// back: 100 = 500 minutes = ~8.3 hours past
// fwd: 2 = 10 minutes future (clock skew tolerance)
```

**Use case**: Flexible time windows that slide automatically.

```javascript
// Validate key's candle number is within sliding window
// Compiles to:
["LET", 128,  // local[0] = current candle = floor(now / size)
  ["DIVU", ["R", 4], size],
  ["LET", 129,  // local[1] = candle from key segment
    ["TONUM", ["SEG", ["R", 0], sep, seg]],
    ["AND",
      ["GTE", ["R", 129], ["SUB", ["R", 128], back]],  // >= current - 100
      ["LTE", ["R", 129], ["ADD", ["R", 128], fwd]]    // <= current + 2
    ]
  ]
]

// Example: Order with key "5777152:organic-tea:buy:abc"
// - Extract candle: 5777152 (from segment 0)
// - Current candle: 5777250
// - Valid if: 5777150 <= 5777152 <= 5777252 ✓
```

**Why "loose epoch"** (so với fixed timestamps):
- ✅ **Sliding window**: Automatically moves with time (no hardcoded dates)
- ✅ **Self-expiring**: Orders older than `back` candles auto-reject
- ✅ **Clock skew tolerant**: `fwd` buffer accepts slightly-future writes
- ✅ **Compact keys**: Candle numbers shorter than timestamps (7 vs 13 digits)

**Flexible configurations**:

```javascript
// Tight window (market orders: ±15 minutes)
SEA.candle({ size: 300000, back: 3, fwd: 3 })

// Medium window (limit orders: ±4 hours)  
SEA.candle({ size: 300000, back: 48, fwd: 2 })

// Loose window (GTC orders: ±8 hours) - DEFAULT
SEA.candle({ size: 300000, back: 100, fwd: 2 })

// Ultra loose (escrow: ±30 days)
SEA.candle({ size: 3600000, back: 720, fwd: 10 })
```

See also: [`docs/thoughts/temporal-proof-of-work-gossip-protocol.md`](../thoughts/temporal-proof-of-work-gossip-protocol.md) for original candle concept.

---

## Common Patterns for P2P Trading

### Pattern 1: Signed Write by Specific User

```javascript
// Only user with pub_key can write
const policy = await SEA.pen({
  expression: ["SGN", "pub_abc123..."]
})

gun.get(policy.soul).put({ order: "data" })
// Must be signed by pub_abc123's key pair
```

### Pattern 2: Item-Scoped Orders

```javascript
// Orders for specific item, signed by creator, with PoW
const item = "organic-green-tea"
const policy = await SEA.pen({
  expression: ["AND",
    ["EQ", ["R", 8], item],    // R[8] = item slug
    ["SGN", ["R", 5]],          // Must be signed
    ["POW", 3]                  // PoW difficulty 3
  ],
  registers: { 8: item },
  candle: { back: 100, fwd: 2 }
})

// Soul = $<base62_bytecode>
gun.get(policy.soul).put({ 
  type: "buy", 
  quantity: 10,
  price: 1500
})
```

### Pattern 3: Multi-Party Escrow Updates

```javascript
// Trade escrow: only maker, taker, or platform can update
const policy = await SEA.pen({
  expression: ["OR",
    ["SGN", maker_pub],
    ["SGN", taker_pub],
    ["SGN", platform_pub]
  ]
})

gun.get(policy.soul).put({ status: "delivered" })
```

### Pattern 4: Create-Only (No Updates)

```javascript
// Order can be created but never modified
const policy = await SEA.pen({
  expression: ["AND",
    ["SGN", ["R", 5]],
    ["CRT"]  // Create-only policy
  ]
})
```

### Pattern 5: Temporal Order Validity (Flexible Epoch)

```javascript
// Order valid within sliding window (8 hours past, 10 min future)
const policy = await SEA.pen({
  expression: ["AND",
    ["SGN", ["R", 5]],
    SEA.candle({ seg: 0, sep: ":", size: 300000, back: 100, fwd: 2 })
  ]
})

// Key format: <candle>:<item>:<type>:<nonce>
// Example: "5777152:organic-tea:buy:a3f7"
// - Candle 5777152 extracted from segment 0
// - Validates against current candle ± window
// - Auto-expires after ~8 hours
```

**Why better than fixed timestamps**:
- No need to know start/end time in advance
- Window slides automatically with current time
- Orders self-expire (no manual cleanup)

---

## Discovery Strategy for Orders

### Problem: No Public Namespaces

❌ **Don't do this**:
```javascript
gun.get("orders").map().on(order => { ... })
// Anyone can overwrite gun.get("orders")!
```

✅ **Do this**: Per-item deterministic Pen souls with candle-based keys

```javascript
// 1. Create deterministic soul for each item
async function getItemOrderSoul(itemSlug) {
  const policy = await SEA.pen({
    expression: ["AND",
      ["EQ", ["R", 8], itemSlug],
      ["SGN", ["R", 5]],
      ["POW", 2],
      // Temporal window: 8 hours past, 10 min future
      SEA.candle({ seg: 0, sep: ":", size: 300000, back: 100, fwd: 2 })
    ],
    registers: { 8: itemSlug }
  })
  return policy.soul
}

// 2. Everyone knows where to look for "organic-green-tea" orders
const soul = await getItemOrderSoul("organic-green-tea")

// 3. Subscribe to orders in current time window
const candle = Math.floor(Date.now() / 300000)
gun.get(soul).get({
  '>': `${candle}:`,     // Current candle onwards
  '<': `${candle}:~`     // Within current candle
}).on((order, orderId) => {
  console.log("New order:", order)
})

// 4. Create new order (must provide signature + PoW + valid candle)
const candle = Math.floor(Date.now() / 300000)
const nonce = Math.random().toString(36).slice(2, 8)
const key = `${candle}:organic-green-tea:buy:${nonce}`

gun.get(soul).get(key).put({
  type: "buy",
  quantity: 10,
  price: 1500,
  trader: my_pub
})
```

**Key format benefits**:
- **Candle-first**: Enables time-range LEX queries
- **Compact**: 7 digits vs 13-digit timestamp
- **Self-expiring**: Old candles auto-reject (no cleanup needed)
- **Discovery-friendly**: Query by item + time range

### Multi-Item Order Replication

For orders spanning multiple items, replicate to each item's soul:

```javascript
const items = ["green-tea", "coffee", "honey"]
const candle = Math.floor(Date.now() / 300000)
const orderId = `${candle}_multi_buy_${nonce}`
const order = {
  type: "buy",
  items: items,
  trader: my_pub
}

// Write to each item's soul
for (const item of items) {
  const soul = await getItemOrderSoul(item)
  gun.get(soul).get(orderId).put(order)
}

// Discovery: traders watching ANY of these items will find the order
```

**Time-based discovery patterns**:

```javascript
// Current candle only (most recent orders)
const now = Math.floor(Date.now() / 300000)
gun.get(soul).get({ '>': `${now}_`, '<': `${now}_~` })

// Last 10 candles (50 minutes of orders)
const start = now - 10
gun.get(soul).get({ '>': `${start}_`, '<': `${now}_~` })

// Subscribe to future orders (realtime)
gun.get(soul).get({ '>': `${now}_` }).on(order => {
  console.log("New order created:", order)
})
```

---

## Optimization Techniques

### 1. Register Shorthands

Use `0xF0-0xFF` for common registers instead of `["R", N]`:

```javascript
// Before: ["EQ", ["R", 0], "key"]
// Bytecode: 0x30 0x00 0x20 0x03 "key" 0x50

// After: ["EQ", 0xF0, "key"]  // 0xF0 = R[0]
// Bytecode: 0xF0 0x20 0x03 "key" 0x50
// Saves 1 byte per register access
```

### 2. Small Integer Literals

Use `0x10-0x17` for values 0-7:

```javascript
// Before: ["POW", 3]
// Bytecode: 0x18 0x03 0xC4

// After: ["POW", 0x13]  // 0x13 = integer 3
// Bytecode: 0x13 0xC4
// Saves 1 byte
```

### 3. SEGR Macro

Access sub-ranges of registers without multiple operations:

```javascript
// Get registers 8-12 as array
["SEGR", 8, 12]

// 2D access (nested arrays)
["SEGR2", 8, 12, 0, 5] // R[8-12][0-5]
```

### 4. LET for Reuse

Store intermediate results to avoid recomputation:

```javascript
["LET", 128,           // Store in R[128]
  ["ADD", ["R", 10], ["R", 11]]
]
["MUL", ["GET", 128], 2]  // Reuse R[128]
```

---

## Security Considerations

### 1. Replay Attacks

**Problem**: Attacker intercepts valid signed write, replays later.

**Solution**: Temporal candles + nonce

```javascript
const policy = await SEA.pen({
  expression: ["AND",
    ["SGN", ["R", 5]],
    ["GE", ["R", 3], ["R", 6]],  // state >= candle_back
    ["LE", ["R", 3], ["R", 7]]   // state <= candle_fwd
  ],
  candle: { back: 100, fwd: 2 }
})
```

### 2. Proof-of-Work

Forces cost on writes (prevents spam):

```javascript
["POW", 4]  // Requires ~16 hash attempts (2^4)
```

**Trade-off**: Higher PoW = more spam protection but slower writes.

### 3. Soul Disclosure

Pen souls are **public by design**. Anyone who knows the soul can:
- Read data (Gun is eventually consistent, public by default)
- Attempt writes (will fail validation)

**Don't encode secrets in souls**: Use SEA encryption for sensitive data.

### 4. Signature Verification

`SGN` checks signature matches public key, but doesn't verify authorization:

```javascript
// ❌ This allows ANY signed write:
["SGN", ["R", 5]]

// ✅ This restricts to specific user:
["SGN", "pub_specific_user"]

// ✅ Or check against whitelist:
["OR",
  ["SGN", "pub_alice"],
  ["SGN", "pub_bob"],
  ["SGN", "pub_charlie"]
]
```

---

## Practical Example: P2P Order Book

### Define Order Schema Pen

```javascript
// src/core/OrderPen.js
import SEA from "/core/SEA.js"

export async function createOrderSoul(itemSlug) {
  const policy = await SEA.pen({
    // Validation rules:
    // 1. Order must be for this specific item
    // 2. Must be signed by creator
    // 3. Must have PoW to prevent spam
    // 4. Must be within temporal window (8h past, 10min future)
    expression: ["AND",
      ["EQ", ["R", 8], itemSlug],      // R[8] = item slug
      ["SGN", ["R", 5]],                // R[5] = creator pub
      ["POW", 3],                        // 2^3 = 8 hashes
      SEA.candle({
        seg: 0,                          // Candle in first segment
        sep: "_",
        size: 300000,                    // 5-minute candles
        back: 100,                       // 500 minutes past
        fwd: 2                           // 10 minutes future
      })
    ],
    registers: { 8: itemSlug }
  })
  
  return policy.soul
}
```

### Create Order

```javascript
// src/UI/routes/order/logic.js
import gun from "/core/Gun.js"
import { createOrderSoul } from "/core/OrderPen.js"

export async function createOrder({ item, type, quantity, price, trader }) {
  const soul = await createOrderSoul(item)
  
  // Calculate current candle
  const candle = Math.floor(Date.now() / 300000)
  
  // Generate random nonce for uniqueness
  const nonce = Math.random().toString(36).slice(2, 8)
  
  // Key format: <candle>:<item>:<type>:<nonce>
  const key = `${candle}:${item}:${type}:${nonce}`
  
  // Must be signed with trader's key pair (happens in SEA layer)
  gun.get(soul).get(key).put({
    type,      // "buy" or "sell"
    quantity,
    price,
    trader,
    item,
    created: Date.now(),
    status: "open"
  }, ack => {
    if (ack.err) {
      console.error("Order rejected:", ack.err)
      // Possible rejections:
      // 1. Candle outside window (too old/new)
      // 2. Item slug mismatch
      // 3. Invalid type
      // 4. Missing signature
      // 5. Insufficient PoW
    } else {
      console.log("Order created:", key)
    }
  })
}
```

### Discover Orders

```javascript
// src/UI/routes/order/logic.js
export async function subscribeToOrders(itemSlug, callback) {
  const soul = await createOrderSoul(itemSlug)
  const candle = Math.floor(Date.now() / 300000)
  
  // Subscribe to orders in current candle window
  gun.get(soul).get({
    '>': `${candle}_${itemSlug}_`,
    '<': `${candle}_${itemSlug}_~`
  }).on((order, orderId) => {
    // Filter out null/deleted
    if (!order || !order.type) return
    
    // Pen already validated signature, PoW, timestamp
    callback({ ...order, id: orderId })
  })
}

// Usage:
subscribeToOrders("organic-green-tea", order => {
  console.log("New order:", order)
  // Update UI order book
})
```

### Match Orders

```javascript
export async function matchOrder(makerOrderId, takerOrder) {
  // 1. Read maker order
  const makerSoul = await createOrderSoul(takerOrder.item)
  gun.get(makerSoul).get(makerOrderId).once(makerOrder => {
    
    // 2. Validate match
    if (makerOrder.type === takerOrder.type) {
      console.error("Cannot match same type orders")
      return
    }
    
    if (makerOrder.quantity !== takerOrder.quantity) {
      console.error("Quantity mismatch")
      return
    }
    
    // 3. Create trade escrow (see Lock.js)
    const candle = Math.floor(Date.now() / 300000)
    const trade = {
      maker: makerOrder.trader,
      taker: takerOrder.trader,
      item: makerOrder.item,
      quantity: makerOrder.quantity,
      price: makerOrder.price,
      status: "locked",
      created: Date.now()
    }
    
    // 4. Write to trade soul (different Pen policy allowing maker/taker/platform)
    const tradeSoul = await createTradeSoul(trade.maker, trade.taker)
    gun.get(tradeSoul).put(trade)
  })
}
```

**Key format**: `<candle>:<item>:<type>:<nonce>`

Example: `5777152:organic-green-tea:buy:a3f7b2`
- Candle: `5777152` (segment 0) - auto-validated against current ±window
- Item: `organic-green-tea` (segment 1) - must match soul's item
- Type: `buy` (segment 2) - used for discovery queries  
- Nonce: `a3f7b2` (segment 3) - ensures uniqueness

---

## Debugging Pen Validation

### Check Bytecode

```javascript
const policy = await SEA.pen({ expression: ["SGN", ["R", 5]] })
console.log("Soul:", policy.soul)
console.log("Bytecode:", policy.bytecode)
console.log("Base62:", policy.policy)
```

### Test Validation Manually

```javascript
// In Gun source, Pen exposes test interface:
import pen from "@akaoio/gun/lib/pen.js"

const result = pen.test({
  bytecode: policy.bytecode,
  registers: {
    0: "test_key",
    1: "test_value",
    5: "pub_xyz"
  }
})

console.log("Validation:", result) // true/false
```

### Common Errors

**"Pen validation failed"**: Check:
1. Signature present and valid?
2. PoW computed (if required)?
3. Timestamp within candle window?
4. Custom registers match expected values?

**"Soul not found"**: 
- Pen souls are content-addressed, not human-readable
- Must use exact soul from `SEA.pen()` output
- Don't try to manually construct souls

---

## Performance Characteristics

### Bytecode Size
- Simple validation: ~20-50 bytes → ~27-68 base62 chars
- Complex logic: ~100-200 bytes → ~136-273 base62 chars
- WASM runtime: 26KB (loaded once, cached)

### Execution Time
- WASM eval: <1ms for typical policies
- Signature verification: ~5-10ms (SEA layer)
- PoW computation: 2^difficulty × hash time

### Network Overhead
- Soul transmitted with every write (part of Gun message)
- Base62 encoding: ~1.37× raw bytecode size
- Typical soul: 50-100 characters

---

## Comparison with Other Systems

| Feature | Pen | Smart Contracts | ACLs |
|---------|-----|-----------------|------|
| Language | Bytecode DSL | Solidity/Vyper | Config |
| Execution | Client-side WASM | On-chain VM | Server |
| Cost | Free (local CPU) | Gas fees | Free |
| Decentralization | Full | Full | Centralized |
| Flexibility | High | Very High | Low |
| Complexity | Medium | High | Low |

**Pen sweet spot**: Decentralized validation without blockchain gas fees, more flexible than static ACLs.

---

## Advanced Topics

### Custom Opcodes (Future)

Pen ISA is versioned. Future versions may add:
- Cryptographic primitives (ECDSA, BLS)
- Hash functions (SHA-256, Blake3)
- JSON path queries
- Regular expressions

Currently, complex logic must use Gun-side preprocessing.

### Pen Composition

Combine multiple policies:

```javascript
// Policy A: Signed by owner
const policyA = await SEA.pen({ expression: ["SGN", owner_pub] })

// Policy B: Within time window
const policyB = await SEA.pen({ expression: ["LE", ["R", 4], deadline] })

// Compose: AND both policies
const combined = await SEA.pen({
  expression: ["AND",
    policyA.expression,
    policyB.expression
  ]
})
```

### Pen Versioning

Soul format: `$<version><base62_bytecode>`

Currently version is implicit (ISA v1). Future versions will have explicit prefix.

---

## Resources

- **Source**: `node_modules/@akaoio/gun/docs/pen.md` (731 lines, authoritative spec)
- **WASM Runtime**: `node_modules/@akaoio/gun/lib/pen.wasm` (26,036 bytes)
- **Compiler**: `node_modules/@akaoio/gun/lib/pen.js` (JavaScript bridge)
- **Examples**: `node_modules/@akaoio/gun/docs/pen.md` section 7 (order namespace)

---

## Summary

**Pen DSL enables trustless, decentralized validation for Gun writes** by:

1. **Encoding validation rules as bytecode** in the soul name
2. **Executing client-side** in WASM (no server, no blockchain)
3. **Preventing unauthorized writes** through signature + PoW + custom logic
4. **Enabling discovery** through deterministic soul generation

**For P2P trading**:
- Create per-item order souls (deterministic from item slug)
- Replicate multi-item orders to each item's soul
- Validate orders with: signature + PoW + temporal candle
- No central registry, no spam, no overwrites

**Key mental model**: Pen is a **distributed smart contract** running in WASM on every peer's machine, validating writes before they enter the Gun graph.
