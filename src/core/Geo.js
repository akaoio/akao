export class Geo {
    static path(id) {
        const str = String(id)
        const segments = []
        for (let i = str.length; i > 0; i -= 2) {
            segments.unshift(str.slice(Math.max(0, i - 2), i))
        }
        if (segments.length && segments[0].length === 1) segments[0] = "0" + segments[0]
        return segments
    }
}

export default Geo