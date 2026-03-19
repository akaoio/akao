import { log } from "../../core/logger.js"
import { crawlDiablo4Items } from "./crawler.js"

export const gameDiablo4 = {
    id: "diablo-4",
    async build(context) {
        const summary = await crawlDiablo4Items({
            output: context.output,
            dryRun: context.dryRun,
        })

        log.info(summary.message)
    },
}
