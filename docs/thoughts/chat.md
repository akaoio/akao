CHAT 3 BÊN

Ứng dụng khi giao dịch

STACKS:
- sea.secret: trao đổi khóa Diffie-Hellman
- sea.pair: derive deterministic keys from seed
- sea.encrypt / sea.decrypt: mã hóa và giải mã tin nhắn chat

Lưu ý: derive trên sea.pair là hệ curve P256, không như ethers là hệ secp256k1, curve khác nhau.

Có 3 bên:
E: Escrow - quyền tối cao, xử lý kiện tụng
B: Buyer - trader
S: Seller - trader

Mỗi người có 1 pair gốc {pub, priv, epub, epriv}

1. B tạo secret với E -> secret seed BE -> derive ra ví BE
2. S tạo secret với E -> secret seed SE -> derive ra ví SE
3. 2 ví BE và SE trao đổi epub để tạo secret BSE -> encrypt/decrypt dựa vào secret BSE

=> Như vậy ta có 1 cái secret BSE mà cả 3 người cùng biết, dùng để làm salt khi encrypt/decrypt.

- B dùng ví gốc của B, S dùng ví gốc của S, nhưng secret là secret BSE.
- E biết secret BSE vì E tính được secret BE và secret SE