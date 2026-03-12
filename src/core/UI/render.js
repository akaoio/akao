// ============ MODULE-LEVEL OPTIMIZATIONS ============
// Counter for unique IDs (faster than Date.now())
let callbackCounter = 0

// Container pool for temporary elements (reduce GC pressure)
const containerPool = []
function getContainer() {
    return containerPool.pop() || document.createElement("div")
}
function releaseContainer(container) {
    container.innerHTML = ""
    if (containerPool.length < 10) containerPool.push(container)
}

/**
 * Helper: Safely get nodes from container
 * ShadowRoot cannot be cloned, so we need special handling
 */
function getNodesFromContainer(container, shouldClone = true) {
    const content = container.nodeName === "TEMPLATE" ? container.content : container

    // If only 1 child, return that child
    if (content.childNodes.length === 1) return shouldClone ? content.firstChild.cloneNode(true) : content.firstChild

    // If container is ShadowRoot, create fragment with cloned children
    if (content instanceof ShadowRoot) {
        const fragment = document.createDocumentFragment()
        Array.from(content.childNodes).forEach((node) => {
            fragment.appendChild(node.cloneNode(true))
        })
        return fragment
    }

    // For other containers
    if (shouldClone) return content.cloneNode(true)
    else {
        // Move all children to a new fragment
        const fragment = document.createDocumentFragment()
        while (content.firstChild) fragment.appendChild(content.firstChild)

        return fragment
    }
}

/**
 * renderTemplateResult() - Core logic to process TemplateResult
 *
 * @param {TemplateResult} templateResult - Object from html()
 * @param {HTMLElement|ShadowRoot} container - Container to mount
 * @param {Object} options - Rendering options (append, prepend)
 */
function renderTemplateResult(templateResult, container, options = {}) {
    const { html, values } = templateResult

    /**
     * STEP 2a: Handle attribute markers BEFORE parsing HTML
     * Replace attribute markers with temporary data attributes
     */
    let processedHtml = html
    const attributeCallbacks = []

    processedHtml = processedHtml.replace(/__attr_mark:(\d+)__/g, (match, indexStr) => {
        const index = parseInt(indexStr, 10)
        const value = values[index]

        if (typeof value === "function") {
            // Create unique marker attribute using counter (faster than Date.now())
            const attrId = `data-cb-${++callbackCounter}`
            attributeCallbacks.push({ attrId, callback: value, index })
            return attrId
        }

        // Non-function values in attributes (shouldn't happen but handle it)
        return String(value || "")
    })

    /**
     * STEP 2b: Create DocumentFragment from processed HTML string
     */
    const template = document.createElement("template")
    template.innerHTML = processedHtml
    const fragment = template.content.cloneNode(true)

    /**
     * OPTIMIZATION: If no values, means no markers
     * → Skip TreeWalker completely! 🚀
     */
    if (values.length === 0) {
        if (options.prepend) {
            container.prepend(fragment)
        } else if (options.append) {
            container.appendChild(fragment)
        } else {
            container.replaceChildren(fragment)
        }
        return
    }

    /**
     * STEP 3: Find and process markers in single pass
     * Combine collection + processing to eliminate extra loop
     */
    const walker = document.createTreeWalker(
        fragment,
        NodeFilter.SHOW_COMMENT, // Only find comment nodes
        null
    )

    let currentNode
    const MARK_PREFIX = "__mark:"

    // Process markers immediately (no intermediate array)
    while ((currentNode = walker.nextNode())) {
        const text = currentNode.textContent
        // Fast path: direct string check instead of regex
        if (!text.startsWith(MARK_PREFIX)) continue

        const index = parseInt(text.slice(MARK_PREFIX.length), 10)
        const node = currentNode
        let value = values[index]
        const parent = node.parentNode

        if (!parent) return

        // Case 0: Function → call it with context parameters
        if (typeof value === "function")
            value = value({
                node, // Comment node marker
                parent, // Parent element
                index, // Marker index
                container, // Root container
                fragment // DocumentFragment being built
            })

        // Case 1: Nested TemplateResult
        if (value?._isTemplateResult) {
            // Use container from pool
            const temp = getContainer()
            renderTemplateResult(value, temp, {})

            // Insert all children at marker position
            while (temp.firstChild) parent.insertBefore(temp.firstChild, node)

            releaseContainer(temp)
            parent.removeChild(node)
            continue
        }

        // Case 2: Array (e.g., items.map(...))
        if (Array.isArray(value)) {
            // Batch array items into DocumentFragment for single insertion
            const frag = document.createDocumentFragment()
            const temp = getContainer()

            value.forEach((item) => {
                // If item is TemplateResult
                if (item?._isTemplateResult) {
                    renderTemplateResult(item, temp, {})
                    while (temp.firstChild) frag.appendChild(temp.firstChild)
                }
                // If item is DOM node
                else if (item?.nodeType) frag.appendChild(item.cloneNode(true))
                // If item is primitive
                else frag.appendChild(document.createTextNode(String(item ?? "")))
            })

            parent.insertBefore(frag, node)
            releaseContainer(temp)
            parent.removeChild(node)
            continue
        }

        // Case 3: DOM node
        if (value?.nodeType) {
            parent.replaceChild(value.cloneNode(true), node)
            continue
        }

        // Case 4: Primitive value (string, number, null, undefined)
        parent.replaceChild(document.createTextNode(String(value ?? "")), node)
    }

    /**
     * STEP 5: Mount fragment to container
     * Default: replace all children
     * Options: append or prepend
     */
    if (options.prepend) {
        container.prepend(fragment)
    } else if (options.append) {
        container.appendChild(fragment)
    } else {
        container.replaceChildren(fragment)
    }

    /**
     * STEP 6: Execute attribute callbacks AFTER appending to DOM
     * Batch query all callback elements in single pass
     */
    if (attributeCallbacks.length > 0) {
        // Build selector for all callback attributes
        const selector = attributeCallbacks.map(({ attrId }) => `[${attrId}]`).join(",")
        const elements = container.querySelectorAll(selector)

        // Map elements by attrId for fast lookup
        const elementMap = new Map()
        elements.forEach((el) => {
            for (const { attrId } of attributeCallbacks) {
                if (el.hasAttribute(attrId)) {
                    elementMap.set(attrId, el)
                    break
                }
            }
        })

        // Execute callbacks
        attributeCallbacks.forEach(({ attrId, callback }) => {
            const element = elementMap.get(attrId)
            if (element) {
                element.removeAttribute(attrId)
                callback({ node: element, element })
            } else console.warn("Could not find element with attribute:", attrId)
        })
    }
}

