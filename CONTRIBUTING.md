# Contributing to Shop

First off, thank you for considering contributing to Shop! It's people like you that make this project a great tool for the community.

## Code of Conduct

This project and everyone participating in it is governed by our commitment to providing a welcoming and inspiring community for all. By participating, you are expected to uphold this standard. Please report unacceptable behavior to the project maintainers.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the existing issues to avoid duplicates. When you create a bug report, include as many details as possible:

- **Use a clear and descriptive title**
- **Describe the exact steps to reproduce the problem**
- **Provide specific examples** (code snippets, screenshots)
- **Describe the behavior you observed and what you expected**
- **Include browser/OS information**

### Suggesting Enhancements

Enhancement suggestions are welcome! When suggesting an enhancement:

- **Use a clear and descriptive title**
- **Provide a detailed description of the suggested enhancement**
- **Explain why this enhancement would be useful**
- **List any alternative solutions you've considered**

### Your First Code Contribution

Unsure where to begin? Look for issues tagged with:

- `good first issue` - Small, well-defined tasks perfect for newcomers
- `help wanted` - Issues where we especially appreciate community help

### Pull Requests

1. **Fork the repo** and create your branch from `main`
2. **Follow the coding style** (see below)
3. **Test your changes** thoroughly
4. **Update documentation** if needed
5. **Write clear commit messages**
6. **Submit a pull request**

## Development Setup

### Prerequisites

- Node.js 18+ (for build tools)
- A modern browser (Chrome, Firefox, Safari, Edge)
- Git
- VS Code (recommended for development)

### VS Code Extensions

- `es6-string-html` (ID: `Tobermory.es6-string-html`) is required for proper HTML syntax highlighting inside JavaScript template literals used across this project.
- The repository includes `.vscode/extensions.json`, so VS Code will automatically recommend this extension when you open the project.
- Quick onboarding in VS Code: when the recommendation prompt appears, click **Install All**.
- If you miss the prompt, open Extensions and search for **@recommended** under Workspace Recommendations, then install `es6-string-html`.

### Setup Steps

```bash
# Clone your fork
git clone https://github.com/YOUR-USERNAME/shop.git
cd shop

# Add upstream remote
git remote add upstream https://github.com/akaoio/shop.git

# Install dependencies
npm install

# Start dev server
npm start
```

The development server runs on `http://localhost:8080` with hot reload enabled.

### Project Structure

```
shop/
├── src/
│   ├── core/          # Core systems (UI, States, Router, etc.)
│   ├── UI/
│   │   ├── components/  # Web Components
│   │   ├── routes/      # Route handlers
│   │   ├── layouts/     # Page layouts
│   │   └── css/         # Global styles
│   └── statics/       # Data files (YAML)
│       ├── i18n/       # Translation files
│       ├── items/      # Product data
│       └── sites/      # Site configurations
├── build/             # Generated output (gitignored)
├── build.js           # Build script
└── server.js          # Dev server
```

## Coding Style

### General Principles

1. **Framework-less** - Use native Web APIs, no framework dependencies
2. **Web Standards** - Leverage modern browser standards
3. **Readability** - Code should be self-documenting
4. **Performance** - Optimize for speed and size

### JavaScript Style

We use Prettier for code formatting. Run `npm run format` before committing.

**Key conventions:**

```javascript
// Use const/let, never var
const items = []
let count = 0

// Arrow functions for short callbacks
items.map(item => item.name)

// Traditional functions for methods
function processData() {
    // ...
}

// Async/await over promises
async function loadData() {
    const data = await fetch(url)
    return data.json()
}

// Destructuring
const { name, price } = product

// Template literals
const message = `Hello ${name}`

// Optional chaining
const value = obj?.nested?.property
```

### Web Components Style

Follow this standard pattern:

```javascript
import template from "./template.js"
import { render } from "/core/UI.js"
import States from "/core/States.js"

export class MyComponent extends HTMLElement {
    constructor() {
        super()
        // Initialize state
        this.states = new States({ key: "value" })
        // Shadow DOM
        this.attachShadow({ mode: "open" })
        // Initial render
        render(template, this.shadowRoot)
        // Subscriptions array for cleanup
        this.subscriptions = []
        // Bind methods that will be used as callbacks
        this.handleEvent = this.handleEvent.bind(this)
    }

    static get observedAttributes() {
        return ["data-attribute"]
    }

    attributeChangedCallback(name, oldValue, newValue) {
        if (oldValue === newValue) return
        // Handle attribute changes
    }

    connectedCallback() {
        // Setup: add event listeners, subscribe to state
        this.subscriptions.push(
            this.states.on("key", this.render.bind(this))
        )
    }

    disconnectedCallback() {
        // Cleanup: remove listeners, unsubscribe
        this.subscriptions.forEach(off => off())
    }

    handleEvent(event) {
        // Event handler logic
    }

    render() {
        // Update DOM based on state
    }
}

customElements.define("ui-my-component", MyComponent)
export default MyComponent
```

### Component File Structure

Each component should have its own directory:

```
my-component/
├── index.js         # Component class
├── template.js      # HTML template
└── styles.css.js    # Component styles
```

**template.js:**

```javascript
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <div class="container">
        <!-- Component markup -->
    </div>
`

export default template
```

**styles.css.js:**

```javascript
import { css } from "/core/UI.js"

export const styles = css`
    :host {
        display: block;
    }

    .container {
        padding: 1rem;
    }
`

