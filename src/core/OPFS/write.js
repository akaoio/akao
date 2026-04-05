// Write data to a file, creating it (and any parent directories) if needed.
// data can be string, ArrayBuffer, TypedArray, Blob, or any WritableStream-compatible value.
//
// Per-path write queue (this._locks) prevents NoModificationAllowedError on concurrent writes.
// _locks lives on the instance (not module level) so multiple OPFS instances stay independent.
// Cleanup via .finally() + identity guard ensures the Map never grows unbounded.
export async function write(path, data) {
    const key = this._path(path).join("/")
    const prev = this._locks.get(key) ?? Promise.resolve()

    const task = prev.then(async () => {
        const handle = await this.$handle(this._path(path), true)
        const writable = await handle.createWritable()
        await writable.write(data)
        await writable.close()
    })

    // Store a silenced version so a failed write doesn't poison the next write in the queue
    const queued = task.catch(() => {})
    this._locks.set(key, queued)

    // Clean up the Map entry once this write is the last one in the chain
    queued.finally(() => {
        if (this._locks.get(key) === queued) this._locks.delete(key)
    })

    // Return the original task so the caller can catch errors
    return task
}
