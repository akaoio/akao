import "/UI/components/camera/index.js"
import "/UI/components/qr/index.js"

export default {
    name: "Camera & QR",
    stories: [
        {
            name: "Camera",
            wide: true,
            code: `<ui-camera></ui-camera>`
        },
        {
            name: "QR Scanner",
            wide: true,
            code: `<ui-qr data-mode="scan" id="scanner"></ui-qr>
<pre id="scan-result">Waiting for scan...</pre>`,
            setup(container) {
                const scanner = container.querySelector("#scanner")
                const output = container.querySelector("#scan-result")
                if (!scanner || !output) return
                scanner.addEventListener("scan", (event) => {
                    output.textContent = event?.detail?.data || "No data"
                })
            }
        }
    ]
}