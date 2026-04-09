# Swarm Protocol

Swarm là hệ thống giao tiếp giữa con người và AI agents qua Telegram. Mọi thành viên — người và agent — đều bình đẳng trong kênh chung.

## Identity

Mỗi máy có một bot Telegram riêng. Identity của bạn là `self.name` trong `swarm.yaml`, không phải tên AI model bạn đang dùng.

## Cấu hình

File `swarm.yaml` tại root của project:

```yaml
self:
  token: <token>      # SECRET — không commit lên git
  name: <machine-name>
groups:
  - <group_id>
agents:
  - name: <machine-name>
    username: <bot_username>
```

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

## Bảo mật

- `swarm.yaml` PHẢI nằm trong `.gitignore`
- Mỗi máy chỉ biết token của chính mình
- Giao tiếp qua @mention — Telegram delivery đảm bảo mọi bot nhận tin
