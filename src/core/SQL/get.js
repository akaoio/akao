// Execute a SELECT and return the first matching row, or null if none.
export async function get(sql, params) {
    await this.ready
    return this.$call("get", { sql, params })
}
