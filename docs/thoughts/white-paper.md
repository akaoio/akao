# P2P Trading Platform Protocol

**Version** 2.1 · **Status** Production · **Date** 2026-04-15

> **Supersedes**: v1.0 eCommerce model (deprecated - see `white-paper-v1-ecommerce-deprecated.md`)
>
> **Philosophy**: DEX-inspired P2P trading where all participants are symmetric traders. Platform provides infrastructure, not custody.

---

## Abstract

This document specifies a **trustless, serverless platform protocol for peer-to-peer trading** of digital assets (game items, NFTs, digital goods). The protocol enables a platform operator to mediate trades between two traders (maker and taker) using deterministic cryptographic key derivation — eliminating the need for smart contracts, custodial accounts, or trusted third-party oracles.

All platform wallet addresses are derived client-side from **Diffie-Hellman shared secrets** and **ZEN deterministic secp256k1 derivation**. AKAO no longer depends on GunDB cryptography or `ethers.js` HD-wallet primitives for offline key computation. **ZEN** owns offline cryptographic derivation; **Ethers.js** is retained only for RPC communication, transaction serialization, and chain transport.

### Think DEX, Not Marketplace

**Traditional marketplace** (v1.0 - deprecated):
- ❌ Fixed roles: Buyer pays, Seller sells
- ❌ Item-centric: Each item has separate platform tree
- ❌ One-directional: Only buyers initiate trades
- ❌ Custodial mindset: Platform holds listings

**DEX/Trading mindset** (v2.0 - current):
- ✅ **Symmetric roles**: Anyone can create buy OR sell orders
- ✅ **Trader-centric**: Maker/Taker like Uniswap, dYdX, Binance
- ✅ **Bi-directional**: Orders are just intentions (buy intention = sell intention from other side)
- ✅ **Order book**: Decentralized discovery via ZEN + Pen validation, partitioned by item soul and order-side soul
- ✅ **Affiliate economy**: Built-in referral commissions
- ✅ **Non-custodial (happy path)**: Platform arbitrates disputes/refunds, not involved in normal trades

**Key innovations**:
- **Dual platform wallets** per trade: Transaction Lock (TL) + Commission Lock (CL)
- **Temporal validation**: Orders auto-expire using candle-based epochs (Pen DSL)
- **Post-match deposits**: Both order types deposit after matching (pure P2P, no Platform involvement)
- **Trustless verification**: Anyone can verify platform addresses on-chain before accepting

---

## Glossary of Abbreviations

| Abbreviation | Full Name | Description |
|--------------|-----------|-------------|
| **P** | Platform | Marketplace operator and trusted arbiter |
| **M** | Maker | Trader who creates an order (buy or sell) |
| **T** | Taker | Trader who accepts/matches an order |
| **A** | Affiliate | Referrer who earns commission on trades |
| **FP** | Fund Proof | Pre-deposit wallet for buy orders (proof of funds) |
| **TL** | Transaction Lock | Platform wallet holding trade payment |
| **CL** | Commission Lock | Platform wallet holding affiliate commission |
| **ZEN** | ZEN Runtime | AKAO's offline cryptography and deterministic secp256k1 derivation runtime |
| **ECDH** | Elliptic Curve Diffie-Hellman | Shared secret key exchange protocol |
| **Public pair** | Public Pair | The `pub/epub` half of a ZEN pair, sufficient for deterministic watch-only lock derivation |
| **Root pair** | Root Full Pair | Full private/public root pair used to derive spending keys |
| **DSL** | Domain-Specific Language | Pen validation language for order keys |

---

## 1. Roles and Trust Model

### 1.1 Parties

| Role | Symbol | Description |
|---|---|---|
| **Platform** | **P** | Marketplace operator. Holds authority to release or refund platform funds. Acts as trusted arbiter in disputes. |
| **Maker** | **M** | Trader who creates an order (buy or sell). |
| **Taker** | **T** | Trader who accepts/matches an existing order. |
| **Affiliate** | **A** | Referrer who brought the payer to the platform. Earns commission on successful trades. |

**Note:** M and T are symmetric — both are traders. A trader can be M in one trade and T in another. The distinction only matters during a specific trade execution. A is optional — if payer has no referrer, no affiliate commission is paid.

### 1.2 Platform Wallets

| Wallet | Symbol | Full Name | Purpose | Derived From |
|--------|--------|-----------|---------|--------------|
| Fund Proof | **FP** | Fund Proof | Pre-deposit for buy orders (proof of funds) | Maker root pair |
| Payment platform | **TL** | Transaction Lock | Holds trade payment (payer → recipient) | Recipient public pair (`pub/epub`) + payer unlock seed |
| Commission platform | **CL** | Commission Lock | Holds affiliate commission (payer → affiliate) | Affiliate public pair (`pub/epub`) + payer unlock seed |

**Notes:** 
- **FP** (buy orders only): Maker deposits before posting order → Taker verifies on-chain → funds transfer to TL/CL after matching
- Each trade creates exactly 2 lock wallets: 1 TL (payment) + 1 CL (commission, if affiliate exists)
- FP uses **maker's own root pair** (maker controls funds), TL/CL use the **recipient/affiliate public pair (`pub/epub`)** for watch-only derivation and the corresponding **root pair** for spending derivation later

### Trust Assumptions

- **P is trusted** to act honestly as arbiter (semi-centralized model)
- **No party other than P** can unilaterally move funds from platform before trade resolution
- **Both traders trust P** to resolve disputes fairly
- All parties have valid **ZEN key pairs** registered via WebAuthn

---

## 2. Cryptographic Primitives

Core primitives remain deterministic, but the implementation is now ZEN-native:

### 2.1 ECDH Shared Secret — `zen.secret`

`zen.secret(theirPublicKey, myKeyPair)` via ZEN.

**Properties:**
- **Symmetric:** `zen.secret(P.epub, M.pair) === zen.secret(M.epub, P.pair)`
- **Deterministic:** Same inputs → same output
- **Pair-scoped:** `secret_MP ≠ secret_TP`

