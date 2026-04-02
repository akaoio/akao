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
       ├─ Indexes.Lives.get("pools").get(dex.id).get(pool.address).put(data)  ← IDB: key = dex.id
       └─ thread.send({ Lives: { pools: { [pool.chain]: { [pool.address]: data } } } })
              ← pool.chain là NUMERIC (e.g. 1, 11155111), không phải string "ETH"
```

> **`dex.id` format:** `"${chain.id}.${dex}${version}"` — e.g. `"1.UniswapV2"`, `"11155111.UniswapV2"`. Không có dấu phân cách giữa tên dex và version.

Dữ liệu pool sau khi quét (từ `onchain.js` scanPools()):

```javascript
// Lives.pools trong bộ nhớ — key là chain.id NUMERIC
Lives.pools = {
    1: {                               // ← chain.id numeric, e.g. 1 = Ethereum mainnet
        "0x004375Dff5...": {
            // từ loadContract({ chain: 1, address: poolAddr })
            chain: 1,                  // ← NUMERIC (từ contract JSON "chain": 1)
            type: "pool",
            dex: "Uniswap",            // ← capitalized, khớp với configs.yaml
            version: "V2",             // ← "V2" hoặc "V3"
            ABI: "UniswapV2Pool",
            address: "0x004375Dff5...",
            fee: 3000,                 // từ contract JSON; V3 dùng làm fee tier
            // token0/token1 = Object.assign({ configs }, rates.tokenX, { address })
            token0: {
                address: "0x2260FAC5...",
                configs: { name: "Wrapped BTC", symbol: "wrapped-bitcoin-wbtc-logo.svg", decimals: 8, chain: 1 },
                quantity: 1234.56,     // reserve sau khi chia decimals
                rate: 15.3             // bao nhiêu token1 / 1 token0 (reserve1/reserve0)
            },
            token1: {
                address: "0xC02aaA39...",
                configs: { name: "Wrapped Ether", symbol: "ethereum-eth-logo.svg", decimals: 18, chain: 1 },
                quantity: 18900.0,
                rate: 0.0653           // bao nhiêu token0 / 1 token1 (reserve0/reserve1)
            },
            pairs: {
                "0x2260FAC5...": { "0xC02aaA39...": 15.3 },    // token0 rate
                "0xC02aaA39...": { "0x2260FAC5...": 0.0653 }   // token1 rate
            }
            // pairs = undefined nếu cả 2 rate đều = 0
        }
    },
    11155111: { ... }   // Sepolia testnet
}
```

> **`token0.rate`** = bao nhiêu token1 bạn nhận được cho 1 token0 = `reserve1/reserve0`.  
> **`configs.symbol`** = SVG filename (e.g. `"wrapped-bitcoin-wbtc-logo.svg"`), không phải ticker.  
> **`pairs` có thể `undefined`** nếu `rate === 0` (pool không có thanh khoản).

Dữ liệu cùng pool trong **IndexedDB** được lưu theo `dex.id` (KHÁC cấu trúc bộ nhớ):
```
Indexes.Lives["pools"]["1.UniswapV2"]["0x004375..."]   ← key là dex.id string
Lives.pools[1]["0x004375..."]                          ← key là chain.id numeric
```

### 1.2 Dữ liệu forex đã có

`offchain.js` → `Lives.forex[base][quote]` = tỉ giá fiat thực.  
`Forex.convert(amount, from, to)` đã viết sẵn trong `src/core/Forex.js`.

### 1.3 Swap API đã có (V2 và V3)

`Dexs["1.UniswapV2"].swap({ token0, token1, amount0, slippage })` trả về TX. (key = `dex.id` = `"${chain.id}.${dex}${version}"`, e.g. `"1.UniswapV2"`, `"11155111.UniswapV3"`)

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

Mỗi token/chain config có field `symbol` = **filename SVG** (không phải ticker). Ví dụ từ `currencies.yaml`:

```yaml
- name: WBTC
  symbol: wrapped-bitcoin-wbtc-logo.svg   # ← filename SVG, không phải ticker
  ABI: WBTC
  address: "0x92f3B59a..."
  decimals: 8
