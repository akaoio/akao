import template from "./template.js"
import { render } from "/core/UI.js"
import stories from "./stories/index.js"

export class SHOWCASE extends HTMLElement {
    constructor() {
        super()
        this.attachShadow({ mode: "open" })
        render(template, this.shadowRoot)
        this._stories = stories
        this._onHashChange = this._onHashChange.bind(this)
    }

    connectedCallback() {
        this._renderSidebar()
        this._renderCanvas()
        globalThis.addEventListener("hashchange", this._onHashChange)
    }

    disconnectedCallback() {
        globalThis.removeEventListener("hashchange", this._onHashChange)
    }

    _currentGroup() {
        const hash = globalThis.location?.hash?.slice(1)
        return this._stories.find((s) => s.name === decodeURIComponent(hash)) || this._stories[0]
    }

    _onHashChange() {
        const group = this._currentGroup()
        this._updateSidebarActive(group)
        this._renderCanvas(group)
    }

    _renderSidebar() {
        const sidebar = this.shadowRoot.getElementById("sidebar")
        const current = this._currentGroup()
        sidebar.innerHTML = ""

        const label = document.createElement("div")
        label.className = "nav-group"
        label.textContent = "Components"
        sidebar.appendChild(label)

        for (const group of this._stories) {
            const item = document.createElement("a")
            item.className = `nav-item${group.name === current.name ? " active" : ""}`
            item.href = `#${encodeURIComponent(group.name)}`
            item.textContent = group.name
            sidebar.appendChild(item)
        }
    }

    _updateSidebarActive(current = this._currentGroup()) {
        for (const item of this.shadowRoot.querySelectorAll(".nav-item")) item.classList.toggle("active", item.textContent === current.name)
    }

    _renderCanvas(group = this._currentGroup()) {
        const canvas = this.shadowRoot.getElementById("canvas")
        canvas.innerHTML = ""

        const title = document.createElement("h2")
        title.className = "canvas-title"
        title.textContent = group.name
        canvas.appendChild(title)

        const grid = document.createElement("div")
        grid.className = "stories-grid"

        for (const story of group.stories) {
            const card = document.createElement("div")
            card.className = `story-card${story.wide ? " wide" : ""}`

            const storyName = document.createElement("div")
            storyName.className = "story-name"
            storyName.textContent = story.name

            const preview = document.createElement("div")
            preview.className = "story-preview"
            preview.innerHTML = story.code
            if (typeof story.setup === "function") story.setup(preview)

            const codeBlock = document.createElement("pre")
            codeBlock.className = "story-code"
            codeBlock.textContent = story.code.trim()

            card.appendChild(storyName)
            card.appendChild(preview)
            card.appendChild(codeBlock)
            grid.appendChild(card)
        }

        canvas.appendChild(grid)
    }
}

customElements.define("route-showcase", SHOWCASE)

export default SHOWCASE
