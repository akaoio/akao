/**
 * HMR Dev Client (SSE Connection + Runtime Loader)
 * Bootstrap is done inline in dev.js to run before ES modules
 */

(function() {
    // Check if DEV mode (already done in bootstrap, but double-check)
    const isDev = globalThis?.location?.hostname === "localhost" || 
                  globalThis?.location?.hostname === "127.0.0.1"
    
    if (!isDev || typeof window === "undefined" || !window.EventSource) {
        console.log("🔥 HMR: Client disabled (production mode)")
        return
    }
    
    const DEV_EVENTS_PATH = "/__dev_events"
    const RECONNECT_INTERVAL = 1000
    
    // State tracking
    window._dev = window._dev || {
        connectedAt: null,
        lastMessageAt: null,
        messageCount: 0,
        readyState: null,
        hmrEnabled: false
    }
    
    let source = null
    let reconnectTimer = null
    
    function connect() {
        if (source && source.readyState !== EventSource.CLOSED) return
        
        source = new EventSource(DEV_EVENTS_PATH)
        
        source.onopen = function() {
            window._dev.connectedAt = Date.now()
            window._dev.readyState = source.readyState
            console.log("🔌 HMR: Connected to dev server")
            clearTimeout(reconnectTimer)
        }
        
        source.onmessage = async function(e) {
            window._dev.messageCount += 1
            window._dev.lastMessageAt = Date.now()
            window._dev.readyState = source.readyState
            
            if (!e || !e.data) return
            
            // Legacy reload message
            if (e.data === "reload") {
                console.log("🔄 HMR: Full page reload requested")
                try {
                    sessionStorage.setItem("__dev_last_reload_at", String(Date.now()))
                } catch (_) {}
                window.location.reload()
                return
            }
            
            // Parse HMR update message
            try {
                const update = JSON.parse(e.data)
                
                if (update.type === "hmr" && window.hmr?.handle) {
                    window._dev.hmrEnabled = true
                    await window.hmr.handle(update)
                } else if (update.type === "full-reload") {
                    console.log("🔄 HMR: Full reload required")
                    window.location.reload()
                }
            } catch (err) {
                console.warn("⚠️ HMR: Failed to parse update message:", err)
            }
        }
        
        source.onerror = function() {
            window._dev.readyState = source.readyState
            
            if (source.readyState === EventSource.CLOSED) {
                console.warn("⚠️ HMR: Connection closed, reconnecting...")
                reconnectTimer = setTimeout(connect, RECONNECT_INTERVAL)
            }
        }
    }
    
    // Load full HMR runtime asynchronously
    async function initHMR() {
        try {
            const hmrModule = await import("/core/HMR.js")
            // Full runtime extends the lightweight bootstrap
            window.hmr = hmrModule.default
            window._dev.hmrEnabled = true
            console.log("🔥 HMR: Runtime initialized")
        } catch (error) {
            console.error("❌ HMR: Failed to initialize runtime:", error)
            window._dev.hmrEnabled = false
        }
    }
    
    // Start
    initHMR().then(() => {
        connect()
    })
    
    // Cleanup on page unload
    window.addEventListener("beforeunload", () => {
        if (source) {
            source.close()
        }
        clearTimeout(reconnectTimer)
    })
})()