```javascript
const secret_MP = await zen.secret(P.epub, M.pair)  // M computes
const secret_MP = await zen.secret(M.epub, P.pair)  // P computes (same result)
```

### 2.2 ZEN Deterministic secp256k1 Derivation

From `ZEN`:

```javascript
const root = await zen.pair(null, { seed: seedHex }) // full pair

const lockPub = await zen.pair(null, {
    pub: root.pub,
    epub: root.epub,
    seed: unlockSeed
}) // watch-only child public key

const lockPair = await zen.pair(null, {
    priv: root.priv,
    epriv: root.epriv,
    seed: unlockSeed
}) // full child pair
```

**Critical property:** Anyone with the published **public pair** (`pub/epub`) plus `unlockSeed` can derive the lock public key/address, but only the holder of the matching **root pair** can derive the lock private key and spend.

### 2.3 Ethers.js Transport Role

`ethers.js` remains in the stack only for:

- RPC providers
- chain reads (`getBalance`, `call`, `estimateGas`)
- transaction serialization/sign/send once a private key is already known
- address formatting / EVM transport adapters

`ethers.js` is **not** the source of AKAO's offline deterministic wallet derivation anymore.

### 2.4 SHA-256 — `sha256`

From `Utils/crypto.js`, returns 64-char hex (32 bytes).

---

## 3. Order Types and Lifecycle

### 3.1 Order Sides

**Buy Order:**
```javascript
{
    side: "buy",
    maker: {
        pub: "maker_pub_key",
        epub: "maker_epub_key"
    },
    base: {
        type: "item",
        id: "diablo-4/penitent-greaves-d76bc",
        quantity: 1
    },
    quote: {
        type: "crypto",
        quantity: 100,
        contract: "USDT",
        chain: 1
    }
    // No status field — status is derived from observable facts
    // No orderWallet field — FP address is computed by anyone from maker.epub + orderId
}
```

**Sell Order:**
```javascript
{
    side: "sell",
    maker: {
        pub: "maker_pub_key",
        epub: "maker_epub_key"
    },
    base: {
        type: "item",
        id: "diablo-4/penitent-greaves-d76bc",
        quantity: 1
    },
    quote: {
        type: "crypto",
        quantity: 95,
        contract: "USDT",
        chain: 1
    }
    // No status field — status is derived from observable facts
}
```

### 3.2 Trade Execution Flow

Trade state is never stored — it is always **derived** from verifiable facts in ZEN and on-chain:

| Derived State | Condition |
|---|---|
| **order:open** | Order node exists in ZEN soul AND (buy: `FP balance ≥ required` \| sell: always) |
| **order:expired** | Candle elapsed OR (buy: FP balance < required) |
| **trade:matched** | Trade node exists with both `maker` and `taker` keys |
| **trade:deposited** | `TL.address` on-chain balance ≥ paymentAmount |
| **trade:delivered** | `zen.get("~" + seller.pub).get("trades").get(tradeId).get("delivered")` is truthy |
| **trade:completed** | `zen.get("~" + buyer.pub).get("trades").get(tradeId).get("unlock_seed_TL")` exists |

```
1. Maker creates order → writes to the Pen soul compiled for the exact `(baseId, side, candle)` market window (no deposit)
2. Taker discovers order → compiles the current and previous `(baseId, side, candle)` souls, then scans both
3. Taker accepts order → trade node created with { orderId, maker, taker } → derived state: matched
4. Payer deposits to platform (Maker for buy, Taker for sell):
   - Computes TL/CL from recipient's public pair (`pub/epub`) + payer's unlock seed
   - Deposits within 10-min timeout or trade auto-cancels → derived state: deposited
5. In-game item delivery (trade/mail/drop) → seller writes delivered marker → derived state: delivered
6. Buyer confirms → reveals unlock seed to seller/affiliate → derived state: completed
7. Recipients withdraw using own root pair + revealed unlock seed
```

### 3.3 Key Derivation Strategy

