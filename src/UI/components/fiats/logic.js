export class Logic {
    static options(fiats) {
        return fiats.map((fiat) => ({ value: fiat.code, label: `${fiat.name} (${fiat.symbol})` }))
    }
}

export default Logic
