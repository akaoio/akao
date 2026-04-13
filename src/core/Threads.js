/**
 * Thread manager for coordinating multiple threads (main thread, worker threads).
 * Handles thread registration, message routing, queue management, and global state updates.
 * Provides API for calling methods on threads and receiving responses via callbacks.
 */

import { NODE, randomKey, diff, merge } from "./Utils.js"
import { FS } from "./FS.js"
import { Lives } from "./Stores.js"
import { events } from "./Events.js"

export class Threads {
    // Map of registered threads by name
    threads = {}

    // Use object so it's easier to find queue in case we have callbacks
    // Maps queue IDs to callback functions for async responses
    queues = {}

    /**
     * Register a new thread (main or worker).
     * Creates Web Workers for browser or worker_threads for Node.js.
     * @param {string} name - Unique identifier for the thread
     * @param {Object} configs - Configuration object { main: boolean, worker: boolean }
     * @returns {Promise} Thread object or module
     */
    async register(name, configs = {}) {
        // If thread already exists, return it
        if (this.threads[name]) return this.threads[name]

        const url = configs?.url || new URL(configs?.path || `./threads/${name}.js`, import.meta.url)
        // If main thread, import the module directly
        if (configs?.main) this.threads[name] = import(url.href)
        // If worker thread, create a new Worker
        else if (configs?.worker) {
            // Get Worker class (Web Worker for browser, worker_threads.Worker for Node.js)
            let $Worker = typeof Worker !== "undefined" ? Worker : NODE && typeof Worker === "undefined" ? (await import("worker_threads"))?.Worker : undefined
            if (typeof $Worker === "undefined") throw new Error("Worker class not found")
            this.threads[name] = new $Worker(url, configs)

            // Set up error and message handlers for the worker
            if (NODE) {
                // Node.js: Handle worker errors
                this.threads[name].on("error", (error) => console.error(`Worker ${name} error:`, error))
                // Node.js: Handle worker exit
                this.threads[name].on("exit", (code) => {
                    if (code !== 0) console.error(`Worker ${name} stopped with exit code ${code}`)
                })
                // Node.js: Handle incoming messages from worker
                this.threads[name].on("message", (data) => this.process(data, name))
            } else {
                // Browser: Handle worker errors
                this.threads[name].onerror = (error) => console.error(`Worker ${name} error:`, error)
                // Browser: Handle incoming messages from worker
                this.threads[name].onmessage = (event) => this.process(event?.data, name)
            }
        }
        console.log(`Thread registered: ${name}`)
        return this.threads[name]
    }

    post(name, data, transfer = []) {
        if (!name || !data || !this.threads?.[name]) return false
        if (Array.isArray(transfer) && transfer.length) this.threads[name].postMessage(data, transfer)
        else this.threads[name].postMessage(data)
        return true
    }

    relay({ source, thread, method, params, queue, transfer } = {}) {
        if (!thread || !method) return

        if (!this.threads?.[thread]) {
            const error = { message: `Thread not found: ${thread}` }
            console.error(error.message)
            if (!queue) return
            if (source && this.threads?.[source]) this.post(source, { queue, error })
            else if (typeof this.queues?.[queue] === "function") {
                this.queues[queue](undefined, error)
                delete this.queues[queue]
            }
            return
        }

        if (queue && source) this.queues[queue] = { thread: source }
        this.post(thread, { queue, method, params, source }, transfer)
    }

    /**
     * Process incoming responses from worker threads.
     * Routes messages to appropriate handlers based on message type.
     * @param {Object} data - Message data from worker (contains queue, response)
     */
    process(data, source) {
        if (typeof data !== "object") return
        if (data?.relay) return this.relay({ source, ...data.relay })
        if (data?.Lives) {
            const diff_data = diff(Lives, data.Lives)
            merge(Lives, diff_data)
            for (const key in diff_data) events.emit("Lives." + key, diff_data[key])
            return
        }
        // data is an object that contains { queue, response }

        const queue = data?.queue
        // queue and response are used for managing queues (responses to method calls)
        if (!queue || !this.queues?.[queue]) return
        // Find the callback for this queue and invoke it
        if (typeof this.queues[queue] == "function") this.queues[queue](data?.response, data?.error)
        else if (this.queues[queue]?.thread) this.post(this.queues[queue].thread, { queue, response: data?.response, error: data?.error, source })
        // Delete task from the queues list
        delete this.queues[queue]
    }

    /**
     * Queue a method call on a thread with a callback for the response.
     * Creates a unique queue ID and stores the callback for when response arrives.
     * @param {Object} options - { thread, method, params, callback }
     */
    queue({ thread, method, params, callback, transfer }) {
        if (!thread || !this.threads?.[thread]) return
        const queue = randomKey()
        if (this.queues?.[queue]) return this.queues[queue]
        // Store callback to be invoked when response arrives
        if (typeof callback == "function") this.queues[queue] = callback
        // Send message to thread with queue ID for correlation
        this.post(thread, { queue, method, params }, transfer)
        return queue
    }

    /**
     * Call a method on a thread without waiting for a response (fire and forget).
     * Used for one-way messages or updates that don't require callbacks.
     * @param {Object} options - { thread, method, params }
     */
    call({ thread, method, params, transfer }) {
        if (!thread || !method || !this.threads?.[thread]) return
        // Send message without queue ID (no response expected)
        this.post(thread, { method, params }, transfer)
    }
}

export default Threads

// Create or reuse global Threads singleton for app-wide thread management
globalThis.threads = globalThis.threads || new Threads()

// Export the global threads instance for convenient access throughout the app
export const threads = globalThis.threads
