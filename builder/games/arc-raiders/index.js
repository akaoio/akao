import { log } from "../../core/logger.js"
import { crawlArcRaidersItems } from "./crawler.js"

export const gameArcRaiders = {
    id: "arc-raiders",
    async build(context) {
        const summary = await crawlArcRaidersItems({
            output: context.output,
            dryRun: context.dryRun,
        })

        log.ok(
            `Arc Raiders: ${summary.totalItems} items, ${summary.pagesFetched} pages, ${summary.downloadedImages} new images`,
        )
    },
}