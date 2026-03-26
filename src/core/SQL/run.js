// Execute a write statement (INSERT / UPDATE / DELETE).
// Returns { changes: number, lastId: number }.
export async function run(sql, params) {
    await this.ready
    return this.$call("run", { sql, params })
}
