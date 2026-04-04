export class Logic {
    static configure(QR) {
        return QR.request({ method: "configure", params: { alsoTryWithoutScanRegion: true } })
    }

    static decode(frame, QR) {
        const transfer = frame.bitmap ? [frame.bitmap] : undefined
        return QR.decode(frame, { transfer })
    }

    static render(data, QR) {
        return QR.render(data, { border: 0 })
    }
}

export default Logic
