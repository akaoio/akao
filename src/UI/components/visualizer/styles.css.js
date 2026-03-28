import { css } from "/core/UI.js"

export const styles = css`
	:host {
		display: block;
		position: relative;
		width: 100%;
		aspect-ratio: 1;

		canvas {
			display: block;
			width: 100%;
			height: 100%;
		}

		.probe {
			position: absolute;
			width: 0;
			height: 0;
			visibility: hidden;
			color: var(--color);
			background-color: var(--color-accent);
		}
	}
`

export default styles