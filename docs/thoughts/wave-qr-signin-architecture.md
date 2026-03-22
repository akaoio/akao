# Wave + QR Signin Architecture (WIP)

## Mục tiêu
- Bổ sung phương thức signin offline-first song song với passkey.
- Tận dụng 2 kênh trao đổi song song:
  - Âm thanh qua ggwave (`ui-wave` + worker `wave`).
  - Hình ảnh qua QR (`ui-qr`).
- Cho phép thiết bị đã đăng nhập (A) chuyển seed đăng nhập cho thiết bị mới (B) theo phiên tạm thời.

## Thành phần chính

### 1) `src/core/threads/wave.js`
Worker chuyên trách codec ggwave:
- `configure(params)`
  - Khởi tạo / cập nhật thông số sample rate, protocol, volume.
  - Duy trì 2 instance riêng cho encode/decode.
- `encode({ message, protocolId, volume })`
  - Trả về byte PCM 16-bit để phát âm thanh.
- `decode({ bytes })`
  - Nhận chunk PCM 16-bit từ micro.
  - Trả `found=true` + `message` khi giải mã thành công.
- `reset()`
  - Giải phóng và dựng lại instance.
- `destroy()`
  - Cleanup toàn bộ instance/module reference.

### 2) `src/core/Wave.js` + `src/core/Wave/worker.js`
- `Wave` vẫn giữ instance API cho worker side (`init/check/request`).
- Thêm static API cho main thread (không `new Wave()` ở UI):
  - `Wave.setup()`
  - `Wave.encode()`
  - `Wave.decode()`
  - `Wave.reset()`
  - `Wave.dispose()`
- `src/core/Wave/worker.js` là cầu nối queue/callback đến thread `wave`.

### 3) `src/UI/components/wave/*`
`ui-wave` là orchestrator giữa:
- microphone + WebAudio,
- wave worker,
- QR encode/scan.

Khả năng chính:
- `listen()` mở micro, stream audio chunk vào `Wave.decode()`.
- `send(payload)`
  - serialize payload,
  - render QR outgoing,
  - encode qua `Wave.encode()`,
  - phát ra loa (`play`).
- `startSignin()` (role receiver)
  - tạo SEA pair tạm,
  - broadcast `{"~": epubB, ":": "auth"}` qua QR + wave,
  - chờ phản hồi chứa seed mã hóa.
- `startShare(builder)` (role sender)
  - nghe request auth,
  - gọi callback builder để tạo payload phản hồi.
- Khi nhận seed thành công, phát event `signin` với `detail: { seed, from, channel }`.

### 4) `src/UI/components/access/index.js`
- `wave()` (UI action) giờ chỉ mở `ui-wave` và gọi `startSignin()`.
- Lắng nghe event `signin` từ `ui-wave`, sau đó gọi `core/Access.wave(detail)` để hoàn tất đăng nhập.
- Bổ sung public methods:
  - `openWaveSignin()`
  - `openWaveShare()`

### 5) `src/core/Access.js`
- `wave({ seed, id })` đã được hiện thực:
  - dùng `next()` để rebuild pair từ seed,
  - gọi `restore()` để kéo lại `pub` (nếu thiếu),
  - set `authenticated=true` như passkey flow.

### 6) `src/UI/components/footer/index.js`
Icon `#wave-auth` trong footer:
- Nếu đã authenticated: mở `openWaveShare()` (vai trò A).
- Nếu chưa authenticated: mở `openWaveSignin()` (vai trò B).

### 7) Showcase
- Thêm story `Wave` tại `src/UI/routes/showcase/stories/wave.js` để demo trực tiếp phát/thu wave + QR.

## Luồng đăng nhập A ↔ B (đang chạy)

### B (chưa đăng nhập)
1. Mở wave signin.
2. Tạo pair tạm `epubB/eprivB`.
3. Broadcast request: `{"~": epubB, ":": "auth"}` qua:
   - `ui-qr` encode,
   - ggwave audio.

### A (đã đăng nhập)
1. Mở wave share (từ footer icon).
2. Nghe request từ QR hoặc wave.
3. Tạo `secret = sea.secret(epubB, pairA)`.
4. Mã hóa seed hiện tại: `encrypted = sea.encrypt(seedA, secret)`.
5. Broadcast response: `{"~": epubB, ":": encrypted, "from": epubA}` qua QR + wave.

### B (nhận response)
1. Lọc payload theo `~ === epubB`.
2. Tạo `secret = sea.secret(epubA, pairTempB)`.
3. `seedA = sea.decrypt(encrypted, secret)`.
4. Emit event `signin` -> gọi `Access.wave({ seed: seedA })` -> authenticated.

## Trạng thái hiện tại
- Hạ tầng worker + static Wave API đã đủ để encode/decode thực tế.
- `ui-wave` đã có send/listen/stop + QR in/out + role sender/receiver.
- Access flow đã nối được sự kiện signin từ wave.
- Footer đã có điểm kích hoạt wave auth/share.

## Rủi ro kỹ thuật cần theo dõi
- `ScriptProcessorNode` là API cũ; cân nhắc chuyển `AudioWorklet` khi ổn định.
- Môi trường browser khác nhau có thể lệch sample-rate -> cần test thêm trên mobile.
- Cần chống replay (nonce/timestamp) ở payload auth response trong pha hardening.
- Nên có timeout/session-id rõ ràng để đóng phiên tự động.

## Bước tiếp theo đề xuất
1. Thêm nonce + timestamp + sessionId vào payload để chống replay.
2. Thêm trạng thái UI rõ ràng hơn cho từng phase (requesting/waiting/verified/failed).
3. Viết test tích hợp cho encode/decode roundtrip ở worker.
4. Đánh giá nâng cấp audio pipeline sang `AudioWorklet`.
