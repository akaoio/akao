// Execute a SELECT and return all matching rows as an array.
export async function all(sql, params) {
    await this.ready
    return this.$call("all", { sql, params })
}
