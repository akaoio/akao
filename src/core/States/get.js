/**
 * Retrieve state values.
 * @param {string|string[]|Object} data - Property name(s) to retrieve
 * @returns {*} State value(s) - returns value, array of values, or mapped object
 */
export function get(data) {
    if (!data) return
    if (typeof data === "string") return this.proxy[data]
    if (Array.isArray(data)) return data.reduce((acc, key) => (acc === undefined || acc === null ? undefined : acc[key]), this.proxy)
    // Map object values to their state counterparts
    return Object.entries(data).reduce((acc, [k, v]) => ({ ...acc, [k]: v ? this.proxy[v] : this.proxy[k] }), {})
}
