export async function accept(signal = {}) {
    const connection = await this.init({ initiator: false })

    if (signal?.description) await connection.setRemoteDescription(signal.description)

    const candidates = signal?.candidate ? [signal.candidate] : signal?.candidates || []
    for (const candidate of candidates)
        if (candidate) await connection.addIceCandidate(candidate)

    return this.snapshot()
}