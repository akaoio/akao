# Tư Duy Phát Triển Hướng Kiểm Thử (Test-Driven Development Mindset)

*Một phương pháp nền tảng để xây dựng hệ thống đáng tin cậy*

---

## 1. Vấn đề với cách làm thông thường

Hầu hết lập trình viên viết code trước, test sau — nếu có. Kết quả là:

- Test được viết để **chứng minh code đúng**, không phải để **định nghĩa code phải làm gì**
- Test bị bias bởi implementation đã có — chỉ test những gì đã biết
- Bug thật sự chỉ lộ ra khi production gặp edge case mà không ai nghĩ tới

Đây là vòng lặp sai: *code → test → fix → test lại → ...*

---

## 2. Nguyên lý nền tảng

> **Kiểm thử là đặc tả. Code là quá trình hiện thực hóa đặc tả đó.**

Và hệ quả trực tiếp:

> **Một hệ thống chỉ đúng khi các kiểm thử của nó định nghĩa đúng.**

Điều này đảo ngược hoàn toàn tư duy thông thường:

- Code **không** định nghĩa hành vi — **test định nghĩa hành vi**
- Test không phải phần phụ — test là **nguồn chân lý**
- Implementation chỉ là quá trình thoả mãn ràng buộc đã được đặt ra trước

---

## 3. Vòng Red → Green → Refactor

Chu trình cốt lõi của TDD:

```
🔴 RED     Viết test → chạy → phải fail
              ↓
🟢 GREEN   Viết code tối thiểu để pass test
              ↓
🔵 REFACTOR Dọn code, không thêm logic
              ↓
           Lặp lại từ đầu
```

Mỗi vòng lặp thường chỉ mất vài phút. Hệ thống lớn lên từng bước nhỏ, luôn trong trạng thái xác minh được.

---

## 4. Tại sao test phải fail trước?

Đây là điều nhiều người bỏ qua. Một test **pass ngay từ đầu** thường có nghĩa là:

- Logic assertion sai (test không kiểm tra gì thực sự)
- Test đang test implementation thay vì hành vi
- Hoặc feature đã được implement rồi mà không ai biết

**Failure là tín hiệu, không phải lỗi.** Nó xác nhận:
1. Test đang đo lường đúng thứ cần đo
2. Hệ thống chưa thoả mãn yêu cầu — và đó là trạng thái kỳ vọng *trước khi viết code*

---

## 5. Test như ràng buộc toán học

Một test suite tốt hoạt động như một **hệ phương trình**:

- Mỗi test là một ràng buộc
- Implementation là nghiệm của hệ
- Nếu tất cả test pass → nghiệm hợp lệ

Test tốt định nghĩa:

| Chiều | Ý nghĩa |
|---|---|
| **Input space** | Đầu vào hợp lệ, không hợp lệ, edge case |
| **Expected output** | Kết quả chính xác và bất biến |
| **Invariants** | Những điều luôn đúng bất kể input |
| **Error contracts** | Lỗi nào được ném ra khi nào |

> Test tốt là **lightweight formal verification** — không cần theorem prover, chỉ cần kỷ luật.

---

## 6. Mocks vs Real — Nguyên tắc "Tất cả phải thật"

Một lỗi phổ biến: mock quá nhiều để test "nhanh hơn". Hệ quả:

- Test pass nhưng production fail — vì mock không phản ánh hành vi thật
- Bug nằm ở ranh giới giữa các layer — đúng là nơi mock che khuất

**Nguyên tắc:** Chỉ mock khi **không thể** dùng thật.

Ưu tiên theo thứ tự:
1. **Real implementation** — luôn là lựa chọn đầu tiên
2. **In-memory variant** — ví dụ: database in-memory, Gun `{localStorage:false}`
3. **Fake** — implementation đơn giản nhưng đúng hành vi (không phải stub)
4. **Mock** — chỉ khi các lựa chọn trên không khả thi (external API, hardware)

Ví dụ từ dự án này: `Order.test.js` dùng Gun thật (in-memory), SEA thật, Ganache EVM thật — không có mock nào. Kết quả: bug thật được tìm thấy (Gun put callback không fire trong memory-only mode), được fix ở upstream.

---

## 7. Thuộc tính của test suite tốt

| Thuộc tính | Mô tả |
|---|---|
| **Completeness** | Bao phủ hành vi quan trọng và edge case |
| **Precision** | Mỗi test chỉ kiểm tra **một** hành vi |
| **Independence** | Không phụ thuộc trạng thái chung giữa các test |
| **Determinism** | Cùng input → luôn ra cùng output |
| **Sensitivity** | Phải fail khi có lỗi thật, không false-positive |
| **Readability** | Đọc test là hiểu được spec của hệ thống |

---

## 8. Anti-pattern cần tránh

| Anti-pattern | Hệ quả |
|---|---|
| Viết code trước test | Test bị bias, chỉ xác nhận những gì đã làm |
| Test luôn pass | Không có giá trị kiểm chứng, lừa dối cả nhóm |
| Assertion mơ hồ (`assert(result)`) | Không phát hiện được regression |
| Mock quá nhiều | Test xanh, production đỏ |
| Coupling test với implementation | Test vỡ khi refactor dù logic đúng |
| Bỏ qua test fail | Tích luỹ nợ kỹ thuật ngầm, mất tin tưởng vào suite |
| Test không có tên rõ ràng | Không ai biết test đang kiểm tra gì khi nó fail |

---

## 9. Lợi ích chiến lược

- **Độ tin cậy cao hơn** — hành vi được chứng minh, không chỉ được giả định
- **Debug nhanh hơn** — lỗi bị cô lập ngay từ khi phát sinh, không phải truy vết sau
- **Documentation sống** — đọc test là đọc spec, luôn cập nhật
- **Refactor tự tin** — test suite là lưới an toàn, không sợ phá vỡ
- **Thiết kế tốt hơn** — code khó test thường là code được thiết kế kém; TDD ép bạn thiết kế tốt hơn
- **Onboarding nhanh hơn** — người mới đọc test để hiểu hệ thống làm gì

---

## 10. Kết luận

Test-Driven Development không phải kỹ thuật viết test — mà là **cách tư duy về phần mềm**:

- Bắt đầu từ câu hỏi *"Hệ thống phải làm gì?"* → viết test
- Không phải *"Tôi vừa làm gì?"* → viết test để xác nhận

Khi áp dụng nhất quán, TDD chuyển đội nhóm từ:

> *"Có vẻ đúng"* → **"Đã được chứng minh"**

Đó là sự khác biệt giữa phần mềm được xây dựng bởi niềm tin và phần mềm được xây dựng bởi bằng chứng.
