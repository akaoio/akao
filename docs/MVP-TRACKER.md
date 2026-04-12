# akao MVP Progress Tracker
**Target:** MVP Demo Ready by 2026-05-24 (8 weeks from 2026-03-24)  
**Current Date:** 2026-04-10  
**Days Elapsed:** 17 days (Week 2.5 of 8 weeks)  
**Days Remaining:** 44 days (~6.3 weeks)

---

## 📊 Overall Progress: ~25% Complete

### Timeline Status
- **Original Start:** 2026-03-24
- **Current:** 2026-04-10 (Week 2-3)
- **Target Finish:** 2026-05-24
- **Status:** ON TRACK (if Sprint 1 deliverables complete this week)

---

## 🎯 MVP Definition

**What investors need to see:**
1. ✅ Buyer pays → money goes to on-chain platform wallet (visible on blockchain explorer)
2. ✅ 3 separate transactions: seller payment, platform fee, affiliate commission
3. ✅ Seller delivers → buyer confirms → money released to seller
4. ✅ Auto-release after 24h if buyer doesn't respond
5. ✅ Full flow runs on testnet without backend server

**Critical Features (Must Have):**
- [ ] Checkout & platform payment flow
- [ ] Order status page (buyer sees money "held" in platform)
- [ ] Seller delivery & buyer confirmation
- [ ] Auto-release after 24h
- [ ] Auto wallet balance updates
- [ ] Affiliate commission calculation & distribution
- [ ] Passkey confirmation before withdrawal

**Out of Scope (Phase 2+):**
- ❌ Dispute resolution UI (handled outside system in Phase 1)
- ❌ Wave Signin (device sync)
- ❌ Profile management page
- ❌ Platform timeline history
- ❌ Advanced tag page features

---

## 📅 Sprint Breakdown

### ✅ Sprint 0 (Pre-work) — COMPLETE
- [x] Project structure
- [x] 19 language i18n system
- [x] WebAuthn passwordless auth
- [x] Multi-chain support (ETH/BSC)
- [x] Basic routes (home, item, checkout, order, inventory, deposit, withdraw)

### 🔄 Sprint 1 (Week 1-2): Foundation — IN PROGRESS
**Goal:** Create VSE/VAE platform wallets on testnet. UI skeletons ready.

**Logic Tasks:**
- [ ] **CRITICAL** `src/core/Platform.js` - VSE wallet derivation (seller platform)
- [ ] **CRITICAL** `src/core/Platform.js` - VAE wallet derivation (affiliate platform)
- [x] Order schema definition (`src/core/Order.js` exists)
- [ ] GunDB helpers: createOrder(), getOrder(), updateOrder()

**UI Tasks:**
- [ ] Checkout page: items list, fee breakdown, chain/currency selector, pay button
- [ ] Order page: order list, status badges, confirm/dispute buttons

**Coordination:**
- [ ] Platform fee % agreed
- [ ] Affiliate commission % agreed
- [ ] Order data schema documented

**Sprint 1 Checkpoint (Week 2):** Create platform wallet on testnet from buyer+seller keys

### ⏳ Sprint 2 (Week 3-4): End-to-End Payment — UPCOMING
**Goal:** Buyer can pay on testnet — 3 on-chain transactions visible on explorer.

**Logic Tasks:**
- [ ] `src/core/Checkout.js` - Send 3 on-chain txs (VSE, platform, VAE)
- [ ] Balance check before checkout (token + gas)
- [ ] Auto balance polling (15-30s worker)

**UI Tasks:**
- [ ] Checkout connected: loading, progress bar, error handling
- [ ] Order page shows real data from GunDB

**Coordination:**
- [ ] Testnet testing: happy path + error cases
- [ ] Decision: Sequential or parallel txs? Partial failure handling?

**🚨 CRITICAL CHECKPOINT (Week 4):** If checkout not working on testnet → re-scope immediately

### ⏳ Sprint 3 (Week 5-6): Seller Flow + Lifecycle — UPCOMING
**Goal:** Full cycle: buyer pays → seller delivers → buyer confirms → money released.

