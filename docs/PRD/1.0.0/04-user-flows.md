# User Flows

Tất cả flows đều yêu cầu user đã có passkey (đăng ký hoặc đăng nhập). Routes reference là routes thực tế trong codebase.

Mỗi flow ghi rõ trạng thái implementation:
- ✅ Đã implement
- 🔲 Chưa implement (cần build)

(ref: src/UI/routes/)

---

## Flow 1: Đăng ký & Đăng nhập ✅

### Đăng ký (lần đầu)
1. User vào trang chủ `/` → bấm nút đăng ký
2. Trình duyệt hiện popup passkey → User xác nhận bằng vân tay/Face ID/security key
3. Hệ thống tạo tài khoản (authenticated state), wallet state khởi tạo mặc định (wallet ID 0)
4. User thấy avatar (identicon) trên header khi wallet component render
5. User có thể chọn giữa nhiều ví (multi-wallet support, tăng/giảm qua identicons UI)

**Lưu ý:** Địa chỉ ví on-chain không được pre-create lúc signup. Address generate on-demand khi user truy cập wallet features (deposit, withdraw, checkout). (ref: src/core/Wallet.js — address getter)

### Đăng nhập (lần sau)
1. User bấm nút đăng nhập
2. Trình duyệt hiện popup passkey → User xác nhận
3. Hệ thống khôi phục tài khoản + wallet state → trạng thái "đã đăng nhập"

### Đăng nhập trên thiết bị khác (Wave) — P2 🔲
1. Thiết bị mới hiển thị QR code
2. User scan QR bằng thiết bị đã đăng nhập
3. Hai thiết bị xác nhận qua sóng âm (ultrasound)
4. Thiết bị mới được cấp quyền đăng nhập

(ref: src/core/Access.js, src/UI/components/access/)

[TBD] Hiện tại code KHÔNG check browser support cho WebAuthn trước khi hiện nút auth. Cần quyết định: hiện warning hay ẩn nút?

---

## Flow 2: Browse & Thêm vào Cart ✅

### Trang chủ
1. User vào trang chủ `/` → thấy 2 section cố định:
   - **Featured**: Items trang 1 (theo build order)
   - **New Arrivals**: Items trang 2-3
2. Trang chủ KHÔNG có filter hay pagination controls — chỉ hiển thị items theo section hardcoded

### Browse theo Game ✅
3. User navigate đến `/game/[game]` → thấy danh sách items của game đó với:
   - Search bar với autocomplete suggestions
   - Filter theo type (collapsible tabs, mobile select)
   - Filter theo rarity (color-coded pills)
   - Sorting: Name, Rarity, Price (ASC/DESC)
   - Lazy-load pagination ("Load More")
   - Game metadata: developer, release date, genres, modes, platforms
   - Game-specific CSS theming (rarity palette colors)

### Browse theo Tag ⚠️
4. User navigate đến `/tag/[tag]` → danh sách items filtered by tag. **Lưu ý:** Tag route hiện chỉ có item listing cơ bản, KHÔNG có search, filter, sort hay lazy-load pagination như game route

### Item Detail ✅
5. User click vào item → chuyển đến `/item/[item-id]`
6. Trang item hiển thị:
   - Icon, tên, mô tả (theo ngôn ngữ user chọn)
   - Giá (hiển thị theo fiat currency user chọn, thanh toán bằng crypto)
   - Giá sale nếu có (ưu tiên `meta.sale` over `meta.price`)
   - Game, rarity, type (nếu có)
   - Variants/attributes (chọn qua radio buttons)
   - Selector số lượng (+/-)
7. User bấm "Add to Cart" → Toast notification hiện tên item + quantity
8. Cart icon trên header cập nhật

(ref: src/UI/routes/home/, src/UI/routes/game/, src/UI/routes/tag/, src/UI/routes/item/, src/core/Cart.js, src/UI/components/cart/)

---

## Flow 3: Xem & Quản lý Cart ✅

1. User bấm icon cart trên header → mở cart modal
2. Cart hiển thị:
   - Danh sách items (item mới nhất lên trên, sort by timestamp DESC)
   - Mỗi item: tên (link đến item page), tổng giá, số lượng
   - **Lưu ý:** Cart UI hiện KHÔNG hiển thị giá đơn vị và variant đã chọn. Chỉ show tên + tổng + quantity
   - Nút +/- để thay đổi số lượng (tối thiểu 1)
   - Nút xóa item (icon X)
   - Tổng giỏ hàng
3. User bấm "Checkout" → chuyển đến `/checkout`

**Cart dedup logic:** Khi add item, hệ thống tạo key từ SHA256(id + sku + sorted_attributes). Nếu key đã tồn tại → merge quantity thay vì tạo duplicate.

(ref: src/core/Cart.js — add, remove, increase, decrease methods)

---

## Flow 4: Deposit crypto vào ví ✅

**Điều kiện:** User đã đăng nhập

1. User vào `/deposit`
2. Chọn chain (ETH/BSC) và currency (native coin hoặc stablecoins: USDT, USDC, BUSD...)
3. Hệ thống hiển thị:
   - Địa chỉ ví (deterministic, luôn cùng 1 địa chỉ cho cùng user + chain + wallet ID)
   - QR code chứa địa chỉ (để scan từ ví ngoài)
4. User gửi crypto từ ví ngoài (Metamask, exchange, ví khác) vào địa chỉ này
5. Balance cập nhật khi user thay đổi chain/currency selector

