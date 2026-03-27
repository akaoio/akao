export function close() {
    if (this.data && this.data.readyState !== "closed") this.data.close()
    if (this.connection && this.connection.signalingState !== "closed") this.connection.close()
    this.data = null
    this.connection = null
    this.candidates = []
    this.$emit("close", this.snapshot())
    return true
}