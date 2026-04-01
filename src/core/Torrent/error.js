export class TorrentError extends Error {
    constructor(code, message, options) {
        super(message, options)
        this.name = "TorrentError"
        this.code = code
    }
}
