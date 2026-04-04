import { signup } from "/core/Access.js"
import WebAuthn from "/core/WebAuthn.js"

export class Logic {
    static async signup(data) {
        return signup(data)
    }

    static async sign(data) {
        return WebAuthn.sign({ data })
    }
}

export default Logic