**Logic Tasks:**
- [ ] Order state machine: Holding → Released / Refunded
- [ ] Seed release mechanism (buyer confirms → seed to seller → seller withdraws)
- [ ] Auto-release 24h timer

**UI Tasks:**
- [ ] `/inventory` route for sellers
- [ ] Connect confirm/dispute buttons
- [ ] Passkey auth before withdrawal

**Coordination:**
- [ ] End-to-end testnet testing

**Milestone (Week 6):** Full order lifecycle working

### ⏳ Sprint 4 (Week 7-8): Polish + Demo Prep — UPCOMING
**Goal:** MVP ready for investor demo.

**Logic Tasks:**
- [ ] Affiliate commission calculation & distribution
- [ ] Error handling: tx revert, gas spike, partial payment
- [ ] WebAuthn browser support check
- [ ] Fix balance NaN display

**UI Tasks:**
- [ ] Cart UX: show unit price + variants
- [ ] Polish: loading states, error messages, responsive, toasts

**Coordination:**
- [ ] Regression testing
- [ ] Demo script preparation

**Milestone (Week 8):** MVP demo-ready

---

## 🔍 Current Sprint Status (Sprint 1 - Week 2-3)

### Completed This Week
- [x] Swarm protocol established
- [x] Team coordination workflow
- [x] Order.js module exists

### In Progress
- [ ] Platform wallet derivation (VSE/VAE) - **STATUS UNKNOWN**
- [ ] Checkout UI skeleton - **STATUS UNKNOWN**
- [ ] Order UI skeleton - **STATUS UNKNOWN**
- [ ] Huy: Swap pool implementation

### Blockers
- ⚠️ Need status update on Platform.js implementation
- ⚠️ Need platform fee & affiliate % decisions
- ⚠️ Need order schema documentation

---

## 📈 Key Metrics to Track

### Development Velocity
- [ ] Story points per sprint (establish baseline)
- [ ] Bug count trend
- [ ] Code review turnaround time

### Technical Health
- [ ] Testnet transaction success rate
- [ ] Build time
- [ ] Test coverage %

### Demo Readiness
- [ ] Critical path features complete: 0/7
- [ ] Nice-to-have features: 0/X
- [ ] Known bugs (critical): TBD
- [ ] Known bugs (minor): TBD

---

## ⚠️ Risks & Mitigation

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| Platform derivation more complex than expected | HIGH - delays Sprint 1-2 | MEDIUM | Prototype on testnet week 1, validate with whitepaper early |
| 3-tx checkout partial failure | HIGH - money stuck | MEDIUM | Sequential send, check each tx, save state for retry |
| GunDB sync unreliable | MEDIUM - UX degraded | LOW | Local-first design: IndexedDB + background sync |
| 24h timer won't run if no one opens app | MEDIUM - money stuck | MEDIUM | Check timestamp on page load, trigger if >24h |
| Team member absence | MEDIUM - capacity loss | LOW | Sprint 4 has buffer; cut #8 (affiliate) or #12 (cart) if needed |
| **Week 4 checkpoint failure** | **CRITICAL** | **UNKNOWN** | **Emergency re-scope meeting** |

---

## 🎬 Next Actions

### Immediate (This Week)
1. **Get status updates from team**
   - Huy: Swap pool progress, checkout UI status
   - Đức: Platform.js implementation status
   - Sơn: Platform fee & affiliate % decisions

2. **Sprint 1 completion verification**
   - Can we derive VSE/VAE wallets on testnet? (critical path)
   - Are checkout & order UI skeletons ready?

3. **Documentation**
   - Document order schema
   - Document fee structure

### Next Week (Sprint 2 Start)
1. Begin 3-tx checkout implementation
2. Wire checkout UI to logic
3. Set up balance polling worker

---

## 📝 Notes
- Original timeline: 2026-03-24 start
- We're in Week 2-3 window now
- Critical checkpoint at Week 4 (end of Sprint 2)
- MVP must be demo-ready by 2026-05-24
- Current focus: Finishing Sprint 1 foundation work
