import { merge } from "../Utils/data.js"
import getPool from "./V2/getPool.js"
import getRate from "./V2/getRate.js"
import swap from "./V2/swap.js"
import addLiquidity from "./V2/addLiquidity.js"
import removeLiquidity from "./V2/removeLiquidity.js"
import calculateEarning from "./V2/calculateEarning.js"

export const V2 = {
    construct: async function () {
        merge(this.configs, { architecture: "V2" })
    },
    getPool,
    getRate,
    swap,
    addLiquidity,
    removeLiquidity,
    calculateEarning
}

export default V2
