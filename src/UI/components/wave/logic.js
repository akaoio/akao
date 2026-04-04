import { Wave } from "/core/Wave.js"

export class Logic {
    static async configure(sampleRate, opts = {}) {
        return Wave.request({
            method: "configure",
            params: { sampleRate, sampleRateInp: sampleRate, sampleRateOut: sampleRate, samplesPerFrame: 1024, ...opts }
        })
    }

    static async encode(text) {
        return Wave.encode({ message: text })
    }

    static async decode(bytes) {
        return Wave.decode({ bytes }, { transfer: [bytes.buffer] })
    }
}

export default Logic
