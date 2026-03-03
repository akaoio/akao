import { merge } from "../Utils/data.js"
import getPool from "./V3/getPool.js"
import getRate from "./V3/getRate.js"
import swap from "./V3/swap.js"
import addLiquidity from "./V3/addLiquidity.js"
import removeLiquidity from "./V3/removeLiquidity.js"
import calculateEarning from "./V3/calculateEarning.js"

export const V3 = {
    construct: async function () {
        merge(this.configs, { architecture: "V3" })
    },
    getPool,
    getRate,
    swap,
    addLiquidity,
    removeLiquidity,
    calculateEarning
}

export default V3
