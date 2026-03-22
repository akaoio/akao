import ggwave_factory from "./ggwave.js"

export async function init() {
    if (!this.ready) {
        this.ready = ggwave_factory().then((module) => {
            this.wave = module
            return this.wave
        })
    }
    if (!this.wave) await this.ready
    return this.wave
}