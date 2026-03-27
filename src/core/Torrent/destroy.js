export async function destroy(options = {}) {
    if (!this.client && !this.ready) return true
    const client = await this.$client()

    return new Promise((resolve, reject) => {
        client.destroy(options, (error) => {
            if (error) {
                reject(error instanceof Error ? error : new Error(String(error)))
                return
            }

            this.torrents.clear()
            this.client = null
            this.ready = null
            resolve(true)
        })
    })
}