**Lưu ý:** Hiện tại balance KHÔNG auto-update realtime. Không có polling hay WebSocket. User cần thay đổi selector hoặc navigate lại trang để refresh balance. [TBD] Cần implement auto-polling cho balance update.

(ref: src/UI/routes/deposit/, src/UI/components/wallets/)

---

## Flow 5: Checkout & Platform Payment 🔲

**Trạng thái:** Route `/checkout` tồn tại nhưng chỉ có empty div, KHÔNG có logic nào. Toàn bộ flow dưới đây là **requirement cần implement**, không phải mô tả code hiện tại.

**Điều kiện:** User đã đăng nhập, cart không rỗng, ví có đủ balance + gas fee

1. User vào `/checkout` → thấy tóm tắt đơn hàng:
   - Danh sách items từ cart
   - Tổng giá items (cho Seller)
   - Phí sàn (cho Platform) — [TBD] Platform fee rate bao nhiêu %?
   - Phí affiliate (nếu user đến từ referral link, cho Affiliate) — [TBD] Affiliate rate bao nhiêu %?
   - Tổng cộng (items + phí sàn + phí affiliate)
   - Gas fee ước tính
2. User chọn chain + currency để thanh toán
3. User bấm "Thanh toán" → xác nhận bằng passkey
4. Hệ thống tạo giao dịch on-chain:
   - Tiền cho Seller → vào ví platform VSE (Platform kiểm soát, Seller chưa rút được)
   - Phí sàn → vào ví Platform
   - Phí affiliate → vào ví platform VAE (nếu có referrer)
5. User thấy progress bar trong quá trình xử lý
6. Thành công → chuyển sang `/order` → thấy status "Holding"
7. Thất bại → hiển thị lỗi cụ thể, tiền không mất (giao dịch revert on-chain)

**Nếu không đủ balance:**
- Hiển thị thông báo "Không đủ số dư" + số tiền cần thêm
- Nút dẫn đến `/deposit`

(ref: docs/thoughts/white-paper-draft.md — Bước 3, 3A)

---

## Flow 6: Seller Fulfill & Buyer Confirm 🔲

**Trạng thái:** Route `/inventory` CHƯA TỒN TẠI trong codebase (chỉ có link trên home page). Route `/order` chỉ có Lorem ipsum placeholder. Toàn bộ flow dưới đây là **requirement cần implement**.

**Sau khi buyer thanh toán thành công:**

### Phía Seller
1. Seller vào `/inventory` → thấy order mới với status "Pending Fulfillment"
2. Seller giao hàng/hoàn thành service (ngoài hệ thống — in-game delivery)
3. Seller bấm "Request Confirm" trên order

### Phía Buyer
4. Buyer nhận thông báo Seller đã giao hàng — [TBD] Notification mechanism: in-app? push? GunDB event? Hiện chưa có notification system
5. Buyer vào `/order` → xem order details
6. **Trường hợp A — Buyer confirm:** Bấm "Xác nhận đã nhận" → Platform release tiền cho Seller → Status = "Released"
7. **Trường hợp B — Buyer không phản hồi:** Sau 24h kể từ khi Seller request confirm → Hệ thống tự động release cho Seller
8. **Trường hợp C — Buyer phản đối:** Bấm "Mở Dispute" → Chuyển sang Flow 8

(ref: docs/thoughts/white-paper-draft.md — Bước 4)

---

## Flow 7: Withdraw crypto ✅

**Điều kiện:** User đã đăng nhập, ví có balance > 0

1. User vào `/withdraw`
2. Chọn chain + currency
3. Nhập:
   - Địa chỉ ví nhận (external wallet)
   - Số tiền muốn rút
4. Hệ thống hiển thị gas fee ước tính (realtime khi user nhập, debounce 500ms)
5. User bấm "Rút tiền" → giao dịch broadcast on-chain trực tiếp (hiện KHÔNG yêu cầu xác nhận passkey trước khi gửi)
6. Thành công → Toast "Đã gửi" (hiện chỉ hiển thị message generic, KHÔNG hiển thị transaction hash)
7. Thất bại → Toast lỗi generic

**Lưu ý bảo mật:** [TBD] Withdraw hiện không require passkey confirm trước khi broadcast. Cần quyết định: thêm passkey verification step hay không?

(ref: src/UI/routes/withdraw/ — đã implement form + gas estimation)

---

## Flow 8: Dispute Resolution 🔲

**Trạng thái:** Route `/dispute` chỉ có Lorem ipsum placeholder. Toàn bộ flow dưới đây là **requirement cần implement**, dựa trên white-paper.

**Điều kiện:** Order đang ở trạng thái "Pending Fulfillment" hoặc Seller đã request confirm

1. Buyer bấm "Mở Dispute" trên order
2. Buyer mô tả vấn đề (chưa nhận hàng, sai hàng, hàng lỗi...)
3. Order chuyển status = "Disputed"
4. Seller được thông báo → cung cấp proof (screenshot, video, tracking...)
5. Platform phân xử:
   - **Seller đúng** → Release tiền cho Seller → Status = "Released"
   - **Buyer đúng** → Refund tiền cho Buyer → Status = "Refunded"
6. Affiliate commission:
   - Order thành công (kể cả dispute Seller thắng) → Affiliate nhận hoa hồng
   - Order refund (dispute Buyer thắng) → Affiliate không nhận hoa hồng, tiền affiliate refund cho Buyer

(ref: docs/thoughts/white-paper-draft.md — Bước 4, 5)

[TBD] Cần confirm: Platform phân xử qua UI hay qua process ngoài hệ thống? Hiện chưa có admin dashboard.
