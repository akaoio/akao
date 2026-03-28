import { passkey } from "/core/Access.js"

export async function signinWithPasskey() {
    return passkey()
}

export default signinWithPasskey
