// Execute multiple SQL statements in a single transaction.
// queries: [{ sql, params }, ...]
// Returns an array of results, one entry per query.
export async function batch(queries) {
    await this.ready
    return this.$call("batch", { queries })
}
