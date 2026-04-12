# User Personas

Hệ thống AKAO có 3 roles chính, dựa trên mô hình platform 4 bên (bên thứ 4 — Platform/Platform — là hệ thống, không phải user).

(ref: docs/thoughts/white-paper-draft.md — E, B, S, A)

---

## Buyer (B)

**Ai:** Gamer muốn mua in-game items hoặc gaming services.

**Hành vi chính:**
- Browse và tìm kiếm items theo game, tag, rarity
- Thêm items vào cart, checkout và thanh toán bằng crypto
- Nạp crypto vào ví trên sàn (deposit)
- Theo dõi trạng thái order (platform status)
- Confirm nhận hàng để release tiền cho seller
- Mở dispute nếu chưa nhận hàng hoặc sai hàng

**Kỳ vọng:**
- Không cần biết về blockchain/crypto ở mức kỹ thuật
- Thanh toán nhanh, ít bước
- Tiền được bảo vệ cho đến khi nhận hàng

---

## Seller (S)

**Ai:** Người bán in-game items hoặc cung cấp gaming services.

**Hành vi chính:**
- Xem danh sách order cần fulfill trên trang /inventory
- Giao hàng/hoàn thành service cho buyer
- Request buyer confirm nhận hàng
- Nhận tiền sau khi buyer confirm (hoặc auto-release sau 24h)
- Rút tiền từ ví trên sàn về ví ngoài (withdraw)
- Cung cấp proof nếu có dispute

**Kỳ vọng:**
- Nhận tiền đúng hạn sau khi hoàn thành giao dịch
- Được bảo vệ khỏi buyer gian lận (24h auto-release)
- Dễ dàng quản lý orders

**[DECISION NEEDED]** Phase 1: Seller có tự list items được không, hay chỉ platform-curated? Hiện tại items là static YAML do platform quản lý (ref: src/statics/items/).

---

## Affiliate (A)

**Ai:** Người giới thiệu buyer vào sàn để mua hàng, nhận hoa hồng.

**Hành vi chính:**
- Tạo referral link cho items cụ thể
- Chia sẻ link cho buyer (qua Discord, social media, stream...)
- Nhận hoa hồng khi buyer mua hàng qua link

**Kỳ vọng:**
- Commission tự động, không cần claim thủ công
- Theo dõi được referral performance

**[TBD]** Affiliate rate cụ thể là bao nhiêu %? Cần confirm. White-paper mô tả flow nhưng không specify rate. (ref: docs/thoughts/white-paper-draft.md — Bước 2, 3A)
