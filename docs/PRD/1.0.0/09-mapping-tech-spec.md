# Mapping PRD → Tech Spec

Bảng này chỉ ra mỗi section PRD cần được implement chi tiết trong Tech Spec section nào.

---

## Mapping Table

| PRD Section | Tech Spec Section tương ứng | Ghi chú |
|-------------|----------------------------|---------|
| **Flow 1: Auth** ✅ | Client Architecture (Access.js, WebAuthn.js), State Flow (auth state) | Cần mô tả WebAuthn credential lifecycle, SEA keypair derivation |
| **Flow 2: Browse & Cart** ✅ | Data Layer (Static JSON items, IndexedDB cart), Component Architecture (ui-items, ui-cart) | Cart persistence qua Indexes.Cart, SHA256 dedup key |
| **Flow 3: Cart Management** ✅ | State Flow (Cart.js reactive updates), Data Layer (IndexedDB) | Cart.add() dedup logic bằng SHA256 key |
| **Flow 4: Deposit** ✅ | Client Architecture (Wallet.js address), On-chain Interface (balance read) | Deterministic address generation. [TBD] Balance auto-polling design |
| **Flow 5: Checkout & Escrow** 🔲 | Key Derivation (VSE/VAE wallet), On-chain Interface (3 transactions), State Flow (checkout → order), Thread Coordination (tx broadcast + confirmation) | **Section quan trọng nhất** — cần mô tả đầy đủ escrow wallet derivation theo white-paper |
| **Flow 6: Fulfill & Confirm** 🔲 | Data Layer (GunDB order data), State Flow (order status updates), Key Derivation (seed release to seller) | GunDB P2P sync cho seller nhận order. Route /inventory chưa tồn tại |
| **Flow 7: Withdraw** ✅ | On-chain Interface (send transaction), RPC Retry (failover), Error Handling (tx failure) | Gas estimation debounce 500ms. [TBD] Passkey confirm trước withdraw |
| **Flow 8: Dispute** 🔲 | Data Layer (dispute state in GunDB), Key Derivation (Platform refund via xprv) | Platform cần access xprv để refund. Route /dispute chỉ placeholder |
| **Game/Tag Browse** ✅ | Data Layer (static items JSON), Component Architecture (search, filter, sort) | Advanced filter/sort UI đã implement |
| **Edge Cases (business)** | Error Handling, RPC Retry & Failover, Thread Coordination | Mỗi business edge case map sang 1+ technical scenario |
| **Constraints (chain)** | Chain Config (supported chains, RPC providers), On-chain Interface (gas estimation) | Chain configs từ statics/chains/ |
| **Constraints (i18n)** | Build Pipeline (locale route generation), Data Layer (i18n JSON loading) | 18 locales × routes = static pages |
| **Constraints (multi-site)** | Build Pipeline (domain → site config mapping), Data Layer (site-specific statics) | src/statics/sites/, domains.yaml |
| **Constraints (performance)** | Thread Coordination (Worker offload), Data Layer (hash-validated caching) | DB.js cache strategy |

---

## TBD Items cần resolve trước khi viết Tech Spec

| TBD | PRD Section | Impact on Tech Spec |
|-----|-------------|---------------------|
| Platform fee rate % | 04-user-flows.md (Flow 5) | Ảnh hưởng calculation logic trong checkout |
| Affiliate rate % | 03-user-personas.md | Ảnh hưởng calculation logic trong checkout |
| Thời gian chờ tối đa trước khi buyer mở dispute | 07-edge-cases.md | Ảnh hưởng timer/state machine design |
| Policy dispute: thiên về buyer hay seller? | 07-edge-cases.md | Ảnh hưởng dispute resolution logic |
| Stock management cho items? | 07-edge-cases.md | Ảnh hưởng checkout validation |
| Recovery mechanism Phase 1? | 07-edge-cases.md | Ảnh hưởng auth architecture |
| Seller tự list items Phase 1? | 03-user-personas.md | Ảnh hưởng item data layer (static vs dynamic) |
| Platform phân xử dispute qua UI hay ngoài hệ thống? | 04-user-flows.md (Flow 8) | Ảnh hưởng có cần admin UI hay không |
| Cross-rate fallback khi không có rate | 07-edge-cases.md | Ảnh hưởng Forex.js display logic |
| Notification mechanism (order events) | 04-user-flows.md (Flow 6) | Ảnh hưởng notification architecture: in-app? push? GunDB event? |
| Browser WebAuthn support check | 04-user-flows.md (Flow 1) | Ảnh hưởng auth UX: hiện warning hay ẩn nút? |
| Passkey confirm trước withdraw | 04-user-flows.md (Flow 7) | Ảnh hưởng security model cho outbound transactions |
| Balance auto-polling | 04-user-flows.md (Flow 4) | Ảnh hưởng Worker thread design cho background balance check |
