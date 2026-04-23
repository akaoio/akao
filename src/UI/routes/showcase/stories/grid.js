export default {
    name: "Grid",

    stories: [
        // ── Five grid tiers ────────────────────────────────────────────────────
        {
            name: "Five Grid Tiers",
            wide: true,
            code: `<div class="sg-tiers">
  <div class="sg-tier sg-tier--xs">
    <div class="sg-col">xs · always</div>
    <div class="sg-col">xs · always</div>
    <div class="sg-col">xs · always</div>
  </div>
  <div class="sg-tier sg-tier--sm">
    <div class="sg-col">sm · ≥576px</div>
    <div class="sg-col">sm · ≥576px</div>
    <div class="sg-col">sm · ≥576px</div>
  </div>
  <div class="sg-tier sg-tier--md">
    <div class="sg-col">md · ≥768px</div>
    <div class="sg-col">md · ≥768px</div>
    <div class="sg-col">md · ≥768px</div>
  </div>
  <div class="sg-tier sg-tier--lg">
    <div class="sg-col">lg · ≥992px</div>
    <div class="sg-col">lg · ≥992px</div>
    <div class="sg-col">lg · ≥992px</div>
  </div>
  <div class="sg-tier sg-tier--xl">
    <div class="sg-col">xl · ≥1200px</div>
    <div class="sg-col">xl · ≥1200px</div>
    <div class="sg-col">xl · ≥1200px</div>
  </div>
</div>`,
        },

        // ── Three equal columns ────────────────────────────────────────────────
        {
            name: "Three Equal Columns",
            wide: true,
            code: `<div class="sg-grid" style="--grid-cols: 3">
  <div class="sg-col">1fr</div>
  <div class="sg-col">1fr</div>
  <div class="sg-col">1fr</div>
</div>`,
        },

        // ── Three unequal columns ──────────────────────────────────────────────
        {
            name: "Three Unequal Columns",
            wide: true,
            code: `<div class="sg-grid-custom" style="grid-template-columns: 1fr 2fr 1fr">
  <div class="sg-col">1fr</div>
  <div class="sg-col">2fr</div>
  <div class="sg-col">1fr</div>
</div>`,
        },

        // ── Two columns ────────────────────────────────────────────────────────
        {
            name: "Two Columns",
            wide: true,
            code: `<div class="sg-grid-custom" style="grid-template-columns: 2fr 1fr">
  <div class="sg-col">2fr</div>
  <div class="sg-col">1fr</div>
</div>`,
        },

        // ── Fluid ──────────────────────────────────────────────────────────────
        {
            name: ".grid--fluid — Auto-fill Columns",
            wide: true,
            code: `<div class="sg-grid--fluid">
  <div class="sg-col">1</div>
  <div class="sg-col">2</div>
  <div class="sg-col">3</div>
  <div class="sg-col">4</div>
  <div class="sg-col">5</div>
</div>`,
        },
        {
            name: ".grid--fluid — Narrow Min Width (8rem)",
            wide: true,
            code: `<div class="sg-grid--fluid" style="--grid-min: 8rem">
  <div class="sg-col">1</div>
  <div class="sg-col">2</div>
  <div class="sg-col">3</div>
  <div class="sg-col">4</div>
  <div class="sg-col">5</div>
  <div class="sg-col">6</div>
  <div class="sg-col">7</div>
  <div class="sg-col">8</div>
</div>`,
        },

        // ── Sidebar ────────────────────────────────────────────────────────────
        {
            name: ".grid--sidebar — Default (15rem)",
            wide: true,
            code: `<div class="sg-grid--sidebar">
  <aside class="sg-col sg-col--sidebar">Sidebar</aside>
  <main class="sg-col sg-col--main">Main content</main>
</div>`,
        },
        {
            name: ".grid--sidebar — Wide (20rem)",
            wide: true,
            code: `<div class="sg-grid--sidebar" style="--sidebar-width: 20rem">
  <aside class="sg-col sg-col--sidebar">Wide Sidebar</aside>
  <main class="sg-col sg-col--main">Main content</main>
</div>`,
        },

        // ── Table ──────────────────────────────────────────────────────────────
        {
            name: ".grid--table — Fixed Column Contract",
            wide: true,
            code: `<div class="sg-table-wrap">
  <div class="sg-grid--table sg-table-header" style="--table-cols: 2rem 1fr 6rem 6rem">
    <span></span>
    <span>Item</span>
    <span>Price</span>
    <span>Stock</span>
  </div>
  <div class="sg-grid--table sg-table-row" style="--table-cols: 2rem 1fr 6rem 6rem">
    <span>1</span><span>AK-7 Neon Keyboard</span><span>$129.99</span><span>42</span>
  </div>
  <div class="sg-grid--table sg-table-row" style="--table-cols: 2rem 1fr 6rem 6rem">
    <span>2</span><span>Hyper Glide Mouse Pad XL</span><span>$39.99</span><span>7</span>
  </div>
  <div class="sg-grid--table sg-table-row" style="--table-cols: 2rem 1fr 6rem 6rem">
    <span>3</span><span>RGB Hub 7-Port USB-C</span><span>$24.99</span><span>0</span>
  </div>
</div>`,
        },

        // ── Breakpoint reference ───────────────────────────────────────────────
        {
            name: "Breakpoint Reference",
            wide: true,
            code: `<div class="sg-bp-table">
  <div class="sg-bp-row sg-bp-row--header">
    <span>Name</span><span>px</span><span>mq.*Up</span><span>mq.*Down</span><span>mq.*Only</span>
  </div>
  <div class="sg-bp-row"><span>xs</span><span>480</span><span>—</span><span>xsDown</span><span>xsOnly</span></div>
  <div class="sg-bp-row"><span>sm</span><span>576</span><span>smUp</span><span>smDown</span><span>smOnly</span></div>
  <div class="sg-bp-row"><span>md</span><span>768</span><span>mdUp</span><span>mdDown</span><span>mdOnly</span></div>
  <div class="sg-bp-row"><span>lg</span><span>992</span><span>lgUp</span><span>lgDown</span><span>lgOnly</span></div>
  <div class="sg-bp-row"><span>xl</span><span>1200</span><span>xlUp</span><span>xlDown</span><span>xlOnly</span></div>
  <div class="sg-bp-row"><span>xxl</span><span>1400</span><span>xxlUp</span><span>xxlDown</span><span>—</span></div>
</div>`,
        },
    ],
}
