# PRD: AKAO — Decentralized P2P Gaming Marketplace

**Version:** 1.0.0
**Ngày tạo:** 2026-03-24
**Trạng thái:** Draft
**Tác giả:** Duke

---

## Overview

AKAO là nền tảng mua bán in-game items và gaming services dạng P2P (peer-to-peer), hoạt động hoàn toàn trên client-side mà không cần backend server truyền thống.

Nền tảng sử dụng mô hình platform 4 bên (Platform, Buyer, Seller, Affiliate) trên blockchain để đảm bảo giao dịch trustless — không bên nào cần tin tưởng bên nào, tiền chỉ được giải phóng khi điều kiện được thỏa mãn.

User đăng nhập bằng passkey (vân tay/Face ID), không cần password. Ví crypto được derive deterministic từ passkey — mỗi khi user cần dùng ví, hệ thống tự generate địa chỉ on-chain từ credential đã có (không pre-create lúc login).

Cơ chế platform hoạt động qua 2 loại ví đặc biệt:
- **Ví VSE** (Seller-Platform): Ví watch-only từ góc nhìn Buyer/Seller, Platform nắm private key. Tiền item nằm ở đây cho đến khi release.
- **Ví VAE** (Affiliate-Platform): Ví Platform kiểm soát hoàn toàn, Affiliate nhận seed để unlock sau khi order thành công.

(ref: docs/thoughts/white-paper-draft.md — VSE ở dòng 29, VAE ở dòng 34)

**Cho ai:** Gamers muốn trade in-game items/services (buyer), người bán items/services (seller), và người giới thiệu (affiliate).

**Giải quyết gì:** Loại bỏ trung gian tập trung, giảm phí giao dịch, chống scam thông qua platform on-chain.

(ref: README.md, docs/thoughts/white-paper-draft.md, docs/thoughts/dinh-huong-business.md)
