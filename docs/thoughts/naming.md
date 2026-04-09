# Naming Convention

## Hàm & Method

- Ký tự cho phép: `[a-z]`, tối đa 10 ký tự
- Tên hàm trùng tên file chứa nó (ví dụ `render.js` export hàm `render`)
- Ưu tiên single word. Nếu buộc phải 2 từ, viết tắt từ thứ hai
- Không dùng camelCase — nó vừa trông như cha, vừa trông như con, không rõ vai trò

```
// tốt
render   load   fetch   save   init   run

// chấp nhận được (viết tắt)
rendertpl   loadcfg   fetchpg

// không dùng
renderTemplate   loadConfig   fetchPage
```

## Ký tự đặc biệt `_` và `$`

| Ký tự | Ý nghĩa | Dùng khi |
|-------|---------|----------|
| `_`   | cấp cha, meta, tổng hợp | thực sự cần nhấn mạnh vai trò tổng |
| `$`   | helper, cấp con, tiện ích nhỏ | thực sự cần phân biệt vai trò phụ |

Cả hai đều **hạn chế** — nếu tên đã rõ nghĩa thì không cần thêm ký tự.

```
_state   // meta/tổng state của module
$format  // helper nhỏ hỗ trợ format
```

## Folder

- Ký tự cho phép: `[a-z-]`
- Không chứa `_`, không camelCase
- Dùng `-` nếu cần nối 2 từ

```
// tốt
core/   geo/   web-authn/   order-detail/

// không dùng
webAuthn/   order_detail/   OrderDetail/
```

## CSS Class & ID

- Gần BEM nhưng rút gọn: `_` để nối cha–con, `-` để nối 2 từ trong 1 segment
- Format: `cha_con`, `cha-con_khau-sung` — **không** dùng `__` như BEM gốc
- Web component đã nhốt styles → ưu tiên single word, không cần prefix cha
- Khi trùng tên: dùng `#id` cho duy nhất, `.class` cho nhiều

```css
/* tốt */
.cart        /* đủ rõ trong shadow DOM */
.cart_item
.cart_item-count
#main-nav

/* không dùng */
.Cart__item--active   /* BEM gốc, quá dài */
.cartItem             /* camelCase */
```

## Tóm tắt

| Loại | Pattern | Ví dụ |
|------|---------|-------|
| Hàm/method | `[a-z]{1,10}` | `render`, `loadcfg` |
| Biến meta | `_name` | `_state`, `_root` |
| Helper nhỏ | `$name` | `$fmt`, `$wrap` |
| Folder | `[a-z-]` | `core/`, `web-authn/` |
| CSS class | `word` hoặc `cha_con` | `.cart`, `.cart_item` |
| CSS nối từ | `word-word` | `.item-count` |
| CSS id | `#word` hoặc `#word-word` | `#main-nav` |
