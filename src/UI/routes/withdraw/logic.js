export class Logic {
    static currency(wallet, name) {
        if (!wallet?.chain?.currencies || !name) return null
        return Object.values(wallet.chain.currencies).find(c => c.name === name) || null
    }

    static async gas({ wallet, name, to, amount }) {
        if (!wallet || !name || !to || !amount) return null
        const currency = Logic.currency(wallet, name)
        if (!currency) return null
        try {
            return await wallet.fee({ to, amount: Number(amount), currency })
        } catch (_) { return null }
    }

    static async send({ wallet, name, to, amount }) {
        if (!wallet || !name || !to || !amount) return { success: false, error: "missingRequiredFields" }
        const currency = Logic.currency(wallet, name)
        if (!currency) return { success: false, error: "missingRequiredFields" }
        try {
            const tx = await wallet.send({ to, amount: Number(amount), currency })
            if (tx && !tx.error) return { success: true, error: null }
            return { success: false, error: "transactionError" }
        } catch (e) {
            return { success: false, error: e?.message || "transactionError" }
        }
    }

}

export default Logic