```

Field `symbol` này được `loadContract()` nạp vào `configs`, nên `Lives.pools[chain][address].token0.configs.symbol` đã chứa sẵn filename.

Pattern render **giống hệt `ui-wallets`** (dùng `ui-svg`, không phải `<img>`):

```javascript
// ui-wallets (reference — wallets/index.js line 152):
html`<ui-svg class="icon" data-src="/images/cryptos/${currency.symbol}" /> ${currency.name}`

// ui-token dùng cùng pattern:
html`<ui-svg class="icon" data-src="/images/cryptos/${token.configs.symbol}" /> ${token.configs.name}`
```

`ui-svg` (component đã có) xử lý fetch + inline SVG.

Danh sách SVG hiện có: `ethereum-eth-logo.svg`, `wrapped-bitcoin-wbtc-logo.svg`, `tether-usdt-logo.svg`, `usd-coin-usdc-logo.svg`, `bnb-bnb-logo.svg`, `binance-usd-busd-logo.svg`, `uniswap-uni-logo.svg`, `chainlink-link-logo.svg`, `ethena-usde-usde-logo.svg`, `near-protocol-near-logo.svg`, `toncoin-ton-logo.svg`, `tron-trx-logo.svg`.

### 1.6 Kiến trúc DB.get — học từ deposit & withdraw

**Deposit và withdraw không gọi `DB.get` trực tiếp.** Toàn bộ data loading được phân chia như sau:

#### Tầng onchain thread (trước khi route render)
```
Construct.Chains()
  └─ chain.init()
       ├─ DB.get(["statics","chains",id,"configs.json"])     ← chain RPCs, id, symbol
       ├─ DB.get(["statics","chains",id,"currencies.json"])  ← danh sách địa chỉ ERC-20
       ├─ DB.get(["statics","chains",id,"stables.json"])
       └─ chain.load({ address: "native" })
            └─ loadContract({ chain: id, address: "native" })
                 └─ DB.get(["statics","chains",id,"contracts","native.json"])

Construct.Dexs()
  └─ dex.init()
       ├─ DB.get([...,"defis",dex,version,"configs.json"])   ← router, factory, quoter
       └─ DB.get([...,"defis",dex,version,"pools.json"])     ← mảng địa chỉ pool

scanPools()  (loop mỗi 10s)
  └─ loadContract({ chain: chain.id, address: poolAddress })
       └─ DB.get(["statics","chains",chain.id,"contracts","{address}.json"])
  └─ loadContract({ chain: chain.id, address: token0Address })   ← configs.symbol ở đây
  └─ loadContract({ chain: chain.id, address: token1Address })
  └─ dex.getRate({ pool })  ← gọi on-chain RPC
  └─ Indexes.Lives.get("pools").get(dex.id).get(pool.address).put(data)
       ← IDB key = dex.id = "1.UniswapV2" (string)
       ← KHÁC với Lives.pools[pool.chain] = Lives.pools[1] (numeric)
  └─ thread.send({ Lives: { pools: { [pool.chain]: { ... } } } })
       ← pool.chain = 1 (numeric)
```

> **Kết quả:** Sau khi `onchain.js` chạy xong, `Chains[id].currencies` chỉ có **native token**. ERC-20 addresses được liệt kê trong `chain.configs.currencies` nhưng **chưa load** thành object.

#### Tầng ui-wallets (khi route mount)
```
ui-wallets.connectedCallback()
  └─ for each chain:
       if (chain.configs.currencies.length !== Object.values(chain.currencies).length)
           await chain.load()   // ← load TẤT CẢ ERC-20 còn thiếu
                └─ loadContract({ chain: id, address })
                     ├─ check Statics.chains[id].contracts[address]  (in-memory cache)
                     └─ DB.get(["statics","chains",id,"contracts","{address}.json"])
                          ├─ check Indexes.Hashes (IDB hash)
                          ├─ fetch hash file
                          └─ serve from Indexes.Statics (IDB) nếu hash khớp
                               else fetch mới → update IDB
