# Wave Signin Architecture

## Thuật ngữ
- **Bên xin** (requester): thiết bị B, chưa đăng nhập, muốn lấy seed từ thiết bị khác → `ui-authenticate`
- **Bên cho** (granter): thiết bị A, đã đăng nhập, chủ động phát seed sau khi được hỏi → `ui-authorize`

## Mục tiêu
- Bổ sung phương thức signin offline-first song song với passkey.
- Thiết bị đã đăng nhập (A) truyền seed cho thiết bị mới (B) qua kênh âm thanh ggwave.
- Không cần server, không cần mạng — chỉ cần loa và micro đối diện nhau.

## Kiến trúc theo lớp

```
Bên xin (B)                          Bên cho (A)
────────────────────                 ────────────────────
ui-access (modal)                    /authorize (route)
  └── ui-authenticate                  └── ui-authorize
        ├── ui-visualizer                    ├── ui-visualizer
        └── ui-wave                          └── ui-wave
              ↕ sóng âm ggwave ↕
```

---

### Layer 1 — ggwave WASM (`src/core/Wave/ggwave.js`)
Compiled ggwave library, loaded via ES module. Entry point là `ggwave_factory()` trả về WASM module.

---

### Layer 2 — `Wave` class (`src/core/Wave.js`)
Thin bridge class. Hai nhóm method hoàn toàn tách biệt:

**Instance methods** — dùng trong worker (`new Wave()` chỉ được gọi trong worker thread):
- `init()` — lazy-load ggwave WASM, cache singleton `this.wave`

**Static methods** — dùng ở main thread / UI (không khởi tạo instance):
- `Wave.check()` — kiểm tra browser có hỗ trợ micro không (dùng permissions API)
- `Wave.permission()` — xin quyền micro (mở stream rồi đóng ngay)
- `Wave.request({ method, params, transfer, timeout })` — queue lệnh đến worker qua `threads.queue()`
- `Wave.encode(params, options)` — wrapper: gọi `Wave.request({ method: "encode" })`
- `Wave.decode(params, options)` — wrapper: gọi `Wave.request({ method: "decode" })`

---

### Layer 3 — Wave worker (`src/core/threads/wave.js`)
Web Worker chạy ggwave WASM. State nội bộ:
```
state = {
    module,        // ggwave WASM instance
    receiver,      // ggwave instance dùng để decode
    sender,        // ggwave instance dùng để encode
    params,        // cấu hình hiện tại
    protocolId,    // mặc định: GGWAVE_PROTOCOL_AUDIBLE_FAST
    volume,        // mặc định: 25
    sampleRate,    // mặc định: 48000
    decoder        // TextDecoder singleton
}
```
Các method:
- `init()` — eagerly khởi tạo module + instances
- `configure(params)` — cập nhật sample rate / protocolId / volume; có thể reset instances
- `encode({ message, protocolId, volume })` — trả `{ ok, bytes: Int16 PCM array, sampleRate, payload }`
- `decode({ bytes })` — nhận Int8Array PCM, trả `{ ok, found, message }`
- `reset()` — giải phóng + tái tạo instances
- `destroy()` — cleanup toàn bộ, null module

Đăng ký tại boot: `src/core/Launcher.js` gọi `threads.register("wave", { worker: true, type: "module" })`.

---

### Layer 4 — `ui-wave` (`src/UI/components/wave/index.js`)
Custom element (`<ui-wave>`) — orchestrator audio I/O. **Không biết gì về auth hay role.**

**Khởi tạo audio:**
- `listen()` — mở micro (echoCancellation/noiseSuppression/autoGainControl đều tắt), tạo `AudioContext` tại 48kHz, configure worker, nối `ScriptProcessor(1024)` → backlog queue. Tạo `micAnalyser` cho visualizer.
- `stop()` — đóng mic, disconnect tất cả nodes, clear backlog.

