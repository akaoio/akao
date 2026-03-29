import { css } from "/core/UI.js"

export const styles = css`
	:host {
		display: flex;
		flex-direction: column;
		gap: var(--space);

		#status {
			white-space: pre-wrap;
			overflow-wrap: anywhere;
			text-align: center;
		}

		ui-visualizer {
			aspect-ratio: 1/1;
			width: min(100%, 10rem);
			align-self: center;
		}
	}
`

export default styles