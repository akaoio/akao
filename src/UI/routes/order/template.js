import "/UI/layouts/main/index.js"
import "/UI/components/context/index.js"
import { html } from "/core/UI.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <layout-main>
        <h1><ui-context data-key="dictionary.order" /></h1>
        <div>
            Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec suscipit auctor dui, at convallis nisl efficitur a. Sed sed nunc eget nunc efficitur efficitur. Donec ut ligula id enim efficitur commodo. In hac habitasse platea dictumst. Curabitur ac felis a enim efficitur tincidunt. Donec in odio sed nisl convallis varius. Maecenas ac nisi id ipsum faucibus fermentum. Donec et ligula a metus efficitur tristique. Donec ut ligula id enim efficitur commodo. In hac habitasse platea dictumst. Curabitur ac felis a enim efficitur tincidunt. Donec in odio sed nisl convallis varius. Maecenas ac nisi id ipsum faucibus fermentum.
        </div>
    </layout-main>
`
export default template