```

#### Tầng route (sau khi ui-wallets load xong)
- **deposit**: lắng nghe `$wallets.states.on("address")` → pipe vào `<ui-qr>`
- **withdraw**: lắng nghe `$wallets.states.on("address")` → enable form → gọi `wallet.fee()` / `wallet.send()`
- **Không route nào gọi DB.get trực tiếp**

#### Ngụ ý cho route pools và swap

| Route | DB.get trực tiếp? | Nguồn data | Cần `ui-wallets`? |
|-------|-------------------|------------|-------------------|
| `/pools` | **Không** | `Indexes.Lives.get("pools").once()` (IDB cache) + `events.on("Lives.pools")` | Không (public, không cần auth) |
| `/swap` | **Không** | `Chains[x].currencies` (sau khi `ui-wallets` trigger `chain.load()`), `Lives.pools` | **Có** (cần wallet để ký TX) |

Route `/pools` đọc data từ **IDB cache** (`Indexes.Lives`) — không cần `DB.get` vì `onchain.js` đã ghi vào IDB qua `Indexes.Lives.get("pools").put(data)`.

Route `/swap` dùng `ui-wallets` y chang deposit/withdraw — component đó tự xử lý `chain.load()` → `loadContract` → `DB.get` khi cần.

### 1.7 Luồng dữ liệu live

```
onchain thread → thread.send({ Lives: { pools: { [pool.chain]: { [address]: data } } } })
  → Threads.process() trên main thread  ← CẦN VÁ (xem 1.8)
  → merge(Lives, diff)
  → events.emit("Lives.pools", diff.pools)
  → UI components lắng nghe events.on("Lives.pools", handler)
