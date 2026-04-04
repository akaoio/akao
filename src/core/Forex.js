/*
Foreign exchange rates management module
This module handles fetching, caching, and updating foreign exchange rates
from various APIs. It maintains a structured object to store exchange rates
between supported fiat currencies, provides data cleaning functions, and
integrates with external APIs to keep rates up-to-date.
*/

import DB from "./DB.js"
import { NODE, BROWSER, load } from "./Utils.js"
import { Statics } from "./Stores.js"

export class Forex {
    constructor() {
        this.rates = {}
        this.init = this.init.bind(this)
        this.filter = this.filter.bind(this)
        this.fetch = this.fetch.bind(this)
        this.process = this.process.bind(this)
        this.update = this.update.bind(this)
        this.convert = this.convert.bind(this)
        // Store init promise for awaiting
        this.ready = this.init()
    }

    async init() {
        this.fiats = Statics?.fiats || (NODE ? await load(["src", "statics", "fiats.yaml"]) : BROWSER ? await DB.get(["statics", "fiats.json"]) : undefined)
        if (NODE) this.rates = await load(["src", "statics", "forex.yaml"]) || {}
        if (BROWSER) this.rates = (await DB.get(["statics", "forex.json"])) || (await this.update())
        // Initialize rate structure for all supported fiat currencies
        // Creates a matrix where each currency has conversion rates to all others
        // Default values:
        // - Same currency pairs = 1 (e.g. USD->USD = 1)
        // - Other pairs = undefined until populated by API data
        for (const fiat of this.fiats) if (!this.rates?.[fiat.code]) this.rates[fiat.code] = Object.fromEntries(this.fiats.map((e) => [e.code, e.code == fiat.code ? 1 : undefined]))
    }

    // Cleans and validates rate data from APIs
    filter(data) {
        if (typeof data === "object") 
            for (const [code, value] of Object.entries(data)) {
                // Remove currencies not in our supported fiats list
                const fiat = this.fiats.find((e) => e.code == code)
                if (!fiat) delete data[code]
                if (!value || !fiat) continue
                // Recursively clean nested objects
                if (typeof value === "object") data[code] = this.filter(value)
                // Convert string values to numbers
                if (typeof value === "string") data[code] = Number(value)
            }
        
        return data
    }

    // Fetches rates from Exchange Rates API
    async fetch(url, key, callback) {
        // Fetches USD-based rates for all currencies
        let data = await fetch(url)
            .then((res) => res.json())
            .catch((error) => console.error(error))
        // Extract and clean rates if available
        data = key && data?.[key] ? this.filter(data[key]) : this.filter(data)
        if (typeof data === "object") return callback && typeof callback === "function" ? callback(data) : data
    }

    // ====================== RATE PROCESSING ======================
    // Merges and calculates cross-rates between currencies
    process(data = {}) {
        for (const base in this.rates) {
            if (!this.rates[base]) this.rates[base] = {}
            // Set self-conversion rate to 1
            this.rates[base][base] = 1
            for (const quote in this.rates[base]) {
                if (!this.rates[quote]) this.rates[quote] = {}
                this.rates[quote][quote] = 1
                // If direct rate exists in new data, update both directions
                if (data?.[base]?.[quote]) {
                    this.rates[base][quote] = data[base][quote]
                    this.rates[quote][base] = 1 / data[base][quote]
                }
                // Calculate missing rates through intermediary currency if possible
                const bridge = Object.values(this.rates).find((bridge) => bridge[base] && bridge[quote])
                if (bridge) this.rates[base][quote] = bridge[quote] / bridge[base]
            }
        }
        return this.rates
    }

    async update() {
        // Skip update in browser if rates already loaded (CORS issues with APIs)
        if (BROWSER && Object.keys(this.rates).length > 0) {
            // Check if rates are properly initialized (has at least one valid conversion)
            const valid = Object.values(this.rates).some((rates) => typeof rates === "object" && Object.values(rates).some((rate) => typeof rate === "number" && rate > 0 && rate !== 1))
            if (valid) return this.rates
        }

        let data = null
        try {
            // Fetch and merge rates from multiple sources
            data = await this.fetch("https://api.coingate.com/v2/rates", "merchant")
            if (data) this.process(data)
            data = await this.fetch("https://open.er-api.com/v6/latest", "rates", ($data) => ({ USD: { ...$data } }))
            if (data) this.process(data)
            // Store updated rates in database
        } catch (error) {
            console.error("Forex update failed:", error)
        }
        return this.rates
    }

    async convert(amount, base, quote) {
        // Wait for initialization to complete
        await this.ready
        base = base || Statics?.site?.fiat
        quote = quote || globalThis.Context?.get("fiat")?.code || Statics?.site?.fiat
        if (base === quote) return amount
        // Check if rate exists, otherwise try to update
        if (!this.rates[base] || !this.rates[base][quote]) await this.update()
        const rate = this.rates[base]?.[quote]
        if (rate) return amount * rate
        return amount
    }
}

export const forex = new Forex()

export default forex
