import { renderSVG, encode } from "./QR/encoder.js"
import QrScanner from "./QR/decoder.js"
import { threads } from "/core/Threads.js"

class QR {
    static request({ method, params, transfer, timeout = 12000 } = {}) {
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
    
    static async decode(params = {}, options = {}) {
        return await this.request({ method: "scan", params, transfer: options.transfer, timeout: options.timeout })
    }

    static encode = encode
    static render = renderSVG
    static scan = QrScanner
}

export default QR