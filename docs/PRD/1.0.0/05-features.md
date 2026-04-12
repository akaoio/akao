# Features

Phân loại theo priority. Mỗi feature ghi rõ: user nào, mô tả, chain dependency, trạng thái implementation.

- ✅ Đã implement
- ⚠️ Implement một phần
- 🔲 Chưa implement

---

## P0 — Must-have (không ship được nếu thiếu)

### Authentication
- **Passkey Signup/Signin** ✅: User đăng ký và đăng nhập bằng vân tay/Face ID/security key, không cần password. Wallet state khởi tạo mặc định (wallet ID 0) sau login.
  - User: Tất cả (Buyer, Seller, Affiliate)
  - Chain dependency: Không (auth là off-chain)
  - (ref: src/core/Access.js, src/core/WebAuthn.js — đã implement)
  - [TBD] Code hiện không check browser WebAuthn support trước khi hiện nút auth

### Wallet
- **Wallet On-demand** ✅: Ví crypto với địa chỉ on-chain được generate deterministic từ passkey khi user truy cập wallet features. Không pre-create lúc login.
  - User: Tất cả
  - Chain dependency: ETH + BSC
  - (ref: src/core/Wallet.js — address getter generate on-demand)

- **Multi-wallet** ✅: User có thể tạo nhiều ví (wallet 0, 1, 2...) qua identicons UI, tăng/giảm số lượng wallet.
  - User: Tất cả
  - (ref: src/core/Wallet.js, src/UI/components/identicons/ — đã implement)

- **Wallet Deposit** ✅: User nạp crypto vào ví bằng cách gửi từ ví ngoài đến địa chỉ hiển thị + QR code trên trang /deposit.
  - User: Buyer (chủ yếu)
  - Chain dependency: ETH + BSC
  - (ref: src/UI/routes/deposit/ — đã implement)

- **Balance Display** ⚠️: User xem số dư ví theo chain + currency đã chọn. Balance cập nhật khi user đổi chain/currency selector, KHÔNG auto-update realtime.
  - User: Tất cả
  - Chain dependency: ETH + BSC
  - (ref: src/UI/components/wallets/ — đã implement, nhưng thiếu auto-polling)
  - [TBD] Cần implement balance auto-polling

### Browse & Cart
- **Home Page** ✅: Trang chủ hiển thị 2 section cố định: Featured (page 1) và New Arrivals (page 2-3). Không có filter hay pagination controls trên home.
  - User: Buyer
  - (ref: src/UI/routes/home/ — đã implement)

- **Game Browse** ✅: Browse items theo game tại `/game/[game]` với search autocomplete, filter theo type/rarity, sorting (Name/Rarity/Price ASC/DESC), lazy-load pagination, game metadata display, game-specific CSS theming.
  - User: Buyer
  - (ref: src/UI/routes/game/ — đã implement đầy đủ)

- **Tag Browse** ⚠️: Browse items theo tag tại `/tag/[tag]`. Hiện chỉ có item listing cơ bản, KHÔNG có search, filter, sort hay lazy-load pagination như game route.
  - User: Buyer
  - (ref: src/UI/routes/tag/ — implement một phần, thiếu search/filter/sort)

- **Item Detail** ✅: User xem chi tiết item (tên, mô tả, giá, giá sale, rarity, type, variants), chọn variant qua radio buttons + số lượng.
  - User: Buyer
  - (ref: src/UI/routes/item/ — đã implement)

- **Shopping Cart** ✅: User thêm/xóa items, thay đổi số lượng, xem tổng giá. Cart persistent qua IndexedDB. Cart UI hiển thị: tên, tổng giá, số lượng. Chưa hiển thị giá đơn vị và variant đã chọn.
  - User: Buyer
  - (ref: src/core/Cart.js — đã implement)

### Checkout & Platform
- **Checkout Flow** 🔲: Buyer xem tóm tắt đơn hàng (items + phí sàn + phí affiliate), chọn chain/currency, bấm thanh toán, xác nhận passkey, hệ thống tạo giao dịch platform on-chain.
  - User: Buyer
  - Chain dependency: ETH + BSC
  - (ref: src/UI/routes/checkout/ — route tồn tại nhưng chỉ có empty div, CHƯA implement logic)

