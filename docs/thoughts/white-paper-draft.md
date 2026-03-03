ESCROW 4 BÊN:
- E: escrow
- B: buyer
- S: seller
- A: affiliate

Stacks:
- sea.secret: Diffie Hellman secret
- ethersjs: derive xprv/xpub non-hardened BIP-32
- offline-first!

## Bước 1: S tạo item trên sàn của E
- S tạo data item -> hash -> item id
- S dùng sea.secret tạo 1 common secret (Diffie Hellman) với E, dùng làm 1 phần seed (chỉ S/E biết)
- seed S/E = item id + secret -> hash
- S dùng ethersjs, derive ra ROOT xprv/xpub của S/E (mỗi item có 1 root xprv riêng)
- S index S/E xpub lên đúng địa chỉ item (theo item id)
- E cũng có thể tạo secret nên cũng biết root xprv S/E

## Bước 2: A giới thiệu B vào sàn E mua item S
- A dùng sea.secret tạo 1 common secret với E, dùng làm 1 phần seed (chỉ A/E biết)
- seed A/E = secret + item id -> hash
- A dùng ethersjs, derive ra ROOT xprv/xpub của A/E (mỗi item có 1 root xprv riêng)
- A dùng root xpub A/E làm affiliate link để gửi cho B

## Bước 3: B thanh toán item S trên sàn của E
- B dùng sea.secret tạo 1 common secret với E (chỉ B/E biết), làm 1 phần seed
- seed B/E = secret + order id -> hash
- B dùng ethersjs, kết hợp S/E xpub + seed (như salt để index) -> derive ra S/E xpub mới tạm gọi là ví **VSE** (dạng ví watch-only, B không có xprv **VSE**, trong đó S/E là chủ, nhưng E biết xprv còn S không biết)
- B gửi số tiền cần thiết vào đúng ví **VSE** -> B xác nhận ví đã thanh toán

## Bước 3A: B thanh toán tiền hoa hồng cho A, phí sàn cho E
- B truy cập link mà A gửi, trong đó có root xpub A/E
- Tương tự như trên, B derive ra 1 ví có seed là secret B/E, nhưng dùng ROOT xpub A/E, tạm gọi là ví VAE (ví này E toàn quyền, A không biết seed B/E nên không có xprv A/E)
- B gửi tiền vào ví **VAE** ? Cũng được, nhưng rườm rà, và thực tế chỉ diễn ra ở lớp logic chứ không ở UI
- UX: Thay vào đó, B nạp sẵn tiền vào ví của B trên sàn của E, khi B thanh toán, client sẽ kiểm tra balance ví B -> B bấm nút thanh toán thì tự khắc tạo 3 tx và broadcast lên RPC:
    - Thanh toán cho E phí sàn
    - Thanh toán cho A phí môi giới
    - Thanh toán cho S phí item/service

## Bước 4: S fulfill đơn hàng
- S nhận đơn hàng, xử lý đơn hàng
- S gửi request B xác nhận đơn hàng thành công
    - B xác nhận đơn hàng thành công -> gửi seed cho S
    - Nếu B bỏ mặc, sau 24h kể từ khi S gửi request, thì E gửi seed cho S
    - Nếu B nói chưa thành công -> tranh chấp -> S gửi proof chứng minh
        - Nếu S đúng -> E gửi seed cho S
        - Nếu B đúng -> E refund cho B
- S bỏ mặc, không request B xác nhận đơn hàng, tiền vẫn nằm đó trong ví V

## Bước 5: E refund cho B, E thanh toán hoa hồng cho A
- E biết S/E secret -> derive root xprv S/E
- E biết B/E secret -> derive ra seed
- E derive ra ví **VSE** dựa vào seed + xprv S/E -> refund cho B
- Nếu đơn hàng thành công, E gửi seed **VAE** cho A để unlock tiền hoa hồng
- Nếu đơn hàng thất bại và S sai -> E refund tiền ví **VAE** cho B cũng theo nguyên tắc logic trên
- Nếu đơn hàng tranh chấp, và B sai ? -> Đơn hàng được coi là THÀNH CÔNG -> A vẫn nhận được seed **VAE** để unlock hoa hồng