import createTorrent from "create-torrent"
import bencode from "bencode"
import { hash } from "uint8-util"

export async function hashInfo(data, name, opts = {}) {
    const buf = await new Promise((resolve, reject) =>
        createTorrent(data, { name, pieceLength: opts.pieceLength },
            (err, b) => err ? reject(err) : resolve(b))
    )
    const { info } = bencode.decode(buf)
    return hash(bencode.encode(info), "hex")
}