```

### 1.8 Khoảng trống cần vá trước khi triển khai

Sau khi nghiên cứu kỹ mã nguồn, có **3 gap** giữa wallet repo và shop hiện tại:

#### Gap 1 — `Threads.process()` không xử lý `Lives` messages ⚠️

`src/core/Threads.js` chỉ xử lý `relay` và `queue` responses. Message `{ Lives: {...} }` từ worker **bị bỏ qua hoàn toàn**. Cần thêm:

```javascript
// src/core/Threads.js — thêm vào đầu Threads.process():
process(data, source) {
    if (typeof data !== "object") return
    if (data?.relay) return this.relay({ source, ...data.relay })

    // ← THÊM: xử lý Lives updates từ worker threads
    if (data?.Lives) {
        const diff_data = diff(Lives, data.Lives)   // import diff từ Utils
        merge(Lives, diff_data)                     // import merge từ Utils
        for (const key in diff_data) {
            events.emit("Lives." + key, diff_data[key])
        }
        return
    }

    const queue = data?.queue
    // ... phần còn lại giữ nguyên
}
```

#### Gap 2 — `Statics.defis` không được populate ⚠️

`onchain.js:scanPools()` gọi `Object.entries(Statics.defis)` nhưng `Statics.defis` không bao giờ được gán. Sau khi `Construct.Dexs()` chạy, data pool config nằm trong `Dexs[id].configs` — không được copy vào `Statics.defis`. Cần một trong hai:

**Option A** — sửa `scanPools()` để dùng `Dexs` trực tiếp (đơn giản hơn):
```javascript
thread.scanPools = () => {
    Object.entries(Dexs).forEach(([id, dex]) => {
        const chain = dex.chain                   // Chain instance
        const pools = dex.configs.pools           // addresses từ dex.configs
        if (!chain || !pools?.length) return
        loop({ ... })
    })
}
```

**Option B** — populate `Statics.defis` trong `Construct.Dex()`:
```javascript
// Thêm vào cuối Construct.Dex():
if (!Statics.defis) Statics.defis = {}
Statics.defis[instance.id] = instance.configs   // { chain, dex, version, pools: [...] }
```

#### Gap 3 — `Statics.chains[chain].stables` không được set ⚠️

`fiatRates()` đọc `Statics.chains[chain]?.stables` nhưng không bao giờ được gán. Chỉ `Statics.chains[chain].contracts[address]` được set bởi `loadContract()`. `chain.configs.stables` (array of stable addresses) cần được mirror vào `Statics.chains[chain].stables`.

Cần thêm vào `Chain.init()` sau khi load stables:
```javascript
// src/core/Chain.js — cuối init(), sau khi fetch stables.json:
if (this.configs.stables) {
    Statics.chains[this.id] = Statics.chains[this.id] || {}
    Statics.chains[this.id].stables = this.configs.stables
}
```

> **Thứ tự fix:** Gap 3 → Gap 2 → Gap 1. Fix 3 trước để `fiatRates()` hoạt động khi chains init. Fix 2 để scanPools không crash. Fix 1 để live updates về UI.

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
        // Lưu ý: IDB structure = { [dex.id]: { [address]: data } } (e.g. "1.UniswapV2")
        //         Memory structure = { [chain numeric]: { [address]: data } } (e.g. 1)
        // Cần flatten data từ IDB thành cấu trúc memory trước khi set vào states
        const cached = await Indexes.Lives.get("pools").once()
        if (cached) {
            const flat = {}
            for (const dexPools of Object.values(cached || {})) {
                for (const pool of Object.values(dexPools || {})) {
                    if (!pool?.chain) continue
                    flat[pool.chain] = flat[pool.chain] || {}
                    flat[pool.chain][pool.address] = pool
                }
            }
            if (Object.keys(flat).length) this.states.set({ pools: flat })
        }

        // Lắng nghe live updates từ onchain thread
        // events.on("Lives.pools") nhận data = { [chain numeric]: { [address]: data } }
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

// Hiển thị (token logo dùng configs.symbol trực tiếp):
// html`<ui-svg data-src="/images/cryptos/${token0.configs.symbol}" /> ${token0.configs.name}`
// /
// html`<ui-svg data-src="/images/cryptos/${token1.configs.symbol}" /> ${token1.configs.name}`
// rate: 1 WBTC = 15.3 WETH  (từ pool.pairs[token0.address][token1.address])
// fiat: ≈ $X,XXX (từ fiatValue({ chain, currency: token0.configs, amount: 1, fiat }))
// liquidity: $X,XXX,XXX TVL  (fiatValue × token0.quantity + fiatValue × token1.quantity)
// chain badge: ETH | BSC  (dùng chain.configs.symbol cho logo)
// dex badge: Uniswap V2 | V3

// token0.configs.symbol = "wrapped-bitcoin-wbtc-logo.svg"
// Lấy từ Lives.pools[chain][address].token0.configs.symbol

// Nhận live data qua:
// events.on("Lives.pools", ...)  ← tự subscribe/unsubscribe
// events.on("Lives.forex", ...)  ← để tính giá trị fiat
```

### 2.7 Tính liquidity và giá fiat

Fiat pricing **bắt buộc** được hiển thị cùng với rate. **Không cần viết thêm helper** — `fiatValue` và `fiatRates` đã có sẵn trong `src/core/Utils/contracts.js`.

#### `fiatRates({ chain, currency })` — async
Tìm tất cả pools trong `Lives.pools[chain]` chứa currency này, tra `Statics.chains[chain].stables` (mảng stablecoin addresses), trả về `{ "USD": 1500, "USDT": 1500 }`. Rate lấy từ `pool.pairs[token0.address][token1.address]`.

- Native token không có `address`: tự động dùng `currency.wrapped` để tìm rate
- Stablecoin (`currency.group === "stables"` + `currency.fiat`): trả về `{ [fiat]: 1 }` ngay

#### `fiatValue({ chain, currency, amount, fiat, forex })` — async
Gọi `fiatRates()` rồi áp `Lives.forex` để chuyển sang fiat mục tiêu. `forex` mặc định là `Lives.forex` (không cần truyền).

