import { merge } from "../Utils/data.js"
import pool from "./V3/pool.js"
import rate from "./V3/rate.js"
import swap from "./V3/swap.js"
import quote from "./V3/quote.js"
import add from "./V3/add.js"
import remove from "./V3/remove.js"
import earning from "./V3/earning.js"

export const V3 = {
    construct: async function () {
        merge(this.configs, { architecture: "V3" })
    },
    pool,
    rate,
    swap,
    quote,
    add,
    remove,
    earning
}

export default V3
