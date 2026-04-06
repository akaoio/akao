import { html } from "/core/UI.js"
import "/UI/components/modal/index.js"
import "/UI/components/identicon/index.js"
import "/UI/components/signout/index.js"
import "/UI/components/authorize/index.js"
import "/UI/components/a/index.js"
import styles from "./styles.css.js"

export const template = html`
    ${styles}
    <ui-modal data-header="profile" class="center">
        <ui-authorize slot="header" />
        <ui-signout slot="header" />
        <section>
            <div class="profile-identity">
                <ui-identicon id="profile-modal-identicon" data-size="7" />
                <div class="profile-identity__info">
                    <span id="profile-modal-name" class="profile-modal-name"></span>
                    <a is="ui-a" data-to="/profile" id="profile-modal-link" class="profile-modal-link">View profile →</a>
                </div>
            </div>
        </section>
    </ui-modal>
`

export default template
