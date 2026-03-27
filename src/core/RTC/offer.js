export async function offer(options = {}) {
    const connection = await this.init({ initiator: true })
    const description = await connection.createOffer({ ...this.offeropts, ...options })
    await connection.setLocalDescription(description)
    await this.$wait(options.timeout)
    return this.signal()
}