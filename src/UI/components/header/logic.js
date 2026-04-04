import { events } from "/core/Events.js"

export class Logic {
    static open() {
        events.emit("game-nav:open")
    }
}

export default Logic
