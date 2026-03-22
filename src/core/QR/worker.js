import { threads } from "/core/Threads.js"

const DEFAULT_TIMEOUT = 12000

function request({ method, params, transfer, timeout = DEFAULT_TIMEOUT } = {}) {
    return new Promise((resolve, reject) => {
        let settled = false
        const timer = setTimeout(() => {
            if (settled) return
            settled = true
            reject(new Error(`QR worker timeout: ${method}`))
        }, timeout)

        threads.queue({
            thread: "qr",
            method,
            params,
            transfer,
            callback: (response, error) => {
                if (settled) return
                settled = true
                clearTimeout(timer)
                if (error) {
                    const message = error?.message || String(error)
                    reject(new Error(message))
                    return
                }
                resolve(response)
            }
        })
    })
}

export async function setup(params = {}) {
    return await request({ method: "configure", params })
}

export async function decode(params = {}, options = {}) {
    return await request({ method: "scan", params, transfer: options.transfer, timeout: options.timeout })
}

export async function dispose() {
    return await request({ method: "destroy" })
}
