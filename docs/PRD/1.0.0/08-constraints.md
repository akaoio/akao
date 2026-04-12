# Constraints

Các ràng buộc sản phẩm áp dụng cho Phase 1 (MVP).

---

## Platform

| Constraint | Chi tiết |
|------------|----------|
| Không có backend server | Toàn bộ logic chạy client-side. Không có REST API, không có database server, không có server-side rendering |
| Static deployment | Deploy dưới dạng static files lên Netlify, Vercel, S3, hoặc GitHub Pages. Không cần server runtime |
| Offline-first | Data được cache locally. Cơ bản app vẫn hoạt động khi offline (browse items, xem cart). Các thao tác on-chain yêu cầu internet |
| Không có admin UI | Phase 1 quản lý qua code/config. Items curated bằng YAML files, disputes resolved ngoài hệ thống |

## Blockchain

| Constraint | Chi tiết |
|------------|----------|
| Chỉ ETH + BSC | 2 mainnet (Ethereum chain ID 1, BSC chain ID 56) + 2 testnet (Sepolia 11155111, BSC Testnet 97) |
| Chỉ crypto payment | Không hỗ trợ fiat payment gateway (Stripe, PayPal...). User phải nạp crypto trước |
| Gas fee do user chịu | Mỗi giao dịch yêu cầu native coin (ETH/BNB) cho gas fee, ngoài token amount |
| On-chain = final | Giao dịch đã confirm on-chain không thể reverse. Refund chỉ thực hiện bằng giao dịch mới từ ví platform |
| Stablecoins ưu tiên | Payment currency ưu tiên: USDT, USDC, BUSD. Hỗ trợ thêm native coin (ETH, BNB) |

## Authentication

| Constraint | Chi tiết |
|------------|----------|
| Chỉ WebAuthn passkey | Không hỗ trợ password, email/OTP, social login. Yêu cầu trình duyệt hỗ trợ WebAuthn |
| Không có recovery (Phase 1) | Mất passkey = mất quyền truy cập. Wave signin (Phase 2) sẽ là giải pháp recovery |
| Browser dependency | WebAuthn support: Chrome 67+, Safari 14+, Edge 79+, Firefox 60+. Không hỗ trợ IE, browsers cũ |

## i18n

| Constraint | Chi tiết |
|------------|----------|
| 18 locales | en, zh, es, fr, de, ja, ko, ru, ar, hi, pt, it, vi, th, he, ur, zh-TW, no (ref: src/statics/locales.yaml) |
| RTL support | Arabic (ar), Hebrew (he), Urdu (ur) — layout phải hiển thị đúng phải-sang-trái |
| Build-time i18n | Routes static per locale (/en/, /vi/, /ar/...). Thay đổi locale = thay đổi URL |
| 44 fiat currencies | Hiển thị giá, không phải payment currency. Forex rates cập nhật từ API lúc build (ref: src/statics/fiats.yaml) |

## Items & Catalog

| Constraint | Chi tiết |
|------------|----------|
| Platform-curated (Phase 1) | Items do platform thêm vào qua YAML files lúc build. Seller không tự list được |
| Pagination 10 items/trang | Config trong system.yaml, pagination = 10 |
| In-game items only | Focus: gaming items, services, accounts. Không phải general marketplace |

## Multi-site

| Constraint | Chi tiết |
|------------|----------|
| Multi-tenant | Mỗi domain có thể config branding, chains, locale, fiat riêng (ref: src/statics/sites/, src/statics/domains.yaml) |
| Sites hiện có | localhost (dev, 4 chains), mimiza (production, pending setup) |

## Performance

| Constraint | Chi tiết |
|------------|----------|
| No framework | Pure Web Components, không React/Vue/Angular. Ảnh hưởng đến UX patterns có thể implement |
| No CDN (Phase 1) | Static files serve trực tiếp. CDN integration là Phase 3 |
| Hash-validated caching | Mỗi static file có SHA-256 hash. Client check hash trước khi dùng cache → đảm bảo data fresh (ref: src/core/DB.js) |
