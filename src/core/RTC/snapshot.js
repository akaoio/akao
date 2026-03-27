function clone(description) {
    if (!description) return null
    if (typeof description.toJSON === "function") return description.toJSON()
    return {
        type: description.type,
        sdp: description.sdp
    }
}

export function snapshot() {
    return {
        channel: this.channel,
        connectionState: this.connection?.connectionState || "idle",
        iceConnectionState: this.connection?.iceConnectionState || "idle",
        iceGatheringState: this.connection?.iceGatheringState || "idle",
        signalingState: this.connection?.signalingState || "idle",
        dataState: this.data?.readyState || "closed",
        localDescription: clone(this.connection?.localDescription),
        remoteDescription: clone(this.connection?.remoteDescription),
        candidates: [...this.candidates]
    }
}