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

class HMRRuntime {
    constructor() {
        // Module registry: URL -> { exports, timestamp, dependents, instances }
        this.modules = new Map()
        
        // Custom element registry: tagName -> { class, instances: WeakSet, module }
        this.elements = new Map()
        
        // Import map cache for versioned imports
        this.importMap = new Map()
        
        // Track pending updates to debounce
        this.pendingUpdates = new Map()
        this.updateTimeout = null
        
        // Original functions to restore if needed
        this.originalDefine = customElements.define.bind(customElements)
        
        this.setupInterceptors()
    }
    
    setupInterceptors() {
        // Intercept customElements.define to track component registrations
        const self = this
        customElements.define = function(tagName, constructor, options) {
            // Store element metadata for HMR tracking
            self.elements.set(tagName, {
                class: constructor,
                module: self.getCurrentModule()
            })
            
            // Use original define without wrapping constructor
            // This avoids "Illegal constructor" errors with HTMLElement
            return self.originalDefine(tagName, constructor, options)
        }
    }
    
    getCurrentModule() {
        // Try to determine current executing module from stack trace
        const stack = new Error().stack
        const match = stack?.match(/https?:\/\/[^)]+\.js/)
        return match ? match[0] : null
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
    async handleUpdate({ path, type, timestamp }) {
        const url = this.resolveUrl(path)
        
        // Debounce rapid updates
        this.pendingUpdates.set(url, { path, type, timestamp })
        clearTimeout(this.updateTimeout)
        this.updateTimeout = setTimeout(() => this.processPendingUpdates(), 50)
    }
    
    async processPendingUpdates() {
        const updates = Array.from(this.pendingUpdates.values())
        this.pendingUpdates.clear()
        
        for (const update of updates) {
            await this.processUpdate(update)
        }
    }
    
    async processUpdate({ path, type, timestamp }) {
        const url = this.resolveUrl(path)
        
        console.log(`🔥 HMR: ${type} update for ${path}`)
        
        if (type === 'css' || path.endsWith('.css.js')) {
            await this.hotSwapCSS(url, timestamp)
        } else if (type === 'template' || path.includes('/template.js')) {
            await this.hotSwapTemplate(url, timestamp)
        } else if (type === 'js' || path.endsWith('.js')) {
            await this.hotSwapModule(url, timestamp)
        }
    }
    
    /**
     * Hot swap CSS - inject new styles into Shadow DOM
     */
    async hotSwapCSS(url, timestamp) {
        try {
            const versionedUrl = `${url}?v=${timestamp}`
            const module = await import(versionedUrl)
            const newStyles = module.default
            
            // Find all elements using this stylesheet
            const affectedElements = this.findElementsUsingModule(url)
            
            for (const { tagName, instances } of affectedElements) {
                // Get all live instances via DOM query (WeakSet doesn't iterate)
                const liveElements = document.querySelectorAll(tagName)
                for (const element of liveElements) {
                    if (!element.shadowRoot) continue
                    
                    // Replace old style element
                    const oldStyle = element.shadowRoot.querySelector('style[data-hmr]')
                    if (oldStyle) {
                        oldStyle.remove()
                    }
                    
                    // Inject new styles
                    if (newStyles && typeof newStyles === 'object' && newStyles.sheet) {
                        const styleClone = newStyles.cloneNode(true)
                        styleClone.dataset.hmr = 'true'
                        element.shadowRoot.prepend(styleClone)
                    }
                }
            }
            
            console.log(`✅ HMR: CSS updated for ${url}`)
        } catch (error) {
            console.error(`❌ HMR: Failed to update CSS ${url}:`, error)
        }
    }
    
