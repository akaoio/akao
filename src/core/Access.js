/**
 * Access control and authentication module using WebAuthn passkeys.
 * Manages user authentication state, wallet information, and key pair generation.
 * Integrates with SEA (Simple End-to-End Cryptography) for encryption and Zen database.
 */

import WebAuthn from "./WebAuthn.js"
import States from "./States.js"
import { DEV } from "./Utils/environment.js"
import zen, { initZEN } from "./ZEN.js"

const DEV_SESSION_KEY = "__dev_auth__"

function uint8ToB64(arr) {
    return btoa(String.fromCharCode(...arr))
}

function b64ToUint8(str) {
    return new Uint8Array(atob(str).split("").map((c) => c.charCodeAt(0)))
}

/**
 * Reactive state store for user authentication and access information.
 * @property {boolean} authenticated - Whether user is currently authenticated
 * @property {ArrayBuffer} id - Passkey credential ID (not user ID)
 * @property {string} pub - Public key of the passkey (stored encrypted in Zen)
 * @property {Object} pair - SEA key pair generated from user ID (used for encryption)
 * @property {Object} wallet - Current wallet information { id, total }
 */
export const Access = new States({
    authenticated: false,
    id: null, // This is the passkey's credential ID
    pub: null, // This is the public key of the passkey, not pub of the pair bellow
    pair: null, // This is the key pair generated from the user's ID
    wallet: null, // This is only ID of the current wallet, not the wallet object itself, we use this to calculate wallet seed.
    avatar: null // This is only ID of the current avatar, not the avatar object itself, we use this to calculate avatar seed.
})

/**
 * Retrieve wallet information from local storage.
 * Returns cached wallet data or defaults to wallet ID 0.
 * Automatically fixes and re-saves malformed wallet data.
 * @returns {Object} Wallet object with id and total
 */
export function getWallet() {
    const memory = globalThis.localStorage ? globalThis.localStorage.getItem("wallet") : null
    let data = { id: 0 }
    let wallet
    try {
        if (memory) data = JSON.parse(memory)
        if (typeof data === "object") wallet = JSON.stringify(data)
    } catch (error) {}
    // Auto-correct and save if data was modified during parsing
    if (wallet && wallet !== memory && globalThis.localStorage) globalThis.localStorage.setItem("wallet", wallet)
    return data
}

/**
 * Save or update wallet information in local storage and Access state.
 * Only updates if user is authenticated.
 * @param {Object} options - Wallet update options
 * @param {number} options.id - Wallet ID (uses current wallet if not provided)
 * @param {number} options.total - Wallet total (uses current wallet if not provided)
 * @returns {Object} Updated wallet data
 */
export function setWallet({ id, total } = {}) {
    // Only proceed if user is authenticated and id is not null and id is different to the current wallet ID
    if (!Access.get("authenticated")) return
    const wallet = Access.get("wallet") ?? getWallet()
    let data = {
        id: id ?? wallet.id ?? 0,
        total: total ?? wallet.total
    }
    let json
    try {
        json = JSON.stringify(data)
    } catch (error) {}
    if (json && globalThis.localStorage) globalThis.localStorage.setItem("wallet", json)
    Access.set({ wallet: data })
    return data
}

/**
 * Retrieve avatar information from local storage.
 * Returns cached avatar data or defaults to avatar ID 0.
 * Automatically fixes and re-saves malformed avatar data.
 * @returns {Object} Avatar object with id and total
 */
export function getAvatar() {
    const memory = globalThis.localStorage ? globalThis.localStorage.getItem("avatar") : null
    let data = { id: 0 }
    let avatar
    try {
        if (memory) data = JSON.parse(memory)
        if (typeof data === "object") avatar = JSON.stringify(data)
    } catch (error) {}
    // Auto-correct and save if data was modified during parsing
    if (avatar && avatar !== memory && globalThis.localStorage) globalThis.localStorage.setItem("avatar", avatar)
    return data
}

/**
 * Save or update avatar information in local storage and Access state.
 * Only updates if user is authenticated.
 * @param {Object} options - Avatar update options
 * @param {number} options.id - Avatar ID (uses current avatar if not provided)
 * @param {number} options.total - Avatar total (uses current avatar if not provided)
 * @returns {Object} Updated avatar data
 */
export function setAvatar({ id, total } = {}) {
    // Only proceed if user is authenticated and id is not null and id is different to the current wallet ID
    if (!Access.get("authenticated")) return
    const avatar = Access.get("avatar") ?? getAvatar()
    let data = {
        id: id ?? avatar.id ?? 0,
        total: total ?? avatar.total
    }
    let json
    try {
        json = JSON.stringify(data)
    } catch (error) {}
    if (json && globalThis.localStorage) globalThis.localStorage.setItem("avatar", json)
    Access.set({ avatar: data })
    return data
}

/**
 * Internal handler for completing authentication after WebAuthn credential verification.
 * Generates SEA key pair from credential hash and updates Access state.
 * @param {Object} credential - WebAuthn credential from create() or authenticate()
 * @returns {Object} Credential object with authentication state updated
 */