**Changes from v1.0:**
- **No per-item platform roots** (items don't need separate published derivation trees for settlement)
- **Per-item, per-type, per-candle Pen souls** for discovery (`item + side + time window` define the soul identity)
- **One root per trader-platform pair**
- **Two platform wallets per trade:**
  - **Payment platform (TL)** — holds trade payment (M/P or T/P root)
  - **Affiliate platform (CL)** — holds affiliate commission (A/P root)

---

## 4. Key Derivation Specification

### 4.1 Notation

| Symbol | Description |
|---|---|
| `⊕` | String concatenation with `":"` separator |
| `sha256(x)` | SHA-256 hex (64 chars) |
| `rootPair(h)` | `await zen.pair(null, { seed: h })` |
| `lockPub(pair, s)` | `await zen.pair(null, { pub: pair.pub, epub: pair.epub, seed: s })` |
| `lockPair(pair, s)` | `await zen.pair(null, { priv: pair.priv, epriv: pair.epriv, seed: s })` |
| `unlockSeed(x)` | deterministic hex/base62 seed derived from `sha256(...)` |

### 4.2 Root Key for Each Trader/Affiliate

Each party derives a single root ZEN pair with Platform:

```javascript
secret_MP  =  zen.secret(P.epub, M.pair)
secret_TP  =  zen.secret(P.epub, T.pair)
secret_AP  =  zen.secret(P.epub, A.pair)

seed_MP    =  sha256(secret_MP)
seed_TP    =  sha256(secret_TP)
seed_AP    =  sha256(secret_AP)

root_MP    =  rootPair(seed_MP)    // full pair — known to M and P
root_TP    =  rootPair(seed_TP)    // full pair — known to T and P
root_AP    =  rootPair(seed_AP)    // full pair — known to A and P
```

**Each party exposes the public half of that root pair when needed:**
- Maker: `{ pub_MP, epub_MP } = { root_MP.pub, root_MP.epub }`
- Taker: `{ pub_TP, epub_TP } = { root_TP.pub, root_TP.epub }`
- Affiliate: `{ pub_AP, epub_AP } = { root_AP.pub, root_AP.epub }`

No separate `rootPub` field is required; `pub/epub` are enough.

### 4.3 Per-Transaction Lock Wallets (Dual Platform)

**Key principle in the ZEN design**: Platform lock wallet = **RECIPIENT's public pair (`pub/epub`)** + **UNLOCK SEED from PAYER's secret**

**Critical design decision**: Both buy and sell orders deposit **AFTER matching** to preserve pure P2P architecture (Platform not involved in happy path).

For each trade, derive **two platform wallets**:

#### 4.3.1 Transaction Lock (TL)

**Symmetric flow for both order types** (Maker always uses Taker's public pair after match):

```javascript
// AFTER MATCHING (both parties known)
// "TR:" domain separator + ":" field separators prevent hash space collision with orderId ("OR:")
tradeId = sha256("TR:" + orderId + ":" + M.pub + ":" + T.pub)

// Step 1: Recipient (Taker for buy order, Maker for sell order) exposes public pair
secret_recipient = zen.secret(P.epub, recipient.pair)
root_recipient = await zen.pair(null, { seed: sha256(secret_recipient) })
recipientPublic = { pub: root_recipient.pub, epub: root_recipient.epub }

// Step 2: Payer (Maker for buy order, Taker for sell order) computes unlock seed
// Domain separator ":TL:" isolates TL seed from CL seed — prevents cross-lock correlation
secret_payer = zen.secret(P.epub, payer.pair)
unlockSeed_TL = sha256(secret_payer + ":TL:" + tradeId)

// Step 3: Payer derives platform from RECIPIENT's public pair + PAYER's unlock seed
TL = await zen.pair(null, {
    pub: recipientPublic.pub,
    epub: recipientPublic.epub,
    seed: unlockSeed_TL
})

// TL.address — derived from TL.pub, payer deposits HERE (after matching)
// TL.privateKey — Payer doesn't have (only public pair, not root pair)
// Recipient needs unlockSeed_TL revealed to derive matching spending key
```

**Who is payer/recipient?**
- **Buy order**: Maker (buyer) is payer, Taker (seller) is recipient
- **Sell order**: Taker (buyer) is payer, Maker (seller) is recipient

#### 4.3.2 Commission Lock (CL)

**Same pattern - payer uses affiliate's public pair + payer's unlock seed:**

```javascript
// AFTER MATCHING (payer known)
tradeId = sha256("TR:" + orderId + ":" + M.pub + ":" + T.pub)

// Affiliate exposes public pair
secret_AP = zen.secret(P.epub, A.pair)
root_AP = await zen.pair(null, { seed: sha256(secret_AP) })
affiliatePublic = { pub: root_AP.pub, epub: root_AP.epub }

// Payer computes unlock seed from own secret
// Domain separator ":CL:" isolates CL seed from TL seed — prevents cross-lock correlation
secret_payer = zen.secret(P.epub, payer.pair)
unlockSeed_CL = sha256(secret_payer + ":CL:" + tradeId)

// Payer derives from AFFILIATE's public pair + PAYER's unlock seed
CL = await zen.pair(null, {
    pub: affiliatePublic.pub,
    epub: affiliatePublic.epub,
    seed: unlockSeed_CL
})

// CL.address — payer deposits commission here (after matching)
// CL.privateKey — Affiliate needs unlockSeed_CL revealed to spend
```

**Security properties**:
- Payer derives platform address from **recipient's public pair** (watch-only)
- Payer uses unlock seed from **own secret**
- Payer **cannot spend** (doesn't have recipient's root pair, only the public pair)
- Recipient **cannot spend yet** (doesn't know unlock seed)
- Platform **can always spend** (knows all secrets → can compute all unlock seeds + reconstruct all root pairs)

**Who pays (AFTER matching):**
- **Buy order:** Maker deposits to `TL` + `CL` (both use Taker's/Affiliate's public pair + Maker's unlock seed)
- **Sell order:** Taker deposits to `TL` + `CL` (both use Maker's/Affiliate's public pair + Taker's unlock seed)

**When deposit happens:**
- **Both order types**: Deposit occurs AFTER matching (when both parties known)
- No pre-deposit required → Platform not involved in happy path → pure P2P

**Access control:**
- **Payer (M or T):** Computes platform addresses from recipient's public pair + own unlock seed. **Cannot spend** (has `pub/epub` only, not the root pair).
- **Recipient (seller):** 
  - Buy order: Taker receives `unlockSeed_TL` from Maker → derives spending key from own `root_TP`
  - Sell order: Maker receives `unlockSeed_TL` from Taker → derives spending key from own `root_MP`
- **Affiliate (A):** Receives `unlockSeed_CL` from payer (or Platform) → derives spending key from own `root_AP`
- **Platform (P):** Knows all secrets → can compute all unlock seeds + derive all spending keys (arbitration power)

**Unlock mechanism**:
```
BOTH ORDER TYPES FOLLOW SAME FLOW (symmetric):

1. Maker posts order (no deposit)
2. Taker accepts → tradeId created
3. Payer computes TL/CL from Recipient's public pair + Payer's unlock seed
4. Payer deposits to TL.address + CL.address (within deposit timeout)
5. Seller delivers item
6. Buyer confirms receipt → reveals `unlockSeed_TL` and `unlockSeed_CL`
7. Recipient uses own root pair + revealed unlock seed → derives spending key → withdraws
```

**Why payer cannot withdraw**:
- Payer only has recipient's **public pair** (`pub/epub`, watch-only)
- Even though payer knows the **unlock seed**, cannot derive private key without the root pair
- ZEN public derivation: `pub/epub + unlockSeed → lock pub/address` ✅
- But: `pub/epub + unlockSeed → lock private key` ❌ (impossible without root pair)

**Why recipient cannot withdraw early**:
- Recipient has **root pair** (own wallet)
- But doesn't know which **unlock seed** was used (payer's secret + domain separator + tradeId)
- Without unlock seed, recipient cannot derive the matching lock pair

**Security mindset preserved**: Platform wallet = recipient's public pair + payer's unlock seed

---

## 5. Protocol Walkthrough

### Step 1 — Maker Creates Order

**Both order types follow same pattern** (symmetric design):

1. M creates order on UI (buy or sell)
2. M generates `orderId = sha256("OR:" + canonicalOrderIntent + ":" + timestamp + ":" + randomness)`
3. M retrieves affiliate (if any) from localStorage: `Context.getReferrer()`
4. M compiles the Pen soul for the exact `(baseId, side, candle)` window, then writes under a Pen-validated key:
   ```
   soul params: { baseId: <base_item_id>, side: <side>, candle: <candle> }
   key:         <timestamp_ms>:<pub>:<nonce>

   Example soul params: { baseId: "penitent-greaves", side: "buy", candle: 5820000 }
   Example key:         1744440123456:maker_full_pub:x8k2m1
   ```
   `pub` is the maker's full public key, not a truncated shard
5. Order includes metadata: `{ orderId, maker, side, base, quote, affiliate }`
   (no status field — open state is derived from the node's existence + FP balance for buy orders)

**No pre-deposit** for either order type → pure P2P (Platform not involved)

### Step 1.5 — Order Wallet (Proof of Funds) for Buy Orders

**Problem**: Takers waste time accepting buy orders from makers who don't actually have funds.

**Solution**: Buy order makers deposit funds into a **deterministic order wallet** before creating the order. This provides **trustless proof of budget** without locking funds in platform yet.

**Order Wallet Derivation** (buy orders only):

```javascript
// Maker derives order wallet from own root pair + orderId
const secret_M = await zen.secret(P.epub, M.pair)
const root_M = await zen.pair(null, { seed: sha256(secret_M) })

// Order-specific unlock seed — "FP:" domain separator isolates FP address space from TL/CL
const unlockSeed_FP = sha256("FP:" + orderId)

// Fund Proof (Maker controls full pair)
const orderWallet = await zen.pair(null, {
    priv: root_M.priv,
    epriv: root_M.epriv,
    seed: unlockSeed_FP
})

// Maker deposits BEFORE posting order
await M.wallet.sendTransaction({
    to: orderWallet.address,
    value: totalAmount  // payment + commission + platform fee
})
```

**Order creation flow (buy orders)**:
1. Maker computes FP address from own root pair + `unlockSeed_FP`
2. Maker deposits full amount to FP address
3. Maker posts order to ZEN with `{ orderId, maker, side, base, quote, affiliate }` — no address stored
4. Taker derives FP address independently from `maker.epub + orderId`, then verifies on-chain balance:
   ```javascript
   const unlockSeed_FP = sha256("FP:" + order.orderId)
   const FP = await zen.pair(null, { pub: makerPublic.pub, epub: makerPublic.epub, seed: unlockSeed_FP })
   const balance = await chain.getBalance(FP.address)
   if (balance < requiredAmount) { UI.error("Maker has insufficient funds"); return }
   ```

**After matching** (Taker accepts):
- Maker re-derives FP wallet (full pair) and transfers FP → Transaction Lock (TL) + Commission Lock (CL)
- ⚠️ **Not atomic**: these are sequential EVM transactions (TL tx, CL tx, platform fee tx). If any fails mid-way, trade is in partial state. Implementation must check each receipt and handle partial completion.
- Gas cost: up to 3 transactions (TL + CL + platform fee)

**Benefits**:
- ✅ Taker can verify funds exist on-chain before accepting (derives FP address independently — no trust required)
- ✅ Trustless proof (deterministic derivation + on-chain balance query)
- ✅ Maker retains full control of funds in FP wallet (can cancel order and withdraw anytime)
- ✅ No Platform involvement until trade is matched

**Cancellation**:
- Maker withdraws from FP wallet back to main wallet using own private key
- Order auto-expires when FP balance drops below required amount

**Sell orders**: No order wallet needed (seller proves ownership by delivering item, not by depositing funds upfront)

---

### Step 2 — Taker Discovers and Accepts Order

1. T compiles the exact market-window souls in ZEN:
    ```javascript
    const current = Order.soul({ baseId: "penitent-greaves", side: "buy", candle })
    const previous = Order.soul({ baseId: "penitent-greaves", side: "buy", candle: candle - 1 })
    ```
2. T scans both souls:
    ```javascript
    zen.get(current).map()
    zen.get(previous).map()
    ```
3. T sees M's order
    - **For buy orders**: T derives FP address from `maker.epub + orderId` and verifies on-chain balance:
      ```javascript
      const unlockSeed_FP = sha256("FP:" + order.orderId)
      const FP = await zen.pair(null, { pub: makerPublic.pub, epub: makerPublic.epub, seed: unlockSeed_FP })
      const balance = await chain.getBalance(FP.address)
     if (balance < requiredAmount) {
         UI.error("Maker has insufficient funds")
         return
     }
     ```
   - **For sell orders**: No verification needed (seller will prove ownership by delivery)
4. T clicks "Accept"
5. **Trade matching** (creates `tradeId`):
   ```javascript
   // tradeId computed once at match time, stored in ZEN trade record — never recomputed
   // "TR:" domain separator prevents hash space collision with orderId ("OR:")
   tradeId = sha256("TR:" + orderId + ":" + M.pub + ":" + T.pub)
   ```
6. Trade node written to ZEN: `{ orderId, maker: { pub, epub }, taker: { pub, epub } }` → derived state: **matched**
   (no status field — matched state is derived from the presence of both `maker` and `taker` in the trade node)

### Step 3 — Payer Deposits to Platform

**Payer** (Maker for buy order, Taker for sell order) now computes platform addresses:

```javascript
// Determine payer and recipient based on order type
const payer = (orderType === 'buy') ? Maker : Taker
const recipient = (orderType === 'buy') ? Taker : Maker

// Recipient public pair is already known at match time
const recipientPublic = { pub: recipient.pub, epub: recipient.epub }

// Resolve affiliate public pair from signed profile data
// Never fetch public derivation material from an arbitrary path — tampering would redirect funds
const referrer = payer.getReferrer()  // returns affiliate's pub key
const affiliatePublic = referrer ? await Profile.publicPair(referrer) : null

// Compute payer's unlock seeds — separate domain separators prevent TL/CL address correlation
const secret_payer = await zen.secret(P.epub, payer.pair)
const unlockSeed_TL = sha256(secret_payer + ":TL:" + tradeId)
const unlockSeed_CL = sha256(secret_payer + ":CL:" + tradeId)

// Derive Transaction Lock from recipient's public pair + payer's TL unlock seed
const TL = await zen.pair(null, {
    pub: recipientPublic.pub,
    epub: recipientPublic.epub,
    seed: unlockSeed_TL
})

// Derive Commission Lock (if affiliate exists)
const CL = referrer ?
    await zen.pair(null, {
        pub: affiliatePublic.pub,
        epub: affiliatePublic.epub,
        seed: unlockSeed_CL
    }) :
    null

// Payer deposits to platform locks
// For buy orders: Transfer from FP wallet → TL/CL (sequential, NOT atomic — check each tx receipt)
// For sell orders: Direct deposit from Taker's wallet → TL/CL

if (orderType === 'buy') {
    // Maker re-derives FP wallet (full pair — Maker has own root)
    const unlockSeed_FP = sha256("FP:" + orderId)
    const FP = await zen.pair(null, {
        priv: M.root.priv,
        epriv: M.root.epriv,
        seed: unlockSeed_FP
    })  // Maker has full pair — can spend
    
    await FP.sendTransaction({ to: TL.address, value: paymentAmount })
    if (CL) {
        await FP.sendTransaction({ to: CL.address, value: commissionAmount })
    }
    await FP.sendTransaction({ 
        to: Platform.operationalWallet, 
        value: platformFee 
    })
} else {
    // Taker deposits directly to platform locks
    await T.wallet.sendTransaction({ to: TL.address, value: paymentAmount })
    if (CL) {
        await T.wallet.sendTransaction({ to: CL.address, value: commissionAmount })
    }
    await T.wallet.sendTransaction({ 
        to: Platform.operationalWallet, 
        value: platformFee 
    })
}
```

**Deposit timeout**: If payer doesn't deposit within 10 minutes → trade auto-cancels → order becomes matchable again

**After confirmations**: `TL.address` on-chain balance ≥ paymentAmount → derived state: **deposited**

### Step 4 — Item Delivery

**Seller** (Taker for buy order, Maker for sell order) delivers in-game:
- Trade window in-game
- In-game mail system
- Drop/pickup coordination

**Seller marks:** "Delivered" in UI → writes `delivered: true` to own ZEN namespace:
```javascript
zen.get("~" + seller.pub).get("trades").get(tradeId).put({ delivered: true }, null, { authenticator: seller.pair })
// zen.get("~" + seller.pub).get("trades").get(tradeId).get("delivered") is truthy → derived state: delivered
```

### Step 5 — Buyer Confirms Receipt and Unlocks Payment

**Buyer** (Maker for buy order, Taker for sell order) marks "Received" in UI

**Buyer reveals unlock seed to seller** (symmetric for both order types):

```javascript
// Buyer computes TL and CL unlock seeds from own secret — domain separators match deposit step
const secret_buyer = await zen.secret(P.epub, buyer.pair)
const unlockSeed_TL = sha256(secret_buyer + ":TL:" + tradeId)
const unlockSeed_CL = sha256(secret_buyer + ":CL:" + tradeId)

// Buyer writes to OWN signed ZEN user namespace
// Seller subscribes to zen.get("~" + buyer.pub).get("trades").get(tradeId)
zen.get("~" + buyer.pub).get("trades").get(tradeId).put({
    unlock_seed_TL: unlockSeed_TL,
    unlock_seed_CL: unlockSeed_CL,
    confirmed: true
}, null, { authenticator: buyer.pair })

// Seller receives unlock seed and unlocks using OWN root
const secret_seller = await zen.secret(P.epub, seller.pair)
const root_seller = await zen.pair(null, { seed: sha256(secret_seller) })
const TL = await zen.pair(null, {
    priv: root_seller.priv,
    epriv: root_seller.epriv,
    seed: unlockSeed_TL
})  // Now has private key!

// Seller self-releases payment
await TL.sendTransaction({
    to: seller.wallet,
    value: paymentAmount
})

// Affiliate (if exists) self-releases commission using CL unlock seed (not TL unlock seed)
if (affiliate) {
    const secret_affiliate = await zen.secret(P.epub, affiliate.pair)
    const root_affiliate = await zen.pair(null, { seed: sha256(secret_affiliate) })
    const CL = await zen.pair(null, {
        priv: root_affiliate.priv,
        epriv: root_affiliate.epriv,
        seed: unlockSeed_CL
    })
    
    await CL.sendTransaction({
        to: affiliate.wallet,
        value: commissionAmount
    })
}
```

`zen.get("~" + buyer.pub).get("trades").get(tradeId).get("unlock_seed_TL")` exists → derived state: **completed**

**OR via Platform auto-release:**
- If buyer doesn't confirm within 24h → P computes unlock seeds and releases to seller + affiliate

### Step 6 — Dispute Resolution and Refunds by P

**Critical limitation**: Refunds REQUIRE Platform involvement due to platform design.

**Why payer cannot self-refund:**
```javascript
// Platform = Recipient's public pair + Payer's unlock seed
TL = await zen.pair(null, {
    pub: recipientPublic.pub,
    epub: recipientPublic.epub,
    seed: unlockSeed_TL
})

// Payer has:
// - Recipient's public pair (watch-only, public)
// - Own unlock seed (secret)
// BUT: Cannot derive private key from public data alone!

// Only recipient has root pair → only recipient can spend
// Payer is cryptographically locked out (by design)
```

**This means:**
- ✅ Happy path (delivery + confirmation): Pure P2P, Platform not involved
- ⚠️ **Unhappy path (timeouts/disputes): Platform MUST intervene to refund**
- This is **not** centralization — Platform acts as trusted arbiter (per Section 1.1)

**Platform-initiated refund mechanism:**

P can always recompute both platform spending keys:

```javascript
// P knows all parties' epub keys and can reconstruct their root pairs
const secret_recipient = await zen.secret(recipient.epub, P.pair)
const root_recipient = await zen.pair(null, { seed: sha256(secret_recipient) })

// P recomputes payer's TL and CL unlock seeds using domain separators
const secret_payer = await zen.secret(payer.epub, P.pair)
const unlockSeed_TL = sha256(secret_payer + ":TL:" + tradeId)
const unlockSeed_CL = sha256(secret_payer + ":CL:" + tradeId)

// P derives both platform spending keys
const TL = await zen.pair(null, {
    priv: root_recipient.priv,
    epriv: root_recipient.epriv,
    seed: unlockSeed_TL
})
// TL.privateKey → P can release to seller OR refund to payer

// Same for affiliate lock
const secret_affiliate = await zen.secret(affiliate.epub, P.pair)
const root_affiliate = await zen.pair(null, { seed: sha256(secret_affiliate) })
const CL = await zen.pair(null, {
    priv: root_affiliate.priv,
    epriv: root_affiliate.epriv,
    seed: unlockSeed_CL
})
// CL.privateKey → P can release to affiliate OR refund to payer
```

**Refund triggers** (Platform monitors for these conditions):

1. **Payer requests refund** (after timeout)
   - Payer submits refund request via UI
   - Platform verifies timeout conditions met
   - Platform executes refund

2. **Automatic timeout** (optional - Platform worker)
   - Platform's `update.js` thread polls trades
   - If trade TL balance ≥ required AND seller delivered marker absent >24h
   - Platform auto-refunds to payer

3. **Dispute resolution**
   - Either party files dispute via UI
   - Platform investigates (chat logs, screenshots, etc.)
   - Platform decides: refund to payer OR release to seller

**Resolution matrix:**

| Scenario | TL (Payment) Action | CL (Commission) Action |
|---|---|---|
| Trade completed successfully | Buyer reveals unlock seed → Seller withdraws | Buyer reveals unlock seed → Affiliate withdraws |
| Buyer disputes, seller proven wrong | Platform refunds to payer (transfers) | Platform refunds to payer (transfers) |
| Buyer disputes, seller proven right | Platform reveals unlock seed to seller → Seller withdraws | Platform reveals unlock seed to affiliate → Affiliate withdraws |
| Both parties inactive 7+ days | Platform refunds to payer (transfers) | Platform refunds to payer (transfers) |
| Auto-release (24h no confirmation) | Platform reveals unlock seed to seller → Seller withdraws | Platform reveals unlock seed to affiliate → Affiliate withdraws |

**Note**: Platform can either:
- **Reveal unlock seed** (seller/affiliate self-withdraws) — preferred for transparency
- **Transfer directly** (Platform executes withdrawal) — faster but less transparent

---

## 6. Order Book with Pen DSL

### 6.1 Temporal Validation

Orders use **parameterized Pen souls** and **timestamp keys**:

```javascript
const orderSoul = ZEN.pen({
    key: { and: [
        // seg 0: timestamp_ms whose derived candle must equal the soul candle
        {
            let: {
                bind: 0,
                def: { divu: [{ tonum: { seg: { sep: ":", idx: 0, of: { reg: 0 } } } }, 300000] },
                body: { and: [
                    { gte: [{ reg: 128 }, candle] },
                    { lte: [{ reg: 128 }, candle] }
                ]}
            }
        },
        // seg 1: full writer pub must equal R[5]
        { eq: [
            { seg: { sep: ":", idx: 1, of: { reg: 0 } } },
            { reg: 5 }
        ]},
        // key: no nonce segment — nonce travels in msg.put["^"] (R[7])
        { eq: [
            { seg: { sep: ":", idx: 3, of: { reg: 0 } } },
            { reg: 5 }
        ]},
    ]},
    val: { type: "string" },  // JSON order metadata (orderId, maker, side, base, quote, affiliate, etc.)
    sign: true,
    pow: { field: 7, difficulty: 3 },  // Anti-spam PoW — nonce from msg.put["^"]
    params: { baseId, side, candle }
})
```

**Important partitioning rule**:
- **Soul identity carries market window**: `params = { baseId, side, candle }`
- **Key carries order instance identity**: `<candle>:<base>:<side>:<pub>` — no nonce in key
- **Nonce** travels in `msg.put["^"]` (R[7]), bound to key via `hash(key+":"+nonce)`
- **Read path starts from the right soul**, not from a global order bucket

### 6.2 Key Format

```text
soul params: { baseId: <base_item_id>, side: <side>, candle: <candle> }
key:         <candle>:<base>:<side>:<pub>

Example params: { baseId: "penitent-greaves", side: "buy", candle: 5820000 }
Example key:    5820000:penitent-greaves:buy:maker_full_pub
```

| Segment | Content | Validated by |
|---|---|---|
| soul | `params = { baseId, side, candle }` | Compile-time soul identity via `ZEN.pen({ params })` |
| seg 0 | `candle = floor(Date.now()/300000)` | `candle === expected candle` |
| seg 1 | `baseId` | Static equality in soul predicate |
| seg 2 | `side` | Static equality in soul predicate |
| seg 3 | `pub = M.pub` | Dynamic equality with writer register `R[5]` |
| `msg.put["^"]` | `nonce` — mined as `hash(key+":"+nonce)` starts with "000" | PoW `difficulty: 3` via R[7] |

**orderId** is stored in the value (not the key), enabling:
- Stable identity independent of timestamp/nonce
- Price changes without key recreation
- Matching/cancellation by key while market window stays derivable from timestamp

**Candle calculation:**
```javascript
const candle = Math.floor(Date.now() / 300000)  // 5-minute candles

// Full key construction inside soul({ baseId, side, candle }):
const timestamp = Date.now()
const pub = pair.pub
const key = `${candle}:${baseId}:${side}:${pair.pub}`
const { nonce } = await ZEN.hash(key, null, null,
  { name: 'SHA-256', encode: 'hex', pow: { difficulty: 3 } })
// nonce passes to pen via opt.pow — written as msg.put["^"] on the wire
```

**Benefits:**
- Soul identity is exact to one market window
- Full pub is stronger than `pub8` for ownership and cancellation checks
- Discovery queries start from exact baseId/side/candle souls
- No manual cleanup needed
- Market noise from other items, sides, or candles does not pollute discovery

---

### 6.3 Trade Record Soul

Trade records contain sensitive data (`unlock_seed_TL`, `unlock_seed_CL`). No shared Pen soul is appropriate. Each party writes to their **own signed ZEN user namespace**, keyed by `tradeId`.

**Two write targets per trade:**

```javascript
zen.get("~" + M.pub).get("trades").get(tradeId)   // Maker writes here — authenticated namespace rejects other writers
zen.get("~" + T.pub).get("trades").get(tradeId)   // Taker writes here — authenticated namespace rejects other writers
```

ZEN signatures enforce write restriction — **no Pen needed**.

**Who writes what:**

| Field | Writer | Namespace | When |
|---|---|---|---|
| `matched` (timestamp) | Taker | Taker's | Step 2 — at accept |
| `deposited` (txHash) | Payer (M or T) | Own | Step 3 — after deposit confirmed |
| `delivered` (true) | Seller (M or T) | Own | Step 4 — item delivered |
| `unlock_seed_TL` (string) | Buyer (M or T) | Own | Step 5 — at confirmation |
| `unlock_seed_CL` (string) | Buyer (M or T) | Own | Step 5 — at confirmation |
| `confirmed` (true) | Buyer (M or T) | Own | Step 5 — receipt confirmed |
| `disputed` (object) | Either party | Own | Step 6 — if dispute filed |
| `release_ready` / `refund_ready` | Platform | Platform's | Step 6 — platform has resolved deterministic spend paths but has not settled on-chain yet |

**Key security property**: `unlock_seed_TL` and `unlock_seed_CL` live in the **buyer's own namespace**. Seller must actively read buyer's namespace to obtain the unlock seed. Buyer controls when seller can withdraw — no premature self-release is possible.

**Read pattern — both parties subscribe to both namespaces:**

```javascript
zen.get("~" + M.pub).get("trades").get(tradeId).on(makerData => merge(makerData))
zen.get("~" + T.pub).get("trades").get(tradeId).on(takerData => merge(takerData))
```

**`Trade.js` soul mapping:**

```javascript
trade.id()     →  tradeId                               // content hash — used in crypto derivation
trade.soul()   →  { maker: M.pub, taker: T.pub }        // keys to compose ZEN paths

// Write to own namespace (caller authenticated as writer)
zen.get("~" + own.pub).get("trades").get(tradeId).put(fields, null, { authenticator: own.pair })

// Read from both namespaces
zen.get("~" + M.pub).get("trades").get(tradeId)              // Maker's side
zen.get("~" + T.pub).get("trades").get(tradeId)              // Taker's side
```

**Platform access**: Platform can read both namespaces (they're public signed ZEN user data). Platform writes to trade records via the trade participant's shared secret — Platform knows all parties' epub keys and can compute all secrets.

---

## 7. Security Analysis

### 7.1 The Refund Limitation

**Critical design constraint**: Payer cannot self-refund in the current platform model.

**Why this limitation exists:**

The platform security relies on **asymmetric key access**:
```
Platform = Recipient's public pair (`pub/epub`, watch-only) + Payer's unlock seed (secret)

Payer has:
├─ Recipient's public pair (`pub/epub`)
└─ Own unlock seed (secret)

Recipient has:
├─ Own root pair
└─ Unknown unlock seed (until reveal)

Platform has:
├─ Recipient's root pair (via shared secret)
└─ Payer's unlock seed (via shared secret)
```

**ZEN cryptographic property**:
- `pub/epub + unlockSeed → child public key/address` ✅ (anyone can compute)
- `rootPair + unlockSeed → child full spending pair` ✅ (only root-pair holder can derive)
- `pub/epub + unlockSeed → child private key` ❌ **IMPOSSIBLE** (by design)

**This means:**
- ✅ **Happy path**: Payer deposits → Recipient delivers → Payer reveals unlock seed → Recipient withdraws (pure P2P, Platform not involved)
- ⚠️ **Unhappy path**: Payer deposits → Recipient ghosts → Payer wants refund → **Platform MUST intervene** (Payer cannot derive private key from `pub/epub` alone)

**Trade-off accepted:**
- Security against payer withdrawal (prevents rug pulls)
- Cost: Refunds require Platform arbitration

**Alternative (not implemented):**
- Use smart contracts with time-locks (enables self-refund after timeout)
- Cost: Adds smart contract dependency, gas fees, complexity

**Current design philosophy**: Accept Platform as trusted arbiter for exceptional cases (disputes, timeouts) to maintain pure P2P for normal trades.

### 7.2 Access Control Guarantees

| Property | Mechanism |
|---|---|
| P holds full platform authority | P knows all **epub** keys → recomputes all DH secrets via `zen.secret(party.epub, P.pair)` → derives all root pairs and child spending keys for both TL and CL |
| Payer cannot withdraw (even for refund) | Payer has recipient's `pub/epub` only (watch-only). Cannot derive private key from public data. **Refunds require Platform intervention.** |
| Recipient cannot claim early | Recipient has root pair but doesn't know unlock seed until payer reveals it |
| Affiliate cannot claim prematurely | A has affiliate `pub/epub` but not `unlockSeed_CL`; receives unlock seed only after successful trade completion |
| No third-party involvement (happy path) | All derivations local and deterministic. Platform only involved for disputes/refunds. |

### 7.3 Per-Trade Address Isolation

Each `tradeId` is unique → `unlockSeed_TL` and `unlockSeed_CL` unique → platform addresses never collide across trades.

Even if same affiliate refers multiple trades, each trade gets unique CL wallet (different `tradeId` → different `unlockSeed_CL`).

### 7.4 Order Book Spam Prevention

**Pen DSL enforces:**
- **Signature requirement** → only authenticated users
- **PoW (difficulty 2)** → CPU cost to create order
- **Temporal window** → old orders auto-expire

**ZEN graph layer enforces:**
- **LEX prefix matching** → efficient discovery queries
- **P2P sync** → distributed order book

---

## 8. Risk Register

| Risk | Severity | Mitigation |
|---|---|---|
| `zen.secret` non-determinism | Critical | Test suite verification |
| ZEN not initialized | High | Guard all operations; wait for `Construct.ZEN()` / `Construct.GDB()` |
| Unlock-seed collision | Low | Domain separators (`FP`, `TL`, `CL`) + `tradeId` / `orderId` hashing |
| Public-pair tampering in ZEN profile | Medium | `pub/epub` must come from the signed user identity/profile. Platform must validate the resolved public pair matches the intended recipient before allowing deposit phase. |
| Crafted/wrong public pair by recipient | **High** | Recipient could present `pub/epub` that do not correspond to the spending pair expected by the protocol → funds permanently locked in TL. Platform and clients must verify the resolved public pair belongs to the intended identity before deposit. |
| Order ID collision | Very low | `sha256(orderId ":" M.pub ":" T.pub ":" timestamp)` |
| Order overwrite attack | High | Pen `sign:true` allows any authenticated user to overwrite order keys at PoW cost ~256 attempts. Mitigate: embed maker pub prefix in key and validate `EQ(SEGR(0,':',4), R[5])` in Pen — or move orders to `zen.get("~" + pub)` namespace. |
| FP race condition | High | Maker can drain FP wallet between Taker accepting and payer depositing to TL/CL. Design limitation of non-smart-contract platform. Taker should re-verify FP balance at matching time, not just at discovery time. |
| Partial FP→TL/CL transfer | High | Sequential EVM transactions mean TL could be funded but CL fails. Trade contract must check all receipts; implementation needs a recovery path (refund from partial TL if CL fails). |
| Commission fate with no affiliate | Medium | When referrer is null, `commissionAmount` is unaccounted for in current protocol. Must define: goes to platform fee, added to payment, or simply not charged. |
| Child key leakage | Medium | Spending keys (TL, CL) must be wiped from memory immediately after `sendTransaction()`. Never log or store them. |
| P key compromise | Critical | Operational security (outside protocol scope) |
| Payer cannot self-refund | **Design limitation** | **Platform MUST intervene for refunds.** Payer only has recipient's `pub/epub` (watch-only), cannot derive private key. This is by design — same mechanism that prevents payer from withdrawing prevents self-refund. |
| Platform unavailable (refunds blocked) | Medium | Multi-sig Platform keys, backup arbitrators, or smart contract fallback (future) |
| In-game item delivery fraud | Medium | Dispute resolution by P with proof requirements |
| Candle drift (client time skew) | Low | Accept ±2 candles forward, ±100 backward |
| Trade record soul | ✅ Resolved | Dual user-namespace model (Section 6.3). Each party writes to `zen.get("~" + own.pub).get("trades").get(tradeId)`. ZEN signatures enforce authorship. No Pen needed. |
| Order soul overwrite | ✅ Resolved in direction | Full writer pub now lives in key seg 1, and soul identity is parameterized by `{ baseId, side, candle }` via `ZEN.pen({ params })` (Section 6.1). |

---

## 9. Out of Scope

- **Fiat payments** (handled by platform account balances)
- **Smart contract enforcement** (intentionally avoided)
- **Multi-signature** (future revision candidate)
- **Cross-chain atomic swaps** (single-chain trades only)
- **In-game API integration** (manual delivery via trade/mail)

---

## 10. Implementation Checklist

- [ ] `src/core/Lock.js` — platform key derivation primitives (TL + CL, `unlockSeed()`, `address()`, `unlock()`)
- [ ] `src/core/Order.js` — ZEN order CRUD helpers
- [ ] `src/core/Affiliate.js` — referral tracking + commission calculation
- [ ] `src/UI/routes/order/` — order book UI + trade flow
- [ ] Pen soul definition for orders
- [ ] Per-baseId + per-side ZEN discovery queries with candle range scanning
- [ ] Auto-release 24h timer (both TL and CL)
- [ ] Affiliate unlock-seed release mechanism (via ZEN)
- [ ] Dispute UI at `/dispute`
- [ ] Test suite: dual platform address derivation consistency
- [ ] Test suite: `zen.secret` determinism
- [ ] Test suite: candle window validation
- [ ] Test suite: affiliate commission calculation

---

*2026-04-04 — Revised for P2P trading model*  
*2026-04-06 — Resolved: No pre-deposit for buy orders (pure P2P, symmetric design)*  
*2026-04-08 — Security audit: epub/pub fix, TL/CL domain separators, tradeId separators, FP domain separator, atomic transition caveat, order overwrite risk, crafted public-pair risk, child key leakage note, commission gap, trade record soul gap*  
*2026-04-15 — ZEN rewrite: offline crypto/computing moved fully to ZEN; Gun/SEA and Ethers HD-wallet assumptions removed; fake/crafted `xpub` concern eliminated because `pub/epub` are sufficient; Ethers.js retained only for RPC transport*
