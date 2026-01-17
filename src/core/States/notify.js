/**
 * Notify all subscribers of a state change.
 * Calls global subscribers and path-specific subscribers.
 * @param {string} key - The state property that changed
 * @param {*} value - The new value
 * @param {*} last - The previous value
 * @param {Object} target - The state object
 * @param {Object} receiver - The proxy receiver
 */
export function notify(data = {}) {
    const { key, value } = data
    // Notify all global subscribers
    this.SET.forEach((sub) => typeof sub === "function" && sub(data))

    // Notify path-specific subscribers
    for (const [path, subs] of this.MAP) {
        if (!subs.size) continue
        // Check if this path matches the changed key or is a nested path starting with key
        if (path === key || (Array.isArray(path) && path[0] === key)) {
            // Extract nested value if path is an array
            const val = Array.isArray(path) ? path.slice(1).reduce((acc, k) => acc && acc[k], value) : value
            if (val === undefined) continue
            subs.forEach((sub) => {
                // Support function callbacks
                if (typeof sub === "function") sub({ ...data, key: Array.isArray(path) ? path.at(-1) : key, value: val })
                // Support direct property assignment [object, propertyName, ...nestedProps]
                else if (Array.isArray(sub) && sub.length >= 2 && sub[0]) {
                    const [target, ...props] = sub
                    let current = target
                    for (let i = 0; i < props.length - 1; i++) {
                        if (typeof current[props[i]] !== "object") current[props[i]] = {}
                        current = current[props[i]]
                    }
                    current[props[props.length - 1]] = val
                }
            })
        }
    }
}
