/**
 * HMR Dev Client
 * Injected into HTML during development to receive hot update events from dev server
 */

(function() {
    if (typeof window === "undefined" || !window.EventSource) return
    
    const DEV_EVENTS_PATH = "/__dev_events"
    const RECONNECT_INTERVAL = 1000
    
    // State tracking
    window.__devSseState = window.__devSseState || {
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
            window.__devSseState.connectedAt = Date.now()
            window.__devSseState.readyState = source.readyState
            console.log("🔌 HMR: Connected to dev server")
            clearTimeout(reconnectTimer)
        }
        
        source.onmessage = async function(e) {
            window.__devSseState.messageCount += 1
            window.__devSseState.lastMessageAt = Date.now()
            window.__devSseState.readyState = source.readyState
            
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
                
                if (update.type === "hmr" && window.__hmr) {
                    window.__devSseState.hmrEnabled = true
                    await window.__hmr.handleUpdate(update)
                } else if (update.type === "full-reload") {
                    console.log("🔄 HMR: Full reload required")
                    window.location.reload()
                }
            } catch (err) {
                console.warn("⚠️ HMR: Failed to parse update message:", err)
            }
        }
        
        source.onerror = function() {
            window.__devSseState.readyState = source.readyState
            
            if (source.readyState === EventSource.CLOSED) {
                console.warn("⚠️ HMR: Connection closed, reconnecting...")
                reconnectTimer = setTimeout(connect, RECONNECT_INTERVAL)
            }
        }
    }
    
    // Initialize HMR runtime
    async function initHMR() {
        try {
            const hmrModule = await import("/core/HMR.js")
            window.__hmr = hmrModule.default
            window.__devSseState.hmrEnabled = true
            console.log("🔥 HMR: Runtime initialized")
        } catch (error) {
            console.error("❌ HMR: Failed to initialize runtime:", error)
            window.__devSseState.hmrEnabled = false
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
