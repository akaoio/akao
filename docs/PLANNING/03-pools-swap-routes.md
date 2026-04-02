# Kế hoạch Route Pools & Swap

> **Ngày lập:** 2026-04-02  
> **Dựa trên:** kiến trúc akao hiện tại + trí tuệ rút lõi từ wallet cũ  
> **Mục tiêu:** Xây 2 route mới — `/pools` (danh sách pools, giá, liquidity) và `/swap` (trao đổi giữa các crypto token)

---

## 1. Nền tảng đã có (không cần viết lại)

### 1.1 Backend logic hoàn chỉnh

Toàn bộ layer blockchain đã hoạt động trong `onchain.js`:

```
onchain thread
  └─ thread.scanPools()
       ├─ Construct.Chains() + Construct.Dexs()
       ├─ loop({ delay: [0, 10000] })     ← quét mỗi 10 giây
       ├─ dex.getRate({ pool: address })  ← gọi getReserves() / slot0()
       └─ thread.send({ Lives: { pools: { [chain]: { [address]: data } } } })
```

Dữ liệu pool sau khi quét có cấu trúc:

```javascript
Lives.pools = {
    "ETH": {
        "0xBb2b8038...": {
            // metadata (từ loadContract)
            chain: "ETH",
            dex: "uniswap",
            version: "V2",
            ABI: "UniswapV2Pool",
            address: "0xBb2b8038...",
            token0: "0x2260FAC5...",
            token1: "0xC02aaA39...",
            // live data (từ getRate)
            token0: {
                address: "0x2260FAC5...",
                configs: { name: "Wrapped BTC", symbol: "WBTC", decimals: 8 },
                quantity: 1234.56,    // reserve tính theo decimal
                rate: 15.3            // bao nhiêu ETH đổi 1 WBTC
            },
            token1: {
                address: "0xC02aaA39...",
                configs: { name: "Wrapped Ether", symbol: "WETH", decimals: 18 },
                quantity: 18900.0,
                rate: 0.0653          // bao nhiêu WBTC đổi 1 WETH
            },
            pairs: {
                "0x2260FAC5...": { "0xC02aaA39...": 15.3 },
                "0xC02aaA39...": { "0x2260FAC5...": 0.0653 }
            }
        }
    }
}
```

### 1.2 Dữ liệu forex đã có

`offchain.js` → `Lives.forex[base][quote]` = tỉ giá fiat thực.  
`Forex.convert(amount, from, to)` đã viết sẵn trong `src/core/Forex.js`.

### 1.3 Swap API đã có (V2 và V3)

`Dexs["ETH.uniswapV2"].swap({ token0, token1, amount0, slippage })` trả về TX.

**V2** (`DeFi/V2.js`):
```
getAmountsOut(amountIn, [token0, token1])
→ amountOutMin = amounts[1] × (100 − slippage) / 100
→ ERC20.approve(router, amountIn)
→ router.swapExactTokensForTokens(amountIn, amountOutMin, path, recipient, deadline)
```

**V3** (`DeFi/V3.js`):
```
quoter.quoteExactInputSingle(token0, token1, fee, amountIn, "0")
→ amountOutMin = quote × (100 − slippage) / 100
→ ERC20.approve(router, amountIn)
→ router.exactInputSingle({ tokenIn, tokenOut, fee, recipient, deadline, amountIn, amountOutMinimum, sqrtPriceLimitX96: "0" })
```

Cả hai versions dùng chung interface: `dex.swap({ token0, token1, amount0, slippage })`.  
`finddex()` trả về `{ dex, pool }` — `pool` cần thiết để lấy `fee` tier cho V3.

### 1.4 Gas estimation đã có trong `Chain` (EVM mixin)

`EVM.js` được mix vào `Chain` qua `Object.assign(this, EVM)`. Đã có:

```javascript
// chain.fee() → { amount, symbol }  (phí tính bằng native token)
const fee = await chain.fee({ from, to, amount, currency })
// chain.gas() → BigInt gas limit
const gasLimit = await chain.gas({ from, to, amount, currency })
// chain.gasPrice() → BigInt gas price
const gasPrice = await chain.gasPrice()
```

