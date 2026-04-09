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

    // Watchdog: if the worker never responds (crash, hang), reject this item
    // and reject ALL remaining queued items since the worker may be wedged.
    // This prevents hanging Promises — callers will get errors and can retry.
    const watchdog = setTimeout(() => {
        timedOut = true
        processing = false
        reject(new Error(`SQL worker unresponsive: ${method}`))
        // Reject remaining queued items so no one hangs forever
        while (queue.length > 0) {
            const { resolve, reject } = queue.shift()
            reject(new Error(`SQL worker unresponsive, flushed queue`))
        }
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
