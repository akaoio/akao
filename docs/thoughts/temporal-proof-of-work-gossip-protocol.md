# Temporal PoW Gossip Protocol - DRAFT

## Lịch sử & vấn đề:

- Một vấn đề lớn trong hệ dữ liệu phi tập trung là thời gian, timestamp. Trong hệ phân tán, thời gian UTC timestamp giữa các máy là không đồng nhất, có thể bị drift tăng dần, tích lũy drift theo thời gian.

- Khoảng những năm 60 70, giờ UTC ra đời, đồng thuận epoch 0 là 00:00:00 UTC on January 1, 1970 -> Unix timestamp ra đời. Vào năm 1985, NTP (Network Time Protocol) ra đời giúp đồng bộ hóa timestamp toàn cầu.

- Ngay cả khi có unix timestamp và NTP, sự đồng bộ vẫn không diễn ra 100% do network latency -> lệch tới vài trăm ms giữa các máy tính.

- Trong một hệ dữ liệu phi tập trung, rất khó để xóa một dữ liệu. Ví dụ trong gundb, bạn chỉ có thể put giá trị null cho 1 key khi muốn xóa nó, nhưng bản chất là quên nó chứ nó không thực sự biến mất trên đĩa cứng và toàn mạng. Nếu bạn xóa trên đĩa cứng, dữ liệu từ peer khác sẽ tràn sang và lấp vào đĩa (zombie data). Cách SAI là duy trì 1 danh sách các dữ liệu cần xóa. Sai là vì bản thân cái danh sách dữ liệu cần xóa chính là một dạng dữ liệu, như vậy chỉ là chuyển mềm sang cứng, chứ không thực sự xóa.

- Mọi người cần 1 nguồn sự thật chung về thời gian để biết ghi dữ liệu vào đâu để tìm thấy được nhau trong mạng. Trong blockchains như ETH/BSC, họ dùng đồng hồ epoch dựa vào block number. Vấn đề của epoch dạng dùng block number là bạn phải duy trì 1 chuỗi các block numbers => phải truy ngược lại từng block để tìm mốc thời gian. Bản thân number của block là hash nên không có giá trị thời gian. Do không tin tưởng nhau, nên từng peer phải gọi RPC để truy lại chuỗi khối => phải duy trì RPC ngày càng đắt đỏ, không offline-first.

## Giải pháp

- Ta chia thời gian ra làm các nến 1m 5m 30m 1h v.v... bắt đầu từ UTC timestamp 0. Mỗi nến chính là số timestamp, được làm tròn chính xác tuyệt đối về mặt toán học, ms là 0 => mọi máy đều tự tính được chính xác danh sách nến kèm giá trị timestamp của từng nến, theo độ dài thời gian (1m 5m 30m v.v...) chỉ cần toán học, không cần chuỗi khối, không cần RPC. Mọi giá trị nến quá khứ hay tương lai đều được mọi người biết trước mà không cần smart contract.

- Từng máy có thể dựa vào đa nguồn NTP, timestamp trong block mới nhất của các chains => suy ra thời gian hiện tại tương đối của bản thân và của mạng => khoảng nến hiện tại (mang giá trị tuyệt đối vì đã được làm tròn toán học). Mỗi peer đều chỉ duy trì dữ liệu và chấp nhận dữ liệu trong một khoảng tập hợp các nến nhất định tạm gọi là time window hay candle range, ví dụ khoảng (current - 100 nến -> current + 2 nến). Ai fair play khi ghi dữ liệu thì sẽ sẽ được chấp nhận bởi toàn mạng, ai không fair play thì mọi người không nhìn thấy và cũng không chấp nhận dữ liệu của họ.

## Nâng cao

- Để giảm thiểu việc spam dữ liệu, ta bắt mine nhẹ khi ghi dữ liệu. Ví dụ mỗi data đều phải ký và phải mine với 1 nonce để ra hash có prefix xxxxxx. Độ dài của prefix được quy định ngay trong soul/key. Các dữ liệu ghi vượt ngoài window thời gian sẽ không được mạng chấp nhận.

- Khi một nến đã già và vượt ra ngoài time window, không ai bảo ai nhưng mọi người tự biết phải xóa trong đĩa cứng => không cần lan truyền tin tức, không cần duy trì danh sách xóa.