Pattern giống withdraw.js: `wallet.fee({ to, amount, currency })` → gọi xuống `chain.fee()`.  
Cho swap, vì gọi router contract thay vì transfer trực tiếp, cần encode calldata swap rồi truyền vào `chain.https.estimateGas({ from, to: router, data: encodedSwapCall })`.

### 1.5 Token logo đã có tại `/images/cryptos/`

Các file SVG đã được build vào `build/images/cryptos/`, phục vụ tại `/images/cryptos/`.  
Danh sách hiện có: `ethereum-eth-logo.svg`, `wrapped-bitcoin-wbtc-logo.svg`, `tether-usdt-logo.svg`, `usd-coin-usdc-logo.svg`, `bnb-bnb-logo.svg`, `binance-usd-busd-logo.svg`, `uniswap-uni-logo.svg`, `chainlink-link-logo.svg`, `ethena-usde-usde-logo.svg`, `near-protocol-near-logo.svg`, `toncoin-ton-logo.svg`, `tron-trx-logo.svg`.

**Quy tắc đặt tên:** `{name.toLowerCase().replace(/\s+/g, '-')}-{symbol.toLowerCase()}-logo.svg`

Ví dụ:
- `name: "Wrapped Bitcoin"`, `symbol: "WBTC"` → `wrapped-bitcoin-wbtc-logo.svg` ✓
- `name: "Ethereum"`, `symbol: "ETH"` → `ethereum-eth-logo.svg` ✓
- `name: "Tether"`, `symbol: "USDT"` → `tether-usdt-logo.svg` ✓

`ui-token` render:
```html
<img src="/images/cryptos/${name}-${symbol}-logo.svg"
     onerror="this.replaceWith(identicon)" />
```
Fallback nếu không có file: dùng `ui-identicon` (đã có sẵn trong akao).

### 1.6 Luồng dữ liệu live

```
onchain thread → thread.send({ Lives: { pools: {...} } })
  → Threads.update() trên main thread
  → events.emit("Lives.pools", data)
  → UI components lắng nghe events.on("Lives.pools", handler)
```

---

---

## 2. Route `/pools`

### 2.1 Mục đích

Hiển thị **danh sách tất cả liquidity pools** trên tất cả chains đã config, với:
- Tên cặp token (WBTC/WETH, ETH/USDT, ...)
- Giá hiện tại (rate)
- Giá trị quy đổi fiat (USD, VND, ...)
- Liquidity (tổng giá trị 2 bên trong pool)
- Chain nguồn (ETH / BSC / ...)
- DEX nguồn (Uniswap V2/V3, Pancakeswap, ...)

### 2.2 URL

```
/{locale}/pools
/{locale}/pools?chain=ETH
/{locale}/pools?chain=BSC&dex=pancakeswap
```

`chain` và `dex` là URL search params, lấy từ `Context.get("params")`.

### 2.3 Cấu trúc file

```
src/UI/routes/pools/
  index.js       ← route component (POOLS class)
  template.js    ← html layout
  styles.css.js  ← scoped styles

src/UI/components/pool/
  index.js       ← ui-pool component (1 dòng trong bảng)
  template.js
  styles.css.js

src/UI/components/token/
  index.js       ← ui-token component (logo + symbol + amount)
  template.js
  styles.css.js
```

> **Chú ý:** Component `pool` và `token` sẽ được tái sử dụng trong route swap.

### 2.4 Route component pattern

```javascript
// src/UI/routes/pools/index.js
import template from "./template.js"
import { html, render } from "/core/UI.js"
import { Context } from "/core/Context.js"
import { events } from "/core/Events.js"
import { Indexes, Lives } from "/core/Stores.js"
import States from "/core/States.js"

export class POOLS extends HTMLElement {
    constructor() {
        super()
        this.states = new States({ chain: null, pools: {} })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.update = this.update.bind(this)
    }

    async connectedCallback() {
        const params = Context.get("params")

        // Đọc dữ liệu đã cache trong IndexedDB để render ngay lập tức (không chờ scan)
        const cached = await Indexes.Lives.get("pools").once()
        if (cached) this.states.set({ pools: cached })

        // Lắng nghe live updates từ onchain thread
        this.subscriptions.push(
            events.on("Lives.pools", this.update),
            events.on("Lives.forex", this.update),
            Context.on("params", ({ value }) => this.filter(value))
        )

        // Filter theo chain/dex nếu có trong URL
        if (params?.chain || params?.dex) this.filter(params)
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    update(data) {
        // Merge dữ liệu mới vào states để trigger re-render
        const current = this.states.get("pools") || {}
        for (const [chain, chainPools] of Object.entries(data || {})) {
            current[chain] = current[chain] || {}
            Object.assign(current[chain], chainPools)
        }
        this.states.set({ pools: { ...current } })
    }

    filter(params) {
        this.states.set({ chain: params?.chain || null })
    }
}

customElements.define("route-pools", POOLS)
export default POOLS
```

