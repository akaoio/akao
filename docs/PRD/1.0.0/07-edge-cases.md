# Edge Cases (Business-level)

Chỉ mô tả edge case ở mức business logic. Edge case kỹ thuật (RPC failover, GunDB sync, Worker crash...) thuộc Tech Spec.

---

## Escrow & Payment

| Edge Case | Expected Behavior |
|-----------|-------------------|
| Buyer thanh toán nhưng transaction chưa confirm (pending on-chain) | Hiển thị status "Đang xử lý", buyer chờ cho đến khi block confirm. Không cho phép tạo order mới cho cùng items trong cart |
| Buyer thanh toán xong nhưng seller không phản hồi (không giao hàng, không request confirm) | Tiền nằm trong ví escrow VSE. Buyer có thể mở dispute sau thời gian chờ hợp lý. [TBD] Cần confirm: Thời gian chờ tối đa trước khi buyer được mở dispute là bao lâu? |
| Seller đã request confirm nhưng buyer bỏ mặc (không confirm, không dispute) | Sau 24h kể từ khi seller request → Hệ thống tự động release tiền cho seller (ref: white-paper Bước 4) |
| Buyer và seller dispute — cả hai đều claim đúng | Platform phân xử dựa trên proof. Nếu không đủ evidence → [TBD] Cần confirm policy: thiên về buyer hay seller? |
| Buyer thanh toán cho item nhưng item hết hàng / không còn khả dụng | [TBD] Cần confirm: Hệ thống check inventory trước khi cho checkout? Hiện tại items là static, chưa có stock management |
| Gas fee tăng đột biến giữa lúc user xem checkout và bấm thanh toán | Gas fee estimation hiển thị ở checkout là ước tính. Nếu gas thực tế > balance → transaction fail, tiền không mất, user retry |
| User thanh toán bằng stablecoin nhưng stablecoin mất peg | Hệ thống không chịu trách nhiệm về biến động giá. Giá item tính theo fiat, thanh toán theo tỷ giá crypto tại thời điểm checkout |

## Wallet & Chain

| Edge Case | Expected Behavior |
|-----------|-------------------|
| User nạp tiền vào sai chain (gửi ETH mainnet nhưng ví đang chọn BSC) | Tiền vào chain đúng nhưng user không thấy nếu đang xem chain khác. UI nên cảnh báo khi user switch chain: "Bạn có balance trên chain khác" |
| User nạp token không được hỗ trợ | Token vẫn vào ví on-chain nhưng không hiển thị trong UI (chỉ hiển thị currencies được configure). User cần dùng ví ngoài để quản lý |
| User rút tiền nhưng nhập sai địa chỉ | Hệ thống validate format address trước khi broadcast. Nhưng nếu address hợp lệ format mà sai người nhận → không thể reverse (on-chain = final) |
| User có balance trên nhiều chain, muốn thanh toán item bằng chain không đủ balance | Hiển thị balance từng chain. Nếu chain đã chọn không đủ → suggest chuyển sang chain có đủ balance hoặc deposit thêm |

## Authentication

| Edge Case | Expected Behavior |
|-----------|-------------------|
| User mất thiết bị (mất passkey) | Phase 1: Không có recovery flow → mất quyền truy cập. Phase 2: Wave signin cho phép transfer auth sang thiết bị mới. [TBD] Cần confirm: Có cần recovery mechanism nào cho Phase 1 không? |
| User xóa passkey khỏi trình duyệt/thiết bị | Tương tự mất thiết bị. Ví và tiền vẫn tồn tại on-chain nhưng user không access được qua platform |
| Trình duyệt không hỗ trợ WebAuthn | Hiển thị thông báo "Trình duyệt không hỗ trợ". Không cho phép đăng ký/đăng nhập. Suggest dùng Chrome/Safari/Edge mới nhất |
| User đăng nhập từ trình duyệt khác trên cùng thiết bị | Passkey sync phụ thuộc vào OS (iCloud Keychain, Google Password Manager, Windows Hello). Nếu OS sync passkey → login OK. Nếu không → cần Wave signin hoặc tạo passkey mới |

## Affiliate

| Edge Case | Expected Behavior |
|-----------|-------------------|
| Buyer mua hàng qua affiliate link nhưng link đã hết hạn hoặc invalid | Buyer vẫn mua được hàng bình thường, chỉ không có affiliate. Không hiển thị error liên quan affiliate cho buyer |
| Affiliate link trỏ đến item không còn tồn tại | Hiển thị "Item không tồn tại" (404), buyer có thể browse items khác |
| Buyer mua hàng, dispute xảy ra, buyer thắng → affiliate đã nhận tiền chưa? | Theo white-paper: Tiền affiliate nằm trong ví VAE, chỉ được unlock khi Platform gửi seed. Nếu buyer thắng dispute → Platform refund ví VAE cho buyer, affiliate không nhận được |

## Multi-language & Currency

| Edge Case | Expected Behavior |
|-----------|-------------------|
| Item chưa có translation cho ngôn ngữ user chọn | Fallback sang English. Nếu không có English → hiển thị key hoặc để trống, không crash |
| Forex rate API không khả dụng (server build offline) | Dùng rate từ file forex.json đã cache (build-time). Rate có thể cũ nhưng app vẫn hoạt động |
| User chọn currency không phổ biến (ví dụ: NGN — Nigerian Naira) mà không có direct rate | Hệ thống tính cross-rate qua USD (intermediary). Nếu vẫn không có → [TBD] hiển thị giá USD gốc + warning? |
