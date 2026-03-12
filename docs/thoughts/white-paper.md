# Four-Party Escrow Protocol

**Version** 1.0 · **Status** Draft · **Date** 2026-03-12

---

## Abstract

This document specifies a trustless, serverless escrow protocol for four-party commerce transactions. The protocol enables a marketplace operator to mediate payments between buyers, sellers, and affiliates using deterministic cryptographic key derivation — eliminating the need for smart contracts, custodial accounts, or trusted third-party oracles. All escrow wallet addresses are derived client-side from Diffie-Hellman shared secrets and BIP-32 hierarchical deterministic key derivation, ensuring that each party holds exactly the access rights the protocol intends, and no more.

---

## 1. Roles and Trust Model

| Role | Symbol | Description |
|---|---|---|
| Escrow / Platform | **E** | Marketplace operator. Holds the authority to release or refund escrow funds. Acts as the trusted arbiter in disputes. |
| Seller | **S** | Lists items on the platform. Receives payment upon successful order fulfilment. |
| Buyer | **B** | Purchases items. Initiates payment into escrow. |
| Affiliate | **A** | Refers buyers to the platform. Earns commission upon successful order. |

### Trust assumptions

- E is trusted to act honestly as arbiter. This is a **semi-centralised** model: custody of funds is enforced cryptographically, but dispute resolution depends on E's judgement.
- No party other than E can unilaterally move funds from an escrow wallet prior to order resolution.
- All parties are assumed to have valid SEA key pairs registered with the platform.

---

## 2. Cryptographic Primitives

The protocol is built entirely on primitives already present in the platform's technology stack. No additional dependencies are required.

### 2.1 ECDH Shared Secret — `sea.secret`

`sea.secret(theirPublicKey, myKeyPair)` implements Elliptic-Curve Diffie-Hellman (ECDH) over the Curve25519 as provided by the GunDB SEA library. It produces a shared secret known exclusively to the two parties whose keys are used.

**Key properties:**
- **Symmetric** — both parties arrive at the same secret independently: `sea.secret(E.pub, S.pair) === sea.secret(S.pub, E.pair)`
- **Deterministic** — identical inputs always produce identical outputs
- **Pair-scoped** — `sea.secret(E.pub, S.pair) ≠ sea.secret(E.pub, A.pair)`, so each bilateral relationship yields a distinct secret

```javascript
// Both calls produce the same secret — only S and E can compute it
const secret_SE = await sea.secret(E.pub, S.pair)   // computed by S
const secret_SE = await sea.secret(S.pub, E.pair)   // computed by E
```

### 2.2 BIP-32 Hierarchical Deterministic Wallets — `HDNodeWallet`

The `ethers.js` library (bundled in the platform) provides a full BIP-32 implementation via `HDNodeWallet` and `HDNodeVoidWallet`.

**Key operations used by this protocol:**

```javascript
import { HDNodeWallet, getBytes } from "/core/Ethers.js"

// Derive a root HD node from a 32-byte seed
const root = HDNodeWallet.fromSeed(getBytes("0x" + seedHex))

// Serialise to extended key strings
root.extendedKey              // xprv — full node, contains private key
root.neuter().extendedKey     // xpub — public node, no private key

// Deserialise
HDNodeWallet.fromExtendedKey("xprv...")  // => HDNodeWallet  (spending)
HDNodeWallet.fromExtendedKey("xpub...")  // => HDNodeVoidWallet (watch-only)

// Derive a non-hardened child at a given index (0 – 2³¹ − 1)
const child = root.deriveChild(index)
child.address      // EVM address
child.privateKey   // spending key — only present on full HDNodeWallet
```

**The non-hardened property is fundamental to this protocol.** It allows any party holding only an `xpub` to independently derive the same child address that the holder of the corresponding `xprv` would derive. This is what enables buyers to trustlessly verify escrow addresses without ever having access to the spending key.

### 2.3 SHA-256 — `sha256`

`sha256(str)` from `Utils/crypto.js` returns a 64-character hexadecimal string (32 bytes). It is used throughout the protocol to derive deterministic seeds from concatenated secrets, ensuring domain separation between different key hierarchies.

---

## 3. Key Derivation Specification

### 3.1 Notation

