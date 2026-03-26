import { threads } from "/core/Threads.js"

// Core dispatch: send a method call to the sql worker and return a Promise.
// All public SQL methods go through here.
export function $call(method, params) {
    return new Promise((resolve, reject) => {
        let settled = false
        const timer = setTimeout(() => {
            if (settled) return
            settled = true
            reject(new Error(`SQL worker timeout: ${method}`))
        }, 30000)

        threads.queue({
            thread: "sql",
            method,
            params: { db: this.name, ...params },
            callback: (response, error) => {
                if (settled) return
                settled = true
                clearTimeout(timer)
                if (error) {
                    reject(new Error(error?.message || String(error)))
                    return
                }
                resolve(response)
            }
        })
    })
}
