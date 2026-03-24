# Acceptance Criteria

Format: Given [context], When [action], Then [expected result].

Mỗi P0 feature có ít nhất 2 AC. Mỗi AC ghi rõ trạng thái:
- ✅ Code đã đáp ứng
- 🔲 Cần implement

---

## Authentication

### Passkey Signup
- ✅ Given user chưa có tài khoản, When bấm "Đăng ký" và xác nhận passkey, Then tài khoản được tạo (authenticated = true), wallet state init với ID 0, identicon render khi wallet component load
- 🔲 Given user đã có passkey, When bấm "Đăng ký", Then hệ thống nhận diện passkey đã tồn tại và đăng nhập thay vì tạo credential mới. **[Hiện chưa implement — code gọi WebAuthn.create() không check existing credential]**

### Passkey Signin
- ✅ Given user đã có passkey, When bấm "Đăng nhập" và xác nhận, Then khôi phục tài khoản + wallet state từ localStorage, trạng thái = đã đăng nhập
- 🔲 Given trình duyệt không hỗ trợ WebAuthn, When mở trang, Then hiển thị thông báo "Trình duyệt không hỗ trợ". **[Hiện chưa implement — code dùng navigator.credentials trực tiếp không check support]**
- ✅ Given user đang ở popup passkey, When bấm cancel, Then WebAuthn trả về null (NotAllowedError caught), UI quay lại trạng thái trước

---

## Wallet

### Wallet On-demand
- ✅ Given user đã login, When truy cập wallet feature (deposit/withdraw), Then wallet address được generate deterministic từ sha256(priv + wallet_id)
- ✅ Given cùng 1 user login trên thiết bị khác, When login bằng cùng passkey, Then ví có cùng địa chỉ (deterministic qua WebAuthn PRF extension → same seed → same address)

### Wallet Deposit
- ✅ Given user đã login, When vào /deposit và chọn chain + currency, Then thấy địa chỉ ví dạng text + QR code
- 🔲 Given user đã gửi crypto từ ví ngoài, When transaction confirm on-chain, Then balance hiển thị cập nhật tự động. **[Hiện chưa implement — balance chỉ refresh khi user đổi chain/currency selector]**

### Balance Display
- ✅ Given user đã login và có balance > 0, When chọn chain + currency trên wallet selector, Then hiển thị số dư chính xác theo currency đó
- 🔲 Given user chưa có balance (ví mới), When xem balance, Then hiển thị "0". **[Hiện chưa đảm bảo — code set textContent = balance trực tiếp, có thể hiện empty string hoặc NaN nếu balance undefined/NaN. Cần defensive check]**

---

## Browse & Cart

### Home Page
- ✅ Given user vào trang chủ, When trang load, Then thấy section Featured (items page 1) và New Arrivals (items page 2-3), mỗi item hiển thị icon, tên, giá theo fiat đã chọn

### Game / Tag Browse
- ✅ Given user vào /game/[game], When trang load, Then thấy items của game đó với search, filter (type/rarity), sorting, lazy-load pagination
- ⚠️ Given user vào /tag/[tag], When trang load, Then thấy items filtered by tag. **[Hiện chỉ có item listing cơ bản — KHÔNG có search, filter, sort hay lazy-load pagination như game route]**

### Item Detail
- ✅ Given user vào /item/[item-id], When trang load, Then thấy đầy đủ: icon, tên, mô tả, giá, giá sale (nếu có), game, rarity, type
- ✅ Given item có variants (attributes), When user chọn variant, Then radio button selected (variant tracking trong cart qua SHA256 key). Lưu ý: giá hiện KHÔNG thay đổi theo variant — tất cả variants cùng giá
- ✅ Given user đổi ngôn ngữ, When chọn locale mới, Then tên + mô tả item hiển thị đúng ngôn ngữ mới (lazy-load locale JSON)

### Shopping Cart
- ✅ Given user ở trang item detail, When bấm "Add to Cart", Then item thêm vào cart, toast notification hiện (tên item + quantity), cart component cập nhật
- ✅ Given item đã có trong cart (cùng id + sku + attributes), When bấm "Add to Cart" lần nữa, Then số lượng tăng thêm (merge via SHA256 key dedup)
- ✅ Given cart có items, When user đóng browser và mở lại, Then cart vẫn còn nguyên (persistent qua IndexedDB Indexes.Cart)
- ✅ Given cart có item, When bấm nút xóa (icon X), Then item bị xóa khỏi cart, tổng giá cập nhật

---

## Checkout & Escrow 🔲

**Toàn bộ section này là requirement cần implement. Route /checkout hiện chỉ có empty div, route /order chỉ có Lorem ipsum.**

### Checkout Flow
- 🔲 Given buyer đã login, cart có items, ví có đủ balance + gas fee, When vào /checkout, Then thấy tóm tắt: danh sách items, tổng giá, phí sàn, phí affiliate (nếu có referrer), tổng cộng, gas fee ước tính
- 🔲 Given buyer bấm "Thanh toán", When xác nhận passkey, Then hệ thống tạo 3 giao dịch on-chain (item → VSE, platform fee, affiliate → VAE), buyer thấy progress, sau khi confirm → chuyển sang /order với status "Holding"
- 🔲 Given buyer không đủ balance, When vào /checkout, Then hiển thị "Không đủ số dư" với số tiền cần thêm và nút dẫn đến /deposit
- 🔲 Given buyer không đủ gas fee (nhưng đủ token amount), When bấm "Thanh toán", Then hiển thị "Không đủ phí giao dịch" với số native coin cần thêm
- 🔲 Given giao dịch on-chain bị revert, When transaction fail, Then hiển thị lỗi cụ thể, tiền KHÔNG bị mất (revert = nguyên trạng), user có thể thử lại

### Order Status
- 🔲 Given buyer đã thanh toán thành công, When vào /order, Then thấy order với status = "Holding", số tiền, chain, và thời gian tạo
- 🔲 Given buyer đã confirm nhận hàng, When xem /order, Then status = "Released"
- 🔲 Given order đang disputed, When xem /order, Then status = "Disputed"
- 🔲 Given buyer đã thanh toán nhiều orders, When vào /order, Then thấy danh sách tất cả orders sắp xếp mới nhất lên trên

---

## i18n & Currency

### Multi-language
- ✅ Given user chọn ngôn ngữ tiếng Việt, When navigate qua các trang, Then toàn bộ UI text hiển thị tiếng Việt, URL prefix = /vi/
- ✅ Given user chọn ngôn ngữ Arabic (RTL), When xem trang, Then layout hiển thị đúng RTL (Router.js set document.documentElement.dir = "rtl")

### Multi-fiat Display
- ✅ Given user chọn VND làm fiat currency, When xem item price 100 USD, Then hiển thị giá tương đương bằng VND (theo forex rate hiện tại)
- ✅ Given forex rate thay đổi, When user load lại trang, Then giá hiển thị cập nhật theo rate mới (DB.js hash-validate → refetch nếu hash khác)