**Decode loop:**
- `onaudio(event)` — nhận float32 frame từ ScriptProcessor, convert sang Int8, push vào `audioBacklog`.
- `pump()` — drain backlog theo batch (6 chunks/lần), gọi `Wave.decode()` tuần tự. Khi `found=true`, gọi `handle()`.
- Backlog giới hạn 64 frames.

**Encode + phát:**
- `send(payload)` — JSON-serialize payload → nếu vượt chunk threshold thì `chunked()`, còn lại `frame()`.
- `frame(text)` — `Wave.encode()` → `play()` → sleep nhỏ.
- `play(bytes, sampleRate)` — decode Int16 PCM → `AudioBuffer` → phát qua `AudioContext`. `micTemporarilyDisabled = true` trong khi phát, flush backlog sau khi xong.

**Chunking (xem mục Wire Format):**
- `chunked(text)` — tách thành các frame chunk.
- `consume(chunk)` — ghép lại khi đủ; gọi `handle()` với full payload.
- `cleanup()` — xóa chunks cũ (TTL 60s).

**Message routing:**
- `handle(message)` — dedup bằng `this.last`, parse JSON, dispatch chunk hoặc emit event `message`.

**Events phát ra:**
- `message` — nhận được message hoàn chỉnh `{ message, parsed }`
- `stream` — stream mic thay đổi
- `analyser` — `AnalyserNode` thay đổi (cho visualizer)

---

### Layer 5 — `ui-authenticate` (`src/UI/components/authenticate/`)
**Bên xin seed.** Custom element (`<ui-authenticate>`), dùng trong `ui-access` modal khi user chưa đăng nhập chọn wave signin.

**State machine:**
```
neutral ──[Request]──→ requesting ──[phát xong]──→ listening
  ↑                        │
  └──────[Stop]────────────┘
```

**neutral**: vào đây ngay khi mount. Hiển thị epub rút gọn (`abcde...vwxyz`) và nút **Request**.

**requesting**:
1. Tạo ephemeral SEA pair: `session = await sea.pair()`
2. `waveEl.listen()` — bật mic trước
3. Phát: `{ "~": session.epub, ":": ">" }`
4. Chuyển sang trạng thái **listening** ngay sau khi phát xong

**listening**: đang chờ tín hiệu từ bên cho.
- Nhận message → kiểm tra format → xử lý (xem phần Message Handling phía dưới)
- Nút Stop → dừng `waveEl`, quay về **neutral**

**Message Handling (bên xin):**
- `parsed[":"] === "!>"` → Access denied — hiện thông báo
- `parsed["~"] && parsed["!"] && parsed["@"] && parsed["#"]` → Grant → reconstruct + decrypt:
  ```js
  secret = sea.secret(parsed["~"], session)
  seed   = sea.decrypt({ ct: parsed["!"], iv: parsed["@"], s: parsed["#"] }, secret)
  // → Access.wave({ seed }) → emit "done"
  ```

---

### Layer 6 — `ui-authorize` (`src/UI/components/authorize/`)
**Bên cho seed.** Custom element (`<ui-authorize>`), dùng trong route `/authorize`. Logic tách hẳn khỏi `ui-authenticate`.

**State machine:**
```
listening ──[nhận request]──→ confirming
  ↑                               │
  └──────[Deny / Grant]───────────┘
```

**listening**: vào đây **ngay khi mount**. `waveEl.listen()` được gọi tự động trong `connectedCallback`.

**confirming**: nhận được `{ "~": epub_B, ":": ">" }` → hiện thông báo:
> Thiết bị `abcde...vwxyz` xin cấp quyền truy cập
> [Deny] [Grant]

**Deny**: phát `{ ":": "!>" }` → quay về **listening**

**Grant**:
```js
secret    = await sea.secret(parsed["~"], Access.get("pair"))
encrypted = await sea.encrypt(Access.get("seed"), secret, null, { raw: true })
reply     = { "~": Access.get("pair").epub, "!": encrypted.ct, "@": encrypted.iv, "#": encrypted.s }
```
Phát reply → quay về **listening**

