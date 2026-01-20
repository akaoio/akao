export class Geo {
    static path(id) {
        return String(id).match(/.{1,2}/g) || []
    }
}

export default Geo