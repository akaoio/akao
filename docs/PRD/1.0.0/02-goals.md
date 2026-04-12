# Goals & Non-Goals

## Goals

### Phase 1 — MVP (Target: 2 tháng)

1. **User trade được in-game items qua platform on-chain** — Buyer thanh toán, tiền vào ví platform, seller giao hàng, buyer confirm, tiền release
2. **User đăng nhập không cần password** — Passkey (vân tay/Face ID) là phương thức duy nhất, ví crypto derive on-demand từ passkey credential
3. **User nạp/rút crypto dễ dàng** — Deposit và withdraw trực tiếp trên nền tảng, không cần ví ngoài
4. **User theo dõi được trạng thái giao dịch** — Platform status (Holding/Released/Disputed) hiển thị rõ ràng
5. **Hỗ trợ multi-chain** — ETH và BSC làm 2 chain chính
6. **Hỗ trợ đa ngôn ngữ** — 18 locales, hoạt động đúng trên tất cả ngôn ngữ bao gồm RTL (Arabic, Hebrew, Urdu)
7. **Hỗ trợ đa tiền tệ** — Hiển thị giá theo 40+ fiat currencies, thanh toán bằng crypto (stablecoins + native coin)

### Metric kỳ vọng

- Metric chưa có, cần baseline sau khi launch MVP trước khi set target cụ thể
- Các metric cần theo dõi: số giao dịch thành công, dispute rate, thời gian trung bình hoàn thành order, conversion rate từ browse → checkout

## Non-Goals (không thuộc Phase 1)

1. **Không làm DAO governance** — Không có AKAO token, không có staking, không có voting (Phase 4+)
2. **Không làm DEX swap trong app** — Dex.js đã integrate nhưng UI chưa expose, không thuộc MVP
3. **Không cho seller tự list items** — Phase 1 items do platform curate (static YAML), seller self-listing là Phase 2
4. **Không hỗ trợ chain ngoài ETH/BSC** — Chỉ 2 mainnet + 2 testnet
5. **Không làm admin dashboard** — Quản lý platform qua code/config trong Phase 1
6. **Không làm mobile app** — Web-only, responsive design
7. **Không tích hợp fiat payment gateway** — Chỉ crypto payments

(ref: docs/thoughts/dinh-huong-business.md — roadmap 4 phases)
