import Thread from "/core/Thread.js"
import { Wave } from "/core/Wave.js"

const thread = new Thread()

let wave = null

thread.init = async function () {
    if (!wave) {
        wave = new Wave()
        await wave.init()
    }
}