### 2.5 Template pattern

```javascript
// src/UI/routes/pools/template.js
import { html } from "/core/UI.js"
import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import "/UI/components/pool/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.pools" /></h1>
        <main>
            <div id="list"></div>
        </main>
    </layout-main>
`
```

> **Render danh sách:** Route `connectedCallback` hoặc `states.on("pools")` sẽ render từng `<ui-pool>` vào `#list` thông qua `html` + `render`. Không render tất cả cùng lúc — cần xem xét virtual scroll hoặc pagination nếu pools nhiều.

### 2.6 Component `ui-pool` (1 dòng pool)

```javascript
// Nhận dataset attributes:
// data-chain, data-address, data-dex, data-version

// Hiển thị:
// [logo token0][symbol0] / [logo token1][symbol1]
// rate: 1 WBTC = 15.3 WETH
// fiat: ≈ $X,XXX
// liquidity: $X,XXX,XXX
// chain badge: ETH | BSC
// dex badge: Uniswap V2 | V3

// Nhận live data qua:
// events.on("Lives.pools", ...)  ← tự subscribe/unsubscribe
// events.on("Lives.forex", ...)  ← để tính giá trị fiat
```

### 2.7 Tính liquidity và giá fiat

Fiat pricing **bắt buộc** được hiển thị cùng với rate. Logic:

```
Bước 1 — Xây price graph từ Lives.pools
  Input: tất cả pools hiện có trong Lives.pools
  Mỗi pool đóng góp 2 cạnh vào graph:
    pool.token0.address → pool.token1.address : token0.rate
    pool.token1.address → pool.token0.address : token1.rate

Bước 2 — Xác định anchor (USD)
  Stablecoins: USDT, USDC, BUSD, USDE (symbol check)
  Nếu token1 là stable: token0.price_in_USD = token0.rate (rate đã là USD/token0)

Bước 3 — Traverse graph (BFS/DFS)
  Xuất phát từ stable anchor, lan dần:
    WETH.price_in_USD  = WETH.rate_against_USDT
    WBTC.price_in_USD  = WBTC.rate_against_WETH × WETH.price_in_USD
    ...
  Kết quả: priceMap = { [tokenAddress]: usdPrice }

Bước 4 — Tính liquidity pool
  liquidity_USD = token0.quantity × priceMap[token0.address]
                + token1.quantity × priceMap[token1.address]

Bước 5 — Áp dụng forex Lives.forex
  userFiat = Context.get("fiat").code   // e.g. "VND"
  display_price = usdPrice × Lives.forex["USD"][userFiat]
```

**Module helper:** `src/core/Utils/prices.js` (mới) — export function `buildpricemap(pools)` trả về `{ [tokenAddress]: usdPrice }`. Được gọi mỗi khi `Lives.pools` hoặc `Lives.forex` thay đổi.

```javascript
// src/core/Utils/prices.js
export function buildpricemap(pools) {
    const graph = {}   // { [address]: [{ neighbor, rate }] }
    const STABLES = new Set(["USDT", "USDC", "BUSD", "DAI", "USDE"])
    const prices = {}  // { [address]: usdPrice }

    // Xây graph từ tất cả pools
    for (const chainPools of Object.values(pools)) {
        for (const pool of Object.values(chainPools)) {
            const a = pool.token0.address, b = pool.token1.address
            if (!a || !b) continue
            graph[a] = graph[a] || []
            graph[b] = graph[b] || []
            graph[a].push({ neighbor: b, rate: pool.token0.rate, symbol: pool.token1.configs?.symbol })
            graph[b].push({ neighbor: a, rate: pool.token1.rate, symbol: pool.token0.configs?.symbol })
        }
    }

    // Seed stables với price = 1 USD
    for (const [address, edges] of Object.entries(graph)) {
        // Kiểm tra nếu token này là stable
        // (symbol nằm trong metadata của poolside)
    }
    // ... BFS từ stable seeds

    return prices
}
```

