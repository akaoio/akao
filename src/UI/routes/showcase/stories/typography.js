import "/UI/components/select/index.js"
import { LOCALES, T } from "./locales.js"

function getDir(code) {
    return LOCALES.find((l) => l.code === code)?.dir || "ltr"
}

export default {
    name: "Typography",

    toolbar(grid) {
        const bar = document.createElement("div")
        bar.className = "story-locale-bar"

        const label = document.createElement("span")
        label.className = "story-locale-label"
        label.textContent = "Language"

        const select = document.createElement("ui-select")
        select.className = "story-locale-select"
        select.states.set({
            options: LOCALES.map((l) => ({ value: l.code, label: `${l.code} — ${l.name}` })),
            selected: "en",
        })
        select.states.on("selected", ({ value }) => {
            for (const preview of grid.querySelectorAll(".story-preview")) preview._updateLocale?.(value)
        })

        bar.appendChild(label)
        bar.appendChild(select)
        return bar
    },

    stories: [
        {
            name: "Neon Colors",
            wide: true,
            code: `<div class="typo-grid-4">
  <div>
    <div class="typo-swatch typo-swatch--g"></div>
    <div class="typo-swatch-label">--neon-g · Neon Green</div>
    <div class="typo-hex typo-hex--g">#00FF9D</div>
  </div>
  <div>
    <div class="typo-swatch typo-swatch--c"></div>
    <div class="typo-swatch-label">--neon-c · Neon Cyan</div>
    <div class="typo-hex typo-hex--c">#00E5FF</div>
  </div>
  <div>
    <div class="typo-swatch typo-swatch--m"></div>
    <div class="typo-swatch-label">--neon-m · Neon Magenta</div>
    <div class="typo-hex typo-hex--m">#FF2D78</div>
  </div>
  <div>
    <div class="typo-swatch typo-swatch--y"></div>
    <div class="typo-swatch-label">--neon-y · Neon Yellow</div>
    <div class="typo-hex typo-hex--y">#F5E642</div>
  </div>
</div>`
        },
        {
            name: "Neon Glow Effects",
            wide: true,
            code: `<div class="typo-stack">
  <div>
    <div class="typo-glow-label">Green Glow · --glow-g</div>
    <div class="typo-glow-demo typo-glow-demo--g">CYBER MARKET</div>
  </div>
  <div>
    <div class="typo-glow-label">Cyan Glow · --glow-c</div>
    <div class="typo-glow-demo typo-glow-demo--c">CYBER MARKET</div>
  </div>
  <div>
    <div class="typo-glow-label">Magenta Glow · --glow-m</div>
    <div class="typo-glow-demo typo-glow-demo--m">CYBER MARKET</div>
  </div>
  <div>
    <div class="typo-glow-label">Yellow Glow · --glow-y</div>
    <div class="typo-glow-demo typo-glow-demo--y">CYBER MARKET</div>
  </div>
</div>`
        },
        {
            name: "Gradient Text",
            wide: true,
            code: `<div class="typo-stack">
  <div>
    <div class="typo-glow-label">Green → Cyan · --neon-g to --neon-c</div>
    <div class="typo-glow-demo typo-gradient typo-gradient--gc">CYBER MARKET</div>
  </div>
  <div>
    <div class="typo-glow-label">Green → Magenta · --neon-g to --neon-m</div>
    <div class="typo-glow-demo typo-gradient typo-gradient--gm">CYBER MARKET</div>
  </div>
  <div>
    <div class="typo-glow-label">Cyan → Magenta · --neon-c to --neon-m</div>
    <div class="typo-glow-demo typo-gradient typo-gradient--cm">CYBER MARKET</div>
  </div>
  <div>
    <div class="typo-glow-label">Cyan → Yellow · --neon-c to --neon-y</div>
    <div class="typo-glow-demo typo-gradient typo-gradient--cy">CYBER MARKET</div>
  </div>
</div>`
        },
        {
            name: "Heading Scale",
            wide: true,
            code: `<div class="typo-stack--sm">
  <div class="typo-row--baseline">
    <span class="typo-meta-tag">H1</span>
    <h1 class="typo-h1">Home</h1>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag">H2</span>
    <h2 class="typo-h2">Cart · Checkout</h2>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag">H3</span>
    <h3 class="typo-h3">Profile · Wallet</h3>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag">H4</span>
    <h4 class="typo-h4">Add to Cart</h4>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag">H5</span>
    <h5 class="typo-h5">Sign In · Sign Up</h5>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag">H6</span>
    <h6 class="typo-h6">Confirm · Total</h6>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    container.querySelector("h1").textContent = t.home
                    container.querySelector("h2").textContent = `${t.cart} · ${t.checkout}`
                    container.querySelector("h3").textContent = `${t.profile} · ${t.wallet}`
                    container.querySelector("h4").textContent = t.addToCart
                    container.querySelector("h5").textContent = `${t.signIn} · ${t.signUp}`
                    container.querySelector("h6").textContent = `${t.confirm} · ${t.total}`
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Font Size Scale",
            wide: true,
            code: `<div class="typo-stack--md">
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">--text-xs</span>
      <span class="typo-meta-val">0.75rem · 12px</span>
    </div>
    <span data-sample="phrase" class="typo-size-xs">Add to Cart · Sign In · Checkout · Total</span>
  </div>
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">--text-sm</span>
      <span class="typo-meta-val">0.875rem · 14px</span>
    </div>
    <span data-sample="phrase" class="typo-size-sm">Add to Cart · Sign In · Checkout · Total</span>
  </div>
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">--text-md · default</span>
      <span class="typo-meta-val">1rem · 16px</span>
    </div>
    <span data-sample="phrase" class="typo-size-md">Add to Cart · Sign In · Checkout · Total</span>
  </div>
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">--text-lg</span>
      <span class="typo-meta-val">1.25rem · 20px</span>
    </div>
    <span data-sample="phrase" class="typo-size-lg">Add to Cart · Sign In · Checkout</span>
  </div>
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">--text-xl</span>
      <span class="typo-meta-val">1.5rem · 24px</span>
    </div>
    <span data-sample="word" class="typo-size-xl">Cart · Checkout</span>
  </div>
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Display</span>
      <span class="typo-meta-val">2rem · 32px</span>
    </div>
    <span data-sample="title" class="typo-size-display">Home</span>
  </div>
  <div class="typo-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Hero</span>
      <span class="typo-meta-val">3rem · 48px</span>
    </div>
    <span data-sample="title" class="typo-size-hero">Home</span>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    for (const el of container.querySelectorAll("[data-sample='phrase']")) el.textContent = `${t.addToCart} · ${t.signIn} · ${t.checkout} · ${t.total}`
                    for (const el of container.querySelectorAll("[data-sample='word']")) el.textContent = `${t.cart} · ${t.checkout}`
                    for (const el of container.querySelectorAll("[data-sample='title']")) el.textContent = t.home
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Line Height",
            wide: true,
            code: `<div class="typo-stack--lh">
  <div class="typo-lh-row">
    <div class="typo-meta-col">
      <span class="typo-meta">None</span>
      <span class="typo-meta-val">1</span>
    </div>
    <div data-sample class="typo-body typo-size-md typo-lh-none typo-lh-text"></div>
  </div>
  <div class="typo-lh-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Tight</span>
      <span class="typo-meta-val">1.25</span>
    </div>
    <div data-sample class="typo-body typo-size-md typo-lh-tight typo-lh-text"></div>
  </div>
  <div class="typo-lh-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Snug</span>
      <span class="typo-meta-val">1.375</span>
    </div>
    <div data-sample class="typo-body typo-size-md typo-lh-snug typo-lh-text"></div>
  </div>
  <div class="typo-lh-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Normal</span>
      <span class="typo-meta-val">1.5</span>
    </div>
    <div data-sample class="typo-body typo-size-md typo-lh-normal typo-lh-text"></div>
  </div>
  <div class="typo-lh-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Relaxed</span>
      <span class="typo-meta-val">1.625</span>
    </div>
    <div data-sample class="typo-body typo-size-md typo-lh-relaxed typo-lh-text"></div>
  </div>
  <div class="typo-lh-row">
    <div class="typo-meta-col">
      <span class="typo-meta">Loose</span>
      <span class="typo-meta-val">2</span>
    </div>
    <div data-sample class="typo-body typo-size-md typo-lh-loose typo-lh-text"></div>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    const phrase = `${t.addToCart} · ${t.signIn} · ${t.checkout} · ${t.total} · ${t.cart} · ${t.profile} · ${t.wallet} · ${t.confirm} · ${t.shipping} · ${t.cancel}`
                    for (const el of container.querySelectorAll("[data-sample]")) el.textContent = phrase
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Font Families",
            wide: true,
            code: `<div class="typo-stack--fw">
  <div>
    <div class="typo-meta--wide typo-mb-md">Orbitron · Headers &amp; Titles</div>
    <div data-sample class="typo-ff-orbitron">Home · Add to Cart · Checkout</div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-md">Exo 2 · Body &amp; UI</div>
    <div data-sample class="typo-ff-exo2">Home · Add to Cart · Checkout</div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-md">Noto Sans · Multilingual Fallback</div>
    <div data-sample class="typo-ff-noto">Home · Add to Cart · Checkout</div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-md">Poppins · Accent &amp; Marketing</div>
    <div data-sample class="typo-ff-poppins">Home · Add to Cart · Checkout</div>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    for (const el of container.querySelectorAll("[data-sample]")) el.textContent = `${t.home} · ${t.addToCart} · ${t.checkout}`
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Font Weights",
            wide: true,
            code: `<div class="typo-grid-2">
  <div>
    <div class="typo-meta--wide typo-mb-lg">Exo 2 · Weight Scale</div>
    <div class="typo-stack--xs">
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">100</span><span data-sample class="typo-body typo-fw-sample typo-w-100">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">200</span><span data-sample class="typo-body typo-fw-sample typo-w-200">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">300</span><span data-sample class="typo-body typo-fw-sample typo-w-300">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">400</span><span data-sample class="typo-body typo-fw-sample typo-w-400">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">500</span><span data-sample class="typo-body typo-fw-sample typo-w-500">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">600</span><span data-sample class="typo-body typo-fw-sample typo-w-600">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">700</span><span data-sample class="typo-body typo-fw-sample typo-w-700">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">800</span><span data-sample class="typo-body typo-fw-sample typo-w-800">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">900</span><span data-sample class="typo-body typo-fw-sample typo-w-900">Home · Wallet</span></div>
    </div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-lg">Orbitron · Weight Scale</div>
    <div class="typo-stack--xs">
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">400</span><span data-sample class="typo-header typo-fw-sample typo-w-400">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">500</span><span data-sample class="typo-header typo-fw-sample typo-w-500">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">600</span><span data-sample class="typo-header typo-fw-sample typo-w-600">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">700</span><span data-sample class="typo-header typo-fw-sample typo-w-700">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">800</span><span data-sample class="typo-header typo-fw-sample typo-w-800">Home · Wallet</span></div>
      <div class="typo-row--sm"><span class="typo-meta-tag--sm">900</span><span data-sample class="typo-header typo-fw-sample typo-w-900">Home · Wallet</span></div>
    </div>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    for (const el of container.querySelectorAll("[data-sample]")) el.textContent = `${t.home} · ${t.wallet}`
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Font Style",
            wide: true,
            code: `<div class="typo-stack--sm">
  <div class="typo-row--baseline">
    <span class="typo-meta-tag--sm">Exo 2</span>
    <span data-sample class="typo-ff-exo2 typo-fw-sample typo-not-italic">Home · Add to Cart</span>
    <span data-sample class="typo-ff-exo2 typo-fw-sample typo-italic">Home · Add to Cart</span>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag--sm">Poppins</span>
    <span data-sample class="typo-ff-poppins typo-fw-sample typo-not-italic">Home · Add to Cart</span>
    <span data-sample class="typo-ff-poppins typo-fw-sample typo-italic">Home · Add to Cart</span>
  </div>
  <div class="typo-row--baseline">
    <span class="typo-meta-tag--sm">Orbitron</span>
    <span data-sample class="typo-header typo-fw-sample typo-not-italic">Home · Add to Cart</span>
    <span data-sample class="typo-header typo-fw-sample typo-italic">Home · Add to Cart</span>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    for (const el of container.querySelectorAll("[data-sample]")) el.textContent = `${t.home} · ${t.addToCart}`
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Letter Spacing",
            wide: true,
            code: `<div class="typo-stack--ls">
  <div class="typo-row"><span class="typo-meta-tag typo-meta-tag--ls">Tight<br><span class="typo-dim">-0.05em</span></span><span data-sample class="typo-ls-sample typo-ls-tight">Home</span></div>
  <div class="typo-row"><span class="typo-meta-tag typo-meta-tag--ls">Normal<br><span class="typo-dim">0</span></span><span data-sample class="typo-ls-sample typo-ls-normal">Home</span></div>
  <div class="typo-row"><span class="typo-meta-tag typo-meta-tag--ls">Wide<br><span class="typo-dim">0.05em</span></span><span data-sample class="typo-ls-sample typo-ls-wide">Home</span></div>
  <div class="typo-row"><span class="typo-meta-tag typo-meta-tag--ls">Wider<br><span class="typo-dim">0.1em</span></span><span data-sample class="typo-ls-sample typo-ls-wider">Home</span></div>
  <div class="typo-row"><span class="typo-meta-tag typo-meta-tag--ls">Widest<br><span class="typo-dim">0.2em</span></span><span data-sample class="typo-ls-sample typo-ls-widest">Home</span></div>
  <div class="typo-row"><span class="typo-meta-tag typo-meta-tag--ls">Ultra<br><span class="typo-dim">0.4em</span></span><span data-sample class="typo-ls-sample typo-ls-ultra">Home</span></div>
</div>`,

            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    for (const el of container.querySelectorAll("[data-sample]")) el.textContent = `${t.home} · ${t.cart} · ${t.checkout} · ${t.profile} · ${t.wallet} · ${t.addToCart} · ${t.signIn} · ${t.signUp} · ${t.confirm} · ${t.total} · ${t.shipping} · ${t.cancel}`
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Word Break",
            wide: true,
            code: `<div class="typo-grid-2">
  <div>
    <div class="typo-meta--wide typo-mb-sm">Normal</div>
    <div class="typo-break-demo"><span data-sample class="typo-body typo-size-md typo-break-normal">Add to Cart Sign In Checkout Total Shipping Cancel</span></div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-sm">Break All</div>
    <div class="typo-break-demo"><span data-sample class="typo-body typo-size-md typo-break-all">Add to Cart Sign In Checkout Total Shipping Cancel</span></div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-sm">Break Word</div>
    <div class="typo-break-demo"><span data-sample class="typo-body typo-size-md typo-break-word">Add to Cart Sign In Checkout Total Shipping Cancel</span></div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-sm">Keep All</div>
    <div class="typo-break-demo"><span data-sample class="typo-body typo-size-md typo-break-keep">Add to Cart Sign In Checkout Total Shipping Cancel</span></div>
  </div>
</div>`,
            setup(container) {
                function update(code) {
                    const t = T[code] || T.en
                    container.dir = getDir(code)
                    for (const el of container.querySelectorAll("[data-sample]")) el.textContent = `${t.addToCart} ${t.signIn} ${t.checkout} ${t.total} ${t.shipping} ${t.cancel}`
                }
                container._updateLocale = update
                update("en")
            }
        },
        {
            name: "Tabular Numbers",
            wide: true,
            code: `<div class="typo-grid-2">
  <div>
    <div class="typo-meta--wide typo-mb-sm">Proportional</div>
    <div class="typo-stack--xs typo-header typo-size-lg typo-nums-proportional">
      <span>¥1,111</span>
      <span>$999.99</span>
      <span>€10,000.00</span>
      <span>£77.77</span>
    </div>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-sm">Tabular</div>
    <div class="typo-stack--xs typo-header typo-size-lg typo-nums-tabular">
      <span>¥1,111</span>
      <span>$999.99</span>
      <span>€10,000.00</span>
      <span>£77.77</span>
    </div>
  </div>
</div>`
        },
        {
            name: "Prose Block",
            wide: true,
            code: `<div class="typo-grid-2">
  <div>
    <div class="typo-meta--wide typo-mb-md">--text-sm · product description</div>
    <p class="typo-size-sm typo-lh-relaxed typo-prose">The AK-7 Neon Keyboard features per-key RGB lighting with 16.8 million colors, hot-swappable switches, and a durable aluminum frame. Designed for extended sessions, its sculpted keycaps reduce fatigue and the USB-C braided cable ensures a reliable, low-latency connection. Compatible with Windows, macOS, and Linux out of the box.</p>
  </div>
  <div>
    <div class="typo-meta--wide typo-mb-md">--text-md · editorial body</div>
    <p class="typo-size-md typo-lh-relaxed typo-prose">The AK-7 Neon Keyboard features per-key RGB lighting with 16.8 million colors, hot-swappable switches, and a durable aluminum frame. Designed for extended sessions, its sculpted keycaps reduce fatigue and the USB-C braided cable ensures a reliable, low-latency connection. Compatible with Windows, macOS, and Linux out of the box.</p>
  </div>
</div>`
        }
    ]
}
