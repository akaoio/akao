import { css } from "/core/UI.js"

export const styles = css`
	:host {
		display: flex;
		flex-direction: column;
		gap: var(--space);

		.controls {
			display: flex;
			flex-direction: column;
			gap: calc(var(--space) / 2);
		}

		textarea {
			width: 100%;
			resize: vertical;
			box-sizing: border-box;
			padding: 0.5rem;
		}

		.actions {
			display: flex;
			flex-wrap: wrap;
			gap: calc(var(--space) / 2);
		}

		button {
			cursor: pointer;
			padding: 0.5rem 0.8rem;
		}

		#status {
			margin: 0;
			white-space: pre-wrap;
			overflow-wrap: anywhere;
		}

		.channels {
			display: grid;
			gap: var(--space);
			grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
		}

		section {
			display: flex;
			flex-direction: column;
			gap: calc(var(--space) / 2);
			min-width: 0;
		}

		h4 {
			margin: 0;
			font-size: 0.95rem;
		}
	}
`

export default styles