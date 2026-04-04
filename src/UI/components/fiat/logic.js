import Forex from "/core/Forex.js"

export class Logic {
    static async convert(amount, base, quote) {
        return Forex.convert(amount, base, quote)
    }
}

export default Logic