| Symbol | Description |
|---|---|
| `⊕` | String concatenation with `":"` separator |
| `sha256(x)` | SHA-256 of string `x`, returned as 64-char hex |
| `fromSeed(h)` | `HDNodeWallet.fromSeed(getBytes("0x" + h))` |
| `index(h)` | `parseInt(h.substring(0, 8), 16) & 0x7fffffff` — 31-bit non-hardened index derived from hash |

### 3.2 Per-Item Root Keys

For each listed item, two independent HD root key pairs are derived — one scoped to the S/E relationship, one to the A/E relationship. The `itemId` acts as a domain separator ensuring root keys are unique per item.

```
secret_SE  =  sea.secret(E.pub, S.pair)
secret_AE  =  sea.secret(E.pub, A.pair)

seed_SE    =  sha256(secret_SE ⊕ itemId)
seed_AE    =  sha256(secret_AE ⊕ itemId)

root_SE    =  fromSeed(seed_SE)    // full node — known to S and E
root_AE    =  fromSeed(seed_AE)    // full node — known to A and E
```

The seller publishes `root_SE.neuter().extendedKey` (i.e. `xpub_SE`) to the platform's public data layer. The private root `xprv_SE` is never transmitted.

### 3.3 Per-Order Child Keys (Escrow Wallets)

For each order, a buyer-specific index is derived from the B/E shared secret and the `orderId`. This index is used to derive a unique child address from each item's root key.

```
secret_BE  =  sea.secret(E.pub, B.pair)
seed_BE    =  sha256(secret_BE ⊕ orderId)
index_BE   =  index(seed_BE)             // 31-bit non-hardened index

VSE        =  root_SE.deriveChild(index_BE)   // Escrow wallet: S/E, for this order
VAE        =  root_AE.deriveChild(index_BE)   // Commission wallet: A/E, for this order
```

`VSE` and `VAE` are the two on-chain escrow wallets for any given order. Their addresses can be computed by any party with the appropriate public key material, but only E possesses the spending keys for both.

---

## 4. Protocol Walkthrough

### Step 1 — Seller lists an item

1. S creates item data and computes `itemId = sha256(itemData)`.
2. S computes `secret_SE` and derives `root_SE`.
3. S publishes `xpub_SE = root_SE.neuter().extendedKey` to the platform's distributed data layer, keyed by `itemId`. The `xprv` is never transmitted.
4. E independently recomputes `root_SE` at any time using `sea.secret(S.pub, E.pair)`.

### Step 2 — Affiliate generates a referral link

1. A computes `secret_AE` and derives `root_AE`.
2. A encodes `xpub_AE = root_AE.neuter().extendedKey` into a referral URL and distributes it to prospective buyers.

### Step 3 — Buyer places an order and pays

1. B generates `orderId` (e.g. `sha256(itemId ⊕ B.pub ⊕ timestamp)`).
2. B computes `secret_BE`, derives `seed_BE`, and calculates `index_BE`.
3. B retrieves `xpub_SE` from the platform data layer and derives:

```javascript
const VSE = HDNodeWallet.fromExtendedKey(xpub_SE).deriveChild(index_BE)
// VSE.address — the escrow wallet address for this order
```

4. B verifies the address is valid on-chain before sending funds.
5. B broadcasts three transactions from their on-chain wallet:
   - Platform fee → E's operational address
   - Commission → `VAE.address` (derived analogously from `xpub_AE`)
   - Item payment → `VSE.address`

The buyer does not hold a spending key for either `VSE` or `VAE`. The funds are cryptographically locked.

### Step 4 — Seller fulfils the order

Upon fulfilment, S requests confirmation from B.

| Outcome | Resolution |
|---|---|
| B confirms receipt | B transmits `index_BE` (or the `seed_BE`) to S. S computes `VSE.privateKey` using `root_SE.deriveChild(index_BE)` and self-executes the release transaction. |
| B does not respond within 24 hours | Auto-release: E transmits `index_BE` to S on B's behalf. |
| B disputes the order | Dispute resolution is initiated (see Step 5). |
| S does not request confirmation | Funds remain in `VSE` indefinitely until a party acts. |

### Step 5 — Escrow resolution by E

E can independently recompute the spending keys for any escrow wallet at any time:

