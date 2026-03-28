import { css } from "/core/UI.js"

export const styles = css`
	:host {
		display: block;

		#status {
			margin: 0;
			white-space: pre-wrap;
			overflow-wrap: anywhere;
			font-size: 0.85rem;
			opacity: 0.6;
		}
	}
`

export default styles