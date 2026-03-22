import { threads } from "/core/Threads.js"

const DEFAULT_TIMEOUT = 12000

function queue({ method, params, transfer, timeout = DEFAULT_TIMEOUT } = {}) {
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

export async function setup(params = {}) {
    return await queue({ method: "configure", params })
}

export async function encode(params = {}, options = {}) {
    return await queue({ method: "encode", params, timeout: options.timeout })
}

export async function decode(params = {}, options = {}) {
    return await queue({ method: "decode", params, transfer: options.transfer, timeout: options.timeout })
}

export async function reset() {
    return await queue({ method: "reset" })
}

export async function dispose() {
    return await queue({ method: "destroy" })
}

export { queue as request }
