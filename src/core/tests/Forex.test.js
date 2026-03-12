import Test from "../Test.js"
import { Forex } from "../Forex.js"

/**
 * For Forex unit tests we create a fresh instance, wait for `ready`,
 * then REPLACE `this.rates` and `this.fiats` in-memory so tests are
 * deterministic and never touch the network.
 */
function makeForex(rates, fiats) {
    const f = new Forex()
    f.fiats = fiats
    f.rates = rates
    return f
}

const FIATS = [
    { code: "USD" },
    { code: "EUR" },
    { code: "GBP" },
    { code: "JPY" }
]

Test.describe("Forex — filter()", () => {

    Test.it("removes currencies not in fiats list", async () => {
        const f = makeForex({}, FIATS)
        const data = { USD: 1, XYZ: 1, EUR: 0.9 }
        const result = f.filter(data)
        Test.assert.falsy("XYZ" in result)
        Test.assert.truthy("USD" in result)
        Test.assert.truthy("EUR" in result)
    })

    Test.it("converts string values to numbers", async () => {
        const f = makeForex({}, FIATS)
        const data = { USD: "1.5", EUR: "0.85" }
        const result = f.filter(data)
        Test.assert.equal(typeof result.USD, "number")
        Test.assert.equal(result.USD, 1.5)
    })

    Test.it("recursively filters nested objects", async () => {
        const f = makeForex({}, FIATS)
        const data = { USD: { EUR: "0.9", XYZ: 99 } }
        const result = f.filter(data)
        Test.assert.falsy("XYZ" in result.USD)
        Test.assert.equal(result.USD.EUR, 0.9)
    })

})

Test.describe("Forex — process()", () => {

    Test.it("sets self-conversion rate to 1", async () => {
        const rates = { USD: { EUR: undefined, USD: undefined }, EUR: { USD: undefined, EUR: undefined } }
        const f = makeForex(rates, FIATS)
        f.process({})
        Test.assert.equal(f.rates.USD.USD, 1)
        Test.assert.equal(f.rates.EUR.EUR, 1)
    })

    Test.it("stores direct rate and calculates inverse", async () => {
        const rates = { USD: { EUR: undefined, USD: 1 }, EUR: { USD: undefined, EUR: 1 } }
        const f = makeForex(rates, FIATS)
        f.process({ USD: { EUR: 0.9 } })
        Test.assert.equal(f.rates.USD.EUR, 0.9)
        // Inverse: EUR→USD ≈ 1/0.9
        Test.assert.truthy(Math.abs(f.rates.EUR.USD - (1 / 0.9)) < 0.0001)
    })

    Test.it("does not lose existing rates when processing new data", async () => {
        const rates = { USD: { EUR: 0.9, USD: 1 }, EUR: { USD: 1.11, EUR: 1 } }
        const f = makeForex(rates, FIATS)
        f.process({ USD: { GBP: 0.8 } })
        Test.assert.truthy(f.rates.USD.EUR !== undefined)
    })

})

Test.describe("Forex — convert()", () => {

    Test.it("self-conversion returns same amount", async () => {
        const f = makeForex(
            { USD: { USD: 1, EUR: 0.9 }, EUR: { USD: 1.11, EUR: 1 } },
            FIATS
        )
        // Bypass async ready
        f.ready = Promise.resolve()
        const result = await f.convert(100, "USD", "USD")
        Test.assert.equal(result, 100)
    })

    Test.it("converts using known rate", async () => {
        const f = makeForex(
            { USD: { USD: 1, EUR: 0.9 }, EUR: { USD: 1.11, EUR: 1 } },
            FIATS
        )
        f.ready = Promise.resolve()
        const result = await f.convert(100, "USD", "EUR")
        Test.assert.equal(result, 90)
    })

    Test.it("returns original amount when rate is unavailable", async () => {
        const f = makeForex(
            { USD: { USD: 1 }, EUR: { EUR: 1 } },
            FIATS
        )
        // After ready resolves, forcibly reset rates so no USD→EUR route exists
        // and stub update() to maintain our isolated rates
        const isolatedRates = { USD: { USD: 1 }, EUR: { EUR: 1 } }
        f.ready = f.ready.then(() => { f.rates = isolatedRates })
        f.update = async () => { f.rates = isolatedRates }
        const result = await f.convert(50, "USD", "EUR")
        // No USD→EUR rate found → returns original amount
        Test.assert.equal(result, 50)
    })

})
