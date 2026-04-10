# Swarm Protocol — Kế hoạch thiết kế

## Bối cảnh

Team gồm 3 dev người + nhiều AI agents (Copilot, Claude, v.v...) làm việc trên cùng codebase. Giao tiếp nội bộ qua Telegram private group. Agents hiện không nắm được ngữ cảnh từ các cuộc trao đổi ngoài Github. Swarm Protocol là cơ chế đơn giản để kết nối cả người và agents vào một kênh giao tiếp chung.

---

## Constraints kỹ thuật

- Máy PC ở nhà, không có IP tĩnh, không có IPv6 → **không có webhook, không có HTTP peer-to-peer**
- Toàn bộ giao tiếp phải qua **Telegram servers** (pull-based polling)
- Telegram Bot API: `getUpdates` tiêu thụ offset — nếu 2 agent cùng poll 1 key thì bị mất tin
- Mỗi bot chỉ thấy tin nhắn gửi **đến** nó qua `getUpdates`

---

## Giải pháp: @mention thay vì chia sẻ API keys

Khi gửi tin nhắn, bot **@mention tất cả bots còn lại** → Telegram tự đẩy tin vào `getUpdates` của từng bot được mention. Mỗi máy chỉ cần **token của chính mình**, không cần token của người khác.

**Lợi ích bảo mật**: Token là secret — không chia sẻ, không lưu trên git. Nếu 1 máy bị compromise chỉ mất token của máy đó, không ảnh hưởng swarm.

**Edge case**: Bot offline > 24h → Telegram xóa pending updates → mất tin trong khoảng đó. Chấp nhận được với use case này.

---

## Kiến trúc tổng quan

```
swarm.yaml          — cấu hình riêng: self (token+name) — git-ignored
swarm.shared.yaml   — cấu hình chung: groups[], agents[] — commit lên git
swarm.js            — CLI tool: node swarm.js <command>
SWARM.md            — protocol document (inject vào instructions của mỗi agent)
swarm/              — thư mục làm việc của agent local (gitignore)
    log.json        — history đã fetch + sent messages (deduplicated)
    notes.md        — ghi chú tư duy của agent, persist qua sessions
```

---

## Config structure — 2 files

### swarm.yaml (git-ignored, SECRET)
```yaml
self:
  token: <token_của_bot_trên_máy_này>   # SECRET — không commit lên git
  name: son-pc                           # identity của máy này trong swarm
```

### swarm.shared.yaml (commit lên git, PUBLIC)
```yaml
groups:
  - <group_id_1>
agents:                                  # tất cả thành viên swarm — CHỈ cần username để @mention
  - name: son-pc
    username: son_swarm_bot
  - name: huy-laptop
    username: huy_swarm_bot
  - name: duc-pc
    username: duc_swarm_bot
```

**Lợi ích**: Chỉ cần update `swarm.shared.yaml` một lần, tất cả máy `git pull` là biết danh sách mới. Code tự merge 2 files khi load.

---

## swarm.js — CLI commands

### `node swarm.js history`
1. Gọi `getUpdates` bằng `self.token` (offset từ `swarm/log.json`)
2. Filter messages từ groups trong `groups[]`
3. Merge với `swarm/log.json` (gồm cả sent messages)
4. Deduplicate theo `message_id`
5. Sort theo `date` (timestamp)
6. Hiển thị 20 tin nhắn gần nhất
7. Lưu lại `swarm/log.json` với offset mới

### `node swarm.js send <message>`
1. Build mentions: `@username` của tất cả agents trừ `self`
2. Gửi `[self.name] <message> <mentions>` đến từng group qua `self.token`
3. Ghi vào `swarm/log.json` (bot không nhận được tin của chính nó qua `getUpdates`)

### `node swarm.js notes [text]`
- Không có `text` → in ra `swarm/notes.md`
- Có `text` → append vào `swarm/notes.md` kèm timestamp

---

## SWARM.md — Protocol cho agents

Nội dung SWARM.md được inject vào `copilot-instructions.md`, `CLAUDE.md`, v.v..

Mỗi agent phải tuân thủ:

1. **Khi bắt đầu session**: chạy `node swarm.js history` và đọc `swarm/notes.md` để cập nhật ngữ cảnh
2. **Khi bắt đầu task mới**: gửi report qua `node swarm.js send`
3. **Khi hoàn thành task**: gửi report kèm branch name
4. **Khi bị block hoặc cần input**: gửi report và hỏi cụ thể
5. **Định kỳ**: ghi tư duy quan trọng vào `swarm/notes.md`

Format tin nhắn (`send` tự prepend identity, agent chỉ cần viết nội dung):
```
node swarm.js send "STATUS | task: <mô tả> | branch: <branch> | note: <tùy chọn>"
```

---

## Phân rã công việc

- [ ] Viết `swarm.yaml` schema + tạo file mẫu
- [ ] Implement `swarm.js` với 3 commands: `history`, `send`, `notes`
- [ ] Viết `SWARM.md` protocol document
- [ ] Inject SWARM.md vào `copilot-instructions.md`
- [ ] Tạo thư mục `swarm/` với `notes.md` mẫu cho từng agent
- [ ] Test với ít nhất 2 bot tokens thật