export default styles
```

### CSS Style

- Use CSS custom properties for theming
- Scope styles with `:host` in Shadow DOM
- Follow BEM naming for class names
- Mobile-first responsive design

```css
:host {
    /* Theme variables */
    color: var(--text-primary);
    background: var(--bg-primary);
}

.component__element {
    /* BEM naming */
}

.component__element--modifier {
    /* Modifier class */
}
```

### File Naming

- **Components:** lowercase with hyphens (`my-component/`)
- **JavaScript:** camelCase (`myComponent.js`)
- **Classes:** PascalCase (`MyComponent`)
- **Custom elements:** lowercase with hyphens (`ui-my-component`)

## Adding Features

### Creating a New Component

1. Create component directory: `src/UI/components/my-component/`
2. Create `index.js`, `template.js`, `styles.css.js`
3. Follow the component pattern (see above)
4. Import and use in other components/routes
5. Add to README if it's a public-facing component

### Creating a New Route

1. Create route directory: `src/UI/routes/my-route/`
2. Create `index.js` (route handler class)
3. Build system auto-detects and generates HTML
4. Router auto-matches at runtime

**Dynamic routes:**

```javascript
// File: src/UI/routes/item/[slug]/index.js
export class ItemRoute extends HTMLElement {
    connectedCallback() {
        const { slug } = Context.get("params")
        // Use slug to load data
    }
}
```

### Adding Translations

1. Create YAML file: `src/statics/i18n/my-string.yaml`
2. Add translations for all 19 locales:

```yaml
en: My String
fr: Ma Chaîne
es: Mi Cadena
de: Meine Zeichenfolge
ja: 私の文字列
zh: 我的字符串
ko: 내 문자열
ru: Моя строка
ar: سلسلة بلدي
hi: मेरी स्ट्रिंग
pt: Minha String
it: La Mia Stringa
vi: Chuỗi Của Tôi
th: สตริงของฉัน
he: המחרוזת שלי
ur: میری سٹرنگ
"zh-TW": 我的字符串
no: Min Streng
```

3. Build process automatically aggregates
4. Use in components:

```javascript
const text = Context.get("dictionary").myString
```

### Adding Products

1. Create item directory: `src/statics/items/my-product/`
2. Add `meta.yaml`:

```yaml
price: 29.99
sale: 24.99
currency: USD
tags:
  - category
  - featured
sku: PROD-001
attributes:
  - name: Size
    options: [S, M, L, XL]
```

3. Add translations (`en.yaml`, `fr.yaml`, etc.):

```yaml
name: Product Name
description: Product description
details: Detailed information
```

4. Rebuild - product appears in listings

## Testing

### Manual Testing

- Test in multiple browsers (Chrome, Firefox, Safari, Edge)
- Test responsive design (mobile, tablet, desktop)
- Test with different locales
- Test with dev tools (console errors, network tab)

### Checklist Before Submitting

- [ ] Code follows the style guide
- [ ] Tested in multiple browsers
- [ ] No console errors or warnings
- [ ] Documentation updated (if needed)
- [ ] Translations added (if needed)
- [ ] Build completes without errors
- [ ] No merge conflicts with main branch

## Build System

Understanding the build process helps when contributing:

### Build Steps

```bash
npm run build
```

1. Clean `build/` directory
2. Load configuration (locales, items, tags)
3. Convert YAML → JSON
4. Generate paginated data
5. Process translations
6. Generate static routes
7. Create hash files
8. Copy assets

### Build Output

```
build/
├── en/
│   ├── index.html
│   ├── item/[slug]/index.html
│   └── tag/[slug]/index.html
├── fr/ (same structure)
├── ... (19 locales)
├── core/          # Core systems (copied)
├── UI/            # Components (copied)
├── statics/       # Data (JSON)
│   ├── locales/   # Translation files
│   ├── items/     # Product data
│   ├── hashes/    # Integrity hashes
│   └── ...
└── images/        # Assets
```

## Commit Messages

Write clear, descriptive commit messages:

```
feat: Add user profile component
fix: Correct routing for nested paths
docs: Update API documentation
style: Format code with Prettier
refactor: Simplify state management logic
perf: Optimize template rendering
test: Add unit tests for utils
chore: Update dependencies
```

**Format:** `type: Description`

**Types:**
- `feat` - New feature
- `fix` - Bug fix
- `docs` - Documentation
- `style` - Code style (formatting, no logic change)
- `refactor` - Code refactoring
- `perf` - Performance improvement
- `test` - Adding/updating tests
- `chore` - Maintenance tasks

## Release Process

(For maintainers)

1. Update version in `package.json`
2. Update CHANGELOG.md
3. Create git tag: `git tag v1.2.3`
4. Push tag: `git push origin v1.2.3`
5. Create GitHub release with notes

## Questions?

- **Issues:** [GitHub Issues](https://github.com/akaoio/shop/issues)
- **Discussions:** [GitHub Discussions](https://github.com/akaoio/shop/discussions)

## Additional Resources

- [Web Components Guide](https://developer.mozilla.org/en-US/docs/Web/Web_Components)
- [Shadow DOM v1](https://developers.google.com/web/fundamentals/web-components/shadowdom)
- [Custom Elements v1](https://developers.google.com/web/fundamentals/web-components/customelements)
- [ES Modules](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Modules)

---

Thank you for contributing to Shop! 🎉