```javascript
// E recomputes root_SE
const secret_SE = await sea.secret(S.pub, E.pair)
const root_SE   = HDNodeWallet.fromSeed(getBytes("0x" + sha256(secret_SE + ":" + itemId)))

// E recomputes index_BE
const secret_BE = await sea.secret(B.pub, E.pair)
const index_BE  = parseInt(sha256(secret_BE + ":" + orderId).substring(0, 8), 16) & 0x7fffffff

// E derives the escrow wallet with full spending authority
const VSE = root_SE.deriveChild(index_BE)
// VSE.privateKey — E uses this to sign the release or refund transaction
```

**Resolution matrix:**

| Scenario | VSE outcome | VAE outcome |
|---|---|---|
| Order successful, no dispute | E releases `VSE` to S | E releases `VAE` spending key to A |
| Order failed, S at fault | E refunds `VSE` to B | E refunds `VAE` to B |
| Dispute resolved in S's favour | E releases `VSE` to S | E releases `VAE` spending key to A |
| Dispute resolved in B's favour | E refunds `VSE` to B | E refunds `VAE` to B |

---

## 5. Security Analysis

### 5.1 Access control guarantees

| Property | Mechanism |
|---|---|
| E holds full authority over `VSE` and `VAE` | E knows every party's public key → recomputes all DH secrets → derives all root xprv and child spending keys |
| S cannot unilaterally withdraw from `VSE` | S holds `xpub_SE` but not `index_BE`. Without `secret_BE`, the child index is unknowable. |
| B cannot retain funds | B derives only a watch-only address via `xpub_SE`. B never possesses a private key for `VSE`. |
| A cannot withdraw commission prematurely | A holds `xpub_AE` but not `index_BE`. The commission is unlocked only when E transmits the spending key post-resolution. |
| No third-party involvement | All derivations are deterministic and local. No server, oracle, or smart contract is required. |

### 5.2 Per-order address isolation

Because `orderId` is unique per order, `seed_BE` and therefore `index_BE` are unique per order. Each order produces a distinct `VSE` and `VAE` address. Compromise of one order's escrow wallet does not affect any other order.

### 5.3 xpub publication safety

Publishing `xpub_SE` is safe by design. Knowledge of an xpub, without the corresponding child derivation index, reveals no actionable information to an adversary. The index `index_BE` is derived from `secret_BE`, which is known only to B and E.

Buyers should nonetheless verify that the `VSE.address` they compute matches on-chain activity before transmitting funds, to guard against data layer tampering.

---

## 6. Address Consistency Verification

Any implementation must verify that watch-only and full derivation paths produce identical addresses for the same parameters:

```javascript
// Watch-only derivation (as performed by B)
const addrFromXpub = HDNodeWallet
    .fromExtendedKey(xpub_SE)
    .deriveChild(index_BE)
    .address

// Full derivation (as performed by E)
const addrFromXprv = root_SE
    .deriveChild(index_BE)
    .address

console.assert(addrFromXpub === addrFromXprv, "Address mismatch — derivation error")
```

This assertion must be part of the test suite for any escrow implementation.

---

## 7. Risk Register

| Risk | Severity | Mitigation |
|---|---|---|
| `sea.secret` non-determinism | Critical | Verify determinism in test suite: two calls with identical arguments must return identical results |
| SEA not initialised at call time | High | Guard all escrow operations; SEA is only available after `Construct.GDB()` completes |
| BIP-32 non-hardened index overflow | Low | Index is masked to 31 bits via `& 0x7fffffff`; `sha256` output provides uniform distribution |
| `HDNodeWallet.fromSeed` entropy requirement | Low | `sha256` always produces 32 bytes, which satisfies the 16–64 byte requirement |
| `xpub` data layer tampering | Medium | Buyers verify the derived address against on-chain state prior to payment |
| Order ID collision | Very low | `sha256(itemId ⊕ B.pub ⊕ timestamp)` provides sufficient collision resistance |
| E key compromise | Critical | Outside protocol scope; requires operational security controls on E's key material |

---

## 8. Out of Scope

The following are explicitly outside the scope of this protocol version:

- **Fiat payment channels** (PayPal, credit card) — escrow for non-crypto payments is handled by platform-level account balances and is not addressed here
- **Smart contract enforcement** — the protocol intentionally avoids on-chain contracts; E's authority is enforced by key custody, not code
- **Multi-signature schemes** — the current model uses E as a single arbiter; multi-sig is a candidate for a future protocol revision
- **Affiliate flow UI** — commission wallet mechanics are fully specified above; UI implementation is deferred

---

*2026-03-12*