```javascript
import { fiatValue } from "/core/Utils/contracts.js"

// Trong ui-pool — async method:
async renderfiat(pool) {
    const fiat = Context.get("fiat")?.code || "USD"
    const chain = pool.chain

    // Token price (1 token0 = bao nhiêu fiat)
    const price0 = await fiatValue({ chain, currency: pool.token0.configs, amount: 1, fiat })
    //=> hiển thị "≈ $1,500" bên cạnh WETH

    // Liquidity TVL
    const liq0 = await fiatValue({ chain, currency: pool.token0.configs, amount: pool.token0.quantity, fiat })
    const liq1 = await fiatValue({ chain, currency: pool.token1.configs, amount: pool.token1.quantity, fiat })
    const tvl = (liq0 || 0) + (liq1 || 0)
    //=> hiển thị "$X,XXX,XXX TVL"

    // Rate từ pool.pairs (không dùng rate field riêng)
    const rate = pool.pairs?.[pool.token0.address]?.[pool.token1.address]
    //=> hiển thị "1 WBTC = 15.3 WETH"

    this.states.set({ price0, tvl, rate })
}
```

> `fiatValue` trả về `undefined` nếu không tìm được rate (token chưa có pool với stable). UI nên hiển thị `"—"` thay vì crash.

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