> **Lưu ý:** Bên cho không dùng `":"` làm type discriminator trong Grant — bên nhận nhận biết Grant bằng sự hiện diện đồng thời của `"~"`, `"!"`, `"@"`, `"#"`.

---

### Layer 7 — Route `/authorize` (`src/UI/routes/authorize/`)
Route mỏng — chỉ render `<ui-authorize>`. Không chứa protocol logic.

```js
// template.js
html`<layout-main><ui-authorize /></layout-main>`
```

Logic authorize được tái sử dụng từ component, có thể nhúng vào bất kỳ route/component nào khác.

---

### Layer 8 — `ui-access` (`src/UI/components/access/index.js`)
App-level modal dùng cho bên xin. Đăng ký vào `Elements.Access` khi mount.

**Screen flow:**
- `unauthenticated-screen` → chọn Sign in / Sign up / Wave
- `signin-screen` → `<ui-authenticate id="auth" />`
- `signup-screen` → form WebAuthn
- `sign-screen` → xác nhận ký giao dịch

**Wave entry point:**
```js
openWaveSignin()
// → showModal() → show("signin-screen")
// ui-authenticate tự mount ở trạng thái neutral, user bấm Request để bắt đầu
```

---

### Layer 9 — `Access.wave()` (`src/core/Access.js`)
```js
export async function wave({ seed, id } = {})
```
- Gọi `next({ id: id || "wave:...", seed })` → rebuild SEA pair từ seed → set `authenticated = true`.
- Gọi `restore()` để kéo lại `pub` từ Gun DB.
- Kết quả giống hệt passkey signin.

---

## Wire Format trên sóng

Tất cả key dùng **1 ký tự** để tiết kiệm bandwidth.

### Auth request (B → broadcast)
```json
{ "~": "<epub B>", ":": ">" }
```
- `"~"`: epub của bên xin (ECDH public key)
- `":"`: `">"` = yêu cầu truy cập

### Auth deny (A → broadcast)
```json
{ ":": "!>" }
```
- `"!>"` = không đồng ý (`!`) yêu cầu truy cập (`>`)

### Auth grant (A → broadcast)
```json
{ "~": "<epub A>", "!": "<ct>", "@": "<iv>", "#": "<s>" }
```
- `"~"`: epub của bên cho (cần để bên xin tính ECDH)
- `"!"`: ciphertext (ct)
- `"@"`: initialization vector (iv)
- `"#"`: salt (s)

Bên xin nhận biết grant bằng sự hiện diện đồng thời của `~`, `!`, `@`, `#`.

### ECDH
```js
// A tính:
secret = sea.secret(session.epub /*epub B*/, pair_A)
// B tính:
secret = sea.secret(parsed["~"] /*epub A*/, session)
// → cùng secret
```

### Chunking
Format chunk cực gọn — toàn bộ key 1 ký tự:

```json
{ "&": 0,    ":": "<nội dung phần đầu>" }    ← chunk đầu tiên (index 0)
{ "&": 1,    ":": "<nội dung tiếp theo>" }
{ "&": "2!", ":": "<nội dung phần cuối>" }    ← chunk cuối (index kèm "!")
```

- `"&"`: index chunk. Nếu là số → chunk tiếp nối. Nếu là string kết thúc bằng `"!"` → chunk cuối.
- `":"`: nội dung của phần chunk đó.

Receiver nhận biết chunk bằng sự hiện diện của key `"&"`. Khi nhận chunk cuối, ghép tất cả `":"` theo thứ tự index → đưa vào luồng xử lý.

> **Lý do không dùng total**: tránh phải phát lại header khi mạng kém; bên nhận chỉ cần đợi chunk có `"!"` là biết xong.

---

## Luồng đăng nhập A ↔ B

