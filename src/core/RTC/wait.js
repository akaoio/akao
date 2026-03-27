export async function $wait(timeout = this.timeout) {
    if (!this.connection) throw new Error("RTC connection is not initialized")
    if (this.connection.iceGatheringState === "complete") return [...this.candidates]

    return new Promise((resolve, reject) => {
        let settled = false

        const cleanup = () => {
            clearTimeout(timer)
            this.connection?.removeEventListener("icegatheringstatechange", onChange)
        }

        const finish = () => {
            if (settled) return
            settled = true
            cleanup()
            resolve([...this.candidates])
        }

        const onChange = () => {
            if (this.connection?.iceGatheringState === "complete") finish()
        }

        const timer = setTimeout(() => {
            if (settled) return
            settled = true
            cleanup()
            reject(new Error("RTC ICE gathering timeout"))
        }, timeout)

        this.connection.addEventListener("icegatheringstatechange", onChange)
    })
}