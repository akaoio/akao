# Danh sách tính năng cần hoàn thiện

Mỗi tính năng ghi rõ: cần làm gì, output cụ thể, giải quyết vấn đề gì, và giúp gì cho việc gọi vốn.

---

## Phần A — Tính năng cần xây mới (chưa có dòng code nào)

---

### 1. Tạo ví platform cho seller (VSE Wallet Derivation)

**Vấn đề hiện tại:** Chưa có cách nào tạo ví trung gian giữ tiền cho seller. Không có ví platform = không có sản phẩm.

**Cần làm:** Viết module tạo ví platform dựa trên thuật toán trong white-paper (Bước 3):
- Input: **`pub` của seller**, secret giữa buyer và platform, mã đơn hàng (order id)
- Thuật toán: `unlockSeed = hash(buyer_platform_secret + order_id)` → dùng **ZEN secp256k1 derivation** derive ra public lock key từ `pub` của seller
- Output: địa chỉ ví VSE (dạng watch-only — ai cũng thấy nhưng chỉ holder của full pair tương ứng mới derive được private key để rút)

**Output cụ thể:** File `src/core/Platform.js` với function `deriveVSE(sellerPairPub, buyerSecret, orderId)` → trả về `{ address, privateKey }`

**Giúp gì cho gọi vốn:** Đây là phần chứng minh công nghệ lõi — ví platform tạo ra on-chain, kiểm chứng được trên blockchain explorer. Nhà đầu tư thấy tiền thật sự nằm trong ví mà không bên nào tự ý rút được.

**Tham khảo:** `docs/thoughts/white-paper-draft.md` — Bước 3, `src/core/Wallet.js` — pattern derive address hiện tại

---

### 2. Tạo ví platform cho affiliate (VAE Wallet Derivation)

**Vấn đề hiện tại:** Chưa có cách giữ tiền hoa hồng cho người giới thiệu (affiliate). Affiliate tracking đã có (lưu referrer vào localStorage) nhưng tiền chưa đi đâu.

**Cần làm:** Mở rộng module Platform để tạo ví VAE theo white-paper (Bước 3A):
- Input: **`pub` của affiliate**, secret giữa buyer và platform
- Thuật toán: tương tự VSE nhưng dùng public pair của affiliate thay vì seller
- Output: địa chỉ ví VAE (platform toàn quyền, affiliate nhận unlock seed để unlock sau khi đơn hàng thành công)

**Output cụ thể:** Function `deriveVAE(affiliatePairPub, buyerSecret)` trong `src/core/Platform.js` → trả về `{ address, privateKey }`

**Giúp gì cho gọi vốn:** Chứng minh mô hình tăng trưởng — affiliate giới thiệu buyer, nhận hoa hồng tự động qua blockchain. Nhà đầu tư thấy cơ chế viral growth có sẵn trong kiến trúc.

**Tham khảo:** `docs/thoughts/white-paper-draft.md` — Bước 3A

---

### 3. Trang thanh toán (Checkout Flow)

**Vấn đề hiện tại:** Route `/checkout` tồn tại nhưng hoàn toàn trống (chỉ có 1 thẻ div rỗng). Buyer thêm hàng vào giỏ nhưng không có cách nào thanh toán.

**Cần làm:**

*Phần giao diện:*
- Hiển thị danh sách items từ giỏ hàng (tên, số lượng, giá)
- Hiển thị phân tách phí: tiền hàng cho seller, phí sàn cho platform, hoa hồng cho affiliate (nếu có referrer)
- Bộ chọn chain (Ethereum / BSC) và loại tiền thanh toán
- Hiển thị phí gas ước tính
- Nút "Thanh toán" — bấm vào sẽ yêu cầu xác nhận passkey
- Thanh tiến trình (progress bar) trong lúc giao dịch đang xử lý
- Thông báo lỗi rõ ràng nếu thất bại

*Phần logic:*
- Kiểm tra số dư ví: đủ token (tiền hàng + phí) VÀ đủ native coin (ETH/BNB cho gas)
- Nếu không đủ → hiển thị "Thiếu X token" kèm nút chuyển đến trang nạp tiền (`/deposit`)
- Tạo và phát 3 giao dịch on-chain:
  1. Tiền hàng → gửi vào ví VSE (platform cho seller)
  2. Phí sàn → gửi vào ví platform
  3. Hoa hồng affiliate → gửi vào ví VAE (nếu có referrer)