```
B (bên xin — ui-authenticate)        A (bên cho — ui-authorize)
──────────────────────────────────────────────────────────────
[mount → neutral]                     [mount → listening, waveEl.listen()]

[user bấm Request]
sea.pair() → session
waveEl.listen()
send({ "~": session.epub, ":": ">" })
[→ listening]                         ← nhận request
                                      hiện: "Thiết bị abcde...vwxyz xin truy cập"
                                      [Deny]  [Grant]

                                      [Grant]:
                                        secret = sea.secret(session.epub, pair_A)
                                        enc = sea.encrypt(seed, secret, {raw:true})
                                        send({ "~": pair_A.epub,
                                               "!": enc.ct, "@": enc.iv, "#": enc.s })
← nhận grant
secret = sea.secret(parsed["~"], session)
seed = sea.decrypt({ ct:parsed["!"],
                     iv:parsed["@"],
                     s:parsed["#"] }, secret)
Access.wave({ seed }) → authenticated ✓

                                      [Deny]:
                                        send({ ":": "!>" })
← nhận deny
hiện: "Access denied"
```

---

## UI / UX Notes

**Nút trên bên xin:**
- Trạng thái neutral: nút "**Request**" (kinh điển hơn "Start"; "Start" mơ hồ, "Request" nói rõ hành động — xin cấp quyền)
- Trạng thái requesting/listening: nút "**Stop**" thay thế
- Hiển thị epub rút gọn dưới nút: `abcde...vwxyz` (5 ký tự đầu + `...` + 5 ký tự cuối)

**Màn hình bên cho:**
- Default: "Đang lắng nghe..." (không cần action của user)
- Khi nhận request: hiện epub rút gọn của bên xin + [Deny] [Grant]
- Sau khi xử lý: quay về lắng nghe

---

## Điểm kích hoạt

| Entry point | Vai trò | Cách gọi |
|---|---|---|
| Route `/authorize` | Bên cho | Điều hướng thủ công hoặc từ footer |
| `Elements.Access.openWaveSignin()` | Bên xin | Từ `ui-access` modal |

---

## Trạng thái triển khai

| Thành phần | Trạng thái |
|---|---|
| ggwave WASM (`Wave/ggwave.js`) | ✅ Hoàn thiện |
| `Wave` class + submodules | ✅ Hoàn thiện |
| Wave worker thread | ✅ Hoàn thiện |
| `ui-wave` (audio I/O) | ✅ Hoàn thiện |
| `ui-visualizer` (audio visualizer) | ✅ Hoàn thiện |
| `Access.wave()` | ✅ Hoàn thiện |
| `ui-authenticate`: state machine mới (neutral→requesting→listening) | 🔄 Cần refactor |
| `ui-authorize` component | 🔄 Cần tạo |
| Route `/authorize` refactor (thin wrapper) | 🔄 Cần refactor |
| Wire format mới (`">"`, `"!>"`, `!@#`) | 🔄 Cần update |
| Chunking format mới (`"&"` key) | 🔄 Cần refactor |
| `ui-access` wave entry point | ✅ Hoàn thiện |
| Footer/nav kích hoạt authorize | ⏳ Chưa làm |
| Timeout / replay protection | ⏳ Chưa làm |
| AudioWorklet thay ScriptProcessor | ⏳ Tương lai |

---

## Ghi chú kỹ thuật

- `ScriptProcessorNode` đã deprecated nhưng vẫn hoạt động tốt ở tất cả browser. `AudioWorklet` phức tạp hơn và cần COOP/COEP headers; để dành nâng cấp sau.
- Sample rate 48kHz được enforce ở cả `AudioContext` lẫn wave worker để tránh mismatch.
- `micTemporarilyDisabled` flag đặt trong suốt `play()` để bỏ qua frame do âm thanh phát ra loa vọng lại micro — không tắt track mic thật (giữ latency thấp).
- Backlog capped 64 frames (~1.4s buffer).
- Chunking threshold cần tính lại theo format mới — overhead key đã giảm đáng kể.
- QR (`ui-qr`) là component riêng biệt, không thuộc wave auth flow.
