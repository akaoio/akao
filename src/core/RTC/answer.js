export async function answer(signal = {}, options = {}) {
    if (!signal?.description) throw new Error("RTC answer requires a remote offer description")

    const connection = await this.init({ initiator: false })
    await connection.setRemoteDescription(signal.description)

    for (const candidate of signal?.candidates || [])
        if (candidate) await connection.addIceCandidate(candidate)

    const description = await connection.createAnswer(options)
    await connection.setLocalDescription(description)
    await this.$wait(options.timeout)
    return this.signal()
}