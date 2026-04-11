/**
 * Hot Module Replacement (HMR) Runtime
 * 
 * Provides full HMR for Web Components without manual import map maintenance.
 * Preserves all application state (Context, Access, States) during hot updates.
 * 
 * Features:
 * - Automatic dependency graph tracking
 * - Custom element instance tracking and hot swap
 * - Dynamic import map versioning
 * - CSS hot injection
 * - Template-only updates (fastest path)
 */

import { DEV } from "/core/Utils/environment.js"

let hmr = {}

if (DEV) {

class HMRRuntime {
    constructor() {
        // Module registry: URL -> { exports, timestamp, dependents, instances }
        this.modules = new Map()
        
        // Custom element registry: tagName -> { class, instances: WeakSet, module }
        // Use existing elements Map if client.js already created it
        this.elements = window.hmr?.elements || new Map()
        
        // Import map cache for versioned imports
        this.importmap = new Map()
        
        // Track pending updates to debounce
        this.pending = new Map()
        this.timer = null
        
        // Original functions to restore if needed
        this.origdefine = window.hmr?.origdefine || customElements.define.bind(customElements)
        
        // Only setup interceptors if not already done by client
        if (!window.hmr?.origdefine) {
            this.setup()
        }
    }
    
    setup() {
        // Intercept customElements.define to track component registrations
        const self = this
        customElements.define = function(tagName, constructor, options) {
            // Auto-detect module URL from call stack
            let moduleUrl = constructor._module || constructor.module || null
            
            // If not explicitly set, try to extract from call stack
            if (!moduleUrl) {
                const stack = new Error().stack
                if (stack) {
                    const lines = stack.split('\n')
                    for (let i = 1; i < Math.min(lines.length, 5); i++) {
                        const line = lines[i]
                        const match = line.match(/(https?:\/\/[^:\s)]+\.js)/)
                        if (match) {
                            moduleUrl = match[1].split('?')[0]
                            break
                        }
                    }
                }
            }
            
            self.elements.set(tagName, {
                class: constructor,
                module: moduleUrl
            })
            
            // Use original define without wrapping constructor
            return self.origdefine(tagName, constructor, options)
        }
    }
    
    /**
     * Register component class with module URL for HMR tracking
     * Called automatically by Component base class
     */
    reg(url, cls) {
        if (!cls || typeof cls !== 'function') return
        
        // If URL not provided, try to read from static module property
        if (!url && cls.module) url = cls.module
        if (!url) return
        
        // Store on class for interceptor to read (use single _ for meta)
        cls._module = url
    }
    
    /**
     * Register a module and its dependencies
     */
    register(url, exports, dependencies = []) {
        this.modules.set(url, {
            exports,
            timestamp: Date.now(),
            dependents: new Set(),
            dependencies: new Set(dependencies)
        })
        
        // Update dependency graph
        for (const dep of dependencies) {
            const depModule = this.modules.get(dep)
            if (depModule) {
                depModule.dependents.add(url)
            }
        }
    }
    
    /**
     * Handle hot update from dev server
     */
    async handle(update) {
        const url = this.resolve(update.path)
        
        // Debounce rapid updates
        this.pending.set(url, update)
        clearTimeout(this.timer)
        this.timer = setTimeout(() => this.process(), 50)
    }
    
    async process() {
        const updates = Array.from(this.pending.values())
        this.pending.clear()
        
        for (const update of updates) {
            await this.apply(update)
        }
    }
    
    async apply({ path, type, timestamp }) {
        const url = this.resolve(path)
        
        console.log(`🔥 HMR: ${type} update for ${path}`)
        
        if (type === 'css' || path.endsWith('.css.js')) {
            await this.swapcss(url, timestamp)
        } else if (type === 'template' || path.includes('/template.js')) {
            await this.swaptpl(url, timestamp)
        } else if (type === 'js' || path.endsWith('.js')) {
            await this.swapmod(url, timestamp)
        }
    }
    
    /**
     * Hot swap CSS - inject new styles into Shadow DOM
     */
    async swapcss(url, timestamp) {
        try {
            const versioned = `${url}?v=${timestamp}`
            
            // Update import map FIRST so template reload will get versioned CSS
            this.importmap.set(url, versioned)

            const module = await import(versioned)
            const styles = module.default
            
            // Find component using this stylesheet
            const compurl = url.replace('/styles.css.js', '/index.js')
            const elems = this.findelem(compurl)
            
            for (const { tagName } of elems) {
                const live = document.querySelectorAll(tagName)
                for (const el of live) {
                    if (!el.shadowRoot) continue
                    
                    // Check if component has <style> elements (styles injected separately)
                    const hasStyleTags = el.shadowRoot.querySelectorAll('style').length > 0
                    
                    if (hasStyleTags) {
                        // Replace all style elements
                        el.shadowRoot.querySelectorAll('style').forEach(s => s.remove())
                        
                        // Inject new styles
                        if (styles) el.shadowRoot.prepend(styles)
                    } else {
                        // Styles are embedded in template - re-render template
                        const tplurl = compurl.replace('/index.js', '/template.js')
                        await this.swaptpl(tplurl, timestamp)
                    }
                }
            }
            
            console.log(`✅ HMR: CSS updated for ${url} (${elems.length} components)`)
        } catch (error) {
            console.error(`❌ HMR: Failed to update CSS ${url}:`, error)
        }
    }
    
    /**
     * Hot swap template - re-render components
     */
    async swaptpl(url, timestamp) {
        try {
            const versioned = `${url}?v=${timestamp}`
            
            // Clear module from cache
            this.importmap.set(url, versioned)
            
            const module = await import(versioned)
            const tpl = module.default || module.template
            
            // Find component using this template
            const compurl = url.replace('/template.js', '/index.js')
            const elems = this.findelem(compurl)
            
            for (const { tagName } of elems) {
                const live = document.querySelectorAll(tagName)
                for (const el of live) {
                    if (!el.shadowRoot) continue
                    
                    // Re-render with new template
                    if (typeof el.render === 'function') {
                        el.render()
                    } else {
                        const { render } = await import('/core/UI.js')
                        render(tpl, el.shadowRoot)
                    }
                }
            }
            
            console.log(`✅ HMR: Template updated for ${url}`)
        } catch (error) {
            console.error(`❌ HMR: Failed to update template ${url}:`, error)
        }
    }
    
    /**
     * Hot swap module - re-import and update dependents
     */
    async swapmod(url, timestamp) {
        try {
            const versioned = `${url}?v=${timestamp}`
            this.importmap.set(url, versioned)
            
            const module = await import(versioned)
            
            // Check if component module
            const iscomp = url.includes('/components/') || url.includes('/routes/')
            
            if (iscomp) {
                const tag = this.findtag(url)
                if (tag) {
                    // Update component class methods without re-defining
                    await this.swapcomp(tag, module, url)
                }
            }
            
            // Update module registry
            const data = this.modules.get(url)
            if (data) {
                data.exports = module
                data.timestamp = timestamp
                
                // Hot update dependents recursively
                for (const dep of data.dependents) {
                    await this.swapmod(dep, timestamp)
                }
            }
            
            console.log(`✅ HMR: Module updated for ${url}`)
        } catch (error) {
            console.error(`❌ HMR: Failed to update module ${url}:`, error)
            console.warn('🔄 Consider full page reload')
        }
    }
    
    /**
     * Hot swap component - update methods on existing instances
     * Respects Component/Route architecture
     */
    async swapcomp(tag, module, url) {
        const data = this.elements.get(tag)
        if (!data) return
        
        // Get new component class
        const Cls = module[Object.keys(module).find(k => 
            module[k]?.prototype instanceof HTMLElement
        )] || module.default
        
        if (!Cls) return
        
        // Get all live instances
        const live = Array.from(document.querySelectorAll(tag))
        
        // Base class methods to skip (from Component and Route)
        const baseMethods = new Set(['constructor', 'connectedCallback', 'disconnectedCallback', 
                                     'onConnect', 'onDisconnect', 'subscribe', 'listen', 'watch',
                                     'query', 'queryAll', 'renderTemplate'])
        
        // Update each instance with new methods
        for (const el of live) {
            if (!el.shadowRoot) continue
            
            // Preserve state
            const states = el.states?.states || {}
            const subs = el.subscriptions || []
            
            try {
                // Patch only custom methods (skip base class lifecycle)
                const methods = Object.getOwnPropertyNames(Cls.prototype)
                for (const method of methods) {
                    if (baseMethods.has(method)) continue
                    if (typeof Cls.prototype[method] === 'function') {
                        el[method] = Cls.prototype[method].bind(el)
                    }
                }
                
                // Re-render if method exists
                if (typeof el.render === 'function') {
                    el.render()
                }
                
                // Restore states
                if (el.states && Object.keys(states).length > 0) {
                    el.states.set(states)
                }
                
                // Preserve subscriptions (Component manages these)
                if (subs.length > 0) {
                    el.subscriptions = subs
                }
            } catch (error) {
                console.warn(`⚠️ HMR: Failed to hot-swap ${tag}:`, error)
            }
        }
        
        // Update registry
        data.class = Cls
        data.module = url
        
        console.log(`✅ HMR: Component ${tag} hot-swapped (${live.length} instances)`)
    }
    
    /**
     * Find custom elements using a module
     */
    findelem(url) {
        const results = []
        
        for (const [tag, data] of this.elements.entries()) {
            if (data.module?.includes(url) || url.includes(data.module)) {
                results.push({ tagName: tag, ...data })
            }
        }
        
        return results
    }
    
    /**
     * Find tag name for module URL
     */
    findtag(url) {
        for (const [tag, data] of this.elements.entries()) {
            if (data.module === url || url.includes(data.module?.replace(/\\/g, '/'))) {
                return tag
            }
        }
        
        // Fallback: guess from path
        const match = url.match(/\/(components|routes)\/([^/]+)\//)
        if (match) {
            const name = match[2]
            return `ui-${name}`
        }
        
        return null
    }
    
    /**
     * Resolve relative URL to absolute
     */
    resolve(path) {
        if (path.startsWith('http://') || path.startsWith('https://')) return path
        
        // Normalize
        path = path.replace(/^\.\//, '').replace(/^\//, '')
        
        // Convert src/ to build/ path
        if (path.startsWith('src/')) path = path.replace('src/', '')
        
        return new URL(path, window.location.origin).href
    }
    
    /**
     * Register module and dependencies
     */
    reg(url, exports, deps = []) {
        this.modules.set(url, {
            exports,
            timestamp: Date.now(),
            dependents: new Set(),
            dependencies: new Set(deps)
        })
        
        // Update dependency graph
        for (const dep of deps) {
            const data = this.modules.get(dep)
            if (data) data.dependents.add(url)
        }
    }
    
    /**
     * Accept updates for this module (called by module code)
     */
    accept(cb) {
        const url = this.getcaller()
        if (!cb || typeof cb !== 'function') return
        
        const data = this.modules.get(url)
        if (data) data.acceptcb = cb
    }
    
    /**
     * Dispose handler for cleanup before hot update
     */
    dispose(cb) {
        const url = this.getcaller()
        if (!cb || typeof cb !== 'function') return
        
        const data = this.modules.get(url)
        if (data) data.disposecb = cb
    }
    
    getcaller() {
        const stack = new Error().stack
        if (!stack) return null
        const lines = stack.split('\n')
        for (let i = 3; i < lines.length; i++) {
            const match = lines[i].match(/https?:\/\/[^):\s]+\.js/)
            if (match) return match[0]
        }
        return null
    }
}

// Create global HMR runtime (or extend existing one from client.js)
if (window.hmr && window.hmr.elements) {
    // Client.js already initialized - extend with full runtime methods
    hmr = window.hmr
    const runtime = new HMRRuntime()
    // Copy all methods from runtime to hmr
    for (const key of Object.getOwnPropertyNames(HMRRuntime.prototype)) {
        if (key !== 'constructor') {
            hmr[key] = runtime[key].bind(hmr)
        }
    }
    // Copy instance properties that aren't already set
    if (!hmr.modules) hmr.modules = runtime.modules
    if (!hmr.importmap) hmr.importmap = runtime.importmap
    if (!hmr.pending) hmr.pending = runtime.pending
    if (!hmr.timer) hmr.timer = runtime.timer
} else {
    // No client - create fresh runtime
    hmr = new HMRRuntime()
}

// Expose on window for dev client
if (typeof window !== 'undefined') {
    window.hmr = hmr
}

console.log("🔥 HMR: Enabled (dev mode)")

} else {
    // Production mode
    console.log("🔥 HMR: Disabled (production mode)")
}

// Export for use in modules
export default hmr

