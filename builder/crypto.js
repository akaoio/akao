import { FS } from "../src/core/FS.js"
import { log } from "./core/logger.js"

const src = ["src", "statics"]
const dest = ["build", "statics"]

async function processABIs() {
    const abiRoot = [...src, "ABIs"]
    if (!(await FS.exist(abiRoot))) {
        log.info("No ABIs directory found, skipping ABI build")
        return { files: 0, methods: 0 }
    }

    const files = (await FS.dir(abiRoot)).filter((file) => file.endsWith(".yaml"))
    let methodCount = 0

    for (const file of files) {
        const name = file.replace(/\.yaml$/i, "")
        const content = await FS.load([...abiRoot, file])
        if (!Array.isArray(content)) continue

        const methods = content.filter((method) => method?.name)
        for (const method of methods) {
            await FS.write([...dest, "ABIs", name, `${method.name}.json`], method)
            methodCount++
        }
    }

    return { files: files.length, methods: methodCount }
}

async function processChains() {
    const chainsRoot = [...src, "chains"]
    if (!(await FS.exist(chainsRoot))) {
        log.info("No chains directory found, skipping chain build")
        return { chains: 0, contracts: 0, pools: 0 }
    }

    const chainNames = await FS.dir(chainsRoot)
    let chainCount = 0
    let contractCount = 0
    let poolCount = 0

    for (const chainName of chainNames) {
        const chainPath = [...chainsRoot, chainName]
        if (!(await isChainDirectory(chainPath))) continue

        const configsPath = [...chainPath, "configs.yaml"]
        const currenciesPath = [...chainPath, "currencies.yaml"]
        if (!(await FS.exist(configsPath)) || !(await FS.exist(currenciesPath))) continue

        const configs = await FS.load(configsPath)
        const currencies = await FS.load(currenciesPath)
        if (!configs?.id || !Array.isArray(currencies)) continue

        chainCount++
        const chainId = configs.id.toString()
        const groups = {}
        const currencyAddresses = []

        for (const currency of currencies) {
            const name = currency?.address ? currency.address : currency?.wrapped ? "native" : null
            if (!name) continue

            currencyAddresses.push(name)
            await FS.write([...dest, "chains", chainId, "contracts", `${name}.json`], currency)
            contractCount++

            if (currency?.group) {
                groups[currency.group] = groups[currency.group] || []
                groups[currency.group].push(name)
            }
        }

        await FS.write([...dest, "chains", chainId, "currencies.json"], currencyAddresses)
        await FS.write([...dest, "chains", chainId, "configs.json"], configs)

        for (const [group, addresses] of Object.entries(groups)) {
            await FS.write([...dest, "chains", chainId, `${group}.json`], addresses)
        }

        const defisRoot = [...chainPath, "defis"]
        const dexVersions = []

        if (await FS.exist(defisRoot)) {
            const dexes = await FS.dir(defisRoot)
            for (const dex of dexes) {
                const dexPath = [...defisRoot, dex]
                if (!(await isChainDirectory(dexPath))) continue

                const versions = await FS.dir(dexPath)
                for (const version of versions) {
                    const versionPath = [...dexPath, version]
                    if (!(await isChainDirectory(versionPath))) continue

                    const defiConfigsPath = [...versionPath, "configs.yaml"]
                    if (!(await FS.exist(defiConfigsPath))) continue

                    const defiConfigs = await FS.load(defiConfigsPath)
                    const contracts = Array.isArray(defiConfigs?.contracts) ? defiConfigs.contracts : []

                    dexVersions.push({ dex, version })

                    const configMap = contracts
                        .filter((contract) => contract?.address && contract?.type)
                        .reduce((acc, contract) => ({ ...acc, [contract.type]: contract.address }), {})

                    await FS.write([...dest, "chains", chainId, "defis", dex, version, "configs.json"], configMap)

                    const poolsPath = [...versionPath, "pools.yaml"]
                    const pools = (await FS.exist(poolsPath)) ? await FS.load(poolsPath) : []
                    const validPools = Array.isArray(pools) ? pools.filter((pool) => pool?.address) : []

                    await FS.write(
                        [...dest, "chains", chainId, "defis", dex, version, "pools.json"],
                        validPools.map((pool) => pool.address)
                    )

                    for (const contract of contracts.filter((contract) => contract?.address)) {
                        await FS.write([...dest, "chains", chainId, "contracts", `${contract.address}.json`], contract)
                        contractCount++
                    }

                    for (const pool of validPools) {
                        await FS.write([...dest, "chains", chainId, "contracts", `${pool.address}.json`], pool)
                        contractCount++
                        poolCount++
                    }
                }
            }
        }

        await FS.write([...dest, "chains", chainId, "defis.json"], dexVersions)
    }

    return { chains: chainCount, contracts: contractCount, pools: poolCount }
}

