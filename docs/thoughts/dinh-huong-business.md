ĐỊNH HƯỚNG BUSINESS

Mô hình: sàn thương mại in-game items, gamming services

Ban đầu, khi bắt đầu dự án này tôi vốn chỉ định xây dựng một hệ thống cho riêng hoạt động kinh doanh dropshipping các sản phẩm vật lý của mình. Khi đó, các items chỉ cần là dữ liệu tĩnh, được lưu dạng json. Trong quá trình kinh doanh dropshipping sản phẩm vật lý từ VN/CN qua US, tôi nhận thấy shipping fee là một vấn đề nan giải, nên đã quyết định chuyển qua mô hình in-game item/service platform. Nay nhận thấy lý thuyết mã hóa đã hoàn thiện hơn, các công nghệ lõi như gun đã trưởng thành, tôi quyết định xây dựng nền tảng này thành một sàn thương mại in-game item/service P2P phi tập trung nhưng trong đó sàn (tôi) có toàn quyền.

**Lý thuyết:** xem [./white-paper-draft.md]

## HIỆN TRẠNG HỆ THỐNG

- Đã hoàn thiện các core libs
- Đang tích hợp lại Chain, Dex, Wallets từ repo cũ (wallet)
- User chưa có chức năng:
    - List items
    - Nạp rút token/coin (core libs đã hoàn thiện, thiếu UI)
    - Giao dịch items/services, quản lý orders
- Chưa có admin
    - Index items
    - Quản lý orders

## KẾ HOẠCH

**Mục tiêu:** ra MVP trong vòng 2 tháng!

### Giai đoạn 1 - 1 tháng:
**Mục tiêu:** dựng UI thô, chưa cần đẹp, quan trọng là chạy được theo đúng lý thuyết
- Nạp/rút token/coin (UI + logic hoàn chỉnh)
- Dựng chức năng cơ bản:
    - trang cá nhân public, có about, danh sách items/services
    - list item (chia theo game, digital/physical/service, processing time)
    - order (thanh toán bằng USDT trên BSC và ETH chains, sau này sẽ thêm token AKAO)
    - review order, review item/service, review seller/buyer
    - dispute

### Giai đoạn 2 - 1 tháng
**Mục tiêu:** tích hợp 
- Thêm chức năng affiliate
- Cải thiện UI/UX
- Test và launch MVP

### Giai đoạn 3 - Tăng cường bảo mật - 2 tháng
**Mục tiêu:** tăng cường bảo mật, vì hiện nay để phát triển nóng thì phần lớn tác vụ liên quan đến giao dịch vẫn đang nằm trên main thread. Sau này cần đưa dần vào worker threads.
- Giấu pairs, seeds vào trong worker thread, chống bị extension trình duyệt tấn công (tất cả libs/primitives giao tiếp và làm nền cần thiết đều đã hoàn thiện, chỉ việc implement)
- Mọi tác vụ encrypt/decrypt, sign đều diễn ra trên worker
- Main thread chỉ biết pub key để verify
- Củng cố I/O của gun để chống spam dữ liệu vào graph (ép schema của gun graph, và client phải mining nhẹ khi ghi dữ liệu)
- Tích hợp cloudflare, phân tán tài nguyên lên các CDN, giảm thiểu DDOS

### Giai đoạn 4 - Tầm nhìn tương lai - 1 năm
- Cân nhắc: Triển khai hệ thống automation (n8n, openclaw) tích hợp AI agent chạy trên terminal/server hỗ trợ quản trị, tham gia dispute
- Cân nhắc: Triển khai AI agent cỡ nhỏ chạy offline trên trình duyệt, hỗ trợ mua bán, index hàng hóa, điều phối nguồn vốn
- Chức năng bargain/bid -> sàn đấu giá in-game item
- Tạo/mint token AKAO trên BSC và ETH chains, có thể cân nhắc cả mạng TRON
- Tạo pool AKAO/USDT trên Pancakeswap và Uniswap
- Tích hợp token AKAO và pool AKAO/USDT vào hệ thống
- Anh em đội dev/sales/founders hưởng ưu đãi mua token giá siêu rẻ
- IPO gọi vốn cộng đồng, thuyết trình to the moon
- Offer AKAO + chính sách affiliate cho các game streamers, KOLs, KOL managers, community admins
- Tạo gói staking nhận lãi AKAO, offer lên các sàn crypto
- Triển khai DAO, ai stake nhiều AKAO thì được làm "admin", có quyền nhận profit trên order, tham gia vote các quyết định quan trọng