- Lưu thông tin đơn hàng vào GunDB (dữ liệu P2P để seller nhìn thấy)
- Thành công → chuyển sang trang `/order`
- Thất bại → hiển thị lỗi, tiền không mất (giao dịch revert trên blockchain = trở về nguyên trạng)

**Output cụ thể:**
- File giao diện: `src/UI/routes/checkout/template.js` + `src/UI/routes/checkout/index.js` — trang checkout hoàn chỉnh
- File logic: `src/core/Checkout.js` — function `createOrder(cartItems, chain, currency)` → tạo 3 tx, broadcast, lưu GunDB
- Buyer có thể thanh toán thành công trên testnet, thấy giao dịch trên blockchain explorer

**Giúp gì cho gọi vốn:** Đây là **demo quan trọng nhất**. Nhà đầu tư bấm "Thanh toán", thấy 3 giao dịch on-chain, thấy tiền vào ví platform. Chứng minh sản phẩm hoạt động thật, không phải mockup.

**Tham khảo:** `docs/thoughts/white-paper-draft.md` — Bước 3, 3A. Pattern gửi giao dịch: `src/core/Wallet.js` — method `send()`

---

### 4. Trang trạng thái đơn hàng (Order Status)

**Vấn đề hiện tại:** Route `/order` chỉ có text Lorem ipsum giả. Buyer thanh toán xong không biết xem đơn hàng ở đâu.

**Cần làm:**

*Phần giao diện:*
- Danh sách tất cả đơn hàng của user (mới nhất lên trên)
- Mỗi đơn hàng hiển thị: mã đơn, tên items, số tiền, chain, trạng thái, thời gian tạo
- Badge trạng thái có màu: Đang giữ (vàng), Đã release (xanh), Đã hoàn tiền (đỏ)
- Nút hành động: "Xác nhận đã nhận hàng" và "Mở tranh chấp" (khi trạng thái = Đang giữ)

*Phần logic:*
- Đọc dữ liệu đơn hàng từ GunDB
- Cập nhật trạng thái realtime khi có thay đổi (GunDB P2P sync)

**Output cụ thể:** Route `/order` hiển thị đơn hàng thật với data từ GunDB, trạng thái chính xác theo lifecycle

**Giúp gì cho gọi vốn:** Nhà đầu tư thấy đơn hàng với trạng thái "Đang giữ" → chứng minh tiền đang nằm trong platform chờ xác nhận.

---

### 5. Cấu trúc dữ liệu đơn hàng (Order Schema cho GunDB)

**Vấn đề hiện tại:** Chưa có schema nào định nghĩa một đơn hàng gồm những trường gì. Cần thống nhất trước khi code.

**Cần làm:** Định nghĩa và document schema:

```
Order {
  id:           string    — mã đơn hàng unique
  buyer:        string    — public key của buyer
  seller:       string    — public key của seller
  items:        array     — danh sách items [{id, name, quantity, price}]
  amount:       number    — tổng tiền hàng
  platformFee:  number    — phí sàn
  affiliateFee: number    — hoa hồng affiliate (0 nếu không có referrer)
  chain:        number    — chain ID (1 = ETH, 56 = BSC, ...)
  currency:     string    — token thanh toán (USDT, USDC, ETH, BNB...)
  vseAddress:   string    — địa chỉ ví platform seller
  vaeAddress:   string    — địa chỉ ví platform affiliate (null nếu không có)
  txHashes:     array     — [txHash1, txHash2, txHash3] — hash 3 giao dịch on-chain
  status:       string    — "holding" | "released" | "refunded"
  createdAt:    number    — timestamp tạo đơn
  updatedAt:    number    — timestamp cập nhật cuối
  fulfilledAt:  number    — timestamp seller request confirm (null nếu chưa)
}
```

**Output cụ thể:** Document schema + GunDB helper functions (put/get/update order)

**Giúp gì:** Thống nhất giữa logic dev và UI dev, tránh mỗi người hiểu một kiểu.

---

### 6. Trang quản lý đơn hàng cho seller (Inventory)

**Vấn đề hiện tại:** Route `/inventory` hoàn toàn không tồn tại. Seller không có chỗ nào xem đơn hàng cần giao.

**Cần làm:**

*Phần giao diện:*
- Danh sách đơn hàng mà seller cần fulfill (lọc theo seller public key)
- Mỗi đơn: mã đơn, items, số tiền, trạng thái, thời gian
- Nút "Yêu cầu xác nhận" (Request Confirm) — bấm khi đã giao hàng xong

*Phần logic:*
- Đọc orders từ GunDB lọc theo seller
- Khi seller bấm "Yêu cầu xác nhận" → cập nhật `fulfilledAt` timestamp trong GunDB → buyer sẽ thấy qua P2P sync

