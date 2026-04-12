# Timeline — 4 Sprint × 2 tuần

---

## Sprint 1 (Tuần 1-2): Nền tảng — Tạo ví platform + Dựng giao diện khung

**Mục tiêu sprint:** Cuối tuần 2, tạo được ví platform VSE và VAE trên testnet. Giao diện checkout và order đã có khung, chờ wire logic.

### Phần logic

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Viết module tạo ví platform VSE | File `src/core/Platform.js` — function `deriveVSE()` chạy đúng trên testnet Sepolia + BSC Testnet | #1 |
| Viết function tạo ví platform VAE | Function `deriveVAE()` trong cùng file `Platform.js` | #2 |
| Định nghĩa cấu trúc dữ liệu đơn hàng + helpers GunDB | Document schema + functions `createOrder()`, `getOrder()`, `updateOrder()` trong `src/core/Order.js` | #5 |

### Phần giao diện

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Xây trang checkout: danh sách items, phân tách phí, chọn chain/currency, nút thanh toán | Route `/checkout` có giao diện hoàn chỉnh (chưa cần chạy logic, dùng data giả để dựng UI) | #3 |
| Xây trang order: danh sách đơn hàng, badge trạng thái, nút xác nhận/tranh chấp | Route `/order` có giao diện hoàn chỉnh (chưa cần data thật) | #4 |

### Phần coordination

| Task | Output cụ thể |
|------|---------------|
| Thống nhất đáp án: phí sàn %, hoa hồng affiliate % | Document ghi rõ con số, team đồng ý |
| Thống nhất cấu trúc dữ liệu đơn hàng | Schema document, cả logic và UI dev đều hiểu giống nhau |

---

## Sprint 2 (Tuần 3-4): Thanh toán end-to-end

**Mục tiêu sprint:** Cuối tuần 4, buyer có thể thanh toán thành công trên testnet — thấy 3 giao dịch on-chain trên blockchain explorer. **Đây là mốc quan trọng nhất.**

### Phần logic

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Viết logic phát 3 giao dịch on-chain (tiền hàng → VSE, phí sàn → platform, hoa hồng → VAE) | File `src/core/Checkout.js` — function tạo + ký + phát 3 tx, trả về tx hashes | #3 |
| Viết kiểm tra số dư trước checkout (đủ token + đủ gas) | Function trả về `{ sufficient: true/false, missing: X token, missingGas: Y }` | #3 |
| Viết tự động cập nhật số dư ví (polling) | Worker thread gọi RPC mỗi 15-30 giây, balance tự update trên giao diện | #9 |

### Phần giao diện

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Kết nối trang checkout với logic: loading, progress bar, thông báo lỗi | Buyer bấm "Thanh toán" → thấy progress → thành công chuyển sang /order, hoặc thất bại thấy lỗi rõ ràng | #3 |
| Kết nối trang order với dữ liệu GunDB: hiển thị đơn hàng thật | Trang /order hiển thị đơn hàng vừa tạo với trạng thái "Đang giữ" | #4 |

### Phần coordination

| Task | Output cụ thể |
|------|---------------|
| Test trên testnet: thanh toán happy path + các trường hợp lỗi (thiếu tiền, tx revert, gas spike) | Checklist test + danh sách bug cần fix |
| Quyết định: 3 tx gửi tuần tự hay song song? Xử lý partial failure thế nào? | Quyết định document |

### 🎯 Checkpoint tuần 4: Nếu checkout chưa chạy → họp đánh giá scope ngay

---

## Sprint 3 (Tuần 5-6): Flow seller + Vòng đời đơn hàng

**Mục tiêu sprint:** Cuối tuần 6, chạy được toàn bộ vòng đời: buyer mua → seller giao → buyer xác nhận → tiền release.

### Phần logic

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Viết state machine đơn hàng: Đang giữ → Đã release / Đã hoàn tiền | Function `updateOrderStatus()` — thay đổi trạng thái trong GunDB, kiểm tra transition hợp lệ | #5 |
| Viết cơ chế gửi seed cho seller khi buyer xác nhận | Function `releaseSeed()` — buyer bấm confirm → seed gửi qua GunDB → seller derive private key → rút tiền | #7 |
| Viết auto-release 24h: tự release nếu buyer không phản hồi | Function `checkAutoRelease()` — kiểm tra `fulfilledAt` timestamp, nếu quá 24h → tự gửi seed | #7 |

