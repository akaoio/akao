import { threads } from "/core/Threads.js"

export function request({ method, params, transfer, timeout = 12000 } = {}) {
    return new Promise((resolve, reject) => {
        let settled = false
        const timer = setTimeout(() => {
            if (settled) return
            settled = true
            reject(new Error(`Wave worker timeout: ${method}`))
        }, timeout)

        threads.queue({
            thread: "wave",
            method,
            params,
            transfer,
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