**Output cụ thể:** Route `/inventory` mới hoàn chỉnh — seller thấy đơn hàng, bấm confirm, buyer nhận được thông báo

**Giúp gì cho gọi vốn:** Chứng minh sản phẩm phục vụ cả hai phía (buyer VÀ seller), không chỉ một chiều.

---

### 7. Cơ chế release tiền cho seller (Seed Release)

**Vấn đề hiện tại:** Chưa có cách nào để tiền từ ví platform VSE đến tay seller. Đây là bước cuối cùng hoàn tất vòng đời đơn hàng.

**Cần làm:** Theo white-paper Bước 4:
- **Buyer xác nhận:** Buyer bấm "Xác nhận đã nhận hàng" → hệ thống gửi seed cho seller qua GunDB → seller dùng seed + xprv derive ra private key của ví VSE → seller rút tiền
- **Auto-release 24h:** Nếu seller đã request confirm nhưng buyer không phản hồi sau 24 giờ → platform tự gửi seed cho seller
  - Cách implement: Mỗi lần buyer hoặc seller mở trang order → kiểm tra `fulfilledAt` timestamp → nếu quá 24h → tự động release

**Output cụ thể:**
- Function `releaseSeed(orderId)` — gửi seed qua GunDB cho seller
- Function `checkAutoRelease(order)` — kiểm tra timestamp, auto-release nếu quá 24h
- Seller nhận seed → derive private key → rút tiền từ ví VSE thành công

**Giúp gì cho gọi vốn:** Chứng minh toàn bộ flow tiền: buyer trả → platform giữ → seller nhận. Vòng đời hoàn chỉnh. Nhà đầu tư thấy tiền thật sự di chuyển trên blockchain, không phải số trong database.

**Tham khảo:** `docs/thoughts/white-paper-draft.md` — Bước 4

---

### 8. Tính toán và phân phối hoa hồng affiliate

**Vấn đề hiện tại:** Hệ thống đã lưu được ai giới thiệu ai (referrer tracking qua URL parameter "r", lưu localStorage). Nhưng khi buyer thanh toán, tiền hoa hồng không được tính và không được gửi đi đâu cả.

**Cần làm:**
- Trong flow checkout: kiểm tra buyer có referrer không (từ `Context.getReferrer()`)
- Nếu có → tính hoa hồng = tổng tiền hàng × affiliate rate %
- Tạo giao dịch thứ 3 gửi hoa hồng vào ví VAE
- Khi đơn hàng thành công → platform gửi seed VAE cho affiliate để unlock tiền

**Output cụ thể:** Affiliate nhận được hoa hồng on-chain khi đơn hàng hoàn tất thành công

**Giúp gì cho gọi vốn:** Chứng minh mô hình viral growth — mỗi user có động lực giới thiệu thêm user mới vì nhận tiền thật on-chain. Nhà đầu tư rất thích mô hình tăng trưởng tự nhiên.

---

## Phần B — Tính năng cần sửa / hoàn thiện (đã có code nhưng chưa đủ)

---

### 9. Tự động cập nhật số dư ví (Balance Auto-polling)

**Vấn đề hiện tại:** Số dư ví chỉ cập nhật khi user đổi chain hoặc currency trong bộ chọn. Nếu có người gửi tiền vào ví, user không thấy số dư thay đổi cho đến khi tự thao tác. Trong buổi demo, nếu nạp tiền mà balance không tự update → trông như sản phẩm bị lỗi.

**Cần làm:**
- Tạo Worker thread chạy nền, cứ mỗi 15-30 giây gọi RPC kiểm tra balance
- Nếu balance thay đổi → cập nhật giao diện tự động
- Dừng polling khi user rời trang wallet

**Output cụ thể:** Sau khi gửi crypto vào ví, số dư tự cập nhật trong vòng 30 giây mà không cần thao tác gì

**Giúp gì cho gọi vốn:** Demo mượt mà — nạp tiền vào, đợi vài giây, số dư tự hiện. Trải nghiệm chuyên nghiệp.

**File cần sửa:** `src/UI/components/wallets/index.js`, có thể cần thêm logic trong `src/core/Threads.js`

---

### 10. Xác nhận passkey trước khi rút tiền

**Vấn đề hiện tại:** Khi user bấm "Rút tiền" trên trang `/withdraw`, giao dịch được gửi đi ngay lập tức mà không yêu cầu xác thực lại. Nếu ai đó truy cập được vào trình duyệt đang đăng nhập → rút hết tiền. Nhà đầu tư chắc chắn sẽ hỏi về bảo mật.