async function next(credential) {
    if (!credential || !credential?.id || !credential?.seed) return { error: "Invalid credential" }
    await initZEN()
    const pair = await zen.pair(null, { seed: credential.seed })
    Access.set({
        authenticated: true,
        id: credential.id,
        seed: credential.seed,
        credential,
        pair,
        wallet: getWallet(), // Get the wallet ID from the local storage
        avatar: getAvatar(), // Get the avatar ID from the local storage
    })
    if (DEV && globalThis.localStorage) {
        const idBytes = typeof credential.id === "string" ? null : new Uint8Array(credential.id instanceof ArrayBuffer ? credential.id : credential.id.buffer)
        globalThis.localStorage.setItem(
            DEV_SESSION_KEY,
            JSON.stringify({
                seed: uint8ToB64(credential.seed),
                id: idBytes ? uint8ToB64(idBytes) : credential.id,
                idIsString: typeof credential.id === "string",
                pub: credential.pub || null
            })
        )
    }
    return credential
}

/**
 * Encrypt and save the passkey public key to Zen database.
 * Stores encrypted public key under user's key pair pub.
 * Called after successful signup to save the passkey pub for later recovery.
 * @param {Object} credential - WebAuthn credential containing pub
 * @returns {Object} Encrypted public key data
 */
async function save(credential) {
    await initZEN()
    const pair = Access.get("pair")
    if (!pair) return { error: "No pair found" }
    const encrypted = await zen.encrypt(credential.pub, pair)
    zen.get("~" + pair.pub).get("@").put(encrypted, null, { authenticator: pair })
    // Register pub in the ~ shard network so it can be discovered by prefix traversal.
    // Build soul = "~/" + all-but-last chunks, key = last chunk, value = link to ~pub.
    const chunks = pair.pub.match(/.{1,2}/g) || []
    const key = chunks.pop()
    const soul = chunks.length ? "~/" + chunks.join("/") : "~"
    zen.get(soul).get(key).put({ "#": "~" + pair.pub }, null, { authenticator: pair })
    return encrypted
}

/**
 * Restore the encrypted passkey public key from Zen database.
 * Decrypts and loads the previously saved public key into Access state.
 * Called after signin to recover the passkey pub.
 * @returns {Object} Decrypted public key or error
 */
async function restore() {
    await initZEN()
    const pair = Access.get("pair")
    if (!pair) return { error: "No pair found" }
    const encrypted = await zen.get("~" + pair.pub).get("@")
    if (!encrypted) return { error: "No encrypted public key found" }
    const decrypted = await zen.decrypt(encrypted, pair)
    if (!decrypted) return { error: "Unable to decrypt data" }
    Access.set({ pub: decrypted })
    return decrypted
}

/**
 * Sign up a new user with WebAuthn passkey.
 * Creates a new passkey credential, authenticates, and saves the public key.
 * @param {Object} data - WebAuthn credential creation options
 * @returns {Promise<Object>} Credential object or error
 */
export function signup(data) {
    return WebAuthn
        .create(data)
        .then(WebAuthn.authenticate)
        .then(next)
        .then(async (credential) => {
            if (!Access.get("authenticated")) return { error: "Unauthenticated" }
            if (!credential?.pub) return { error: "No public key found" }
            save(credential)
            return credential
        })
}

export async function wave({ seed, id } = {}) {
    if (!seed) return { error: "No seed received from wave channel" }
    const credential = await next({
        id: id || `wave:${Date.now()}`,
        seed
    })
    if (credential?.error) return credential
    if (!Access.get("pub")) await restore()
    return credential
}

/**
 * Sign in an existing user with WebAuthn passkey.
 * Authenticates with existing passkey and restores the public key from Zen.
 * @param {Object} data - WebAuthn authentication options
 * @returns {Promise<Object>} Credential object or error
 */
export function passkey(data) {
    return WebAuthn
        .authenticate(data)
        .then(next)
        .then(async (credential) => {
            if (!Access.get("authenticated")) return { error: "Unauthenticated" }
            if (!Access.get("pub")) restore()
            return credential
        })
}

/**
 * Sign out the current user.
 * Clears all authentication state and user information from Access store.
 */
export function signout() {
    Access.set({ authenticated: false, id: null, seed: null, pub: null, pair: null, wallet: null, avatar: null })
    if (DEV && globalThis.localStorage) globalThis.localStorage.removeItem(DEV_SESSION_KEY)
}

// Called from main.js after Construct.ZEN() so that the runtime singleton is ready
export async function restoreDevSession() {
    if (!DEV || !globalThis.localStorage) return
    const stored = globalThis.localStorage.getItem(DEV_SESSION_KEY)
    if (!stored) return
    try {
        const { seed, id, idIsString, pub } = JSON.parse(stored)
        const credential = {
            seed: b64ToUint8(seed),
            id: idIsString ? id : b64ToUint8(id).buffer,
            pub: pub || undefined
        }
        const result = await next(credential)
        if (result?.error) globalThis.localStorage.removeItem(DEV_SESSION_KEY)
        else if (pub) Access.set({ pub })
        else restore()
    } catch (e) {
        globalThis.localStorage.removeItem(DEV_SESSION_KEY)
    }
}
