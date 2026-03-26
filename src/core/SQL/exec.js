// Execute any SQL statement. Returns result rows for SELECT, empty array for DML.
export async function exec(sql, params) {
    await this.ready
    return this.$call("exec", { sql, params })
}
