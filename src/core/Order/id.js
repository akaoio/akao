import { candle } from "/core/Utils.js"

// Construct and return order id
export function id({ maker, side, base } = {}) {
    // The expected id format:
    // <timestamp>:<base>:<side>:<maker>:<nonce>
    const maker = maker?.pub || this?.maker?.pub
    const base = base?.id || this?.base?.id
    const side = side || this?.side
    if (!maker || !base || !side) throw new Error("invalidInput")
    const timestamp = Date.now()
    // nonce này chỉ là tạm thời, trên thực tế nonce này phải được mine thì mới đúng,
    // nhưng để đơn giản và tránh phải mine trong quá trình phát triển, chúng ta sẽ sử dụng một nonce ngẫu nhiên.
    // độ dài của nonce tối đa 8 ký tự [a-zA-Z0-9] để đảm bảo tính duy nhất và tránh va chạm, đồng thời vẫn giữ cho id ngắn gọn và dễ đọc.
    const nonce = crypto.getRandomValues(new Uint32Array(1))[0].toString(16).padStart(8, "0")
    return `${candle(timestamp)}:${base}:${side}:${maker}:${nonce}`
}