> Hàm `buildpricemap` được gọi trong `route-pools` và `route-swap` (qua `events.on`) — không phải trong onchain thread.

---

## 3. Route `/swap`

### 3.1 Mục đích

Cho phép user **trao đổi token** thông qua DEX. UI chọn token vào (from) và token ra (to), nhập số lượng, xem preview tỉ giá, rồi thực hiện swap.

### 3.2 URL

```
/{locale}/swap
/{locale}/swap?from=0xC02aaA39...&to=0x2260FAC5...&chain=ETH
```

Các params `from`, `to`, `chain` được đọc từ URL để pre-fill form.

### 3.3 Cấu trúc file

```
src/UI/routes/swap/
  index.js
  template.js
  styles.css.js

src/UI/components/token-select/
  index.js       ← ui-token-select: dropdown chọn token (tên + logo + balance)
  template.js
  styles.css.js
```

> `ui-token-select` tái sử dụng `ui-token` component từ route pools.

### 3.4 Route component pattern

```javascript
// src/UI/routes/swap/index.js
export class SWAP extends HTMLElement {
    constructor() {
        super()
        this.states = new States({
            from: null,     // { address, symbol, decimals, balance }
            to: null,       // { address, symbol, decimals }
            amount: "",     // số lượng token from
            quote: null,    // kết quả getAmountsOut / quoteExactInputSingle
            slippage: 0.5,  // % slippage tolerance
            loading: false
        })
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this.subscriptions = []
        this.quote = this.quote.bind(this)
        this.submit = this.submit.bind(this)
        this.estimategas = this.estimategas.bind(this)
    }

    connectedCallback() {
        const params = Context.get("params")
        // Pre-fill từ URL params nếu có
        if (params?.from || params?.to) this.prefill(params)

        this.$from = this.shadowRoot.querySelector("#from")
        this.$to = this.shadowRoot.querySelector("#to")
        this.$amount = this.shadowRoot.querySelector("#amount")
        this.$quote = this.shadowRoot.querySelector("#quote")
        this.$gas = this.shadowRoot.querySelector("#gas")
        const $submit = this.shadowRoot.querySelector("#submit")

        this.$amount.addEventListener("input", this.quote)
        $submit.addEventListener("click", this.submit)

        this.subscriptions.push(
            this.$from.events.on("select", ({ detail }) => {
                this.states.set({ from: detail })
                this.quote()
            }),
            this.$to.events.on("select", ({ detail }) => {
                this.states.set({ to: detail })
                this.quote()
            }),
            events.on("Lives.pools", () => this.refreshtokens()),
            () => {
                this.$amount.removeEventListener("input", this.quote)
                $submit.removeEventListener("click", this.submit)
            }
        )

        Elements.Access?.checkpoint()
    }

    disconnectedCallback() {
        this.subscriptions.forEach((off) => off())
    }

    prefill(params) {
        if (params.from) this.states.set({ from: { address: params.from } })
        if (params.to) this.states.set({ to: { address: params.to } })
    }

    // Tạo danh sách token unique từ tất cả pools
    refreshtokens() {
        const tokens = {}
        const chain = this.states.get("chain") || Context.get("params")?.chain
        const pools = chain ? Lives.pools?.[chain] : Lives.pools
        for (const chainPools of Object.values(pools || {})) {
            for (const pool of Object.values(chainPools)) {
                tokens[pool.token0.address] = pool.token0
                tokens[pool.token1.address] = pool.token1
            }
        }
        this.$from.options = Object.values(tokens)
        this.$to.options = Object.values(tokens)
    }

    // Debounced quote (500ms)
    quote() {
        clearTimeout(this._quotePending)
        this._quotePending = setTimeout(async () => {
            const from = this.states.get("from")
            const to = this.states.get("to")
            const amount = Number(this.$amount.value)
            if (!from || !to || !amount) return

const { dex, pool } = this.finddex(from.address, to.address) || {}
            if (!dex) return

            this.states.set({ loading: true })
            // V2: getAmountsOut, V3: quoteExactInputSingle (fee lấy từ pool.fee)
            const result = await dex.getAmountsOut?.({ token0: from.address, token1: to.address, amount })
                        || await dex.quote?.({ token0: from.address, token1: to.address, fee: pool?.fee, amountIn: amount })
            this.states.set({ loading: false, quote: result })

            if (result && !result.error) {
                this.$quote.textContent = `≈ ${result.amount} ${to.configs?.symbol}`
                this.estimategas()
            }
        }, 500)
    }

    // Tìm DEX + pool phù hợp (có pool chứa cặp from/to)
    // Trả về { dex, pool } — pool cần thiết để lấy fee tier cho V3
    finddex(fromAddress, toAddress) {
        for (const dex of Object.values(Dexs)) {
            const chainPools = Lives.pools?.[dex.configs.chain] || {}
            for (const pool of Object.values(chainPools)) {
                if (
                    (pool.token0.address === fromAddress && pool.token1.address === toAddress) ||
                    (pool.token1.address === fromAddress && pool.token0.address === toAddress)
                ) return { dex, pool }
            }
        }
        return null
    }

    async estimategas() {
        clearTimeout(this._gasPending)
        this._gasPending = setTimeout(async () => {
            const from = this.states.get("from")
            const to = this.states.get("to")
            const amount = Number(this.$amount.value)
            if (!from || !to || !amount) return

            const { dex } = this.finddex(from.address, to.address) || {}
            if (!dex) return

            // chain.fee() là EVM mixin method: fee({ from, to, amount, currency })
            // Cho swap: encode calldata router → truyền vào chain.https.estimateGas()
            const wallet = Wallets[dex.configs.chain]
            if (!wallet?.address) return
            const chain = dex.chain

            // currency = token from (ERC20 config object)
            const currency = from.configs
            const fee = await chain.fee({ from: wallet.address, to: dex.configs.router, amount, currency })
            if (!fee) return

            const label = Context.get(["dictionary", "gasFee"])
            this.$gas.textContent = `${label}: ${fee.amount} ${fee.symbol}`
        }, 500)
    }

    async submit() {
        const from = this.states.get("from")
        const to = this.states.get("to")
        const amount = Number(this.$amount.value)
        const slippage = this.states.get("slippage")

        if (!from || !to || !amount)
            return notify({ content: Context.get(["dictionary", "missingRequiredFields"]), autoClose: true })

        const { dex, pool } = this.finddex(from.address, to.address) || {}
        if (!dex)
            return notify({ content: Context.get(["dictionary", "nopoolFound"]), autoClose: true })

        this.states.set({ loading: true })
        // dex.swap() xử lý V2 và V3 nội bộ qua architecture mixin
        // V3 cần fee tier từ pool config
        const tx = await dex.swap({ token0: from.address, token1: to.address, amount0: amount, slippage, fee: pool?.fee })
        this.states.set({ loading: false })

        if (tx && !tx.error) {
            notify({ content: Context.get(["dictionary", "transactionSent"]), autoClose: true })
            this.states.set({ amount: "", quote: null })
            this.$amount.value = ""
            this.$quote.textContent = ""
        } else {
            notify({ content: Context.get(["dictionary", "transactionError"]), autoClose: true })
        }
    }
}

customElements.define("route-swap", SWAP)
```

