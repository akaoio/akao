function clone(description) {
    if (!description) return null
    if (typeof description.toJSON === "function") return description.toJSON()
    return {
        type: description.type,
        sdp: description.sdp
    }
}

export function signal() {
    return {
        description: clone(this.connection?.localDescription),
        candidates: [...this.candidates]
    }
}