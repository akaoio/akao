# Swarm Protocol

Swarm là hệ thống giao tiếp giữa con người và AI agents qua Telegram. Mọi thành viên — người và agent — đều bình đẳng trong kênh chung.

## Identity

Mỗi máy có một bot Telegram riêng. Identity của bạn là `self.name` trong `swarm.yaml`, không phải tên AI model bạn đang dùng.

## Cấu hình

Swarm dùng 2 file config:

### `swarm.shared.yaml` — COMMIT lên git (public)
Chứa thông tin chung của cả swarm:

```yaml
groups:
  - <group_id>        # Telegram group chat IDs
agents:
  - name: <machine-name>
    username: <bot_username>
  - name: <machine-name-2>
    username: <bot_username_2>
```

### `swarm.yaml` — Git-ignored (SECRET)
Chứa thông tin riêng của máy:

```yaml
self:
  token: <token>      # SECRET — chỉ máy này biết
  name: <machine-name>
```

**Lợi ích**: Chỉ cần update `swarm.shared.yaml` một lần, tất cả máy `git pull` là biết danh sách bots và groups mới.

## Các lệnh

```bash
node swarm.js history          # đọc 20 tin nhắn gần nhất từ group
node swarm.js send "<message>" # gửi tin nhắn (tự prepend identity + @mention tất cả bots)
node swarm.js notes            # đọc swarm/notes.md
node swarm.js notes "<text>"   # append vào swarm/notes.md kèm timestamp
node swarm.js chats            # liệt kê tất cả chat IDs mà bot đã nhận tin (dùng để lấy group ID)
```

## Protocol bắt buộc

Agent PHẢI thực hiện khi:

1. **Bắt đầu session** → `node swarm.js history` rồi đọc `swarm/notes.md`
2. **Bắt đầu task** → `node swarm.js send "start | task: <mô tả> | branch: <branch>"`
3. **Hoàn thành task** → `node swarm.js send "done | task: <mô tả> | branch: <branch>"`
4. **Bị block / cần input** → `node swarm.js send "blocked | <vấn đề> | cần: <yêu cầu>"`
5. **Insight quan trọng** → `node swarm.js notes "<ghi chú>"`

Agent KHÔNG cần xin phép trước khi dùng swarm — cứ chạy lệnh.

## Format tin nhắn

`send` tự động prepend `[identity]` và append `@mentions`. Agent chỉ viết nội dung:

```
node swarm.js send "done | task: implement login | branch: feature/auth"
```

Tin nhắn thực tế gửi đi:
```
[son-pc] done | task: implement login | branch: feature/auth @huy_swarm_bot @duc_swarm_bot
```

## Notes

`swarm/notes.md` là bộ nhớ xuyên session của agent. Ghi lại:
- Quyết định thiết kế quan trọng
- Task đang làm dở
- Ngữ cảnh cần nhớ khi session mới mở

## Setup máy mới

```bash
git pull                          # Lấy swarm.shared.yaml
cp swarm.example.yaml swarm.yaml  # Tạo config riêng
# Edit swarm.yaml: điền token + name từ @BotFather
node swarm.js history             # Test connection
```

## Thêm bot mới vào swarm

```bash
# 1. Tạo bot trên Telegram (@BotFather → /newbot)
# 2. Add bot vào Telegram group
# 3. Edit swarm.shared.yaml:
agents:
  - name: new-bot
    username: new_bot_username
# 4. Commit
git add swarm.shared.yaml
git commit -m "Add new-bot to swarm"
git push
# 5. Tất cả máy khác git pull → tự động biết bot mới
```

## Bảo mật

- `swarm.yaml` PHẢI nằm trong `.gitignore` — chỉ chứa token
- `swarm.shared.yaml` an toàn để commit — không chứa secrets
- Mỗi máy chỉ biết token của chính mình
- Giao tiếp qua @mention — Telegram delivery đảm bảo mọi bot nhận tin
