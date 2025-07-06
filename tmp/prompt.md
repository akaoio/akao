# KẾ HOẠCH HOÀN THIỆN AKAO CLI - SẢN PHẨM HOÀN CHỈNH 100% (KHÔNG LIB NGOÀI, TẤT CẢ BUILTIN)

## 1. Đánh giá hiện trạng
- Liệt kê các lệnh CLI đã có thực tế bằng `--help`.
- So sánh với bảng lệnh chuẩn trong `blueprint/cli.md`.
- Ghi chú các lệnh/phân nhóm còn thiếu hoặc chưa đúng.

## 2. Bổ sung đầy đủ lệnh CLI
- Cài đặt đầy đủ, chi tiết, production-ready tất cả các lệnh: `validate` (thay thế cho check, trace, self-validate), `init`, `generate`, `build`, `test`, `docs`, `metrics`, `security`, `rules`, `config`.
- Mỗi lệnh là một class riêng biệt, mỗi class nằm trong một file riêng trong `interfaces/cli/` (theo đúng luật "one class per file").
- Mỗi lệnh phải có parser, executor, formatter riêng biệt nếu cần, và phải hoạt động thực tế, không được để placeholder, mockup hay stub.
- Đảm bảo các tuỳ chọn/phân nhóm/phụ lệnh đúng như mô tả trong `cli.md`, mọi tính năng đều phải thực thi được.
- Lệnh `validate` phải hỗ trợ đầy đủ các chức năng: validation thông thường, self-validation, check specific rules, trace violations.

## 3. Chuẩn hoá đầu ra
- Mọi lệnh đều phải hỗ trợ các định dạng: table (mặc định), json, yaml, và xuất đúng dữ liệu thực tế.
- Formatter cho từng định dạng phải hoàn chỉnh, không để placeholder.

## 4. Kiểm soát phụ thuộc & dữ liệu
- Tuyệt đối không sử dụng bất kỳ thư viện ngoài nào, kể cả open source, chỉ dùng C++ chuẩn (STL) và code tự viết.
- Tất cả dữ liệu cấu hình, rule, philosophy phải là YAML (không dùng JSON).
- Built-in YAML parser, logic engine, metrics, trace... đều phải tự cài đặt hoàn chỉnh, không được dùng bất kỳ mã nguồn ngoài nào.

## 5. Traceability & Interface parity
- Mọi vi phạm đều có ID, file, line, suggestion, stack trace đúng chuẩn, và phải được truy vết thực tế qua toàn bộ hệ thống.
- Đảm bảo interface parity: CLI = API = Web UI. Nếu có API/Web UI thì phải cài đặt thực tế, không để stub hay placeholder.

## 6. Hoàn thiện cấu trúc thư mục
- Tất cả các thư mục/phần như: `core/engine/logic/`, `.akao/`, `languages/`, `builders/`, `docs/`, `automation/`, `plugins/`, `metrics/`, `security/`, `universe/` phải được cài đặt thực tế, có chức năng đúng như blueprint, không để rỗng hay stub.
- Mỗi class, mỗi thành phần đều phải có test thực tế, kiểm thử đầy đủ trong `tests/` ("one class one test").

## 7. Universe generation
- Cài đặt đầy đủ khả năng "universe generation" (tự sinh dự án, framework, ngôn ngữ mới) đúng blueprint, không để stub hay placeholder.

## 8. Kiểm thử & tự kiểm thử
- Viết test thực tế cho từng lệnh, từng thành phần.
- Đảm bảo lệnh `validate` có khả năng tự kiểm tra chính nó (self-validate), trace violations, check specific rules và phải hoạt động thực tế.

## 9. Đảm bảo tuân thủ triệt để blueprint
- Mọi thay đổi đều phải tuân thủ các nguyên tắc trong `blueprint/tree.md` và `blueprint/cli.md`.
- Kiểm tra lại toàn bộ hệ thống sau khi hoàn thiện, đối chiếu checklist với blueprint, không được bỏ sót bất kỳ chi tiết nào.

## 10. Gợi ý workflow thực hiện
1. Chạy lại `--help` để xác nhận các lệnh hiện có.
2. Cài đặt đầy đủ các class/file cho các lệnh/phần còn thiếu, không để stub.
3. Bổ sung parser, executor, formatter cho từng lệnh, đảm bảo hoạt động thực tế.
4. Viết formatter cho các định dạng đầu ra, xuất dữ liệu thực tế.
5. Đảm bảo traceability cho mọi lệnh, kiểm tra thực tế.
6. Tạo/cập nhật test thực tế cho từng class.
7. Cài đặt đầy đủ universe generation.
8. Kiểm tra lại toàn bộ hệ thống, đối chiếu với blueprint, không để sót chi tiết nào.
9. Chạy `validate --self` và kiểm thử toàn diện.
10. Tổng hợp checklist hoàn thiện, xác nhận dự án đạt 100% production-ready.