### Phần giao diện

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Xây trang /inventory cho seller: danh sách đơn, nút "Yêu cầu xác nhận" | Route mới `/inventory` hoàn chỉnh, seller thấy đơn hàng, bấm confirm | #6 |
| Kết nối nút "Xác nhận đã nhận" và "Mở tranh chấp" trên trang /order | Buyer bấm xác nhận → trạng thái đổi → tiền release | #4, #7 |
| Thêm xác nhận passkey trước khi rút tiền | Bấm rút tiền → popup vân tay → xác nhận → mới gửi tx | #10 |

### Phần coordination

| Task | Output cụ thể |
|------|---------------|
| Test end-to-end trên testnet: toàn bộ flow buyer → seller | Báo cáo test: mua → giao → confirm → release chạy thành công |

### 🎯 Milestone tuần 6: Full order lifecycle hoạt động

---

## Sprint 4 (Tuần 7-8): Hoàn thiện + Chuẩn bị demo

**Mục tiêu sprint:** MVP sẵn sàng demo cho nhà đầu tư. Xử lý các trường hợp lỗi, polish giao diện, chuẩn bị kịch bản demo.

### Phần logic

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Tính toán và phân phối hoa hồng affiliate trong checkout | Khi buyer có referrer → giao dịch thứ 3 gửi hoa hồng vào ví VAE, affiliate nhận seed khi order thành công | #8 |
| Xử lý các trường hợp lỗi: giao dịch revert, gas tăng đột biến, partial payment failure | Code defensive: retry, thông báo lỗi cụ thể, tiền không bị kẹt | #3 |
| Kiểm tra trình duyệt hỗ trợ WebAuthn | Trình duyệt cũ → hiện thông báo rõ ràng thay vì lỗi JavaScript | #11 |
| Sửa lỗi hiển thị balance NaN | Ví trống luôn hiện "0" | #13 |

### Phần giao diện

| Task | Output cụ thể | Liên quan feature # |
|------|---------------|---------------------|
| Cải thiện giỏ hàng: hiện giá đơn vị + variant đã chọn | Mỗi item trong giỏ: tên, variant, đơn giá × số lượng = tổng | #12 |
| Polish toàn bộ: loading states, thông báo lỗi, responsive, toast messages | Giao diện mượt, không có trạng thái "treo" hay lỗi không rõ | Tất cả |
| Cải thiện trang tag: thêm tìm kiếm/lọc (nếu còn thời gian) | Tuỳ chọn — chỉ làm nếu hoàn thành sớm | — |

### Phần coordination

| Task | Output cụ thể |
|------|---------------|
| Test regression toàn bộ trên testnet | Danh sách bug = 0 critical, sign-off release candidate |
| Chuẩn bị kịch bản demo cho nhà đầu tư | Script chi tiết: mở app → đăng ký → nạp tiền → mua hàng → seller giao → confirm → tiền release |

### 🎯 Milestone tuần 8: MVP sẵn sàng demo

---

## Rủi ro và cách giảm thiểu

| Rủi ro | Hậu quả | Cách giảm thiểu |
|--------|---------|-----------------|
| Thuật toán tạo ví platform (VSE/VAE) phức tạp hơn dự kiến | Trễ Sprint 1-2, ảnh hưởng toàn bộ timeline | Prototype trên testnet ngay tuần 1, validate với white-paper sớm |
| 3 giao dịch on-chain — giao dịch thứ 2 thất bại giữa chừng | Tiền bị kẹt: 1 phần trong platform, 1 phần chưa gửi | Gửi tuần tự, kiểm tra mỗi tx thành công trước khi gửi tiếp. Nếu fail giữa chừng → lưu trạng thái để retry |
| GunDB đồng bộ P2P chậm hoặc không ổn định | Seller không thấy đơn hàng, buyer không thấy cập nhật | Thiết kế local-first: lưu IndexedDB trước, GunDB sync nền. Nếu sync chậm thì data local vẫn đúng |
| Timer 24h auto-release không hoạt động nếu không ai mở app | Tiền bị kẹt trong platform vô thời hạn | Không dùng timer chạy nền. Kiểm tra timestamp mỗi lần ai đó mở trang → trigger release nếu quá 24h |
| 1 trong 3 thành viên nghỉ → mất 33% capacity | Trễ 1 sprint | Sprint 4 có buffer cho unexpected. Nếu trễ nhiều → cắt feature #8 (affiliate) và #12 (cart UX) |
