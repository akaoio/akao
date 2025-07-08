Suy nghĩ đánh giá của tôi (chủ dự án) về dự án akao hiện tại:

1. Về kiến trúc:

- Kiến trúc của hệ thống hiện nay đang thể hiện sự mơ hồ giữa khái niệm triết lý và luật.
- Có nhiều triết lý khi mô tả bằng ngôn ngữ thì hay nhưng khi mã hóa thành logic hình thức thì mang tính áp đặt như là luật.
- Quá nhiều triết lý và luật đã bị cấy ghép bậy bạ cảm tính vào trong hệ thống một cách thủ công, gây ra sự bất đồng bộ.
- Cần phải tinh gọn bộ máy, nếu triết lý mà không để làm gì (không có tương tác logic với các thành phần khác trong hệ thống) thì chúng ta chỉ cần luật và bộ luật thôi.
- Kiến trúc hiện tại thể hiện sự mâu thuẫn nội tại ngay trong chính tư duy của người thiết kế (là tôi huhu hihi).
- Cần giảm thiểu triết lý, luật, bộ luật. Chỉ giữ lại những gì thật sự hữu dụng, thật sự có giá trị áp dụng (logic chặt chẽ và ảnh hưởng tương quan đa chiều với nhau) thì mới giữ lại. Còn đâu phải bỏ hết.
- Cần refactor lại toàn bộ triết lý, luật, bộ luật để rõ ràng về ý nghĩa tồn tại của chính chúng.
- Bộ luật hiện nay chia nhóm theo ngôn ngữ, như vậy không đúng ý đồ thiết kế. Ý đồ thiết kế của tôi là bộ luật (ruleset) đóng vai trò như category nhưng chia category dưới góc độ quản trị kiến trúc chứ không phải quản trị ngôn ngữ.
- Validate và self validate có sự mơ hồ: Tôi không có ý đồ thiết kế cái gọi là "self validate". Vì validate phải là trọn vẹn, nó có tác dụng trong bất kỳ codebase nào, bao gồm codebase của nó. Đây là sự mâu thuẫn trong thiết kế.

2. Về quản trị:

- Chưa có quy chuẩn bắt buộc để đánh id, mô tả (what, when, where, why, how, links...) từng thư mục từng file và từng phần trong các file.
- Chưa có mô tả cách thức làm việc, quy trình làm việc, các nguyên tắc làm việc bắt buộc. Hậu quả là các qua nhiều development session, rác và nợ kỹ thuật phát sinh ra thêm.
- Cần áp dụng một khu là "artifacts/", trong đó chia thư mục con từng branch tương ứng. Ví dụ artifacts/{branch}/{phase}/{step}, trong đó phải có plan.md, có checklist.md bắt buộc. Nó cũng phải có phases trong đó có các steps, và trong từng step có unit test của step đó. Unit test của nó phải có đầu vào, đầu ra, deterministic expected outputs.
- Tất cả các file trong toàn hệ thống đều phải tuân theo chuẩn của hệ thống, kể cả artifacts/*.* hay triết lý, hay luật, hay bộ luật.

3. Về công nghệ:

- Pure Logic Engine:
   - Engine chưa đủ trưởng thành. Hiện nay hệ thống đang dùng YAML để mô tả cho Pure Logic Engine. Nhưng phần lõi của Pure Logic Engine đã đủ mạnh, bằng chứng là nó đã chạy được mô hình tính toán chứng minh định lý Bất tòan của Godel.
   - Tôi muốn: Phát triển Akao Pure Logic thành ngôn ngữ lập trình cho logic hình thức
   - Cần khai sinh một định dạng mới ".a" (Akao Pure Logic). Yêu cầu:
      - Dễ viết, dễ hiểu như python
      - Hiểu được các ký tự logic phức tạp thuộc logic hình thức, số học Godel v.v..
      - Hiểu song song: code bằng các lệnh ký tự Logic hình thức được, hoặc bằng các lệnh tương đương được (hỗ trợ bàn phím thông dụng, rất quan trọng)
   - Akao trở thành 1 runtime cho Pure Logic, như Node là runtime cho JS. Một số ví dụ:
      - "akao main.p": tương đương "node main.js"
      - "akao start": tương đương "npm start"
      - "akao run build": tương đương "npm run build"
   - Ý nghĩa của việc này:
      - Thoát ly khỏi YAML và chỉ coi YAML như một các diễn đạt khác như JSON.
      - 
- Parser vạn năng:
   **Parser vạn năng** là một hệ thống phân tích cú pháp có khả năng:

   * Hiểu và trích xuất cấu trúc logic từ **bất kỳ ngôn ngữ lập trình hoặc cấu hình nào**
   * Chuyển hóa chúng thành một **biểu diễn trung gian thống nhất** (Intermediate Representation – IR)
   * Từ đó, **biến đổi sang Akao Pure Logic** một cách **phi-ngữ-cảnh, không phụ thuộc vào ngôn ngữ gốc**

   ---

   ## ⚙️ **Đặc Điểm Cốt Lõi**

   ### 1. **Đa ngôn ngữ (Multi-language aware)**

   * Hỗ trợ các ngôn ngữ: C, C++, Python, JavaScript, YAML, JSON, Prolog, v.v...
   * Tự động nhận diện ngôn ngữ đầu vào **mà không cần chỉ định thủ công**

   ### 2. **Sinh AST phổ quát (Universal AST)**

   * Mọi ngôn ngữ đều được ánh xạ về một **cấu trúc cây trừu tượng thống nhất** với các node logic như:

   * `Definition`, `Assignment`, `FunctionCall`, `If`, `Loop`, `Import`, `Type`, `Structure`, `Rule`
   * Không phụ thuộc vào từ khóa hoặc cấu trúc riêng của từng ngôn ngữ

   ### 3. **Chuẩn hóa ngữ nghĩa (Semantic normalization)**

   * Các biểu thức logic, toán học, rẽ nhánh, gọi hàm được **biểu diễn lại thành tập hợp logic sơ cấp**
   * Biến thể cú pháp khác nhau (e.g. `if x > 0:` vs `if (x > 0) {`) được quy về cùng 1 dạng logic

   ### 4. **Không cần thư viện ngoài (Zero-dependency)**

   * Tự viết toàn bộ bằng C++
   * Không phụ thuộc vào bất kỳ trình parser ngoài nào như tree-sitter, yacc, hay boost
   * Tương thích tuyệt đối với triết lý Green Computing và Tối giản của Akao

   - Chưa có parser vạn năng để transpile mọi ngôn ngữ (phổ biến) sang Pure Logic, nên dữ liệu đầu vào cho Pure Logic sai. Hậu quả là hệ thống triết lý và luật vốn đã mơ hồ, lại cộng thêm dữ liệu đầu vào sai, nên công nghệ lõi chưa hoàn thiện.
   - Cho dù có parser vạn năng đi chăng nữa, thì thực trạng là chưa có tiêu chuẩn bắt buộc về việc đánh id và mô tả, cộng thêm chưa có parser vạn năng để quét, nên chưa thể bắt lỗi hệ thống. Bản thân bộ phận bắt lỗi chưa thực sự hoạt động.