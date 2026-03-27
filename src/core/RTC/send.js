export function send(data) {
    if (!this.data || this.data.readyState !== "open") throw new Error("RTC data channel is not open")
    this.data.send(data)
    return true
}