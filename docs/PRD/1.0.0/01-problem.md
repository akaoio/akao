# Problem

## Vấn đề hiện tại

### 1. Rủi ro scam khi trade in-game items

Hiện tại, giao dịch in-game items giữa người chơi chủ yếu diễn ra qua:
- **Các nền tảng tập trung** (G2G, PlayerAuctions, Eldorado...): Phí cao (5-15%), chậm xử lý, dễ bị lock tài khoản, phụ thuộc hoàn toàn vào bên thứ ba
- **Giao dịch trực tiếp** (Discord, Facebook groups): Không có bảo vệ, scam phổ biến, không có cơ chế dispute

### 2. Không có giải pháp trustless cho gaming items

Các marketplace crypto hiện tại (OpenSea, Rarible) tập trung vào NFT, không hỗ trợ:
- In-game items truyền thống (không phải NFT)
- Gaming services (boosting, coaching, account trading)
- Mô hình escrow phù hợp cho delivery không tức thời (seller cần thời gian fulfill)

### 3. Rào cản crypto cho gamers

Đa số gamers không quen với crypto:
- Phải quản lý seed phrase, private key → dễ mất tiền
- Phải hiểu gas fee, chain selection → phức tạp
- Phải dùng Metamask hoặc ví ngoài → nhiều bước

## Evidence

- Mô hình escrow 4 bên được thiết kế để giải quyết trust problem giữa buyer/seller, trong đó platform giữ quyền kiểm soát ví escrow nhưng không giữ tiền trực tiếp (ref: docs/thoughts/white-paper-draft.md)
- Auth bằng WebAuthn passkey thay vì password/seed phrase — giảm rào cản crypto (ref: src/core/Access.js, docs/WebAuthn-PRF-Extension.md)
- Ví được derive tự động từ passkey, user không cần quản lý private key (ref: src/core/Wallet.js)

[ASSUMPTION] Chưa có data cụ thể về tỷ lệ scam hoặc phí trung bình của các nền tảng hiện tại. Cần thu thập market research trước khi set target chính xác.
