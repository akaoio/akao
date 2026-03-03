import { write, load, dir, exist } from "../src/core/FS.js"
import { log } from "./core/logger.js"

const src = ["src", "statics"]
const dest = ["build", "statics"]

async function processABIs() {
    const abiRoot = [...src, "ABIs"]
    if (!(await exist(abiRoot))) {
        log.info("No ABIs directory found, skipping ABI build")
        return { files: 0, methods: 0 }
    }

    const files = (await dir(abiRoot)).filter((file) => file.endsWith(".json"))
    let methodCount = 0

    for (const file of files) {
        const name = file.replace(/\.json$/i, "")
        const content = await load([...abiRoot, file])
        if (!Array.isArray(content)) continue

        const methods = content.filter((method) => method?.name)
        for (const method of methods) {
            await write([...dest, "ABIs", name, `${method.name}.json`], method)
            methodCount++
        }
    }

    return { files: files.length, methods: methodCount }
}

async function processChains() {
    const chainsRoot = [...src, "chains"]
    if (!(await exist(chainsRoot))) {
        log.info("No chains directory found, skipping chain build")
        return { chains: 0, contracts: 0, pools: 0 }
    }

    const chainNames = await dir(chainsRoot)
    let chainCount = 0
    let contractCount = 0
    let poolCount = 0

    for (const chainName of chainNames) {
        const chainPath = [...chainsRoot, chainName]
        if (!(await isChainDirectory(chainPath))) continue

        const configsPath = [...chainPath, "configs.json"]
        const currenciesPath = [...chainPath, "currencies.json"]
        if (!(await exist(configsPath)) || !(await exist(currenciesPath))) continue

        const configs = await load(configsPath)
        const currencies = await load(currenciesPath)
        if (!configs?.id || !Array.isArray(currencies)) continue

        chainCount++
        const chainId = configs.id.toString()
        const groups = {}
        const currencyAddresses = []

        for (const currency of currencies) {
            const name = currency?.address ? currency.address : currency?.wrapped ? "native" : null
            if (!name) continue

            currencyAddresses.push(name)
            await write([...dest, "chains", chainId, "contracts", `${name}.json`], currency)
            contractCount++

            if (currency?.group) {
                groups[currency.group] = groups[currency.group] || []
                groups[currency.group].push(name)
            }
        }

        await write([...dest, "chains", chainId, "currencies.json"], currencyAddresses)
        await write([...dest, "chains", chainId, "configs.json"], configs)

        for (const [group, addresses] of Object.entries(groups)) {
            await write([...dest, "chains", chainId, `${group}.json`], addresses)
        }

        const defisRoot = [...chainPath, "defis"]
        const dexVersions = []

        if (await exist(defisRoot)) {
            const dexes = await dir(defisRoot)
            for (const dex of dexes) {
                const dexPath = [...defisRoot, dex]
                if (!(await isChainDirectory(dexPath))) continue

                const versions = await dir(dexPath)
                for (const version of versions) {
                    const versionPath = [...dexPath, version]
                    if (!(await isChainDirectory(versionPath))) continue

                    const defiConfigsPath = [...versionPath, "configs.json"]
                    if (!(await exist(defiConfigsPath))) continue

                    const defiConfigs = await load(defiConfigsPath)
                    const contracts = Array.isArray(defiConfigs?.contracts) ? defiConfigs.contracts : []

                    dexVersions.push({ dex, version })

                    const configMap = contracts
                        .filter((contract) => contract?.address && contract?.type)
                        .reduce((acc, contract) => ({ ...acc, [contract.type]: contract.address }), {})

                    await write([...dest, "chains", chainId, "defis", dex, version, "configs.json"], configMap)

                    const poolsPath = [...versionPath, "pools.json"]
                    const pools = (await exist(poolsPath)) ? await load(poolsPath) : []
                    const validPools = Array.isArray(pools) ? pools.filter((pool) => pool?.address) : []

                    await write(
                        [...dest, "chains", chainId, "defis", dex, version, "pools.json"],
                        validPools.map((pool) => pool.address)
                    )

                    for (const contract of contracts.filter((contract) => contract?.address)) {
                        await write([...dest, "chains", chainId, "contracts", `${contract.address}.json`], contract)
                        contractCount++
                    }

                    for (const pool of validPools) {
                        await write([...dest, "chains", chainId, "contracts", `${pool.address}.json`], pool)
                        contractCount++
                        poolCount++
                    }
                }
            }
        }

        await write([...dest, "chains", chainId, "defis.json"], dexVersions)
    }

    return { chains: chainCount, contracts: contractCount, pools: poolCount }
}

async function isChainDirectory(path) {
    try {
        const entries = await dir(path)
        return Array.isArray(entries)
    } catch {
        return false
    }
}

log.start("Starting crypto build...")

const abi = await processABIs()
log.ok(`Processed ${abi.files} ABI files into ${abi.methods} ABI method files`)

const chains = await processChains()
log.ok(`Processed ${chains.chains} chains, ${chains.contracts} contracts, ${chains.pools} pools`)

log.start("Crypto build completed successfully!")