**Pattern từ deposit/withdraw — bắt buộc với swap:** Cả 2 route đều nhúng `<ui-wallets />`. Đây **không chỉ là UI** — `ui-wallets.connectedCallback()` là trigger duy nhất gọi `chain.load()` → `loadContract` → `DB.get` để nạp tất cả ERC-20 configs vào `chain.currencies`. Sau đó `withdraw` mới resolve được currency object qua `Object.values(wallet.chain.currencies).find(c => c.name === currencyName)`. Route swap cũng phải làm y chang: nhúng `<ui-wallets />`, lắng nghe `$wallets.states.on("address")`, rồi mới truy cập `wallet.chain.currencies`.

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

        this.$wallets = this.shadowRoot.querySelector("ui-wallets")
        this.$from = this.shadowRoot.querySelector("#from")
        this.$to = this.shadowRoot.querySelector("#to")
        this.$amount = this.shadowRoot.querySelector("#amount")
        this.$quote = this.shadowRoot.querySelector("#quote")
        this.$gas = this.shadowRoot.querySelector("#gas")
        const $submit = this.shadowRoot.querySelector("#submit")

        this.$amount.addEventListener("input", this.quote)
        $submit.addEventListener("click", this.submit)

        this.subscriptions.push(
            // ui-wallets triggers chain.load() on mount — sau đó mới có chain.currencies ERC-20
            this.$wallets.states.on("chain", ({ value: chain }) => {
                if (chain) this.refreshtokens()
            }),
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
// ui-wallets PHẢI có trong template — nó trigger chain.load() khi connectedCallback chạy
// (giống hệt deposit/withdraw — xem src/UI/routes/withdraw/template.js)
export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.swap" /></h1>
        <main>
            <ui-wallets />

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

Dropdown chọn token. Nguồn dữ liệu: `Chains[x].currencies` (có `configs.symbol` = SVG filename, giống `ui-wallets`) cộng token từ pools chưa có trong currencies.

```javascript
// Nhận: options = [{ address, configs: { symbol, name, decimals } }]
// configs.symbol = SVG filename ("tether-usdt-logo.svg")
// Emit: CustomEvent("select", { detail: { address, configs } })

// Render mỗi option — giống hệt ui-wallets.get currencies() (line 152):
html`<ui-svg class="icon" data-src="/images/cryptos/${token.configs.symbol}" /> ${token.configs.name}`

// Xây options (gọi SAU KHI ui-wallets đã load — qua $wallets.states.on("chain")):
// 1. Từ Chains[activeChain].currencies (đã load đủ configs.symbol sau chain.load())
// 2. Bổ sung token từ Lives.pools chưa có trong currencies (đã load configs bởi onchain.js)
// 3. Filter theo chain từ ui-wallets.states.get("chain")

// SAI nếu gọi ngay trong connectedCallback — chain.currencies chỉ có native lúc đó.
// ĐÚNG: đợi $wallets.states.on("chain", ...) rồi mới build.

// Internal state: States({ selected: null, open: false, filter: "" })
// Search: input filter theo token.configs.name
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

### Bước 0 — Vá 3 gaps trong core (prerequisite)

Phải làm trước, nếu không pools sẽ crash hoặc không nhận live data:

1. **`Chain.js`** — thêm vào cuối `init()`:
   ```javascript
   Statics.chains[this.id] = Statics.chains[this.id] || {}
   Statics.chains[this.id].stables = this.configs.stables  // ← cho fiatRates()
   ```
2. **`onchain.js`** — sửa `scanPools()` để dùng `Dexs` trực tiếp (thay vì `Statics.defis`):
   ```javascript
   Object.entries(Dexs).forEach(([id, dex]) => {
       const chain = dex.chain
       const pools = dex.configs.pools
       // ... giữ nguyên phần còn lại
   })
   ```
3. **`Threads.js`** — thêm Lives handling vào `process()`:
   ```javascript
   if (data?.Lives) {
       const diff_data = diff(Lives, data.Lives)
       merge(Lives, diff_data)
       for (const key in diff_data) events.emit("Lives." + key, diff_data[key])
       return
   }
   ```

### Bước 1 — Utility + components nền

4. Xác nhận `fiatValue` và `fiatRates` trong `src/core/Utils/contracts.js` đã sẵn sàng (Bước 0 phải hoàn thành trước để `fiatRates` đọc được `Statics.chains[chain].stables`)
5. Tạo `src/UI/components/token/` — `ui-token`: `<ui-svg data-src="/images/cryptos/${token.configs.symbol}" />` + name + optional amount; `configs.symbol` là SVG filename trực tiếp
6. Thêm i18n keys: `pools`, `swap`, `rate`, `liquidity`, `price`, `from`, `to`, `slippage`, `quote`, `nopoolFound`, `insufficientBalance`, `selectToken`, `searchToken`, `dex`, `gasFee`
7. `npm run build:core`

### Bước 2 — Route `/pools`

8. Tạo `src/UI/components/pool/` — `ui-pool`: hiển thị 1 pool row
   - Pair tokens (2× `ui-token`)
   - Rate: `pool.pairs[t0][t1]` hiển thị `"1 WBTC = 15.3 WETH"`
   - Fiat price: `await fiatValue({ chain: pool.chain, currency: pool.token0.configs, amount: 1, fiat })` — `pool.chain` là **numeric**
   - Liquidity fiat: `$X,XXX,XXX TVL`
   - Badges: `pool.dex` + `pool.version` (V2/V3) + chain
   - Self-subscribe `events.on("Lives.pools")` và `events.on("Lives.forex")`
9. Tạo `src/UI/routes/pools/` — subscribe `events.on("Lives.pools")`, đọc cache `Indexes.Lives`, flatten từ `dex.id` key sang `chain` key khi đọc IDB
10. `npm run build:core` → kiểm tra tại `/en/pools`; V2 và V3 pools phải đều hiển thị

### Bước 3 — Component `ui-token-select`

11. Tạo `src/UI/components/token-select/` — dropdown chọn token
   - Nguồn: `Chains[x].currencies` (có `configs.symbol`) + bổ sung từ `Lives.pools`
   - Render label: `html\`<ui-svg data-src="/images/cryptos/${t.configs.symbol}" /> ${t.configs.name}\`` (giống `ui-wallets` line 152)
   - Search input filter theo name
   - Emit `CustomEvent("select", { detail: { address, configs } })`

### Bước 4 — Route `/swap`

12. Tạo `src/UI/routes/swap/`
13. Integrate `ui-token-select` (2 cái: from + to) + `ui-wallets`
14. `finddex()` trả về `{ dex, pool }` — hỗ trợ cả V2 (direct) và V3 (direct với fee)
15. `quote()` debounce 500ms:
    - V2: `dex.getAmountsOut({ token0, token1, amount })`
    - V3: `dex.quote({ token0, token1, fee: pool.fee, amountIn: amount })` — `pool.fee` từ contract JSON
    - Hiển thị output + fiat equivalent: `await fiatValue({ chain: pool.chain, currency: toToken.configs, amount: result.amount, fiat })`
16. `estimategas()` debounce 500ms → `chain.fee({ from: walletAddress, to: router, amount, currency: fromToken.configs })`
17. `submit()` → `dex.swap({ token0, token1, amount0, slippage, fee: pool?.fee })` (V2 và V3 xử lý nội bộ)
18. Balance check: `amount ≤ Lives.balances[pool.chain][from.address]` trước khi submit (chú ý `pool.chain` là numeric)
19. `npm run build:core` → kiểm tra tại `/en/swap`; test với V2 pool và V3 pool

---

## 7. Rủi ro và cần làm rõ

| Vấn đề | Hiện trạng | Cần làm |
|--------|-----------|---------|
| Multi-hop swap path | `finddex()` chỉ tìm direct pair | Hỗ trợ A→B→C trong iteration 2; MVP chỉ direct pairs |
| Token logo không có SVG | `configs.symbol` trỏ đúng filename; `ui-svg` load fail silently | Bổ sung SVG vào `build/images/cryptos/` khi thêm token mới |
| Gas cho swap router | `chain.fee()` tính cho ERC20 transfer, không phải router call | Cần encode swap calldata → `chain.https.estimateGas({ to: router, data })` |
| V3 fee tier | Pool V3 cần `fee` (500/3000/10000) trong swap params | `finddex()` trả về `pool` kèm `fee` — dùng trực tiếp |
| Balance check | `Lives.balances[chain][address]` có từ `onchain.js` | Disable submit + hiển thị `insufficientBalance` nếu amount > balance |
| Multi-chain pools hiển thị | `Dex.id = "{chain}.{dex}{version}"` | UI filter theo `Context.get("params").chain` nếu có |

| File | Vai trò |
|------|---------|
| [src/core/threads/onchain.js](../../src/core/threads/onchain.js) | `scanPools()` — gọi `loadContract` → `DB.get`, ghi vào `Indexes.Lives` |
| [src/core/Dex.js](../../src/core/Dex.js) | Interface `getRate()`, `swap()` |
| [src/core/DeFi/V2.js](../../src/core/DeFi/V2.js) | V2: `getAmountsOut` → `swapExactTokensForTokens` |
| [src/core/DeFi/V3.js](../../src/core/DeFi/V3.js) | V3: `quoteExactInputSingle` → `exactInputSingle` |
| [src/core/Chains/EVM.js](../../src/core/Chains/EVM.js) | `chain.fee()`, `chain.gas()`, `chain.gasPrice()` |
| [src/core/Chain.js](../../src/core/Chain.js) | `chain.init()` (native only), `chain.load()` (ERC-20 on-demand) |
| [src/core/Utils/contracts.js](../../src/core/Utils/contracts.js) | `loadContract()` → memory cache → `DB.get` |
| [src/core/DB.js](../../src/core/DB.js) | `DB.get(path)` với hash-based IDB caching via `Indexes.Statics` |
| [src/core/Stores.js](../../src/core/Stores.js) | `Lives`, `Dexs`, `Wallets`, `Chains`, `Indexes` globals |
| [src/core/Events.js](../../src/core/Events.js) | `events.on("Lives.pools", ...)`, `events.on("Lives.forex", ...)` |
| [src/core/Forex.js](../../src/core/Forex.js) | `Forex.convert()` |
| [src/core/Utils/contracts.js](../../src/core/Utils/contracts.js) | `loadContract()`, `fiatValue()`, `fiatRates()` — **đã có sẵn** |
| [build/images/cryptos/](../../build/images/cryptos/) | SVG logos: 12 token; `configs.symbol` = filename trực tiếp |
| [src/UI/components/svg/index.js](../../src/UI/components/svg/index.js) | `ui-svg`: fetch + inline SVG; dùng thay `<img>` |
| [src/UI/routes/deposit/index.js](../../src/UI/routes/deposit/index.js) | Pattern tham chiếu: không gọi DB.get, dùng ui-wallets |
| [src/UI/routes/withdraw/index.js](../../src/UI/routes/withdraw/index.js) | Pattern tham chiếu: `wallet.fee()`, `wallet.send()`, currency resolution |
| [src/UI/components/wallets/index.js](../../src/UI/components/wallets/index.js) | Trigger `chain.load()` trong `connectedCallback`; pattern logo ui-svg |