async function isChainDirectory(path) {
    try {
        const entries = await FS.dir(path)
        return Array.isArray(entries)
    } catch {
        return false
    }
}

async function processImages() {
    const chainsRoot = [...src, "chains"]
    if (!(await FS.exist(chainsRoot))) {
        log.info("No chains directory found, skipping image build")
        return { downloaded: 0, skipped: 0, failed: 0 }
    }

    // Collect all unique symbol filenames from every currencies.yaml found
    const symbols = new Set()
    const chainNames = await FS.dir(chainsRoot)

    for (const chainName of chainNames) {
        const chainPath = [...chainsRoot, chainName]
        if (!(await isChainDirectory(chainPath))) continue

        const currenciesPath = [...chainPath, "currencies.yaml"]
        if (!(await FS.exist(currenciesPath))) continue

        const currencies = await FS.load(currenciesPath)
        if (!Array.isArray(currencies)) continue

        for (const currency of currencies) {
            if (currency?.symbol) symbols.add(currency.symbol)
        }

        const configsPath = [...chainPath, "configs.yaml"]
        if (await FS.exist(configsPath)) {
            const config = await FS.load(configsPath)
            if (config?.symbol) symbols.add(config.symbol)
        }
    }

    const dexsPath = [...src, "dexs.yaml"]
    if (await FS.exist(dexsPath)) {
        const dexs = await FS.load(dexsPath)
        for (const dex of Object.values(dexs || {})) {
            if (dex?.symbol) symbols.add(dex.symbol)
        }
    }

    log.info(`Found ${symbols.size} unique symbols across all chains`)

    // Ensure src/images/cryptos/ exists
    const cryptoSrcDir = ["src", "images", "cryptos"]
    await FS.ensure(FS.join(cryptoSrcDir))

    let downloaded = 0
    let skipped = 0
    let failed = 0

    for (const symbol of symbols) {
        const destPath = [...cryptoSrcDir, symbol]
        if (await FS.exist(destPath)) {
            skipped++
            continue
        }

        const url = `https://cryptologos.cc/logos/${symbol}`
        const result = await FS.download(url, destPath)

        if (result?.success) {
            downloaded++
            log.ok(`Downloaded: ${symbol}`)
        } else {
            failed++
            log.info(`Not found: ${symbol}`)
        }
    }

    // Merge src/images/cryptos/ into build/images/cryptos/ (non-destructive: adds files, never deletes)
    if (await FS.exist(cryptoSrcDir)) {
        await FS.copy(cryptoSrcDir, ["build", "images", "cryptos"])
    }

    return { downloaded, skipped, failed }
}

log.start("Starting crypto build...")

const images = await processImages()
log.ok(`Images: ${images.downloaded} downloaded, ${images.skipped} already exist, ${images.failed} not found`)

const abi = await processABIs()
log.ok(`Processed ${abi.files} ABI files into ${abi.methods} ABI method files`)

const chains = await processChains()
log.ok(`Processed ${chains.chains} chains, ${chains.contracts} contracts, ${chains.pools} pools`)

log.start("Crypto build completed successfully!")