### 3.5 Template pattern

```javascript
// src/UI/routes/swap/template.js
export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.swap" /></h1>
        <main>
            <div class="swap-box">
                <div class="field">
                    <label><ui-context data-key="dictionary.from" /></label>
                    <ui-token-select id="from" />
                    <input type="number" id="amount" min="0" step="any" />
                </div>

                <div class="arrow">↕</div>

                <div class="field">
                    <label><ui-context data-key="dictionary.to" /></label>
                    <ui-token-select id="to" />
                    <div id="quote"></div>
                </div>

                <div class="meta">
                    <div id="gas"></div>
                    <div id="slippage">
                        Slippage:
                        <input type="number" id="slippage-input" value="0.5" min="0.1" max="50" step="0.1" />%
                    </div>
                </div>

                <div class="buttons">
                    <ui-button class="full" data-left="swap" id="submit">
                        <ui-context data-key="dictionary.swap" />
                    </ui-button>
                </div>
            </div>
        </main>
    </layout-main>
`
```

### 3.6 Component `ui-token-select`

Dropdown chọn token từ danh sách available. Tương tự `ui-wallets` nhưng cho token:

```javascript
// Nhận: options = [{ address, configs: {symbol, name, decimals}, quantity }]
// Emit: CustomEvent("select", { detail: { address, configs, quantity } })