- **Order Status** 🔲: Buyer/Seller xem trạng thái platform (Holding / Released / Disputed / Refunded) trên trang /order.
  - User: Buyer, Seller
  - Chain dependency: ETH + BSC (đọc balance on-chain)
  - (ref: src/UI/routes/order/ — route tồn tại nhưng chỉ có Lorem ipsum placeholder, CHƯA implement)

### i18n & Currency
- **Multi-language** ✅: Toàn bộ UI hỗ trợ 18 ngôn ngữ (en, zh, es, fr, de, ja, ko, ru, ar, hi, pt, it, vi, th, he, ur, zh-TW, no), user chọn ngôn ngữ qua switcher, URL prefix theo locale. RTL support cho ar, he, ur.
  - User: Tất cả
  - (ref: src/statics/locales.yaml — 18 locales, src/core/Router.js — RTL via document dir attribute)

- **Multi-fiat Display** ✅: Giá items hiển thị theo fiat currency user chọn (44 currencies), chuyển đổi tự động qua forex rate. Cross-rate calculation qua bridge currency khi không có direct rate.
  - User: Tất cả
  - (ref: src/core/Forex.js — convert + cross-rate logic, src/statics/fiats.yaml — 44 currencies)

---

## P1 — Should-have (có thể delay sang phase 2)

- **Seller Fulfill & Confirm** 🔲: Seller xem orders cần fulfill trên /inventory, request buyer confirm sau khi giao hàng. Auto-release sau 24h nếu buyer không phản hồi.
  - User: Seller, Buyer
  - (ref: Route /inventory CHƯA TỒN TẠI trong codebase. Business logic ref: docs/thoughts/white-paper-draft.md — Bước 4)

- **Dispute Flow** 🔲: Buyer mở dispute khi chưa nhận hàng, Seller cung cấp proof, Platform phân xử (release hoặc refund).
  - User: Buyer, Seller
  - (ref: src/UI/routes/dispute/ — route tồn tại nhưng chỉ có Lorem ipsum, CHƯA implement. Business logic ref: docs/thoughts/white-paper-draft.md — Bước 4, 5)

- **Withdraw Flow** ✅: User rút crypto từ ví trên sàn về ví ngoài, với gas fee estimation realtime (debounce 500ms).
  - User: Seller (chủ yếu), Buyer
  - Chain dependency: ETH + BSC
  - (ref: src/UI/routes/withdraw/ — đã implement form + gas estimation + submit)
  - [TBD] Hiện không require passkey confirm trước withdraw — security concern

- **Affiliate Link** ⚠️: Referrer tracking đã implement (read từ URL param "r", lưu localStorage + GunDB). Nhưng affiliate commission calculation và distribution logic CHƯA implement.
  - User: Affiliate
  - (ref: src/core/Context.js — getReferrer/setReferrer đã implement. Commission logic chưa có)

---

## P2 — Nice-to-have (làm nếu còn bandwidth)

- **Wave Signin** ⚠️: Đăng nhập trên thiết bị mới bằng QR code + sóng âm (ultrasound). Cấu trúc wave() function trong Access.js đã có, ggwave.js đang integrate.
  - User: Tất cả
  - (ref: src/core/Access.js — wave() method, docs/todos/son/20260322.md — đang phát triển)

- **Multi-chain Selection** 🔲: User chọn chain (ETH/BSC) khi checkout, hệ thống hiển thị giá + gas fee tương ứng.
  - User: Buyer
  - [ASSUMPTION] Hiện tại mỗi site config define chains supported, nhưng UI chọn chain khi checkout chưa rõ flow.

- **Platform History Timeline** 🔲: Hiển thị toàn bộ lịch sử thay đổi trạng thái platform (created → holding → released) với timestamp.
  - User: Buyer, Seller

- **Profile Management** 🔲: User cập nhật avatar, quản lý danh sách ví, xem lịch sử giao dịch trên /profile.
  - User: Tất cả
  - (ref: src/UI/routes/profile/ — route tồn tại nhưng chỉ có placeholder)

- **Theme Toggle** ✅: User chuyển đổi giữa light/dark mode, hệ thống nhớ preference (localStorage + system preference detection).
  - User: Tất cả
  - (ref: src/UI/components/themes/, src/core/Context.js — getTheme/setTheme — đã implement)

- **Multi-site / Multi-tenant** ✅: Mỗi domain có thể config branding, chains, locale, fiat riêng qua site configs.
  - (ref: src/statics/sites/, src/statics/domains.yaml — đã implement)
