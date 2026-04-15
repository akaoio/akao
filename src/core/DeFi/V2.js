import { merge } from "../Utils/data.js"
import pool from "./V2/pool.js"
import rate from "./V2/rate.js"
import swap from "./V2/swap.js"
import quote from "./V2/quote.js"
import add from "./V2/add.js"
import remove from "./V2/remove.js"
import earning from "./V2/earning.js"

export const V2 = {
    construct: async function () {
        merge(this.configs, { architecture: "V2" })
    },
    pool,
    rate,
    swap,
    quote,
    add,
    remove,
    earning
}

export default V2
