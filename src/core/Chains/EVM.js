import { BigNumber } from "../Utils/bignumber.js"
import { loadContract, loadABI } from "../Utils/contracts.js"
import { base64UrlToHex } from "../Utils/crypto.js"
import { merge } from "../Utils/data.js"
import { ethers } from "../Ethers.js"

export const EVM = {
    construct: async function () {
        merge(this.configs, { architecture: "EVM" })
        // Configure timeouts
        const timeout = 10000 // 10 seconds
        const retries = 3

        // Try to get latest blocks with retries
        if (this?.https) {
            for (let attempt = 1; attempt <= retries; attempt++) {
                try {
                    const latestBlock = await Promise.race([this.https.getBlock("latest"), new Promise((_, reject) => setTimeout(() => reject(new Error("Timeout")), timeout))])
                    this.watch(latestBlock)

                    const previousBlock = await Promise.race([this.https.getBlock(Number(latestBlock.number) - 1), new Promise((_, reject) => setTimeout(() => reject(new Error("Timeout")), timeout))])
                    this.watch(previousBlock)
                    break // Success, exit retry loop
                } catch (error) {
                    if (attempt === retries) {
                        console.error("Failed to fetch initial blocks after retries:", error)
                    }
                }
            }
        }

        // Try to setup WebSocket subscription with retries
        if (this?.wss) {
            for (let attempt = 1; attempt <= retries; attempt++) {
                try {
                    this.wss.on("block", (block) => this.watch(block))
                    this.wss.on("error", (error) => {
                        console.error("WebSocket subscription error:", error)
                        this.reconnectwss()
                    })
                    break // Success, exit retry loop
                } catch (error) {
                    if (attempt === retries) {
                        console.error("Failed to establish WebSocket connection after retries:", error)
                    }
                }
            }
        }
    },
    _Contract: function ({ address, ABI }) {
        return new this.connector.Contract(address, ABI, this.https)
    },
    connector: ethers,
    connect: function (RPC) {
        if (!RPC) throw new Error("Invalid RPC URL")
        const network = {
            chainId: parseInt(this.id),
            name: `chain-${this.id}`
        }
        if (RPC.startsWith("ws")) {
            return new this.connector.WebSocketProvider(RPC, network)
        }
        return new this.connector.JsonRpcProvider(RPC, network)
    },
    private: function (seed) {
        const hex = base64UrlToHex(seed)
        return "0x" + hex
    },
    public: function (key) {
        const wallet = new this.connector.Wallet(key)
        return wallet.address
    },
    balance: async function ({ address, currency }) {
        let balance = 0
        try {
            if (!currency) currency = this.currencies.native
            let decimals = currency.decimals
            // For native currencies
            if (!currency.address && currency.wrapped) {
                balance = await this.https.getBalance(address)
                decimals = decimals || 18
            }
            // For non-native currencies
            else {
                if (typeof currency !== "object") return
                // Get token contract
                const token = { configs: await loadContract({ chain: this.id, address: currency.address }) }
                if (!token.configs) throw new Error("Token contract not found")

                // Load ABI methods and create contract
                token.ABI = await loadABI({ ABI: token.configs.ABI, methods: ["balanceOf", "decimals"] })
                if (!token.ABI) throw new Error("Token ABI not found")
                token.contract = new ethers.Contract(token.configs.address, token.ABI, this.https)

                // Get balance
                balance = await token.contract.balanceOf(address)

                // Get decimals if not provided
                if (decimals === undefined) {
                    if (token.configs?.decimals) {
                        decimals = token.configs.decimals
                    } else {
                        decimals = await token.contract.decimals()
                    }
                }
            }
            balance = new BigNumber(balance.toString()).dividedBy(new BigNumber(10).pow(decimals)).toNumber()
            return balance
        } catch (error) {
            console.error(error)
        }
    },
    gasPrice: async function () {
        const feeData = await this.https.getFeeData()
        return feeData.gasPrice
    },
    gas: async function ({ from, to, amount, currency }) {
        try {
            let estimateContent = {}
            if (!currency) currency = this.currencies.native
            let decimals = currency.decimals
            // For native currencies
            if (!currency.address && currency.wrapped) {
                decimals = decimals || 18
                estimateContent = {
                    from,
                    to,
                    value: amount
                }
            }
            // For non-native currencies
            else {
                // Get token contract
                const token = { configs: await loadContract({ chain: this.id, address: currency.address }) }
                if (!token.configs) throw new Error("Token contract not found")

                // Load ABI methods and create contract
                token.ABI = await loadABI({ ABI: token.configs.ABI, methods: ["transfer", "decimals"] })
                if (!token.ABI) throw new Error("Token ABI not found")
                token.contract = new ethers.Contract(token.configs.address, token.ABI, this.https)

                // Get decimals if not provided
                if (decimals === undefined) {
                    if (token.configs?.decimals) {
                        decimals = token.configs.decimals
                    } else {
                        decimals = await token.contract.decimals()
                    }
                }

                const data = token.contract.interface.encodeFunctionData("transfer", [to, amount])
                estimateContent = {
                    from,
                    to: token.configs.address,
                    data: data
                }
            }

            // Convert amount to chain units
            amount = new BigNumber(amount).multipliedBy(new BigNumber(10).pow(decimals)).toString(10)
            if (estimateContent.value !== undefined) {
                estimateContent.value = amount
            }

            return await this.https.estimateGas(estimateContent)
        } catch (error) {
            console.error("Gas estimation error:", error)
            // Return a safe fallback in case estimation fails
            return 60000
        }
    },
    send: async function ({ from, to, amount, currency, gas }) {
        try {
            if (!currency) currency = this.currencies.native

            // Estimate gas if not provided
            gas = gas || (await this.gas({ from, to, amount, currency }))

            // Get nonce to avoid duplicate transactions
            const nonce = await this.https.getTransactionCount(from, "latest")
            const gasPrice = await this.gasPrice()

            // Create content for transaction
            let content = {
                gasLimit: gas,
                gasPrice: gasPrice,
                nonce: nonce,
                chainId: this.id
            }

            let decimals = currency.decimals
            // For native currencies
            if (!currency.address && currency.wrapped) {
                decimals = decimals || 18
                content = {
                    to,
                    value: amount,
                    ...content
                }
            }
            // For non-native currencies
            else {
                // Get token contract
                const token = { configs: await loadContract({ chain: this.id, address: currency.address }) }
                if (!token.configs) throw new Error("Token contract not found")

                // Load ABI methods and create contract
                token.ABI = await loadABI({ ABI: token.configs.ABI, methods: ["transfer", "decimals"] })
                if (!token.ABI) throw new Error("Token ABI not found")
                token.contract = new ethers.Contract(token.configs.address, token.ABI, this.https)

                // Get decimals if not provided
                if (decimals === undefined) {
                    if (token.configs?.decimals) {
                        decimals = token.configs.decimals
                    } else {
                        decimals = await token.contract.decimals()
                    }
                }

                const data = token.contract.interface.encodeFunctionData("transfer", [to, amount])
                content = {
                    to: token.configs.address,
                    value: 0,
                    data: data,
                    ...content
                }
            }

            // Convert amount to chain units
            amount = new BigNumber(amount).multipliedBy(new BigNumber(10).pow(decimals)).toString(10)
            if (content.value !== undefined) {
                content.value = amount
            }

            // Create wallet and sign transaction
            const wallet = new ethers.Wallet(this.private, this.https)
            const transaction = await wallet.sendTransaction(content)
            const receipt = await transaction.wait()

            return receipt
        } catch (error) {
            console.error(error)
        }
    }
}

export default EVM