// Hiển thị mỗi option:
// [logo/identicon token] [symbol] [name] [balance nếu có]

// Internal state: States({ selected: null, open: false })
```

---

## 4. Luồng dữ liệu tổng thể

```
[onchain.js thread]
 scanPools() mỗi 10s
   → Indexes.Lives.get("pools").get(dex.id).get(address).put(data)
   → thread.send({ Lives: { pools: {...} } })

[Threads.js - main thread]
 Threads.update({ pools: {...} })
   → merge(Lives, data)
   → events.emit("Lives.pools", changedData)

[route-pools component]
 events.on("Lives.pools", update)
   → states.set({ pools: merged })
   → render() → ui-pool elements update

[route-swap component]
 events.on("Lives.pools", refreshtokens)
   → rebuild token dropdown options
 amount input → quote() debounced 500ms
   → dex.getAmountsOut() or dex.quote()
   → update quote display

[offchain.js thread]
 Lives.forex từ GunDB pub
   → events.emit("Lives.forex", data)
   → route-pools.update() → tính lại fiat values
```

---

## 5. Keys i18n cần thêm

Tạo file `src/statics/i18n/*.yaml` cho các key chưa có:

| Key | File | EN | VI |
|-----|------|----|----|
| `pools` | `pools.yaml` | Pools | Pools |
| `swap` | `swap.yaml` | Swap | Hoán đổi |
| `from` | `from.yaml` | From | Từ |
| `to` | `to.yaml` | To | Đến |
| `liquidity` | `liquidity.yaml` | Liquidity | Thanh khoản |
| `price` | `price.yaml` | Price | Giá |
| `rate` | `rate.yaml` | Rate | Tỉ giá |
| `slippage` | `slippage.yaml` | Slippage | Trượt giá |
| `nopoolFound` | `nopoolFound.yaml` | No pool found | Không tìm thấy pool |
| `quote` | `quote.yaml` | Quote | Báo giá |
| `insufficientBalance` | `insufficientBalance.yaml` | Insufficient balance | Số dư không đủ |
| `selectToken` | `selectToken.yaml` | Select token | Chọn token |
| `searchToken` | `searchToken.yaml` | Search token... | Tìm token... |
| `dex` | `dex.yaml` | DEX | DEX |

> Xem đủ 19 locale đang có trong `src/statics/i18n/` để điền cho đủ tất cả ngôn ngữ.

---

## 6. Thứ tự triển khai

### Bước 1 — Utility + components nền

1. Viết `src/core/Utils/prices.js` — export `buildpricemap(pools)` (price graph BFS từ stables)
2. Tạo `src/UI/components/token/` — `ui-token`: logo SVG (`/images/cryptos/`) + symbol + optional amount; fallback `ui-identicon`
3. Thêm i18n keys: `pools`, `swap`, `rate`, `liquidity`, `price`, `from`, `to`, `slippage`, `quote`, `nopoolFound`, `insufficientBalance`, `selectToken`, `searchToken`, `dex`, `gasFee`
4. `npm run build:core`

### Bước 2 — Route `/pools`

5. Tạo `src/UI/components/pool/` — `ui-pool`: hiển thị 1 pool row
   - Pair tokens (2× `ui-token`)
   - Rate: `1 WBTC = 15.3 WETH`
   - Fiat price: `≈ $X,XXX` (dùng `buildpricemap` + `Lives.forex` + `Context.get("fiat")`)
   - Liquidity fiat: `$X,XXX,XXX TVL`
   - Badges: chain + DEX name + version (V2/V3)
   - Self-subscribe `events.on("Lives.pools")` và `events.on("Lives.forex")`
6. Tạo `src/UI/routes/pools/` — subscribe `events.on("Lives.pools")`, đọc cache `Indexes.Lives`, filter theo URL params `chain`/`dex`
7. `npm run build:core` → kiểm tra tại `/en/pools`; V2 và V3 pools phải đều hiển thị

### Bước 3 — Component `ui-token-select`

8. Tạo `src/UI/components/token-select/` — dropdown chọn token
   - Xây từ unique tokens trong `Lives.pools`
   - Mỗi option: `ui-token` + name + balance (từ `Lives.balances`)
   - Search/filter theo symbol hoặc name
   - Emit `CustomEvent("select", { detail: token })`

### Bước 4 — Route `/swap`

9. Tạo `src/UI/routes/swap/`
10. Integrate `ui-token-select` (2 cái: from + to) + `ui-wallets`
11. `finddex()` trả về `{ dex, pool }` — hỗ trợ cả V2 (direct) và V3 (direct với fee)
12. `quote()` debounce 500ms:
    - V2: `dex.getAmountsOut({ token0, token1, amount })`
    - V3: `dex.quote({ token0, token1, fee: pool.fee, amountIn: amount })`
    - Hiển thị output + fiat equivalent dùng `buildpricemap`
13. `estimategas()` debounce 500ms → `chain.fee({ from: walletAddress, to: router, amount, currency: fromToken.configs })`
14. `submit()` → `dex.swap({ token0, token1, amount0, slippage, fee: pool?.fee })` (V2 và V3 xử lý nội bộ)
15. Balance check: `amount ≤ Lives.balances[chain][from.address]` trước khi submit
16. `npm run build:core` → kiểm tra tại `/en/swap`; test với V2 pool và V3 pool

---

## 7. Rủi ro và cần làm rõ

| Vấn đề | Hiện trạng | Cần làm |
|--------|-----------|---------|
| Multi-hop swap path | `finddex()` chỉ tìm direct pair | Hỗ trợ A→B→C trong iteration 2; MVP chỉ direct pairs |
| Token logo không có file | `/images/cryptos/` có 12 token | Fallback `ui-identicon`; bổ sung SVG khi có token mới |
| Gas cho swap router | `chain.fee()` tính cho ERC20 transfer, không phải router call | Cần encode swap calldata → `chain.https.estimateGas({ to: router, data })` |
| V3 fee tier | Pool V3 cần `fee` (500/3000/10000) trong swap params | `finddex()` trả về `pool` kèm `fee` — dùng trực tiếp |
| Balance check | `Lives.balances[chain][address]` có từ `onchain.js` | Disable submit + hiển thị `insufficientBalance` nếu amount > balance |
| Multi-chain pools hiển thị | `Dex.id = "{chain}.{dex}{version}"` | UI filter theo `Context.get("params").chain` nếu có |

| File | Vai trò |
|------|---------|
| [src/core/threads/onchain.js](../../src/core/threads/onchain.js) | `scanPools()` — đã hoạt động |
| [src/core/Dex.js](../../src/core/Dex.js) | Interface `getRate()`, `swap()` |
| [src/core/DeFi/V2.js](../../src/core/DeFi/V2.js) | V2: `getAmountsOut` → `swapExactTokensForTokens` |
| [src/core/DeFi/V3.js](../../src/core/DeFi/V3.js) | V3: `quoteExactInputSingle` → `exactInputSingle` |
| [src/core/Chains/EVM.js](../../src/core/Chains/EVM.js) | `chain.fee()`, `chain.gas()`, `chain.gasPrice()` |
| [src/core/Stores.js](../../src/core/Stores.js) | `Lives`, `Dexs`, `Wallets`, `Chains` globals |
| [src/core/Events.js](../../src/core/Events.js) | `events.on("Lives.pools", ...)`, `events.on("Lives.forex", ...)` |
| [src/core/Forex.js](../../src/core/Forex.js) | `Forex.convert()` |
| [src/core/Utils/prices.js](../../src/core/Utils/prices.js) | `buildpricemap(pools)` — **tạo mới** |
| [build/images/cryptos/](../../build/images/cryptos/) | SVG logos: 12 token; path `/images/cryptos/` |
| [src/UI/routes/withdraw/index.js](../../src/UI/routes/withdraw/index.js) | Pattern tham chiếu: gas estimation + submit form |
| [src/UI/components/wallets/index.js](../../src/UI/components/wallets/index.js) | Pattern tham chiếu cho `ui-token-select` |