/**
 * UI.render() - Render template object to DOM and optionally mount to container
 *
 * This function receives TemplateResult from html() and:
 * 1. Convert HTML string to DOM
 * 2. Find markers (<!--__mark:i-->) using TreeWalker
 * 3. Replace markers with corresponding values
 * 4. Process nested templates recursively
 * 5. Mount to container if provided, always return rendered nodes
 *
 * @param {TemplateResult|Node|string|Array} template - Template to render
 * @param {HTMLElement|ShadowRoot} [container] - Container to mount DOM (optional)
 * @param {Object} [options] - Rendering options
 * @param {boolean} [options.append] - Append to container instead of replacing
 * @param {boolean} [options.prepend] - Prepend to container instead of replacing
 * @returns {DocumentFragment|Node} - Always returns rendered nodes
 *
 * @example
 * // Render to container (replaces all children by default)
 * const nodes = render(template, document.body)
 *
 * @example
 * // Append to container
 * render(template, document.body, { append: true })
 *
 * @example
 * // Prepend to container
 * render(template, document.body, { prepend: true })
 *
 * @example
 * // Get rendered nodes without container
 * const nodes = render(html`<div>Hello ${name}</div>`)
 * someElement.appendChild(nodes)
 *
 * @example
 * // Nested templates
 * const inner = html`<span>World</span>`
 * const outer = html`<div>Hello ${inner}</div>`
 * const nodes = render(outer, shadowRoot)
 */
export function render(template, container, options = {}) {
    // Check if container was provided by user
    const hasRealContainer = container && container.nodeType

    // Create temp container if none provided
    if (!hasRealContainer) container = document.createElement("template")

    /**
     * STEP 1: Handle different input types
     */

    // If it's a TemplateResult from html()
    if (template?._isTemplateResult) {
        // If container is <template>, render to .content
        const target = container.nodeName === "TEMPLATE" ? container.content : container
        renderTemplateResult(template, target, options)

        // If user provided a real container, nodes are already appended - return container
        // If temp container, extract and return nodes
        if (hasRealContainer) {
            // Force synchronous upgrade of custom elements (e.g. inside Shadow DOM
            // before the host is connected to the live document)
            if (typeof customElements !== "undefined") customElements.upgrade(container)
            return container
        }

        return getNodesFromContainer(container, false)
    }

    // If it's a direct DOM node
    if (template?.nodeType) {
        const target = container.nodeName === "TEMPLATE" ? container.content : container

        if (options.prepend) {
            target.prepend(template)
        } else if (options.append) {
            target.appendChild(template)
        } else {
            target.replaceChildren(template)
        }

        // Return the appended node or container
        return hasRealContainer ? container : template
    }

    // If it's an array (e.g., items.map(...))
    if (Array.isArray(template)) {
        // For arrays with append/prepend, we need to handle differently
        if (!options.append && !options.prepend && hasRealContainer) {
            // Clear container first for default replace behavior
            const target = container.nodeName === "TEMPLATE" ? container.content : container
            while (target.firstChild) target.removeChild(target.firstChild)
        }

        template.forEach((item) => render(item, container, { ...options, append: options.prepend ? false : options.append || true }))
        // Return container if real, otherwise extract nodes
        if (hasRealContainer) return container

        return getNodesFromContainer(container, false)
    }

    // If it's a primitive value (string, number, etc.)
    const textContent = String(template ?? "")
    if (container.nodeName === "TEMPLATE") {
        if (options.prepend) {
            container.content.prepend(document.createTextNode(textContent))
        } else if (options.append) {
            container.content.appendChild(document.createTextNode(textContent))
        } else {
            container.content.textContent = textContent
        }
    } else {
        if (options.prepend) {
            container.prepend(document.createTextNode(textContent))
        } else if (options.append) {
            container.appendChild(document.createTextNode(textContent))
        } else {
            container.textContent = textContent
        }
    }

    // Return container if real, otherwise extract nodes
    if (hasRealContainer) return container

    return getNodesFromContainer(container, false)
}

export default render