**Cần làm:**
- Trước khi gọi `wallet.send()`, thêm bước gọi `WebAuthn.sign()` để user phải xác nhận bằng vân tay/Face ID
- Nếu user huỷ xác nhận → không gửi giao dịch

**Output cụ thể:** User bấm "Rút tiền" → popup vân tay/Face ID hiện ra → xác nhận → giao dịch mới được gửi

**File cần sửa:** `src/UI/routes/withdraw/index.js` — thêm WebAuthn.sign() trước dòng `wallet.send()` (hiện tại ở khoảng dòng 89)

---

### 11. Kiểm tra trình duyệt có hỗ trợ WebAuthn không

**Vấn đề hiện tại:** Nếu user dùng trình duyệt cũ không hỗ trợ WebAuthn (ví dụ IE, trình duyệt mobile cũ), bấm nút đăng ký/đăng nhập sẽ bị lỗi JavaScript không rõ nguyên nhân. Không có thông báo gì cho user.

**Cần làm:**
- Khi trang load, kiểm tra `window.PublicKeyCredential` tồn tại hay không
- Nếu không hỗ trợ → hiển thị thông báo "Trình duyệt của bạn không hỗ trợ đăng nhập bằng passkey. Vui lòng dùng Chrome, Safari, hoặc Edge phiên bản mới nhất."
- Ẩn hoặc disable nút đăng ký/đăng nhập

**Output cụ thể:** User trên trình duyệt cũ thấy thông báo rõ ràng thay vì lỗi khó hiểu

**File cần sửa:** `src/UI/components/access/` hoặc `src/core/Access.js`

---

### 12. Hiển thị giá đơn vị và variant trong giỏ hàng

**Vấn đề hiện tại:** Giỏ hàng chỉ hiển thị: tên item, tổng giá, số lượng. Không thấy giá mỗi đơn vị là bao nhiêu, và nếu item có nhiều biến thể (ví dụ: màu sắc, size) thì không biết mình đã chọn biến thể nào.

**Cần làm:**
- Thêm hiển thị giá đơn vị (unit price) bên cạnh tổng giá
- Hiển thị variant/attributes đã chọn (nếu có) dưới tên item

**Output cụ thể:** Mỗi item trong giỏ hàng hiện: tên, variant đã chọn, giá đơn vị × số lượng = tổng giá

**File cần sửa:** `src/UI/components/cart/template.js` và `src/UI/components/cart/index.js`

---

### 13. Sửa lỗi hiển thị số dư khi ví trống

**Vấn đề hiện tại:** Code kiểm tra balance có lỗi logic: `typeof balance !== undefined` luôn luôn đúng (vì `typeof` trả về chuỗi, không bao giờ trả về `undefined`). Kết quả: nếu balance là `undefined` hoặc `NaN`, giao diện có thể hiển thị chữ "NaN" hoặc để trống thay vì hiện "0".

**Cần làm:**
- Sửa điều kiện kiểm tra: `if (balance !== undefined && balance !== null && !isNaN(balance))`
- Nếu balance không hợp lệ → hiển thị "0"

**Output cụ thể:** Ví mới (chưa có tiền) luôn hiển thị "0" thay vì trống hoặc NaN

**File cần sửa:** `src/UI/components/wallets/index.js` — dòng 133

---

## Phần C — Quyết định cần thống nhất trước khi code

Những câu hỏi này chặn development — cần có đáp án trước khi bắt tay vào Sprint 1.

| # | Câu hỏi | Ảnh hưởng đến đâu | Cần có đáp án trước |
|---|---------|-------------------|---------------------|
| 1 | Phí sàn (platform fee) là bao nhiêu %? | Không có con số này → không code được trang checkout | Sprint 1 |
| 2 | Hoa hồng affiliate là bao nhiêu %? | Không có con số này → không tính được giao dịch thứ 3 | Sprint 1 |
| 3 | Khi phát 3 giao dịch on-chain: nếu giao dịch 1 thành công nhưng giao dịch 2 thất bại thì xử lý thế nào? Gửi tuần tự (chờ cái trước xong mới gửi cái sau) hay song song? | Ảnh hưởng độ tin cậy của checkout — tiền có thể bị kẹt giữa chừng | Sprint 2 |
| 4 | Khi seller nhận được đơn hàng mới, thông báo bằng cách nào? Trong app? GunDB event? Hay seller phải tự vào trang /inventory kiểm tra? | Ảnh hưởng trải nghiệm seller | Sprint 3 |
