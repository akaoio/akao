import { threads } from "/core/Threads.js"

// Serialize queue — all SQL calls go through here one at a time.
// Prevents flooding the SQL worker when many calls arrive simultaneously.
const queue = []
let processing = false

function pump() {
    if (processing || queue.length === 0) return
    processing = true

    const { method, params, resolve, reject } = queue.shift()
    let timedOut = false

    // Watchdog: if the worker never responds (crash, hang), unblock the queue.
    // This rejects the stuck item and sets processing=false so the next $call()
    // will trigger pump() with a fresh worker. We don't pump() immediately here
    // to avoid flooding a potentially wedged worker with more work.
    const watchdog = setTimeout(() => {
        timedOut = true
        processing = false
        reject(new Error(`SQL worker unresponsive: ${method}`))
        // Don't pump() — let the next $call() restart the worker fresh
    }, 10000)

    threads.queue({
        thread: "sql",
        method,
        params,
        callback: (response, error) => {
            if (timedOut) return  // ignore late response after timeout
            clearTimeout(watchdog)
            processing = false
            if (error) reject(new Error(error?.message || String(error)))
            else resolve(response)
            pump()
        }
    })
}

// Core dispatch: send a method call to the sql worker and return a Promise.
// All public SQL methods go through here.
export function $call(method, params) {
    return new Promise((resolve, reject) => {
        queue.push({ method, params: { db: this.name, ...params }, resolve, reject })
        pump()
    })
}