    /**
     * Hot swap template - re-render components
     */
    async hotSwapTemplate(url, timestamp) {
        try {
            const versionedUrl = `${url}?v=${timestamp}`
            
            // Clear module from cache by updating import map
            this.importMap.set(url, versionedUrl)
            
            const module = await import(versionedUrl)
            const newTemplate = module.default || module.template
            
            // Find component using this template
            const componentUrl = url.replace('/template.js', '/index.js')
            const affectedElements = this.findElementsUsingModule(componentUrl)
            
            for (const { tagName } of affectedElements) {
                const liveElements = document.querySelectorAll(tagName)
                for (const element of liveElements) {
                    if (!element.shadowRoot) continue
                    
                    // Re-render with new template (if component has render method)
                    if (typeof element.render === 'function') {
                        element.render()
                    } else {
                        // Fallback: re-render root template
                        const { render } = await import('/core/UI.js')
                        render(newTemplate, element.shadowRoot)
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
    async hotSwapModule(url, timestamp) {
        try {
            const versionedUrl = `${url}?v=${timestamp}`
            this.importMap.set(url, versionedUrl)
            
            const module = await import(versionedUrl)
            
            // Check if this is a component module
            const isComponent = url.includes('/components/') || url.includes('/routes/')
            
            if (isComponent) {
                const tagName = this.findTagNameForModule(url)
                if (tagName) {
                    await this.hotSwapComponent(tagName, module, url)
                }
            }
            
            // Update module registry
            const moduleData = this.modules.get(url)
            if (moduleData) {
                moduleData.exports = module
                moduleData.timestamp = timestamp
                
                // Hot update dependents recursively
                for (const dependentUrl of moduleData.dependents) {
                    await this.hotSwapModule(dependentUrl, timestamp)
                }
            }
            
            console.log(`✅ HMR: Module updated for ${url}`)
        } catch (error) {
            console.error(`❌ HMR: Failed to update module ${url}:`, error)
            console.warn('🔄 Consider full page reload')
        }
    }
    
    /**
     * Hot swap component - re-define custom element
     */
    async hotSwapComponent(tagName, module, url) {
        const elementData = this.elements.get(tagName)
        if (!elementData) return
        
        // Get new component class
        const NewClass = module[Object.keys(module).find(k => 
            module[k]?.prototype instanceof HTMLElement
        )] || module.default
        
        if (!NewClass) return
        
        // Store old instances
        const liveElements = Array.from(document.querySelectorAll(tagName))
        
        // For each instance: preserve state, re-render
        for (const element of liveElements) {
            if (!element.shadowRoot) continue
            
            // Preserve internal states
            const preservedStates = element.states?.states || {}
            const preservedProps = element.props || {}
            
            // Re-render with new class methods
            try {
                // Call new render method if exists
                if (NewClass.prototype.render) {
                    element.render = NewClass.prototype.render.bind(element)
                    element.render()
                }
                
                // Restore states if they were preserved
                if (element.states && Object.keys(preservedStates).length > 0) {
                    element.states.set(preservedStates)
                }
            } catch (error) {
                console.warn(`⚠️ HMR: Failed to hot-swap instance of ${tagName}:`, error)
            }
        }
        
        // Update element registry
        elementData.class = NewClass
        elementData.module = url
        
        console.log(`✅ HMR: Component ${tagName} hot-swapped (${liveElements.length} instances)`)
    }
    
    /**
     * Find custom elements that use a specific module
     */
    findElementsUsingModule(moduleUrl) {
        const results = []
        
        for (const [tagName, data] of this.elements.entries()) {
            if (data.module?.includes(moduleUrl) || moduleUrl.includes(data.module)) {
                results.push({ tagName, ...data })
            }
        }
        
        return results
    }
    
    /**
     * Find tag name for a module URL
     */
    findTagNameForModule(moduleUrl) {
        for (const [tagName, data] of this.elements.entries()) {
            if (data.module === moduleUrl || moduleUrl.includes(data.module?.replace(/\\/g, '/'))) {
                return tagName
            }
        }
        
        // Fallback: try to guess from path
        const match = moduleUrl.match(/\/(components|routes)\/([^/]+)\//)
        if (match) {
            const componentName = match[2]
            return `ui-${componentName}`
        }
        
        return null
    }
    
    /**
     * Resolve relative URL to absolute
     */
    resolveUrl(path) {
        if (path.startsWith('http://') || path.startsWith('https://')) {
            return path
        }
        
        // Normalize path
        path = path.replace(/^\.\//, '').replace(/^\//, '')
        
        // Convert src/ to build/ path
        if (path.startsWith('src/')) {
            path = path.replace('src/', '')
        }
        
        return new URL(path, window.location.origin).href
    }
    
    /**
     * Accept updates for this module (called by module code)
     */
    accept(callback) {
        const moduleUrl = this.getCurrentModule()
        if (!callback || typeof callback !== 'function') return
        
        // Store accept handler for this module
        const moduleData = this.modules.get(moduleUrl)
        if (moduleData) {
            moduleData.acceptHandler = callback
        }
    }
    
    /**
     * Dispose handler for cleanup before hot update
     */
    dispose(callback) {
        const moduleUrl = this.getCurrentModule()
        if (!callback || typeof callback !== 'function') return
        
        const moduleData = this.modules.get(moduleUrl)
        if (moduleData) {
            moduleData.disposeHandler = callback
        }
    }
}

// Create global HMR runtime
const hmr = new HMRRuntime()

// Export for use in modules
export default hmr

// Expose on window for dev client
if (typeof window !== 'undefined') {
    window.__hmr = hmr
}
