import "/UI/components/camera/index.js"
import "/UI/components/qr/index.js"

export default {
    name: "Camera & QR",
    stories: [
        {
            name: "Camera",
            wide: true,
            code: `<ui-camera></ui-camera>`,
            setup(container) {
                container.style.position = "relative"
                container.style.height = "20rem"
                container.style.padding = "0"
            }
        },
        {
            name: "QR Scanner",
            wide: true,
            code: `<ui-qr data-mode="scan" id="scanner"></ui-qr>
<pre id="scan-result">Waiting for scan...</pre>`,
            setup(container) {
                container.style.position = "relative"
                container.style.height = "20rem"
                container.style.padding = "0"
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