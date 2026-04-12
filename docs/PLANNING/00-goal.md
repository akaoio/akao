# Mục tiêu MVP — 2 tháng

**Thời gian:** 2026-03-24 → 2026-05-24 (8 tuần, chia thành 4 sprint, mỗi sprint 2 tuần)
**Team:** 3 người — 1 senior xử lý logic, 1 senior xử lý giao diện, 1 xử lý documents và điều phối
**Mục đích cuối cùng:** Có một bản demo hoàn chỉnh để trình bày cho nhà đầu tư, chứng minh được tiền chạy từ đầu đến cuối trên blockchain thông qua hệ thống platform

---

## Vấn đề mà sản phẩm đang giải quyết

Thị trường mua bán vật phẩm game hiện tại có 3 vấn đề lớn:

1. **Rủi ro lừa đảo:** Buyer trả tiền nhưng không nhận được hàng, hoặc seller giao hàng nhưng không nhận được tiền. Không có bên trung gian đáng tin cậy giữ tiền.
2. **Phí trung gian cao:** Các sàn truyền thống (G2G, PlayerAuctions) thu phí 5-15% và giữ tiền trong hệ thống tập trung — user phải tin tưởng sàn.
3. **Rào cản thanh toán quốc tế:** Game thủ ở nhiều quốc gia khác nhau, thanh toán fiat xuyên biên giới phức tạp và tốn phí.

**Giải pháp của AKAO:** Platform on-chain — tiền nằm trong ví blockchain do smart contract logic kiểm soát, không bên nào (kể cả sàn) có thể lấy tiền tùy ý. Buyer thanh toán → tiền vào platform → seller giao hàng → tiền release. Minh bạch, kiểm chứng được trên blockchain.

---

## Nhà đầu tư cần thấy gì trong buổi demo?

| Điều cần chứng minh | Tại sao quan trọng |
|---------------------|--------------------|
| Buyer thanh toán → tiền vào ví platform on-chain (thấy được trên blockchain explorer) | Chứng minh tiền thật sự nằm trên blockchain, không phải database nội bộ |
| 3 giao dịch tách biệt: tiền cho seller, phí sàn, hoa hồng affiliate | Chứng minh mô hình kinh doanh có doanh thu (phí sàn) và có cơ chế tăng trưởng (affiliate) |
| Seller giao hàng → buyer xác nhận → tiền release cho seller | Chứng minh vòng đời đơn hàng hoàn chỉnh, platform hoạt động đúng |
| Tự động release sau 24h nếu buyer không phản hồi | Chứng minh hệ thống không bị kẹt, seller được bảo vệ |
| Toàn bộ flow chạy trên testnet không cần backend server | Chứng minh kiến trúc serverless — chi phí vận hành thấp, scale dễ |

---

## Những gì nằm trong phạm vi MVP (phải làm xong)

1. **Checkout và thanh toán platform** — flow cốt lõi, không có cái này thì không có sản phẩm
2. **Trang trạng thái đơn hàng** — buyer thấy tiền đang "giữ" trong platform
3. **Seller giao hàng và buyer xác nhận** — hoàn tất vòng đời đơn hàng
4. **Tự động release sau 24h** — bảo vệ seller khi buyer không phản hồi
5. **Tự động cập nhật số dư ví** — trải nghiệm người dùng cơ bản, demo mà balance không tự update thì trông như bị lỗi
6. **Tính toán và phân phối hoa hồng affiliate** — chứng minh mô hình tăng trưởng
7. **Xác nhận passkey trước khi rút tiền** — bảo mật cơ bản, nhà đầu tư sẽ hỏi

## Những gì cắt khỏi MVP (làm sau)

| Feature | Lý do cắt |
|---------|----------|
| Giao diện giải quyết tranh chấp (dispute) | Ràng buộc trong PRD đã ghi: Phase 1 giải quyết tranh chấp ngoài hệ thống, không cần giao diện |
| Đăng nhập trên thiết bị mới qua sóng âm (Wave Signin) | Tính năng P2, không ảnh hưởng demo core flow |
| Trang quản lý hồ sơ cá nhân (Profile) | Tính năng P2, không ảnh hưởng demo core flow |
| Lịch sử chi tiết platform (timeline) | Nice-to-have, không cần cho lần demo đầu |
| Cải thiện trang tag (thêm tìm kiếm, lọc, sắp xếp) | Cosmetic, trang game đã có đầy đủ, đủ demo |

---

## Mốc kiểm tra tiến độ

| Tuần | Mốc | Nếu đạt thì chứng minh được |
|------|-----|------------------------------|
| Tuần 2 | Tạo được ví platform trên testnet từ key của buyer + seller | Thuật toán derivation hoạt động, nền tảng kỹ thuật vững |
| Tuần 4 | Buyer thanh toán thành công, tiền vào ví platform, thấy trên blockchain explorer | **Checkpoint quan trọng nhất** — core value proposition hoạt động |
| Tuần 6 | Buyer mua → Seller giao → Buyer xác nhận → Tiền release | Vòng đời đơn hàng hoàn chỉnh |
| Tuần 8 | Demo flow mượt mà, xử lý được các trường hợp lỗi | Sẵn sàng demo cho nhà đầu tư |

**Nếu tuần 4 mà checkout chưa chạy được trên testnet → phải họp lại đánh giá scope ngay lập tức.**
