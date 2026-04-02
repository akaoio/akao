import { threads } from "./Threads.js"
import { reset } from "./Utils/reset.js"
globalThis.reset = reset
// Register threads
threads.register("main", { main: true, type: "module" })
threads.register("update", { worker: true, type: "module" })
threads.register("discovery", { worker: true, type: "module" })
threads.register("wave", { worker: true, type: "module" })
threads.register("qr", { worker: true, type: "module" })
threads.register("sql", { worker: true, type: "module" })
threads.register("onchain", { worker: true, type: "module" })
threads.register("offchain", { worker: true, type: "module" })